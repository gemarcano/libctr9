/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */

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
	//This is seemingly not confirmed on 3dbrew, but it seems PDN_MPCORE_CFG's
	//second and third bit are only set on the N3DS, while the first bit is
	//set for all systems. Use that to detect the type of system.
	return 0x07 == *PDN_MPCORE_CFG ? SYSTEM_N3DS : SYSTEM_O3DS;
}

bool ctr_detect_a9lh_entry(void)
{
	//Aurora determined that this register isn't yet set when a9lh launches.
	return *PDN_SPI_CNT == 0;
}

void ctr_system_poweroff(void)
{
	i2cWriteRegister(I2C_DEV_MCU, 0x20, 1);
	while (true);
}

void ctr_system_reset(void)
{
	i2cWriteRegister(I2C_DEV_MCU, 0x20, 1 << 2);
	while (true);
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
		use_aeskey(0x03);
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
	uint8_t key15y[16];
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
	bool found = false;
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
	//FIXME do a sanity check to see if the key has been set up for some reason
/*	static bool setup = false;
	if (!setup && ctr_detect_a9lh_entry())
	{
		load_key_from_sha(0x11);

		//decrypt secret sector
		//get first key in secret sector (FIXME do we always want this to happen??? even for 9.5+?)
		load_key_from_secret_sector(0, 0x11);

		//Decrypt FIRM and extract headers
		uint8_t ctr[16];
		ctr_nand_interface io;
		ctr_nand_interface_initialize(&io);
		ctr_nand_crypto_interface firm_io;
		ctr_nand_crypto_interface_initialize(&firm_io, 0x06, NAND_CTR, &io.base);

		ctr_firm_header firm_header;
		ctr_arm9bin_header a9_header;
		load_firm_headers(&firm_io, &firm_header, &a9_header);

		//Prepare key15
		load_key15(&a9_header);

		//Extract the size of the arm9bin
		char ascii_size[sizeof(a9_header.ascii_size)+1];
		ascii_size[sizeof(a9_header.ascii_size)] = '\0'; //Make sure this thing is null terminated
		memcpy(ascii_size, a9_header.ascii_size, sizeof(a9_header.ascii_size));
		size_t arm9_size = (size_t)strtol(ascii_size, NULL, 10);


		//If we want to use firm_io as the underlying thing, we need to subtract the position/16 from the ctr, since the internal advance function will add that offset back
		memcpy(ctr, a9_header.ctr, 16);

		subtract_ctr(ctr, (0x0B130000 + firm_header.section_headers[2].offset + 0x800)/ AES_BLOCK_SIZE);
		ctr_crypto_interface arm9_io;
		ctr_crypto_interface_initialize(&arm9_io, 0x15, AES_CNT_CTRNAND_MODE, CTR_CRYPTO_ENCRYPTED, CRYPTO_CTR, ctr, &firm_io.base);

		//Now, actually read the arm9bin
		uint8_t arm9bin[arm9_size];
		ctr_io_read(&arm9_io, arm9bin, sizeof(arm9bin), 0x0B130000 + firm_header.section_headers[2].offset + 0x800, arm9_size);

		const uint32_t target_hash = 106069265;
		const uint8_t target_sha[32] = { 0x98, 0x24, 0x27, 0x14, 0x22, 0xb0, 0x6b, 0xf2, 0x10, 0x96, 0x9c, 0x36, 0x42, 0x53, 0x7c, 0x86, 0x62, 0x22, 0x5c, 0xfd, 0x6f, 0xae, 0x9b, 0x0a, 0x85, 0xa5, 0xce, 0x21, 0xaa, 0xb6, 0xc8, 0x4d };

		size_t index = hash_search(arm9bin, sizeof(arm9bin), target_hash, target_sha);
		tfp_printf("index: %d\n", index);

		FATFS fs3;
		FIL dump;
		f_mount(&fs3, "SD:", 0);
		unsigned int br;

		f_open(&dump, "SD:/arm9.dump", FA_WRITE | FA_READ  | FA_CREATE_ALWAYS);
		f_write(&dump, arm9bin, arm9_size, &br);
		f_close(&dump);

		input_wait();
	}
	setup = true;*/
}

