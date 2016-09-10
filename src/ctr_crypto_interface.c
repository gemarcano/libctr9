/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

#include <ctr9/io/ctr_crypto_interface.h>
#include <ctr9/io/sdmmc/sdmmc.h>
#include <string.h>
#include <ctr9/aes.h>
#include <ctr9/sha.h>

#include <stdalign.h>

//FIXME these are unique per instance... or are they?

static const ctr_io_interface crypto_base =
{
	ctr_crypto_interface_read,
	ctr_crypto_interface_write,
	ctr_crypto_interface_read_sector,
	ctr_crypto_interface_write_sector,
	ctr_crypto_interface_disk_size,
	ctr_crypto_interface_sector_size
};

static inline void ecb_wrapper(void* inbuf, void* outbuf, size_t size, uint32_t mode, uint8_t *ctr)
{
	ecb_decrypt(inbuf, outbuf, size, mode);
}

static inline void process_aes_ctr_blocks(void *buffer, uint8_t *ctr, size_t blocks, uint32_t mode)
{
	ctr_decrypt(buffer, buffer, blocks, mode, ctr);
}

int ctr_crypto_interface_initialize(ctr_crypto_interface *crypto_io, uint8_t keySlot, uint32_t mode, uint32_t *ctr, ctr_io_interface *lower_io)
{
 /*
	crypto_io->base = crypto_base;
	crypto_io->lower_io = lower_io;
	crypto_io->keySlot = keySlot;
	crypto_io->input_mode = (mode & ~(7u << 27)); //Mask out any mode bits
	crypto_io->output_mode = (mode & ~(7u << 27)); //Mask out any mode bits
	memcpy(crypto_io->ctr, ctr, 16);

	uint32_t *encrypt_mode, *decrypt_mode;
	void (**crypto_encrypt)(void* inbuf, void* outbuf, size_t size, uint32_t mode, uint8_t *ctr);
	void (**crypto_decrypt)(void* inbuf, void* outbuf, size_t size, uint32_t mode, uint8_t *ctr);

	if (type == CTR_CRYPTO_PLAINTEXT)
	{
		encrypt_mode = &crypto_io->output_mode;
		decrypt_mode = &crypto_io->input_mode;
		crypto_encrypt = &crypto_io->crypto_output;
		crypto_decrypt = &crypto_io->crypto_input;
	}
	else
	{
		encrypt_mode = &crypto_io->input_mode;
		decrypt_mode = &crypto_io->output_mode;
		crypto_encrypt = &crypto_io->crypto_input;
		crypto_decrypt = &crypto_io->crypto_output;
	}

	switch (type)
	{
		case CRYPTO_CCM:
			*encrypt_mode |= AES_CCM_ENCRYPT_MODE;
			*decrypt_mode |= AES_CCM_DECRYPT_MODE;
			*crypto_encrypt = ccm_encrypt;
			*crypto_decrypt = ccm_decrypt;
			break;
		case CRYPTO_CTR:
			*encrypt_mode |= AES_CTR_MODE;
			*decrypt_mode |= AES_CTR_MODE;
			*crypto_encrypt = ctr_decrypt;
			*crypto_decrypt = ctr_decrypt;

			break;

		case CRYPTO_CBC:
			*encrypt_mode |= AES_CBC_ENCRYPT_MODE;
			*decrypt_mode |= AES_CBC_DECRYPT_MODE;
			*crypto_encrypt = cbc_encrypt;
			*crypto_decrypt = cbc_decrypt;

			break;
		case CRYPTO_ECB:
		default:
			*encrypt_mode |= AES_ECB_ENCRYPT_MODE;
			*decrypt_mode |= AES_ECB_DECRYPT_MODE;
			*crypto_encrypt = ecb_wrapper;
			*crypto_decrypt = ecb_wrapper;

			break;
	}
*/
	return 0;
}

static void encrypt_sector(ctr_crypto_interface *crypto_io, uint8_t* buffer, uint32_t sector, uint32_t count)
{
	if (count)
	{
		uint32_t mode = crypto_io->mode;
		size_t sector_size = ctr_io_sector_size(crypto_io);

		alignas(4) uint8_t ctr[16];

		memcpy(ctr, crypto_io->ctr, 16);
		add_ctr(ctr, sector * (sector_size / 0x10));

		//apply AES CTR to the data
		use_aeskey(crypto_io->keySlot);

		size_t blocks_to_do = count * 0x200 / 0x10;
		process_aes_ctr_blocks(buffer, ctr, blocks_to_do, mode);
	}
}

static void applyAESCTRSector(ctr_crypto_interface *crypto_io, uint8_t* buffer, uint32_t sector, uint32_t count)
{
	if (count)
	{
		uint32_t mode = crypto_io->mode;

		alignas(4) uint8_t ctr[16];

		memcpy(ctr, crypto_io->ctr, 16);
		add_ctr(ctr, sector * (0x200 / 0x10));

		//apply AES CTR to the data
		use_aeskey(crypto_io->keySlot);

		size_t blocks_to_do = count * 0x200 / 0x10;
		process_aes_ctr_blocks(buffer, ctr, blocks_to_do, mode);
	}
}
static void applyAESCTR(ctr_crypto_interface *crypto_io, uint8_t* buffer, uint32_t location, uint32_t count)
{
	if (count)
	{
		uint32_t mode = crypto_io->mode;
		alignas(4) uint8_t ctr[16];
		alignas(4) uint8_t block_buffer[16];
		uint32_t amount_read = 0;

		use_aeskey(crypto_io->keySlot);

		memcpy(ctr, crypto_io->ctr, 16);
		add_ctr(ctr, location / 0x10);

		//Section 1: First block always exists, may or may not be aligned to block boundaries.
		uint8_t block_offset = location & 0xF;
		size_t current_section_size = 16u - block_offset;
		if (current_section_size > count)
		{
			current_section_size = count;
		}

		memcpy(block_buffer + block_offset, buffer, current_section_size);
		process_aes_ctr_blocks(block_buffer, ctr, 1, mode);
		memcpy(buffer, block_buffer + block_offset, current_section_size);

		amount_read = current_section_size;

		//Section 2: All intermediate blocks (may not exist)
		current_section_size = count - amount_read;
		current_section_size -= current_section_size % 16;

		if (current_section_size)
		{
			process_aes_ctr_blocks(buffer + amount_read, ctr, current_section_size / 0x10, mode);
		}

		amount_read += current_section_size;

		//Section 3: Last block may or may not exist. May or may not end at a block boundary.
		current_section_size = (count - amount_read) % 16;
		if (current_section_size)
		{
			memcpy(block_buffer, buffer + amount_read, current_section_size);
			process_aes_ctr_blocks(block_buffer, ctr, 1, mode);
			memcpy(buffer + amount_read, block_buffer, current_section_size);
		}
	}
}

void ctr_crypto_interface_destroy(ctr_crypto_interface *crypto_io)
{
	*crypto_io = (ctr_crypto_interface){0};
}

int ctr_crypto_interface_read(void *io, void *buffer, size_t buffer_size, uint64_t position, size_t count)
{
	int res = 0;
	if (count)
	{
		ctr_crypto_interface *crypto_io = io;
		res = crypto_io->lower_io->read(io, buffer, buffer_size, position, count);

		//we now have raw data, apply crypto
		applyAESCTR(io, (uint8_t*)buffer, position, count < buffer_size ? count : buffer_size);
	}

	return res;
}

int ctr_crypto_interface_write(void *io, const void *buffer, size_t buffer_size, uint64_t position)
{
	ctr_crypto_interface *crypto_io = io;
	alignas(4) uint8_t buf[0x200*4];
	int res = 0;
	for (size_t i = 0; i < buffer_size && !res; i += sizeof(buf))
	{
		size_t write_size = buffer_size - i > sizeof(buf) ? sizeof(buf) : buffer_size - i;
		memcpy(buf, buffer, write_size);

		applyAESCTR(io, buf, position + i, write_size);
		res |= crypto_io->lower_io->write(io, buf, write_size, position + i);
	}

	return res;
}

int ctr_crypto_interface_read_sector(void *io, void *buffer, size_t buffer_size, size_t sector, size_t count)
{
	int res = 0;
	if (count)
	{
		ctr_crypto_interface *crypto_io = io;
		res = crypto_io->lower_io->read_sector(io, buffer, buffer_size, sector, count);
		applyAESCTRSector(io, (uint8_t*)buffer, sector, count < buffer_size/0x200 ? count : buffer_size/0x200 );
	}
	return res;
}


int ctr_crypto_interface_write_sector(void *io, const void *buffer, size_t buffer_size, size_t sector)
{
	int res = 0;
	size_t number_of_sectors = buffer_size / 0x200;

	if (number_of_sectors)
	{
		ctr_crypto_interface *crypto_io = io;
		uint8_t buf[0x200*4];
		for (size_t i = 0; i < number_of_sectors && !res; i += sizeof(buf) / 0x200)
		{
			size_t write_sectors = ((number_of_sectors - i) >= (sizeof(buf) / 0x200) ? sizeof(buf) / 0x200 : buffer_size/0x200 - i);

			memcpy(buf, buffer, write_sectors * 0x200);

			applyAESCTRSector(io, buf, sector + i, write_sectors);

			res |= crypto_io->lower_io->write_sector(io, buf, write_sectors * 0x200, sector + i);
		}
	}
	return res;
}

uint64_t ctr_crypto_interface_disk_size(void *io)
{
	ctr_crypto_interface *crypto_io = io;
	return crypto_io->lower_io->disk_size(io);
}

size_t ctr_crypto_interface_sector_size(void *io)
{
	ctr_crypto_interface *crypto_io = io;
	return crypto_io->lower_io->sector_size(io);
}

