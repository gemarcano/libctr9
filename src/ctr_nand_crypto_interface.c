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

static inline void process_aes_ctr_block(void *buffer, uint8_t *ctr, uint32_t mode)
{
	set_ctr(ctr);
	aes_decrypt(buffer, buffer, 1, mode);
	add_ctr(ctr, 0x1);		
}

int ctr_nand_crypto_interface_initialize(ctr_nand_crypto_interface *io, uint8_t keySlot, ctr_io_interface *lower_io)
{
	io->base = nand_crypto_base;
	io->lower_io = lower_io;
	io->keySlot = keySlot;

	//Get the nonces for CTRNAND and TWL decryption
	uint32_t NandCid[4];
	alignas(32) uint8_t shasum[32];

	sdmmc_get_cid(true, NandCid);
	
	sha_init(SHA256_MODE);
	sha_update((uint8_t*)NandCid, 16);
	sha_get(shasum);
	memcpy(io->CtrNandCtr, shasum, 16);

	sha_init(SHA1_MODE);
	sha_update((uint8_t*)NandCid, 16);
	sha_get(shasum);
	for(uint32_t i = 0; i < 16u; i++) // little endian and reversed order
		io->TwlNandCtr[i] = shasum[15-i];

	return 0;
}


static void applyAESCTRSector(ctr_nand_crypto_interface *io, uint8_t* buffer, uint32_t sector, uint32_t count)
{
	if (count)
	{
		//FIXME perhaps let mode be set at construction time?
		uint32_t mode = (sector >= (0x0B100000u / 0x200)) ? AES_CNT_CTRNAND_MODE : AES_CNT_TWLNAND_MODE;

		alignas(32) uint8_t ctr[16];

		memcpy(ctr, (sector >= (0x0B100000 / 0x200)) ? io->CtrNandCtr : io->TwlNandCtr, 16);
		add_ctr(ctr, sector * (0x200 / 0x10));
		
		//apply AES CTR to the data
		use_aeskey(io->keySlot);
		for (uint32_t block = 0; block < (count * 0x200 / 0x10); ++block)
		{
			process_aes_ctr_block(buffer + (block * 0x10), ctr, mode);
		}
	}
} 

static void applyAESCTR(ctr_nand_crypto_interface *io, uint8_t* buffer, uint32_t location, uint32_t count)
{
	if (count)
	{
		//FIXME Maybe move mode out?
		uint32_t mode = (location >= 0x0B100000u) ? AES_CNT_CTRNAND_MODE : AES_CNT_TWLNAND_MODE;
		alignas(32) uint8_t ctr[16];
		alignas(32) uint8_t block_buffer[16];
		uint32_t amount_read = 0;

		use_aeskey(io->keySlot);
		
		memcpy(ctr, (location >= 0x0B100000 ) ? io->CtrNandCtr : io->TwlNandCtr, 16);
		add_ctr(ctr, location / 0x10);

		//Section 1: First block always exists, may or may not be aligned to block boundaries.
		uint8_t block_offset = location & 0xF;
		size_t current_section_size = 16u - block_offset;
		if (current_section_size > count)
		{
			current_section_size = count;
		}

		memcpy(block_buffer + block_offset, buffer, current_section_size);
		process_aes_ctr_block(block_buffer, ctr, mode);
		memcpy(buffer, block_buffer + block_offset, current_section_size);

		amount_read = current_section_size;

		//Section 2: All intermediate blocks (may not exist)
		current_section_size = count - amount_read;
		current_section_size -= current_section_size % 16;

		if (current_section_size)
		{
			for (size_t byte = 0x0; byte < current_section_size; byte += 0x10)
			{
				process_aes_ctr_block(buffer + amount_read + byte, ctr, mode);
			}
		}

		amount_read += current_section_size;

		//Section 3: Last block may or may not exist. May or may not end at a block boundary.
		current_section_size = (count - amount_read) % 16;
		if (current_section_size)
		{
			memcpy(block_buffer, buffer + amount_read, current_section_size);
			process_aes_ctr_block(block_buffer, ctr, mode);
			memcpy(buffer + amount_read, block_buffer, current_section_size);
		}
	}
}

void ctr_nand_crypto_interface_destroy(ctr_nand_crypto_interface *io)
{
	*io = (ctr_nand_crypto_interface){0};
}

int ctr_nand_crypto_interface_read(void *ctx, void *buffer, size_t buffer_size, size_t position, size_t count)
{
	int res = 0;
	if (count)
	{
		ctr_nand_crypto_interface* io = ctx;
		res = io->lower_io->read(ctx, buffer, buffer_size, position, count);
		
		//we now have raw data, apply crypto
		applyAESCTR(io, (uint8_t*)buffer, position, count < buffer_size ? count : buffer_size);
	}
	
	return res;
}

int ctr_nand_crypto_interface_write(void *ctx, const void *buffer, size_t buffer_size, size_t position)
{
	ctr_nand_crypto_interface* io = ctx;
	alignas(32) uint8_t buf[0x200*4];
	int res = 0;
	for (size_t i = 0; i < buffer_size && !res; i += sizeof(buf))
	{
		size_t write_size = buffer_size - i > sizeof(buf) ? sizeof(buf) : buffer_size - i;
		memcpy(buf, buffer, write_size);

		applyAESCTR(io, buf, position + i, write_size);
		res |= io->lower_io->write(ctx, buf, write_size, position + i);
	}

	return res;
}

int ctr_nand_crypto_interface_read_sector(void *ctx, void *buffer, size_t buffer_size, size_t sector, size_t count)
{
	int res = 0;
	if (count)
	{
		ctr_nand_crypto_interface* io = ctx;
		res = io->lower_io->read_sector(ctx, buffer, buffer_size, sector, count);
		applyAESCTRSector(io, (uint8_t*)buffer, sector, count < buffer_size/0x200 ? count : buffer_size/0x200 );
	}
	return res;
}

int ctr_nand_crypto_interface_write_sector(void *ctx, const void *buffer, size_t buffer_size, size_t sector)
{
	int res = 0;
	size_t number_of_sectors = buffer_size / 0x200;

	if (number_of_sectors)
	{
		ctr_nand_crypto_interface* io = ctx;
		uint8_t buf[0x200*4];
		for (size_t i = 0; i < number_of_sectors && !res; i += sizeof(buf) / 0x200)
		{
			size_t write_sectors = ((number_of_sectors - i) >= (sizeof(buf) / 0x200) ? sizeof(buf) / 0x200 : buffer_size/0x200 - i);
			
			memcpy(buf, buffer, write_sectors * 0x200);

			applyAESCTRSector(io, buf, sector + i, write_sectors);
			res |= io->lower_io->write_sector(ctx, buf, write_sectors * 0x200, sector + i);
		}
	}
	return res;
}

size_t ctr_nand_crypto_interface_disk_size(void *ctx)
{
	ctr_nand_crypto_interface *io = ctx;
	return io->lower_io->disk_size(ctx);
}

size_t ctr_nand_crypto_interface_sector_size(void *ctx)
{
	ctr_nand_crypto_interface *io = ctx;
	return io->lower_io->sector_size(ctx);
}

