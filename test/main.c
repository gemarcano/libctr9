#include <ctr9/io.h>

#include <string.h>
#include <stdbool.h>
#include <ctr/console.h>
#include <ctr/printf.h>
#include <ctr/draw.h>
#include <ctr/headers.h>
#include <ctr/hid.h>

#include <ctr9/i2c.h>
#include <ctr9/io/fatfs/ff.h>
#include <ctr9/io/fatfs/diskio.h>

#define SD 3

#include "test.h"

typedef struct
{
	char *buffer;
	size_t buffer_size;
	ctr_nand_interface nand_io;

} nand_test_data;

typedef struct
{
	char *buffer;
	size_t buffer_size;
	ctr_nand_crypto_interface io;
	ctr_nand_interface *lower_io;
} nand_crypto_test_data;

typedef struct
{
	char *buffer;
	size_t buffer_size;
	ctr_sd_interface io;
} sd_test_data;

static bool nand_test1(void *ctx)
{
	nand_test_data *data = ctx;

	int res = ctr_nand_interface_initialize(&data->nand_io);

	return !res;
}

static bool nand_test2(void *ctx)
{
	nand_test_data *data = ctx;
	char *buffer = data->buffer;
	size_t buffer_size = data->buffer_size;

	int res = ctr_io_read(&data->nand_io, buffer, buffer_size, 0x100, 4);
	return !res && memcmp(buffer, "NCSD", 4) == 0;
}

static bool nand_test3(void *ctx)
{
	nand_test_data *data = ctx;
	char *buffer = data->buffer;
	size_t buffer_size = data->buffer_size;

	int res = ctr_io_read_sector(&data->nand_io, buffer, buffer_size, 0, 1);
	return !res && memcmp(buffer + 0x100, "NCSD", 4) == 0;
}

static bool nand_test4(void *ctx)
{
	nand_test_data *data = ctx;
	char *buffer = data->buffer;
	size_t buffer_size = data->buffer_size;

	int res = ctr_io_read(&data->nand_io, buffer+0x200, buffer_size-0x200, 0, 0x200);
	res |= ctr_io_read_sector(&data->nand_io, buffer, 512, 0, 1);

	return !res && memcmp(buffer + 0x200, buffer, 0x100) == 0;
}

static bool nand_test5(void *ctx)
{
	nand_test_data *data = ctx;
	char *buffer = data->buffer;
	size_t buffer_size = data->buffer_size;

	int res = ctr_io_read(&data->nand_io, buffer, buffer_size, 0, 0);
	res |= ctr_io_read_sector(&data->nand_io, buffer, 512, 0, 0);

	return !res;
}

static bool nand_test6(void *ctx)
{
	nand_test_data *data = ctx;

	size_t size = ctr_io_disk_size(&data->nand_io);

	return size == 0x3AF00000 || size == 0x3BA00000 || size == 0x4D800000 || size == 0x76000000;
}

static bool nand_test7(void *ctx)
{
	nand_test_data *data = ctx;

	size_t sector_size = ctr_io_sector_size(&data->nand_io);

	return sector_size == 512u;
}

static bool nand_test8(void *ctx)
{
	nand_test_data *data = ctx;
	char *buffer = data->buffer;
	size_t buffer_size = data->buffer_size;

	uint32_t loc = 0x0B130000; //FIRM0
	int res = ctr_io_read(&data->nand_io, buffer, buffer_size/2, loc, buffer_size/2);
	res |= ctr_io_read_sector(&data->nand_io, buffer+buffer_size/2, buffer_size/2, loc/512, buffer_size/2/512);

	bool test1 = memcmp(buffer + buffer_size/2, buffer, buffer_size/2) == 0;

	res |= ctr_io_read(&data->nand_io, buffer, buffer_size/2, loc + 11, buffer_size/2-12);

	bool test2 = memcmp(buffer + buffer_size/2 + 11, buffer, buffer_size/2-12) == 0;

	return !res && test1 && test2;

}

static bool nand_test9(void *ctx)
{
	nand_test_data *data = ctx;
	char *buffer = data->buffer;
	size_t buffer_size = data->buffer_size;
	const char* text = "TEST";
	int res = ctr_io_write(&data->nand_io, text, 5, 0x5A000*0x200);
	ctr_io_read(&data->nand_io, buffer, buffer_size, 0x5A000*0x200, 5);

	bool test1 = !memcmp(text, buffer, 5);

	res |= ctr_io_write(&data->nand_io, text, 5, 0x5A000*0x200-3);
	ctr_io_read(&data->nand_io, buffer+5, buffer_size-5, 0x5A000*0x200-3, 5);

	bool test2 = !memcmp(text, buffer+5, 5);

	return !res && test1 && test2;
}

static bool nand_test10(void *ctx)
{
	nand_test_data *data = ctx;
	char *buffer = data->buffer;
	size_t buffer_size = data->buffer_size;
	char text[2048];

	for(size_t i = 0; i < 2048; ++i)
	{
		text[i] = (char)i;
	}

	uint32_t loc = 0x5A000*0x200-3;

	int res = ctr_io_write(&data->nand_io, text, 2048, loc);
	ctr_io_read(&data->nand_io, buffer, buffer_size, loc, 2048);

	return !res && !memcmp(text, buffer, 2048);
}

static bool nand_test11(void *ctx)
{
	nand_test_data *data = ctx;
	char *buffer = data->buffer;
	size_t buffer_size = data->buffer_size;
	char text[2048];

	for(size_t i = 0; i < 2048; ++i)
	{
		text[i] = (char)i;
	}

	uint32_t loc = 0x5A000-3;

	int res = ctr_io_write_sector(&data->nand_io, text, 2048, loc);
	ctr_io_read_sector(&data->nand_io, buffer, buffer_size, loc, 2048);

	return !res && !memcmp(text, buffer, 2048);
}

static bool nand_ctrnand_test1(void *ctx)
{
	nand_crypto_test_data *data = ctx;

	int res = ctr_nand_crypto_interface_initialize(&data->io, 0x04, NAND_CTR, &data->lower_io->base);

	return !res;
}

static bool nand_ctrnand_test2(void *ctx)
{
	nand_crypto_test_data *data = ctx;
	char *buffer = data->buffer;
	size_t buffer_size = data->buffer_size;

	uint32_t loc = 0x0B95CA00/ 0x200; //CTRNAND
	int res = ctr_io_read_sector(&data->io, buffer, buffer_size, loc, 1);

	return strncmp("CTR", buffer+3, 3) == 0;
}

static bool nand_ctrnand_test3(void *ctx)
{
	nand_crypto_test_data *data = ctx;
	char *buffer = data->buffer;
	size_t buffer_size = data->buffer_size;

	uint32_t loc = 0x0B95CA00; //CTRNAND
	int res = ctr_io_read(&data->io, buffer, buffer_size, loc + 3, 3);

	return strncmp("CTR", buffer, 3) == 0;
}

static bool nand_ctrnand_test4(void *ctx)
{
	nand_crypto_test_data *data = ctx;
	char *buffer = data->buffer;
	size_t buffer_size = data->buffer_size;

	uint32_t loc = 0x0B95CA00; //CTRNAND
	int res = ctr_io_read(&data->io, buffer, buffer_size/2, loc, buffer_size/2);
	res |= ctr_io_read_sector(&data->io, buffer+buffer_size/2, buffer_size/2, loc /0x200, buffer_size/2/512);

	bool test1 = memcmp(buffer, buffer + buffer_size/2, buffer_size/2) == 0;

	res |= ctr_io_read(&data->io, buffer, buffer_size/2, loc+3, buffer_size/2-4);

	bool test2 = memcmp(buffer, buffer + buffer_size/2 + 3, buffer_size/2 - 4) == 0;

	return !res && test1 && test2;
}

static bool nand_ctrnand_test5(void *ctx)
{
	nand_crypto_test_data *data = ctx;
	char *buffer = data->buffer;
	size_t buffer_size = data->buffer_size;

	uint32_t loc = 0x5A000*0x200;
	char text[0x200*8] = "DATA";
	size_t text_size = 5;

	int res = ctr_io_write(&data->io, text, text_size, loc);
	res |= ctr_io_read(&data->io, buffer, buffer_size, loc, text_size);

	bool test1 = !memcmp(buffer, text, text_size);

	for (size_t i = 0; i < sizeof(text); ++i)
	{
		text[i] = i;
	}

	loc -= 0x200*4 + 3;
	text_size = sizeof(text);
	res |= ctr_io_write(&data->io, text, text_size, loc);
	res |= ctr_io_read(&data->io, buffer, buffer_size, loc, text_size);

	bool test2 = !memcmp(buffer, text, text_size);

	return !res && test1 && test2;
}

static bool nand_ctrnand_test6(void *ctx)
{
	nand_crypto_test_data *data = ctx;
	char *buffer = data->buffer;
	size_t buffer_size = data->buffer_size;

	uint32_t loc = 0x5A000;
	char text[0x200*8] = { 'D', 'A', 'T', 'A', 0 };
	size_t text_size = 0x200;

	int res = ctr_io_write_sector(&data->io, text, text_size, loc);
	res |= ctr_io_read_sector(&data->io, buffer, buffer_size, loc, 1);

	bool test1 = !memcmp(buffer, text, text_size);

	for (size_t i = 0; i < sizeof(text); ++i)
	{
		text[i] = i;
	}

	loc -= 0x200*4;
	text_size = sizeof(text);
	res |= ctr_io_write_sector(&data->io, text, text_size, loc);
	res |= ctr_io_read_sector(&data->io, buffer, buffer_size, loc, text_size/0x200);

	bool test2 = !memcmp(buffer, text, text_size);

	return !res && test1 && test2;
}

static bool sd_test1(void *ctx)
{
	sd_test_data *data = ctx;

	int res = ctr_sd_interface_initialize(&data->io);

	return !res;
}

static bool sd_test2(void *ctx)
{
	sd_test_data *data = ctx;
	char *buffer = data->buffer;
	size_t buffer_size = data->buffer_size;

	uint32_t loc = 0;
	int res = ctr_io_read(&data->io, buffer, buffer_size/2, loc, buffer_size/2);
	res |= ctr_io_read_sector(&data->io, buffer+buffer_size/2, buffer_size/2, loc /0x200, buffer_size/2/512);
	bool test1 = memcmp(buffer, buffer + buffer_size/2, buffer_size/2) == 0;

	res |= ctr_io_read(&data->io, buffer, buffer_size/2, loc+3, buffer_size/2-4);

	bool test2 = memcmp(buffer, buffer + buffer_size/2 + 3, buffer_size/2 - 4) == 0;

	return !res && test1 && test2;
}

static bool sd_test3(void *ctx)
{
	FATFS fs = { 0 };
	FIL test_file = { 0 };

	sd_test_data *data = ctx;
	ctr_setup_disk_parameters params = {&data->io, 0, ctr_io_disk_size(&data->io)};
	disk_ioctl(3, CTR_SETUP_DISK, &params);

	int res2 = 0;
	if ((res2 = f_mount(&fs, "SD:", 1)) == FR_OK &&
	(res2 = f_open(&test_file, "SD:/otp.bin", FA_READ)) == FR_OK)
	{
		size_t size = f_size(&test_file);
		f_close(&test_file);
		return size == 256;
	}
	return false;
}

#include <ctr9/aes.h>
#include <stdalign.h>

static bool twl_test1(void *ctx)
{
	
	nand_crypto_test_data *data = ctx;
	char *buffer = data->buffer;
	size_t buffer_size = data->buffer_size;


	if ((*(volatile uint32_t*) 0x101401C0) == 0) { // only for a9lh
		uint32_t* TwlCustId = (uint32_t*) (0x01FFB808);
		alignas(32) uint8_t TwlKeyX[16];
		alignas(32) uint8_t TwlKeyY[16];
		
		// thanks b1l1s & Normmatt
		// see source from https://gbatemp.net/threads/release-twltool-dsi-downgrading-save-injection-etc-multitool.393488/
		const char* nintendo = "NINTENDO";
		uint32_t* TwlKeyXW = (uint32_t*) TwlKeyX;
		TwlKeyXW[0] = (TwlCustId[0] ^ 0xB358A6AF) | 0x80000000;
		TwlKeyXW[3] = TwlCustId[1] ^ 0x08C267B7;
		memcpy(TwlKeyX + 4, nintendo, 8);
		
		// see: https://www.3dbrew.org/wiki/Memory_layout#ARM9_ITCM
		uint32_t TwlKeyYW3 = 0xE1A00005;
		memcpy(TwlKeyY, (uint8_t*) 0x01FFD3C8, 12);
		memcpy(TwlKeyY + 12, &TwlKeyYW3, 4);
		
		setup_aeskeyX(0x03, TwlKeyX);
		setup_aeskeyY(0x03, TwlKeyY);
		use_aeskey(0x03);
	}

	int res = ctr_nand_crypto_interface_initialize(&data->io, 0x03, NAND_TWL, &data->lower_io->base);

	return !res;
}

static bool twl_test2(void *ctx)
{
	FATFS fs = { 0 };
	FIL test_file = { 0 };
	
	nand_crypto_test_data *data = ctx;
	char *buffer = data->buffer;
	size_t buffer_size = data->buffer_size;

	ctr_setup_disk_parameters params = { &data->io, 0x00012E00/0x200, 0x08FB5200/0x200 }; 
	disk_ioctl(1, CTR_SETUP_DISK, &params);
	bool test1 = false, test2 = false;
	int res2 = 0;
	if ((res2 = f_mount(&fs, "TWL:", 1)) == FR_OK &&
	(res2 |= f_open(&test_file, "TWL:/sys/TWLFontTable.dat", FA_READ)) == FR_OK)
	{
		size_t size = f_size(&test_file);
		f_close(&test_file);
		test1 = size == 863296;
		ctr_fatfs_interface io;
		f_open(&test_file, "TWL:/shared2/0000", FA_READ | FA_WRITE);
		ctr_fatfs_interface_initialize(&io, &test_file);

		ctr_io_read(&io, buffer, buffer_size, 0, 513);
		for (size_t i = 0; i < 513; ++i)
		{
			if (i && !(i%8)) printf("\n");
			if (i && !(i%64)) input_wait();

			printf("%02X ", ((char*)buffer)[i]);
		}
	}

	params = (ctr_setup_disk_parameters){&data->io, 0x09011A00/0x200, 0x020B6600/0x200};
	disk_ioctl(2, CTR_SETUP_DISK, &params);

	if ((res2 |= f_mount(&fs, "TWLP:", 1)) == FR_OK &&
	(res2 |= f_open(&test_file, "TWLP:/photo/private/ds/app/484E494A/pit.bin", FA_READ)) == FR_OK)
	{
		size_t size = f_size(&test_file);
		f_close(&test_file);
		test2 = size == 8032;
	}
	return !res2 && test1 && test2;
}

static bool twl_test3(void *ctx)
{
	FATFS fs = { 0 }, fs2 = { 0 };
	FIL test_file = { 0 };
	
	nand_crypto_test_data *data = ctx;
	char *buffer = data->buffer;
	size_t buffer_size = data->buffer_size;

	ctr_io_read_sector(&data->io, buffer, buffer_size, 0x00012E00/0x200 + 1, 1);

	ctr_setup_disk_parameters params = { &data->io, 0, ctr_io_disk_size(&data->io)/0x200 };
	disk_ioctl(4, CTR_SETUP_DISK, &params);

	bool test1 = false;
	int res2 = 0;
	if ((res2 = f_mount(&fs, "TWL:", 1)) == FR_OK &&
	(res2 |= f_open(&test_file, "TWL:/shared2/0000", FA_READ)) == FR_OK)
	{
		ctr_fatfs_interface io;
		ctr_fatfs_interface_initialize(&io, &test_file);

		ctr_io_read(&io, buffer, buffer_size, 0, 513);
		for (size_t i = 0; i < 513; ++i)
		{
			if (i && !(i%8)) printf("\n");
			if (i && !(i%64)) input_wait();

			printf("%02X ", ((char*)buffer)[i]);
		}
		test1 = FR_OK == f_mount(&fs2, "EMU1:", 1);
	}

	return !res2 && test1;
}

int main()
{
	draw_init((draw_s*)0x23FFFE00);
	console_init(0xFFFFFF, 0);
	draw_clear_screen(SCREEN_TOP, 0xAAAAAA);
	printf("UNIT TESTING\n");

	char buffer[0x1000] = {0};
	nand_test_data nand_ctx = {buffer, sizeof(buffer), {{0}} };
	nand_crypto_test_data nand_crypto_ctx = {buffer, sizeof(buffer), {{0}}, &nand_ctx.nand_io};
	nand_crypto_test_data twl_crypto_ctx = {buffer, sizeof(buffer), {{0}}, &nand_ctx.nand_io};
	sd_test_data sd_ctx = {buffer, sizeof(buffer), {{0}}};

	ctr_unit_test nand_tests_f[11];
	ctr_unit_tests nand_tests;
	ctr_unit_tests_initialize(&nand_tests, "NAND tests", nand_tests_f, 11);
	ctr_unit_tests_add_test(&nand_tests, (ctr_unit_test){ "ctr_nand_initialize", &nand_ctx, nand_test1 } );
	ctr_unit_tests_add_test(&nand_tests, (ctr_unit_test){ "ctr_nand_read", &nand_ctx, nand_test2 } );
	ctr_unit_tests_add_test(&nand_tests, (ctr_unit_test){ "ctr_nand_read_sector", &nand_ctx, nand_test3 } );
	ctr_unit_tests_add_test(&nand_tests, (ctr_unit_test){ "ctr_nand compare", &nand_ctx, nand_test4 } );
	ctr_unit_tests_add_test(&nand_tests, (ctr_unit_test){ "ctr_nand_read test zero", &nand_ctx, nand_test5 } );
	ctr_unit_tests_add_test(&nand_tests, (ctr_unit_test){ "ctr_nand_disk_size", &nand_ctx, nand_test6 } );
	ctr_unit_tests_add_test(&nand_tests, (ctr_unit_test){ "ctr_nand_sector_size", &nand_ctx, nand_test7 } );
	ctr_unit_tests_add_test(&nand_tests, (ctr_unit_test){ "ctr_nand_read larger than a sector", &nand_ctx, nand_test8 } );
	ctr_unit_tests_add_test(&nand_tests, (ctr_unit_test){ "ctr_nand_write", &nand_ctx, nand_test9 } );
	ctr_unit_tests_add_test(&nand_tests, (ctr_unit_test){ "ctr_nand_write 2048 across sectors", &nand_ctx, nand_test10 } );
	ctr_unit_tests_add_test(&nand_tests, (ctr_unit_test){ "ctr_nand_write 2048 across sectors", &nand_ctx, nand_test11 } );

	ctr_unit_test nand_crypto_tests_f[6];
	ctr_unit_tests nand_crypto_tests;
	ctr_unit_tests_initialize(&nand_crypto_tests, "NAND crypto tests", nand_crypto_tests_f, 6);
	ctr_unit_tests_add_test(&nand_crypto_tests, (ctr_unit_test){ "ctr_nand_crypto_initialize", &nand_crypto_ctx, nand_ctrnand_test1 });
	ctr_unit_tests_add_test(&nand_crypto_tests, (ctr_unit_test){ "ctr_nand_crypto_read", &nand_crypto_ctx, nand_ctrnand_test3 });
	ctr_unit_tests_add_test(&nand_crypto_tests, (ctr_unit_test){ "ctr_nand_crypto_read_sector", &nand_crypto_ctx, nand_ctrnand_test2 });
	ctr_unit_tests_add_test(&nand_crypto_tests, (ctr_unit_test){ "ctr_nand_crypto_read compare", &nand_crypto_ctx, nand_ctrnand_test4 });
	ctr_unit_tests_add_test(&nand_crypto_tests, (ctr_unit_test){ "ctr_nand_crypto_write", &nand_crypto_ctx, nand_ctrnand_test5 });
	ctr_unit_tests_add_test(&nand_crypto_tests, (ctr_unit_test){ "ctr_nand_crypto_write_sector", &nand_crypto_ctx, nand_ctrnand_test6 });

	ctr_unit_test sd_tests_f[3];
	ctr_unit_tests sd_tests;
	ctr_unit_tests_initialize(&sd_tests, "SD tests", sd_tests_f, 3);
	ctr_unit_tests_add_test(&sd_tests, (ctr_unit_test){ "ctr_sd_initialize", &sd_ctx, sd_test1});
	ctr_unit_tests_add_test(&sd_tests, (ctr_unit_test){ "ctr_sd_read* compare", &sd_ctx, sd_test2});
	ctr_unit_tests_add_test(&sd_tests, (ctr_unit_test){ "ctr_sd_interface FATFS read", &sd_ctx, sd_test3});

	ctr_unit_test twl_crypto_tests_f[3];
	ctr_unit_tests twl_crypto_tests;
	ctr_unit_tests_initialize(&twl_crypto_tests, "TWL tests", twl_crypto_tests_f, 3);
	ctr_unit_tests_add_test(&twl_crypto_tests, (ctr_unit_test){ "twl_crypto_initialize", &twl_crypto_ctx, twl_test1 });
	ctr_unit_tests_add_test(&twl_crypto_tests, (ctr_unit_test){ "twl mount and read", &twl_crypto_ctx, twl_test2 });
	ctr_unit_tests_add_test(&twl_crypto_tests, (ctr_unit_test){ "twl mount nested", &twl_crypto_ctx, twl_test3 });

	int res = ctr_execute_unit_tests(&nand_tests);
	res |= ctr_execute_unit_tests(&nand_crypto_tests);
	res |= ctr_execute_unit_tests(&sd_tests);
	res |= ctr_execute_unit_tests(&twl_crypto_tests);

	FATFS fs = { 0 };
	FIL test_file = { 0 };

	ctr_setup_disk_parameters params = {&nand_crypto_ctx.io, 0x0B930000/0x200, 0x2F5D0000/0x200};
	disk_ioctl(0, CTR_SETUP_DISK, &params);

	int res2 = 0;
	printf("trying to mount\n");
	if ((res2 = f_mount(&fs, "CTRNAND:", 1)) != FR_OK)
	{
		printf("WTF MOUNT FAILED; %d\n", res2);
	}
	else if ((res2 = f_open(&test_file, "CTRNAND:/rw/sys/SecureInfo_A", FA_READ)) != FR_OK)
	{
		printf("WTF READ OPEN FAILED; %d\n", res2);
	}
	else
	{
		printf("Size: %d\n", f_size(&test_file));
	}

	ctr_sd_interface_destroy(&sd_ctx.io);
	ctr_nand_crypto_interface_destroy(&nand_crypto_ctx.io);
	ctr_nand_interface_destroy(&nand_ctx.nand_io);

	printf("Press any key to continue...\n");
	input_wait();
	
	printf("I'm alive, I swear!\n");
	i2cWriteRegister(I2C_DEV_MCU, 0x20, 1);
	return 0;
}

