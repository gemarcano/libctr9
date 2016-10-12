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

void ctr_n3ds_ctrnand_keyslot_setup(void)
{
/*
	//FIXME do a sanity check to see if the key has been set up for some reason
	static bool setup = false;
	if (!setup && ctr_detect_a9lh_entry())
	{
		//Get OTP sha hash ASAP
		uint8_t otp_sha[32];
		memcpy(otp_sha, (void*)REG_SHAHASH, sizeof(otp_sha));

		//setup 0x11 to read secret sector
		setup_aeskeyX(0x11, otp_sha);
		setup_aeskeyY(0x11, otp_sha + 16);
		use_aeskey(0x11);

		//decrypt secret sector
		ctr_nand_interface io;
		ctr_nand_interface_initialize(&io);
		ctr_crypto_interface cr;
		uint8_t ctr[16];
		ctr_crypto_interface_initialize(&cr, 0x11, AES_CNT_ECB_ENCRYPT_MODE, CTR_CRYPTO_ENCRYPTED, CRYPTO_ECB, ctr, &io.base);
		uint8_t sector[512];
		ctr_io_read_sector(&cr, sector, sizeof(sector), 0x96, 1);
		for (int i = 0; i < 16; ++i)
		{
			tfp_printf("%02X", sector[i]);
		}
		tfp_printf("\n");

		for (int i = 16; i < 32; ++i)
		{
			tfp_printf("%02X", sector[i]);
		}
		tfp_printf("\n");

		ctr_io_read(&cr, sector, sizeof(sector), 0x96 * 512, 16);
		for (int i = 0; i < 16; ++i)
		{
			tfp_printf("%02X", sector[i]);
		}
		tfp_printf("\n");

		//get first key in secret sector (FIXME do we always want this to happen??? even for 9.5+?)
		setup_aeskey(0x11, &sector[0]);

		//Decrypt FIRM and extract headers
		ctr_nand_crypto_interface firm_io;
		ctr_nand_crypto_interface_initialize(&firm_io, 0x06, NAND_CTR, &io);

		uint8_t firm_data[0x200];
		ctr_io_read_sector(&firm_io, firm_data, sizeof(firm_data), 0x0B130000/0x200, 1);
		for (int i = 0; i < 64; ++i)
		{
			tfp_printf("%02X", firm_data[i]);
		}
		tfp_printf("\n");

		ctr_firm_header firm_header;
		ctr_firm_header_load(&firm_header, firm_data);

		tfp_printf("A9 entry: %08X\n", firm_header.arm9_entry);
		tfp_printf("ARM9 section: offset: %08X\n", firm_header.section_headers[2].offset);

		ctr_io_read_sector(&firm_io, firm_data, sizeof(firm_data), (0x0B130000 + firm_header.section_headers[2].offset)/0x200, 1);
		ctr_arm9bin_header a9_header;
		ctr_arm9bin_header_load(&a9_header, firm_data);

		tfp_printf("ctr: ");
		for (int i = 0; i < 16; ++i)
			tfp_printf("%02X", a9_header.ctr[i]);
		tfp_printf("\n");

		//Prepare key15
		uint8_t key15x[16];
		memcpy(key15x, a9_header.enc_keyx, 16);
		ctr_memory_interface mem_io;
		ctr_memory_interface_initialize(&mem_io, key15x, sizeof(key15x));
		ctr_crypto_interface ecb_io;
		ctr_crypto_interface_initialize(&ecb_io, 0x11, AES_CNT_ECB_ENCRYPT_MODE, CTR_CRYPTO_ENCRYPTED, CRYPTO_ECB, ctr, &mem_io.base);

		uint8_t tmp2[16];
		ctr_io_read(&ecb_io, tmp2, sizeof(tmp2), 0, 16);

		for (int i = 0; i < 16; ++i)
		{
			tfp_printf("%02X", tmp2[i]);
		}
		tfp_printf("\n");
		setup_aeskeyX(0x15, tmp2);
		setup_aeskeyY(0x15, a9_header.keyy);

		for (int i = 0; i < 16; ++i)
		{
			tfp_printf("%02X", a9_header.keyy[i]);
		}
		tfp_printf("\n");

		//Extract the size of the arm9bin
		size_t arm9_size = 0;
		for (size_t i = 0; i < sizeof(a9_header.ascii_size) && a9_header.ascii_size[i]; ++i)
		{
			tfp_printf("%c", a9_header.ascii_size[i]);
			arm9_size = (arm9_size * 10) + ((size_t)a9_header.ascii_size[i] - '0');
		}
		tfp_printf("\nSize:%d\n", arm9_size);

		//Now, actually read the arm9bin
		uint8_t arm9bin[arm9_size];

		tfp_printf("offset: %d\n", firm_header.section_headers[2].offset);
		ctr_io_read(&firm_io, arm9bin, sizeof(arm9bin), 0x0B130000 + firm_header.section_headers[2].offset + 0x800, arm9_size);

		ctr_sd_interface sd;
		ctr_fatfs_initialize(NULL, NULL, NULL, &sd);

		FATFS fs3;
		FIL dump;
		f_mount(&fs3, "SD:", 0);
		f_open(&dump, "SD:/arm9.raw.dump", FA_WRITE | FA_READ  | FA_CREATE_ALWAYS);
		unsigned int br;
		f_write(&dump, arm9bin, arm9_size, &br);
		f_close(&dump);

		//If we want to use firm_io as the underlying thing, we need to subtract the position/16 from the ctr, since the internal advance function will add that offset back
		uint8_t arm9bin2[arm9_size];
		memcpy(arm9bin2, arm9bin, arm9_size);
		ctr_memory_interface_initialize(&mem_io, arm9bin2, arm9_size);
		ctr_crypto_interface arm9_io;
		ctr_crypto_interface_initialize(&arm9_io, 0x15, AES_CNT_CTRNAND_MODE, CTR_CRYPTO_ENCRYPTED, CRYPTO_CTR, a9_header.ctr, &mem_io.base);
		ctr_io_read(&arm9_io, arm9bin, sizeof(arm9bin), 0, arm9_size);

		f_open(&dump, "SD:/arm9.dump", FA_WRITE | FA_READ  | FA_CREATE_ALWAYS);
		f_write(&dump, arm9bin, arm9_size, &br);
		f_close(&dump);

		input_wait();
	}
	setup = true;*/
}

