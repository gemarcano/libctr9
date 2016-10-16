/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

#include <string.h>
#include <ctr9/aes.h>
#include <ctr9/sha.h>

#include <stdalign.h>

#include <ctr9/io/ctr_nand_crypto_interface.h>
#include <ctr9/io/ctr_io_implementation.h>
#include <ctr9/ctr_system.h>

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

static inline void check_and_do_n3ds_init()
{
	static bool setup = false;
	if (!setup && ctr_detect_a9lh_entry() && ctr_get_system_type() == SYSTEM_N3DS)
	{
		ctr_n3ds_ctrnand_keyslot_setup();
	}
}

int ctr_nand_crypto_interface_initialize(ctr_nand_crypto_interface *crypto_io, uint8_t keySlot, ctr_nand_crypto_type crypto_type, ctr_io_interface *lower_io)
{
	crypto_io->base = nand_crypto_base;


	//Get the nonces for CTRNAND and TWL decryption
	uint32_t mode;
	uint32_t NandCid[4];
	alignas(4) uint8_t shasum[32];

	sdmmc_get_cid(true, NandCid);
	uint32_t ctr[4];

	switch (crypto_type)
	{
		case NAND_CTR:
			check_and_do_n3ds_init();
			sha_init(SHA256_MODE);
			sha_update((uint8_t*)NandCid, 16);
			sha_get(shasum);
			memcpy(ctr, shasum, 16);
			mode = AES_CNT_CTRNAND_MODE;
			break;

		case NAND_TWL:
			sha_init(SHA1_MODE);
			sha_update((uint8_t*)NandCid, 16);
			sha_get(shasum);
			for(uint32_t i = 0; i < 16u; i++) // little endian and reversed order
			{
				((uint8_t*)ctr)[i] = shasum[15-i];
			}
			mode = AES_CNT_TWLNAND_MODE;
			break;

		default:
			return 1; //Unknown type
	}

	ctr_crypto_interface_initialize(&crypto_io->crypto_io, keySlot, mode, CTR_CRYPTO_ENCRYPTED, CRYPTO_CTR, (uint8_t*)ctr, lower_io);
	return 0;
}

void ctr_nand_crypto_interface_destroy(ctr_nand_crypto_interface *crypto_io)
{
	ctr_crypto_interface_destroy(&crypto_io->crypto_io);
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

int ctr_nand_crypto_interface_read_sector(void *io, void *buffer, size_t buffer_size, size_t sector, size_t count)
{
	ctr_nand_crypto_interface *crypto_io = io;
	return ctr_crypto_interface_read_sector(&crypto_io->crypto_io, buffer, buffer_size, sector, count);
}

int ctr_nand_crypto_interface_write_sector(void *io, const void *buffer, size_t buffer_size, size_t sector)
{
	ctr_nand_crypto_interface *crypto_io = io;
	return ctr_crypto_interface_write_sector(&crypto_io->crypto_io, buffer, buffer_size, sector);
}

uint64_t ctr_nand_crypto_interface_disk_size(void *io)
{
	ctr_nand_crypto_interface *crypto_io = io;
	return crypto_io->crypto_io.base.disk_size(&crypto_io->crypto_io);
}

size_t ctr_nand_crypto_interface_sector_size(void *io)
{
	ctr_nand_crypto_interface *crypto_io = io;
	return crypto_io->crypto_io.base.sector_size(&crypto_io->crypto_io);
}

