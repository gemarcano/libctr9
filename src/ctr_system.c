/*******************************************************************************
 * Copyright (C) 2016, 2017 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */

#include <ctr9/io/ctr_nand_crypto_interface.h>
#include <ctr9/ctr_system.h>
#include <ctr9/aes.h>
#include <ctr9/i2c.h>
#include <ctr9/sha.h>

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdalign.h>

#define PDN_MPCORE_CFG ((volatile uint8_t*)0x10140FFC)
#define PDN_SPI_CNT ((volatile uint8_t*)0x101401C0)

ctr_system_type ctr_get_system_type(void)
{
	return ctr_core_get_system_type();
}

bool ctr_detect_a9lh_entry(void)
{
	//Aurora determined that this register isn't yet set when a9lh launches.
	return ctr_core_detect_a9lh_entry();
}

void ctr_system_poweroff(void)
{
	ctr_core_system_poweroff();
}

void ctr_system_reset(void)
{
	ctr_core_system_reset();
}

void ctr_twl_keyslot_setup(void)
{
	//Only a9lh really needs to bother with this, and it really only needs to happen once, before ITCM gets messed up.
	//FIXME do a sanity check to see if the key has been set up for some reason
	static bool setup = false;
	if (!setup && ctr_detect_a9lh_entry())
	{
		uint32_t* TwlCustId = (uint32_t*) (0x01FFB808);
		uint32_t TwlKeyX[4];
		alignas(32) uint8_t TwlKeyY[16];

		// thanks b1l1s & Normmatt
		// see source from https://gbatemp.net/threads/release-twltool-dsi-downgrading-save-injection-etc-multitool.393488/
		const char* nintendo = "NINTENDO";
		TwlKeyX[0] = (TwlCustId[0] ^ 0xB358A6AF) | 0x80000000;
		TwlKeyX[3] = TwlCustId[1] ^ 0x08C267B7;
		memcpy(TwlKeyX + 1, nintendo, 8);

		// see: https://www.3dbrew.org/wiki/Memory_layout#ARM9_ITCM
		uint32_t TwlKeyYW3 = 0xE1A00005;
		memcpy(TwlKeyY, (uint8_t*) 0x01FFD3C8, 12);
		memcpy(TwlKeyY + 12, &TwlKeyYW3, 4);

		setup_aeskeyX(0x03, (uint8_t*)TwlKeyX);
		setup_aeskeyY(0x03, TwlKeyY);
	}
	setup = true;
}

#include <ctr9/io.h>
#include <ctr9/ctr_firm.h>
#include <stdlib.h>

static inline void volatile_memcpy(void *restrict dest, volatile void *restrict src, size_t size)
{
	char *d = dest;
	volatile char *s = src;
	while (size--)
		*(d++) = *(s++);
}

static inline void load_key_from_sha(uint8_t key)
{
	uint8_t sha[32];
	volatile_memcpy(sha, REG_SHAHASH, sizeof(sha));
	setup_aeskeyX(key, sha);
	setup_aeskeyY(key, sha + 16);
}

static inline void load_key_from_secret_sector(uint8_t index, uint8_t keyslot)
{
	ctr_nand_interface io;
	ctr_nand_interface_initialize(&io);
	ctr_crypto_interface cr;
	uint8_t ctr[16];
	ctr_crypto_interface_initialize(&cr, 0x11, AES_CNT_ECB_ENCRYPT_MODE, CTR_CRYPTO_ENCRYPTED, CRYPTO_ECB, ctr, &io.base);
	uint8_t key[16];
	ctr_io_read(&cr, key, sizeof(key), 0x96 * 512 + (uint64_t)index * 16, 16);

	//get first key in secret sector (FIXME do we always want this to happen??? even for 9.5+?)
	setup_aeskey(keyslot, key);
}

static inline void load_firm_headers(ctr_nand_crypto_interface *firm, ctr_firm_header *firm_header, ctr_arm9bin_header *a9_header)
{
	uint8_t firm_data[0x200];
	ctr_io_read_sector(firm, firm_data, sizeof(firm_data), 0x0B130000/0x200, 1);

	ctr_firm_header_load(firm_header, firm_data);
	uint64_t arm9bin_offset = 0x0B130000 + firm_header->section_headers[2].offset;
	ctr_io_read_sector(firm, firm_data, sizeof(firm_data), arm9bin_offset/0x200, 1);

	ctr_arm9bin_header_load(a9_header, firm_data);
}

static inline void load_key15(const ctr_arm9bin_header *a9_header)
{
	uint8_t key15x_enc[16];
	uint8_t key15x[16];
	uint8_t ctr[16];
	memcpy(key15x_enc, a9_header->enc_keyx, sizeof(key15x_enc));
	ctr_memory_interface mem_io;
	ctr_memory_interface_initialize(&mem_io, key15x_enc, sizeof(key15x));
	ctr_crypto_interface ecb_io;
	ctr_crypto_interface_initialize(&ecb_io, 0x11, AES_CNT_ECB_ENCRYPT_MODE, CTR_CRYPTO_ENCRYPTED, CRYPTO_ECB, ctr, &mem_io.base);

	ctr_io_read(&ecb_io, key15x, sizeof(key15x), 0, 16);

	setup_aeskeyX(0x15, key15x);
	setup_aeskeyY(0x15, a9_header->keyy);
}

static uint32_t ipow(uint32_t n, uint32_t p)
{
	uint32_t result = 1;
	for (uint32_t i = 0; i < p; ++i)
	{
		result *= n;
	}

	return result;
}

static uint32_t hash(const uint8_t *data)
{
	uint32_t hash = 0;
	for (size_t i = 0; i < 16; ++i)
	{
		hash += data[i] * ipow(257, 15-i);
	}
	return hash;
}

static uint32_t slide_forward(uint32_t hash, const uint8_t *data)
{
	hash -= data[0] * ipow(257, 15);
	hash *= 257;
	hash += data[16];
	return hash;
}

static size_t hash_search(const void *data, size_t data_size, uint32_t target_hash, const uint8_t *target_sha256)
{
	bool found;
	const uint8_t *d = data;
	uint32_t current = hash((const uint8_t *)data);
	size_t i;

	found = current == target_hash;
	for (i = 0; !found && i < (data_size-16); ++i,++d)
	{
		current = slide_forward(current, d);
		found = target_hash == current;
		if (found)
		{
			alignas(4) uint8_t shasum[32];
			sha_init(SHA256_MODE);
			sha_update(d + 1, 16);
			sha_get(shasum);
			found = memcmp(shasum, target_sha256, 32) == 0;
		}
	}

	return i;
}

void ctr_n3ds_ctrnand_keyslot_setup(void)
{
	char data2[16];
	ctr_nand_interface io;
	ctr_nand_crypto_interface io2;
	ctr_nand_interface_initialize(&io);
	ctr_nand_crypto_interface_initialize(&io2, 0x03, NAND_TWL, &io.base);

	static const char data[] = { 0x03 ,0xc3 ,0x1c ,0x0a ,0xcd ,0xc7 ,0x55 ,0x66 ,0x5d ,0xe1 ,0x57 ,0xe8 ,0x0c ,0x13 ,0x2c ,0x9e };

	for (size_t i = 0; i < 2; ++i)
	{
		ctr_io_read(&io, data2+i*8, sizeof(data2)-i*8, 0x100, 4);
		ctr_io_read(&io2, data2+i*8+4, sizeof(data2)-i*8-4, 0x00012E00+3, 4);
	}

	ctr_nand_crypto_interface_destroy(&io2);
	ctr_nand_interface_destroy(&io);

	for (size_t i = 0; i < 16; ++i)
	{
		data2[i] ^= data[i];
	}

	setup_aeskeyY(0x05, data2);
}

