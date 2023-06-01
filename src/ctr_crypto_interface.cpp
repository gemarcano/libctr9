/*******************************************************************************
 * Copyright (C) 2016, 2017 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

#include <ctr9/io/ctr_crypto_interface.h>
#include <ctr9/io/ctr_io_implementation.hpp>

#include <ctr9/io/sdmmc/sdmmc.h>
#include <string.h>
#include <ctr9/aes.h>
#include <ctr9/sha.h>

#include <stdalign.h>

static inline size_t get_chunk(uint64_t position, size_t chunk_size);
static inline size_t get_chunk_following(uint64_t position, size_t chunk_size);
static inline uint64_t get_chunk_position(size_t chunk, size_t chunk_size);
static inline size_t get_prev_relative_chunk(size_t chunk, size_t chunk_size, size_t other_chunk_size);
static inline size_t get_next_relative_chunk(size_t chunk, size_t chunk_size, size_t other_chunk_size);
static inline uint64_t get_prev_relative_chunk_position(size_t chunk, size_t chunk_size, size_t other_chunk_size);
static inline uint64_t get_next_relative_chunk_position(size_t chunk, size_t chunk_size, size_t other_chunk_size);
static inline size_t get_chunks_to_complete_relative_chunk_backwards(size_t chunk, size_t chunk_size, size_t other_chunk_size);
static inline size_t get_chunks_to_complete_relative_chunk(size_t chunk, size_t chunk_size, size_t other_chunk_size);

/*
static inline void ecb_wrapper(void* inbuf, void* outbuf, size_t size, uint32_t mode, uint8_t *ctr)
{
	ecb_decrypt(inbuf, outbuf, size, mode);
}

static inline void cbc_advance_ctr(ctr_crypto_interface *io, uint8_t *buffer, size_t buffer_size, size_t block, uint8_t *ctr)
{
	if (!block)
		return;

	//else read the previous processed block and used that as the ctr
	uint64_t pos = get_chunk_position(block - 1, io->block_size);
	ctr_io_read(io->lower_io, ctr, 16, pos, 16);
}

static inline void ctr_advance_ctr(ctr_crypto_interface *io, uint8_t *buffer, size_t buffer_size, size_t block, uint8_t *ctr)
{
	add_ctr(ctr, block);
}

static inline void ecb_advance_ctr(ctr_crypto_interface *io, uint8_t *buffer, size_t buffer_size, size_t block, uint8_t *ctr)
{
	//Nothing needs to happen
}

static inline void ccm_advance_ctr(ctr_crypto_interface *io, uint8_t *buffer, size_t buffer_size, size_t block, uint8_t *ctr)
{

}

static inline void input(void *io, void *buffer, uint64_t block, size_t block_count)
{
	ctr_crypto_interface *crypto_io = io;
	if (block_count)
	{
		uint32_t mode = crypto_io->input_mode;
		alignas(4) uint8_t ctr[16];

		memcpy(ctr, crypto_io->ctr, 16);
		crypto_io->advance_ctr_input(crypto_io, buffer, block_count * crypto_io->block_size, block, ctr);

		use_aeskey(crypto_io->keySlot);

		crypto_io->crypto_input(buffer, buffer, block_count, mode, ctr);
	}
}

static inline void output(void *io, void *buffer, uint64_t block, size_t block_count)
{

	ctr_crypto_interface *crypto_io = io;
	if (block_count)
	{
		uint32_t mode = crypto_io->output_mode;
		alignas(4) uint8_t ctr[16];

		memcpy(ctr, crypto_io->ctr, 16);
		crypto_io->advance_ctr_output(crypto_io, buffer, block_count * crypto_io->block_size, block, ctr);

		use_aeskey(crypto_io->keySlot);

		crypto_io->crypto_output(buffer, buffer, block_count, mode, ctr);
	}
}
*/

namespace ctr9
{
	ecb_disk_crypto_wrapper::ecb_disk_crypto_wrapper(ecb_crypto& crypto, const std::array<std::uint8_t, aes_block_size()>&)
	:crypto_(crypto)
	{}
	
	void ecb_disk_crypto_wrapper::encrypt(const void *in, void *out, size_t blocks, size_t block_position)
	{
		crypto_.encrypt(in, out, blocks, block_position);
	}
	
	void ecb_disk_crypto_wrapper::decrypt(const void *in, void *out, size_t blocks, size_t block_position)
	{
		crypto_.decrypt(in, out, blocks, block_position);
	}
	
	static constexpr std::size_t ecb_disk_crypto_wrapper::block_size()
	{
		return crypto_.block_size();
	}
}

namespace ctr9
{
	static ctr9::crypto::generic_interface *create_crypto_generic_interface(std::uint8_t key_slot, std::uint32_t mode, const std::array<std::uint8_t, aes_block_size()>& ctr, ctr_io_interface *lower_io)
	{
		//FIXME
		ctr9::crypto_generic_interface *result = nullptr;// = new (std::nothrow) ctr9::
		return result;
	}
}

ctr_crypto_interface *ctr_crypto_interface_initialize(uint8_t key_slot, uint32_t mode, ctr_crypto_disk_type disk_type, ctr_crypto_type type, uint8_t *ctr, void *lower_io)
{
	ctr9::crypto_generic_interface *result = ctr9::create_crypto_generic_interface(key_slot, mode, ctr, lower_io);
	return reinterpret_cast<ctr_crypto_interface*>(result);
}

void ctr_crypto_interface_destroy(ctr_crypto_interface *crypto_io)
{
	ctr9::crypto_generic_interface *crypto = reinterpret_cast<ctr9::crypto_generic_interface *>(crypto_io);
	*crypto_io = (ctr_crypto_interface){0};
}

//Begin block/sector hell

#define CEIL(x, y) ( ((x) + (y) - 1)/ (y) )
#define FLOOR(x, y) ( (x)/(y) )

namespace ctr9
{
	//Helper functions to deal with chunks of data, be it sectors and blocks

	class chunk_disk
	{
	public:
		size_t get_chunk(uint64_t position) const;
		size_t get_chunk_following(uint64_t position) const;
		uint64_t get_position(size_t chunk) const;

		size_t get_chunk_size() const;
	private:
		size_t chunk_size_;
	};
	//Gets the chunk in which the position lies
	size_t chunk_disk::get_chunk(uint64_t position) const
	{
		return FLOOR(position, chunk_size_);
	}

	//Gets the chunk after the chunk wherein position lies
	size_t chunk_disk::get_chunk_following(uint64_t position) const
	{
		return CEIL(position, chunk_size_);
	}

	//Returns the position of the given chunk
	std::uint64_t chunk_disk::get_chunk_position(size_t chunk) const
	{
		return (std::uint64_t)chunk * chunk_size_;
	}

	std::size_t chunk_disk::get_chunk_size() const
	{
		return _chunk_size;
	}

	/*static inline size_t get_chunks_to_complete_relative_chunk(size_t chunk, size_t chunk_size, size_t other_chunk_size)
	{
		uint64_t delta = get_next_relative_chunk_position(chunk, chunk_size, other_chunk_size) - get_chunk_position(chunk, chunk_size);
		if (delta)
			return CEIL(delta, chunk_size);
		return 0;
	}*/

	//helper function. Retrieves an AES block, regardless of whether it is aligned or not with the underlying sector geometry
	static inline int get_misaligned_block(ctr_crypto_interface *io, size_t sector, size_t sector_size, size_t block_size, uint8_t *buffer, void (*block_function)(void *io, void *buffer, uint64_t block, size_t block_count))
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

		block_function(io, pos, current_block, 1);
		memcpy(buffer, pos, block_size);

		return res;
	}

	int ctr_crypto_interface_read_sector(void *io, void *buffer, size_t buffer_size, size_t sector, size_t count)
	{
		int res = 0;
		ctr_crypto_interface *crypto_io = io;
		const size_t sector_size = ctr_io_sector_size(crypto_io->lower_io);
		const size_t block_size = AES_BLOCK_SIZE;

		size_t result_count = count < buffer_size/sector_size ? count : buffer_size/sector_size;

		void (*block_function)(void *io, void *buffer, uint64_t block, size_t block_count);
		block_function = output;

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

	//Helper struct/context for write window
	typedef struct
	{
		uint8_t *window;
		size_t window_size;
		size_t window_offset;

		size_t sector;
		size_t sector_size;

		size_t block;
		size_t block_offset;
		size_t block_size;

		const uint8_t *buffer;
		const size_t buffer_size;
		size_t buffer_offset;

		size_t current_sector;
	} write_window;

	//The way the window functions were designed was to do an initialization and then repeatedly call a progress/process function until there is no more to process.

	//Sets up the window
	static inline int setup_window(ctr_crypto_interface *io, write_window *window, size_t sector, size_t sector_size, size_t block_size)
	{
		//window_size is a multiple of the sector size by definition

		//What if sectors_to_copy > window_size?
		//That can never happen, since
		//	window_size == lcm(sector_size, block_size) * 4 + sector_size * sectors_per_block
		//so window size should be large enough for at least 4 blocks
		size_t sectors_to_copy_prior = get_chunks_to_complete_relative_chunk_backwards(sector, sector_size, block_size);

		window->sector = sector - sectors_to_copy_prior;
		window->current_sector = sector;
		window->sector_size = sector_size;

		window->block = get_prev_relative_chunk(window->current_sector, sector_size, block_size);
		window->block_offset = get_chunk_position(window->block, block_size) - get_chunk_position(window->sector, sector_size);
		window->block_size = block_size;

		window->window_offset = sectors_to_copy_prior * sector_size;

		int res = ctr_io_read_sector(io->lower_io, window->window, window->window_size, window->sector, sectors_to_copy_prior);

		return res;
	}

	//Called by process_window. Used to update the window in preparation for the next process_window call
	static inline void update_window(write_window *window, size_t sectors_processed)
	{
		size_t sector_size = window->sector_size;
		size_t block_size = window->block_size;
		size_t bytes_processed = sectors_processed * sector_size;

		//Copy sector that contain part of next block
		size_t remaining_bytes = window->window_size - bytes_processed;
		memmove(window->window, window->window + window->window_offset, remaining_bytes);

		window->current_sector += sectors_processed;
		window->sector += sectors_processed - remaining_bytes / sector_size;

		window->block = get_prev_relative_chunk(window->current_sector, sector_size, block_size);
		window->block_offset = remaining_bytes;
		window->window_offset = remaining_bytes;
	}

	//process the current window, and advance to the next one.
	//Returns a negative number on a failure, a zero when there are still more windows to go, and a 1 when done.
	static inline int process_window(ctr_crypto_interface *io, write_window *window, size_t sector_size, size_t block_size, void (*input_function)(void *io, void *buffer, uint64_t block, size_t block_count), void (*output_function)(void *io, void *buffer, uint64_t block, size_t block_count))
	{
		//FIXME what if we try to read more than there is disk? Return zeros?
		size_t sectors_to_read = (window->window_size - window->window_offset) / sector_size;
		int res = ctr_io_read_sector(io->lower_io, window->window + window->window_offset, window->window_size - window->window_offset, window->current_sector, sectors_to_read);
		if (res)
			return -1;

		size_t block_start_offset = window->block_offset;
		uint8_t *pos = window->window + block_start_offset;

		size_t amount_to_copy = window->window_size - window->window_offset;
		if (amount_to_copy > window->buffer_size - window->buffer_offset)
			amount_to_copy = window->buffer_size - window->buffer_offset;

		//In the case that blocks are aligned to sectors, this is not necessary... FIXME
		//only process parts that won't be overwritten completely
		//	from block_start_offset to window->window_offset
		size_t blocks_to_process = CEIL(window->window_offset - block_start_offset, block_size);
		if (!blocks_to_process) blocks_to_process = 1;
		output_function(io, pos, window->block, blocks_to_process);

		//now copy data from buffer
		memcpy(window->window + window->window_offset, window->buffer + window->buffer_offset, amount_to_copy);
		window->buffer_offset += amount_to_copy;

		blocks_to_process = CEIL(amount_to_copy + window->window_offset - block_start_offset, block_size);
		input_function(io, pos, window->block, blocks_to_process);

		//We need to write out the full blocks processed actually, not just the sectors
		size_t sectors_processed = (amount_to_copy + window->window_offset) / sector_size;
		size_t sectors_to_copy = CEIL(blocks_to_process * block_size, sector_size);
		res = ctr_io_write_sector(io->lower_io, window->window, sectors_to_copy * sector_size, window->sector);
		if (res)
			return -2;

		//Copy sector that contain part of next block
		update_window(window, sectors_processed);

		return window->buffer_offset >= window->buffer_size;
	}

	int ctr_crypto_interface_write_sector(void *io, const void *buffer, size_t buffer_size, size_t sector)
	{
		int res = 0;
		ctr_crypto_interface *crypto_io = io;
		const size_t sector_size = ctr_io_sector_size(crypto_io->lower_io);
		const size_t number_of_sectors = FLOOR(buffer_size, sector_size);
		const size_t block_size = AES_BLOCK_SIZE;
		size_t sectors_per_block = block_size / sector_size;

		if (!sectors_per_block)
			sectors_per_block = 1;

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

			res = process_window(io, &window, sector_size, block_size, input, output);
			while(!res)
			{
				res = process_window(io, &window, sector_size, block_size, input, output);
			}
		}
		return res == 1 ? 0 : res;
	}
}

