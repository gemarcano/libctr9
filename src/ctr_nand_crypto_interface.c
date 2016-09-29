/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

#include <ctr9/io/ctr_nand_crypto_interface.h>
#include <ctr9/io/sdmmc/sdmmc.h>
#include <string.h>
#include <ctr9/aes.h>
#include <ctr9/sha.h>

#include <stdalign.h>

#include <ctr9/io/ctr_io_implementation.h>

//FIXME these are unique per instance... or are they?

static const ctr_io_interface nand_crypto_base =
{
	ctr_nand_crypto_interface_read,
	ctr_nand_crypto_interface_write,
	ctr_nand_crypto_interface_read_sector,
	ctr_nand_crypto_interface_write_sector,
	ctr_nand_crypto_interface_disk_size,
	ctr_nand_crypto_interface_sector_size
};

static inline void process_aes_ctr_blocks(void *buffer, void *ctr, uint64_t blocks, uint32_t mode)
{
	ctr_decrypt(buffer, buffer, blocks, mode, ctr);
}

int ctr_nand_crypto_interface_initialize(ctr_nand_crypto_interface *crypto_io, uint8_t keySlot, ctr_nand_crypto_type crypto_type, ctr_io_interface *lower_io)
{
	crypto_io->base = nand_crypto_base;
	crypto_io->lower_io = lower_io;
	crypto_io->keySlot = keySlot;
	crypto_io->mode = AES_CNT_CTRNAND_MODE;

	//Get the nonces for CTRNAND and TWL decryption
	uint32_t NandCid[4];
	alignas(4) uint8_t shasum[32];

	sdmmc_get_cid(true, NandCid);

	switch (crypto_type)
	{
		case NAND_CTR:
			sha_init(SHA256_MODE);
			sha_update((uint8_t*)NandCid, 16);
			sha_get(shasum);
			memcpy(crypto_io->ctr, shasum, 16);
			crypto_io->mode = AES_CNT_CTRNAND_MODE;
			break;

		case NAND_TWL:
			sha_init(SHA1_MODE);
			sha_update((uint8_t*)NandCid, 16);
			sha_get(shasum);
			for(uint32_t i = 0; i < 16u; i++) // little endian and reversed order
			{
				crypto_io->ctr[i] = shasum[15-i];
			}
			crypto_io->mode = AES_CNT_TWLNAND_MODE;
			break;

		default:
			return 1; //Unknown type
	}
	return 0;
}

//Buffer must be block aligned
static void apply_ctr_blocks(void *io, void *buffer, uint64_t block, size_t block_count)
{
	ctr_nand_crypto_interface *crypto_io = io;
	if (block_count)
	{
		uint32_t mode = crypto_io->mode;
		alignas(4) uint8_t ctr[16];

		memcpy(ctr, crypto_io->ctr, 16);
		add_ctr(ctr, block);

		//apply AES CTR to the data
		use_aeskey(crypto_io->keySlot);

		ctr_decrypt(buffer, buffer, block_count, mode, ctr);
	}
}

void ctr_nand_crypto_interface_destroy(ctr_nand_crypto_interface *crypto_io)
{
	*crypto_io = (ctr_nand_crypto_interface){0};
}

int ctr_nand_crypto_interface_read(void *io, void *buffer, size_t buffer_size, uint64_t position, size_t count)
{
	return ctr_io_implementation_read(io, buffer, buffer_size, position, count, ctr_nand_crypto_interface_read_sector);
}

int ctr_nand_crypto_interface_write(void *io, const void *buffer, size_t buffer_size, uint64_t position)
{
	return ctr_io_implementation_write(io, buffer, buffer_size, position, ctr_nand_crypto_interface_read_sector, ctr_nand_crypto_interface_write_sector);
}

#define CEIL(x, y) ( ((x) + (y) - 1)/ (y) )
#define FLOOR(x, y) ( (x)/(y) )

static inline size_t get_chunk(uint64_t position, size_t chunk_size)
{
	return FLOOR(position, chunk_size);
}

static inline size_t get_chunk_following(uint64_t position, size_t chunk_size)
{
	return CEIL(position, chunk_size);
}

static inline uint64_t get_chunk_position(size_t chunk, size_t chunk_size)
{
	return (uint64_t)chunk * chunk_size;
}

static inline size_t get_prev_relative_chunk(size_t chunk, size_t chunk_size, size_t other_chunk_size)
{
	return get_chunk((uint64_t)chunk * chunk_size, other_chunk_size);
}

static inline size_t get_next_relative_chunk(size_t chunk, size_t chunk_size, size_t other_chunk_size)
{
	if (chunk)
		return get_chunk_following((uint64_t)chunk * chunk_size, other_chunk_size);
	return 0;
}

static inline uint64_t get_prev_relative_chunk_position(size_t chunk, size_t chunk_size, size_t other_chunk_size)
{
	return get_chunk_position(get_prev_relative_chunk(chunk, chunk_size, other_chunk_size), other_chunk_size);
}

static inline uint64_t get_next_relative_chunk_position(size_t chunk, size_t chunk_size, size_t other_chunk_size)
{
	return get_chunk_position(get_next_relative_chunk(chunk, chunk_size, other_chunk_size), other_chunk_size);
}

static inline size_t get_chunks_to_complete_relative_chunk_backwards(size_t chunk, size_t chunk_size, size_t other_chunk_size)
{
	uint64_t delta = get_chunk_position(chunk, chunk_size) - get_prev_relative_chunk_position(chunk, chunk_size, other_chunk_size);
	if (delta)
		return CEIL(delta, chunk_size);
	return 0;
}

static inline size_t get_chunks_to_complete_relative_chunk(size_t chunk, size_t chunk_size, size_t other_chunk_size)
{
	uint64_t delta = get_next_relative_chunk_position(chunk, chunk_size, other_chunk_size) - get_chunk_position(chunk, chunk_size);
	if (delta)
		return CEIL(delta, chunk_size);
	return 0;
}

static inline int get_misaligned_block(ctr_nand_crypto_interface *io, size_t sector, size_t sector_size, size_t block_size, uint8_t *buffer, void (*block_function)(void *io, void *buffer, uint64_t block, size_t block_count))
{
	size_t sectors_to_copy_prior = get_chunks_to_complete_relative_chunk_backwards(sector, sector_size, block_size);
	size_t sectors_to_copy_after = get_chunks_to_complete_relative_chunk(sector, sector_size, block_size);
	if (!sectors_to_copy_after)
		sectors_to_copy_after = block_size;

	size_t sector_count = sectors_to_copy_prior + sectors_to_copy_after;
	size_t buf_size = sector_count * sector_size;
	uint8_t buf[buf_size];
	int res = ctr_io_read_sector(io->lower_io, buf, buf_size, sector - sectors_to_copy_prior, sector_count);

	if (res)
		return res;

	size_t current_block = get_prev_relative_chunk(sector, sector_size, block_size);
	uint64_t block_pos = get_chunk_position(current_block, block_size);
	uint64_t block_start_offset = block_pos - get_chunk_position(sector - sectors_to_copy_prior, sector_size);
	uint8_t *pos = buf + block_start_offset;
	size_t sector_pos = get_chunk_position(sector, sector_size);

	block_function(io, pos, current_block, 1);
	memcpy(buffer, pos, block_size);

	return res;
}

static int ctr_apply_process_blocks_read(void *io, void *buffer, size_t buffer_size, size_t sector, size_t count, size_t block_size, void (*block_function)(void *io, void *buffer, uint64_t block, size_t block_count))
{
	int res = 0;
	ctr_nand_crypto_interface *crypto_io = io;
	const size_t sector_size = ctr_io_sector_size(crypto_io->lower_io);

	size_t result_count = count < buffer_size/sector_size ? count : buffer_size/sector_size;

	if (result_count)
	{
		size_t result_size = result_count * sector_size;
		uint8_t *current_processed = buffer;
		size_t current_block = get_prev_relative_chunk(sector, sector_size, block_size);

		res = ctr_io_read_sector(crypto_io->lower_io, buffer, buffer_size, sector, result_count);
		if (res)
			return res;

		//Part 1, deal with misaligned first block
		size_t sectors_to_copy_prior = get_chunks_to_complete_relative_chunk_backwards(sector, sector_size, block_size);
		if (sectors_to_copy_prior)
		{
			uint8_t buf[block_size];
			res = get_misaligned_block(crypto_io, sector, sector_size, block_size, buf, block_function);
			if (res)
				return res;

			//We now have the full block-- we now need to figure out which part of it to copy
			uint64_t sector_pos = get_chunk_position(sector, sector_size);
			uint64_t block_pos = get_chunk_position(current_block, block_size);

			size_t amount_to_copy = block_size - (sector_pos - block_pos);
			amount_to_copy = amount_to_copy < buffer_size ? amount_to_copy : buffer_size;
			memcpy(buffer, buf + (sector_pos - block_pos), amount_to_copy);
			current_processed += amount_to_copy;

			current_block++;
		}

		//Part 2, Deal with all intermediate blocks
		size_t bytes_left = result_size - (size_t)(current_processed - (uint8_t*)buffer);
		size_t blocks = FLOOR(bytes_left, block_size);
		if (blocks)
		{
			block_function(io, current_processed, current_block, blocks);
			current_block += blocks;
			current_processed += blocks * block_size;
			bytes_left -= blocks * block_size;
		}

		//Part 3, deal with the final block
		//FIXME what if we need to deal with a block that actually continues past the end of the disk? pad with zero?
		if (bytes_left)
		{
			uint8_t buf[block_size];
			//size_t current_sector = get_next_relative_block
			size_t block_sector = get_prev_relative_chunk(current_block, block_size, sector_size);
			res = get_misaligned_block(crypto_io, block_sector, sector_size, block_size, buf, block_function);
			if (res)
				return res;

			//We now have the full block-- we now need to figure out which part of it to copy
			uint64_t sector_pos = get_chunk_position(block_sector, sector_size);
			uint64_t block_pos = get_chunk_position(current_block, block_size);

			size_t amount_to_copy = block_size - (sector_pos - block_pos);
			amount_to_copy = amount_to_copy < bytes_left ? amount_to_copy : bytes_left;
			memcpy(current_processed, buf + (sector_pos - block_pos), amount_to_copy);
		}

	}
	return res;
}

int ctr_nand_crypto_interface_read_sector(void *io, void *buffer, size_t buffer_size, size_t sector, size_t count)
{
	return ctr_apply_process_blocks_read(io, buffer, buffer_size, sector, count, AES_BLOCK_SIZE, apply_ctr_blocks);
}

static inline size_t gcd(size_t a, size_t b)
{
	while (a != b)
	{
		if (a > b)
			a -= b;
		else
			b -= a;
	}
	return a;
}

static inline size_t lcm(size_t a, size_t b)
{
	return a / gcd(a,b) * b;
}

typedef struct
{
	uint8_t *window;
	size_t window_size;
	size_t window_offset;

	const uint8_t *buffer;
	const size_t buffer_size;
	size_t buffer_offset;

	size_t current_sector;
} write_window;

//window_size is a multiple of the sector size by definition
static inline void setup_window(ctr_nand_crypto_interface *io, write_window *window, size_t sector, size_t sector_size, size_t block_size)
{
	//FIXME what if sectors_to_copy > window_size?
	//That can never happen, since window_size == lcm(sector_size, block_size) * 4 + sector_size * sectors_per_block, so window size should be large enough for at least 4 blocks
	size_t sectors_to_copy_prior = get_chunks_to_complete_relative_chunk_backwards(sector, sector_size, block_size);
	window->current_sector -= sectors_to_copy_prior;
	window->window_offset = sectors_to_copy_prior * sector_size;
}

static inline int process_window(ctr_nand_crypto_interface *io, write_window *window, size_t sector_size, size_t block_size, void (*block_function)(void *io, void *buffer, uint64_t block, size_t block_count))
{
	size_t sectors_to_read = (window->window_size - window->window_offset) / sector_size;
	int res = ctr_io_read_sector(io->lower_io, window->window + window->window_offset, window->window_size - window->window_offset, window->current_sector, sectors_to_read);
	if (res)
		return -1;

	size_t current_block = get_next_relative_chunk(window->current_sector, sector_size, block_size);
	uint64_t block_pos = get_chunk_position(current_block, block_size);
	uint64_t block_start_offset = block_pos - get_chunk_position(window->current_sector, sector_size);
	uint8_t *pos = window->window + block_start_offset;
	size_t blocks_to_process = window->window_size / block_size;

	//In the case that blocks are aligned to sectors, this is not necessary... FIXME
	block_function(io, pos, current_block, blocks_to_process);

	//now copy data from buffer
	size_t amount_to_copy = window->window_size - window->window_offset;
	if (amount_to_copy > window->buffer_size - window->buffer_offset)
		amount_to_copy = window->buffer_size - window->buffer_offset;

	memcpy(window->window + window->window_offset, window->buffer + window->buffer_offset, window->window_size - window->window_offset);
	window->buffer_offset += window->window_size - window->window_offset;

	block_function(io, pos, current_block, blocks_to_process);

	size_t sectors_processed = (window->window_size - window->window_offset)/sector_size;
	res = ctr_io_write_sector(io->lower_io, window->window, sectors_processed * sector_size, window->current_sector);
	if (res)
		return -2;

	//Copy sector that contain part of next block
	size_t remaining_bytes = window->window_size - (uint64_t)sectors_processed * sector_size;
	memmove(window->window, window->window + window->window_offset, remaining_bytes);

	window->current_sector += sectors_processed;
	window->window_offset = remaining_bytes;

	return window->buffer_offset >= window->buffer_size;
}

int ctr_nand_crypto_interface_write_sector(void *io, const void *buffer, size_t buffer_size, size_t sector)
{
	int res = 0;
	ctr_nand_crypto_interface *crypto_io = io;
	const size_t sector_size = ctr_io_sector_size(crypto_io->lower_io);
	const size_t number_of_sectors = FLOOR(buffer_size, sector_size);
	const size_t block_size = AES_BLOCK_SIZE;
	size_t sectors_per_block = block_size / sector_size;

	if (!sectors_per_block)
		sectors_per_block = 1;

	void (*block_function)(void *io, void *buffer, uint64_t block, size_t block_count);
	block_function = apply_ctr_blocks;

	if (number_of_sectors)
	{
		size_t block_sector_lcm = lcm(sector_size, block_size);

		uint8_t window_buffer[4 * block_sector_lcm + sectors_per_block * sector_size];
		write_window window =
		{
			.window = window_buffer,
			.window_size = sizeof(window_buffer),
			.window_offset = 0,
			.buffer = buffer,
			.buffer_size = buffer_size,
			.buffer_offset = 0,
			.current_sector = sector
		};

		setup_window(io, &window, sector, sector_size, block_size);

		size_t sectors_left = number_of_sectors;
		res = process_window(io, &window, sector_size, block_size, block_function);
		while(!res)
		{
			res = process_window(io, &window, sector_size, block_size, block_function);
		}
	}
	return res == 1 ? 0 : res;
}

uint64_t ctr_nand_crypto_interface_disk_size(void *io)
{
	ctr_nand_crypto_interface *crypto_io = io;
	return crypto_io->lower_io->disk_size(io);
}

size_t ctr_nand_crypto_interface_sector_size(void *io)
{
	ctr_nand_crypto_interface *crypto_io = io;
	return crypto_io->lower_io->sector_size(io);
}

