/*******************************************************************************
 * Copyright (C) 2016, 2017 Gabriel Marcano
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
#include <ctr9/io/ctr_io_implementation.hpp>
#include <ctr9/ctr_system.h>

#include <new>
#include <utility>

static inline void process_aes_ctr_blocks(void *buffer, void *ctr, uint64_t blocks, uint32_t mode)
{
	ctr_decrypt(buffer, buffer, blocks, mode, (uint8_t*)ctr);
}

static inline void check_and_do_n3ds_init(void)
{
	static bool setup = false;
	if (!setup && ctr_detect_a9lh_entry() && ctr_get_system_type() == SYSTEM_N3DS)
	{
		ctr_n3ds_ctrnand_keyslot_setup();
	}
}

static inline void check_and_do_twl_init(void)
{
	static bool setup = false;
	if (!setup && ctr_detect_a9lh_entry())
	{
		ctr_twl_keyslot_setup();
	}
}

ctr_nand_crypto_interface *ctr_nand_crypto_interface_initialize(uint8_t keySlot, ctr_nand_crypto_type crypto_type, void *lower_io)
{
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
			check_and_do_twl_init();
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
			return NULL; //Unknown type
	}

	ctr_crypto_interface_initialize(keySlot, mode, CTR_CRYPTO_ENCRYPTED, CRYPTO_CTR, (uint8_t*)ctr, lower_io);
	ctr9::io_interface &lower = *reinterpret_cast<ctr9::io_interface*>(lower_io);
	return reinterpret_cast<ctr_nand_crypto_interface*>(new (std::nothrow) ctr9::nand_crypto_generic_interface(keySlot, crypto_type, std::forward<decltype(lower)>(lower)));//FIXME
}

void ctr_nand_crypto_interface_destroy(ctr_nand_crypto_interface *crypto_io)
{
	ctr9::nand_crypto_generic_interface *io = reinterpret_cast<ctr9::nand_crypto_generic_interface*>(crypto_io);
	//ctr_crypto_interface_destroy(&crypto_io->crypto_io);
	delete io;
}

namespace ctr9
{
	template<class IO>
	int nand_crypto_interface<IO>::read(void *buffer, size_t buffer_size, uint64_t position, size_t count)
	{
		return ctr_io_implementation_read(*this, buffer, buffer_size, position, count);
	}

	template<class IO>
	int nand_crypto_interface<IO>::write(const void *buffer, size_t buffer_size, uint64_t position)
	{
		return ctr_io_implementation_write(*this, buffer, buffer_size, position);
	}

	template<class IO>
	int nand_crypto_interface<IO>::read_sector(void *buffer, size_t buffer_size, size_t sector, size_t count)
	{
		//ctr_nand_crypto_interface *crypto_io = io;
		//return ctr_crypto_interface_read_sector(&crypto_io->crypto_io, buffer, buffer_size, sector, count);
		return -1;
	}

	template<class IO>
	int nand_crypto_interface<IO>::write_sector(const void *buffer, size_t buffer_size, size_t sector)
	{
		//ctr_nand_crypto_interface *crypto_io = io;
		//return ctr_crypto_interface_write_sector(&crypto_io->crypto_io, buffer, buffer_size, sector);
		return -1;
	}

	template<class IO>
	uint64_t nand_crypto_interface<IO>::disk_size() const
	{
		//ctr_nand_crypto_interface *crypto_io = io;
		//return crypto_io->crypto_io.base.disk_size(&crypto_io->crypto_io);
		return 0;
	}

	template<class IO>
	constexpr size_t nand_crypto_interface<IO>::sector_size() const
	{
		//ctr_nand_crypto_interface *crypto_io = io;
		//return crypto_io->crypto_io.base.sector_size(&crypto_io->crypto_io);
		return 0;
	}
}

