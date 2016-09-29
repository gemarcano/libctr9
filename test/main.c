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
#include <ctr9/ctr_system.h>
#include <ctr9/ctr_interrupt.h>
#include <ctr9/ctr_screen.h>

#include <ctr9/ctr_rtc.h>
#include <ctr9/io/ctr_cart_interface.h>
#include <ctr9/ctr_timer.h>
#include <ctr9/ctr_system_clock.h>
#include <ctr9/ctr_irq.h>
#include <ctr9/ctr_gfx.h>
#include <ctr9/ctr_cache.h>

int DEBUG = 0;

#include "test.h"

void scribble_screen(void);

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

typedef struct
{
	char *buffer;
	size_t buffer_size;

	uint8_t data[0x1000];
	ctr_memory_interface mem_io;
} memory_test_data;

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

static bool sd_test4(void *ctx)
{
	FATFS fs = { 0 };
	FIL test_file = { 0 };

	sd_test_data *data = ctx;
	ctr_setup_disk_parameters params = {&data->io, 0, ctr_io_disk_size(&data->io)};
	disk_ioctl(3, CTR_SETUP_DISK, &params);

	const char * string = "HELLO WORLD!!!";

	int res2 = 0;
	if ((res2 = f_mount(&fs, "SD:", 1)) == FR_OK &&
	(res2 = f_open(&test_file, "SD:/test_data.txt", FA_WRITE | FA_READ  | FA_OPEN_ALWAYS)) == FR_OK)
	{
		UINT written;
		f_write(&test_file, string, strlen(string), &written);
		size_t size = f_size(&test_file);
		f_close(&test_file);
		return size == strlen(string);
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

	ctr_twl_keyslot_setup();

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
	if ((res2 = f_mount(&fs, "TWLN:", 1)) == FR_OK &&
	(res2 |= f_open(&test_file, "TWLN:/sys/TWLFontTable.dat", FA_READ)) == FR_OK)
	{
		size_t size = f_size(&test_file);
		f_close(&test_file);
		test1 = size == 863296;
		ctr_fatfs_interface io;
		f_open(&test_file, "TWLN:/shared2/0000", FA_READ | FA_WRITE);
		ctr_fatfs_interface_initialize(&io, &test_file);

		ctr_io_read(&io, buffer, buffer_size, 0, 513);
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
	FIL test_file2 = { 0 };

	nand_crypto_test_data *data = ctx;
	char *buffer = data->buffer;
	size_t buffer_size = data->buffer_size;

	ctr_io_read_sector(&data->io, buffer, buffer_size, 0x00012E00/0x200 + 1, 1);


	bool test1 = false;
	int res2 = 0;
	if ((res2 = f_mount(&fs, "TWLN:", 1)) == FR_OK &&
	(res2 |= f_open(&test_file, "TWLN:/shared2/0000", FA_READ | FA_WRITE)) == FR_OK)
	{
		ctr_fatfs_interface io;
		ctr_fatfs_interface_initialize(&io, &test_file);

		ctr_setup_disk_parameters params = { &io, 0, ctr_io_disk_size(&data->io)/0x200 };
		disk_ioctl(4, CTR_SETUP_DISK, &params);

		ctr_io_read(&io, buffer, buffer_size, 0, 513);
		test1 = FR_OK == f_mount(&fs2, "DISK1:", 1);

		int res3 = f_open(&test_file2, "DISK1:/foobar.txt", FA_WRITE | FA_OPEN_ALWAYS);
		printf("Did it open: %d\n", res3);
		UINT written;
		int res4 = f_write(&test_file2, "FOOBAR\nhello world!!!", sizeof("FOOBAR\nhello world!!!")-1,&written);
		printf("this much got written: %d\n, status: %d\n", written, res4);
		f_close(&test_file2);

	}

	return !res2 && test1;
}

static bool memory_test1(void *ctx)
{
	memory_test_data *data = ctx;
	ctr_memory_interface_initialize(&data->mem_io, data->data, sizeof(data->data));

	for (size_t i = 0; i < sizeof(data->data); ++i)
		data->data[i] = i;

	return true;
}

static bool memory_test2(void *ctx)
{
	memory_test_data *data = ctx;
	char *buffer = data->buffer;
	size_t buffer_size = data->buffer_size;
	ctr_memory_interface_read_sector(&data->mem_io, buffer, buffer_size, 55, 55);

	for (size_t i = 0; i < 55; ++i)
		if (i+55 != buffer[i])
			return false;

	return true;
}

static bool memory_test3(void *ctx)
{
	memory_test_data *data = ctx;
	char *buffer = data->buffer;
	size_t buffer_size = data->buffer_size;
	ctr_memory_interface_read(&data->mem_io, buffer, buffer_size, 55, 55);

	for (size_t i = 0; i < 55; ++i)
		if (i+55 != buffer[i])
			return false;

	return true;
}

static bool memory_test4(void *ctx)
{
	memory_test_data *data = ctx;
	uint8_t stuff[0x200];
	for (size_t i = 0; i < sizeof(stuff); ++i)
		stuff[i] = 255-i;

	ctr_memory_interface_write_sector(&data->mem_io, stuff, sizeof(stuff), 55);

	return (!memcmp((char*)data->mem_io.buffer + 55, stuff, sizeof(stuff)));
}

static bool memory_test5(void *ctx)
{
	memory_test_data *data = ctx;
	uint8_t stuff[0x200];
	for (size_t i = 0; i < sizeof(stuff); ++i)
		stuff[i] = 255-i;

	ctr_memory_interface_write(&data->mem_io, stuff, sizeof(stuff), 55);

	return (!memcmp((char*)data->mem_io.buffer + 55, stuff, sizeof(stuff)));
}

static bool crypto_memory_test1(void *ctx)
{
	nand_crypto_test_data *data = ctx;
	int res = ctr_nand_crypto_interface_initialize(&data->io, 0x04, NAND_CTR, &data->lower_io->base);

	return !res;
}

static bool crypto_memory_test2(void *ctx)
{
	nand_crypto_test_data *data = ctx;
	char *buffer = data->buffer;
	size_t buffer_size = data->buffer_size;

	ctr_nand_interface nand_io;
	ctr_nand_crypto_interface crypto_io;

	ctr_nand_interface_initialize(&nand_io);
	ctr_nand_crypto_interface_initialize(&crypto_io, 0x04, NAND_CTR, &nand_io.base);

	ctr_io_read_sector(&nand_io, ((ctr_memory_interface*)(data->io.lower_io))->buffer, ((ctr_memory_interface*)(data->io.lower_io))->buffer_size, 0, 1);
	char buffer2[512];
	ctr_io_read_sector(&crypto_io, buffer2, sizeof(buffer2), 0, 1);

	int res = ctr_nand_crypto_interface_read_sector(&data->io, buffer, buffer_size, 55, 55);


	return memcmp(buffer, buffer2 + 55, 55) == 0;
}

static bool crypto_memory_test3(void *ctx)
{
	nand_crypto_test_data *data = ctx;
	char *buffer = data->buffer;
	size_t buffer_size = data->buffer_size;
	int res = ctr_nand_crypto_interface_read(&data->io, buffer, buffer_size, 55, 55);

	return true;
}

static bool crypto_memory_test4(void *ctx)
{
	nand_crypto_test_data *data = ctx;
	uint8_t stuff[0x200];
	for (size_t i = 0; i < sizeof(stuff); ++i)
		stuff[i] = 255-i;

	int res = ctr_nand_crypto_interface_write_sector(&data->io, stuff, sizeof(stuff), 0x0);
	uint8_t stuff2[sizeof(stuff)];
	res = ctr_nand_crypto_interface_read_sector(&data->io, stuff2, sizeof(stuff2), 0x0, sizeof(stuff2));

	bool result = !memcmp(stuff, stuff2, sizeof(stuff));
	return result;
}

static bool crypto_memory_test5(void *ctx)
{
	nand_crypto_test_data *data = ctx;
	uint8_t stuff[0x200];
	for (size_t i = 0; i < sizeof(stuff); ++i)
		stuff[i] = 255-i;

	int res = ctr_nand_crypto_interface_write(&data->io, stuff, 5, 10);
	uint8_t stuff2[sizeof(stuff)];
	res = ctr_nand_crypto_interface_read(&data->io, stuff2, sizeof(stuff2), 0x5, sizeof(stuff2)-5);

	bool result = !memcmp(stuff, stuff2 + 5, 10);
	return result;
}
#include <ctr9/io/ctr_fatfs.h>

extern void(*ctr_interrupt_handlers[7])(const uint32_t*);
void abort_interrupt(uint32_t*);
void prefetch_abort(uint32_t*);
void undefined_instruction(uint32_t*);

int main()
{
	draw_s *cakehax_fbs = (draw_s*)0x23FFFE00;
	ctr_gfx_screen top_screen, bottom_screen;
	ctr_gfx_screen_initialize(&top_screen, cakehax_fbs->top_left, 400, 240, CTR_GFX_PIXEL_RGB8);
	ctr_gfx_screen_initialize(&bottom_screen, cakehax_fbs->sub, 320, 240, CTR_GFX_PIXEL_RGB8);

	draw_init(cakehax_fbs);
	console_init(0xFFFFFF, 0);
	draw_clear_screen(SCREEN_TOP, 0x111111);
	printf("UNIT TESTING\n");

	ctr_interrupt_prepare();
	ctr_interrupt_set(CTR_INTERRUPT_DATABRT, abort_interrupt);
	ctr_interrupt_set(CTR_INTERRUPT_UNDEF, undefined_instruction);
	ctr_interrupt_set(CTR_INTERRUPT_PREABRT, prefetch_abort);

	char buffer[0x1000] = {0};
	nand_test_data nand_ctx = {buffer, sizeof(buffer), {{0}} };
	nand_crypto_test_data nand_crypto_ctx = {buffer, sizeof(buffer), {{0}}, &nand_ctx.nand_io};
	nand_crypto_test_data twl_crypto_ctx = {buffer, sizeof(buffer), {{0}}, &nand_ctx.nand_io};
	sd_test_data sd_ctx = {buffer, sizeof(buffer), {{0}}};
	memory_test_data memory_ctx = {buffer, sizeof(buffer), {0}, {{0}}};
	nand_crypto_test_data crypto_memory_ctx = {buffer, sizeof(buffer), {{0}}, &memory_ctx.mem_io};

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

	ctr_unit_test sd_tests_f[4];
	ctr_unit_tests sd_tests;
	ctr_unit_tests_initialize(&sd_tests, "SD tests", sd_tests_f, 4);
	ctr_unit_tests_add_test(&sd_tests, (ctr_unit_test){ "ctr_sd_initialize", &sd_ctx, sd_test1});
	ctr_unit_tests_add_test(&sd_tests, (ctr_unit_test){ "ctr_sd_read* compare", &sd_ctx, sd_test2});
	ctr_unit_tests_add_test(&sd_tests, (ctr_unit_test){ "ctr_sd_interface FATFS read", &sd_ctx, sd_test3});
	ctr_unit_tests_add_test(&sd_tests, (ctr_unit_test){ "ctr_sd_interface FATFS write", &sd_ctx, sd_test4});

	ctr_unit_test twl_crypto_tests_f[3];
	ctr_unit_tests twl_crypto_tests;
	ctr_unit_tests_initialize(&twl_crypto_tests, "TWL tests", twl_crypto_tests_f, 3);
	ctr_unit_tests_add_test(&twl_crypto_tests, (ctr_unit_test){ "twl_crypto_initialize", &twl_crypto_ctx, twl_test1 });
	ctr_unit_tests_add_test(&twl_crypto_tests, (ctr_unit_test){ "twl mount and read", &twl_crypto_ctx, twl_test2 });
	ctr_unit_tests_add_test(&twl_crypto_tests, (ctr_unit_test){ "twl mount nested", &twl_crypto_ctx, twl_test3 });

	ctr_unit_test memory_tests_f[5];
	ctr_unit_tests memory_tests;
	ctr_unit_tests_initialize(&memory_tests, "ctr io memory tests", memory_tests_f, 5);
	ctr_unit_tests_add_test(&memory_tests, (ctr_unit_test){ "memory_initialize", &memory_ctx, memory_test1 });
	ctr_unit_tests_add_test(&memory_tests, (ctr_unit_test){ "memory_read_sector", &memory_ctx, memory_test2 });
	ctr_unit_tests_add_test(&memory_tests, (ctr_unit_test){ "memory_read", &memory_ctx, memory_test3 });
	ctr_unit_tests_add_test(&memory_tests, (ctr_unit_test){ "memory_write_sector", &memory_ctx, memory_test4 });
	ctr_unit_tests_add_test(&memory_tests, (ctr_unit_test){ "memory_write", &memory_ctx, memory_test5 });


	ctr_unit_test crypto_memory_tests_f[5];
	ctr_unit_tests crypto_memory_tests;
	ctr_unit_tests_initialize(&crypto_memory_tests, "ctr crypto io memory tests", crypto_memory_tests_f, 5);
	ctr_unit_tests_add_test(&crypto_memory_tests, (ctr_unit_test){ "crypto memory_initialize", &crypto_memory_ctx, crypto_memory_test1 });
	ctr_unit_tests_add_test(&crypto_memory_tests, (ctr_unit_test){ "crypto memory_read_sector", &crypto_memory_ctx, crypto_memory_test2 });
	ctr_unit_tests_add_test(&crypto_memory_tests, (ctr_unit_test){ "crypto memory_read", &crypto_memory_ctx, crypto_memory_test3 });
	ctr_unit_tests_add_test(&crypto_memory_tests, (ctr_unit_test){ "crypto memory_write_sector", &crypto_memory_ctx, crypto_memory_test4 });
	ctr_unit_tests_add_test(&crypto_memory_tests, (ctr_unit_test){ "crypto memory_write", &crypto_memory_ctx, crypto_memory_test5 });


	int res = ctr_execute_unit_tests(&nand_tests);
	res |= ctr_execute_unit_tests(&nand_crypto_tests);
	res |= ctr_execute_unit_tests(&sd_tests);
	res |= ctr_execute_unit_tests(&twl_crypto_tests);
	res |= ctr_execute_unit_tests(&memory_tests);
	res |= ctr_execute_unit_tests(&crypto_memory_tests);

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

	int res3 = ctr_fatfs_initialize(&(nand_ctx.nand_io), NULL, NULL, &(sd_ctx.io));
	printf("ctr_fatfs_initialize result: %X\n", res3);

	ctr_sd_interface_destroy(&sd_ctx.io);
	ctr_nand_crypto_interface_destroy(&nand_crypto_ctx.io);
	ctr_nand_interface_destroy(&nand_ctx.nand_io);

	printf("Press any key to continue...\n");
	input_wait();

	printf("Preparing interrupts\n");
		printf("abort handler: %X\n", ctr_interrupt_handlers[4]);
	printf("testing abort\n");

	//Cause a data abort :P
	*(volatile u32*)0xFFFFFFF0;
	printf("Returned from the abort.\n");

	printf("Trying to turn off top screen\n");
	input_wait();
	ctr_screen_disable_backlight(CTR_SCREEN_TOP);
	input_wait();
	ctr_screen_enable_backlight(CTR_SCREEN_TOP);
	printf("done: Trying to turn off top screen\n");

	printf("Trying to turn off bottom screen\n");
	input_wait();
	ctr_screen_disable_backlight(CTR_SCREEN_BOTTOM);
	input_wait();
	ctr_screen_enable_backlight(CTR_SCREEN_BOTTOM);
	printf("done: Trying to turn off bottom screen\n");

	printf("Trying to turn off both screens\n");
	input_wait();
	ctr_screen_disable_backlight(CTR_SCREEN_BOTH);
	input_wait();
	ctr_screen_enable_backlight(CTR_SCREEN_BOTTOM | CTR_SCREEN_TOP);
	printf("Trying to turn off both screens\n");
	input_wait();

	printf("Testing i2c write crap\n");

	ctr_rtc_data rtc = ctr_rtc_gettime();
	printf("%d %d %d %d %d %d\n", rtc.seconds, rtc.minutes, rtc.hours, rtc.day, rtc.month, rtc.year);
	printf("Testing a single write\n");
	input_wait();
	i2cWriteRegister(I2C_DEV_MCU, 0x30, 0x11);
	rtc = ctr_rtc_gettime();
	printf("%d %d %d %d %d %d\n", rtc.seconds, rtc.minutes, rtc.hours, rtc.day, rtc.month, rtc.year);

	printf("Testing a single write from buffer\n");
	input_wait();
	uint8_t temp_rtc[8] = { 11, 22, 33, 44, 0xBB, 0xAA,  0x99, 0x88};
	i2cWriteRegisterBuffer(I2C_DEV_MCU, 0x30, temp_rtc, 1);
	rtc = ctr_rtc_gettime();
	printf("%d %d %d %d %d %d\n", rtc.seconds, rtc.minutes, rtc.hours, rtc.day, rtc.month, rtc.year);

	printf("Testing a multiple write from buffer\n");
	input_wait();
	i2cWriteRegisterBuffer(I2C_DEV_MCU, 0x30, temp_rtc, 8);
	rtc = ctr_rtc_gettime();
	printf("%d %d %d %d %d %d\n", rtc.seconds, rtc.minutes, rtc.hours, rtc.day, rtc.month, rtc.year);
/*
	printf("Trying to read cart header...\n");
	ctr_cart_interface cart;
	printf("Initialize cart: %d\n", ctr_cart_interface_initialize(&cart));
	printf("%c%c%c%c\n", cart.ncsd_header.magic[0], cart.ncsd_header.magic[1], cart.ncsd_header.magic[2], cart.ncsd_header.magic[3]);
	printf("%c%c%c%c\n", cart.ncch_header.magic[0], cart.ncch_header.magic[1], cart.ncch_header.magic[2], cart.ncch_header.magic[3]);

	uint8_t temporary_cart[0x8000] = {0};
	printf("Cart inserted? : %d\n", ctr_cart_inserted());

	if (!cart.media_unit_size) cart.media_unit_size = 512; //Make sure this is zero, because lazy
	ctr_io_read_sector(&cart, temporary_cart, 0x8000, 0, 0x8000 / cart.media_unit_size);

	printf("cart media unit size: %d\n", cart.media_unit_size);
	printf("Finished reading, now trying to dump to SD.\n");
	ctr_sd_interface sd_interface;
	ctr_fatfs_initialize(NULL, NULL, NULL, &sd_interface);
	//FATFS fs;
	FIL file;
	f_mount(&fs, "SD:", 0);
	int open_res = f_open(&file, "SD:/dump.dump.bin", FA_WRITE | FA_CREATE_ALWAYS);
	unsigned int bw;
	int write_res = f_write(&file, temporary_cart, 0x8000, &bw);
	ctr_io_read(&cart, temporary_cart, 0x8000, 0x100, 4);
	f_write(&file, temporary_cart, 4, &bw);
	f_close(&file);
	printf("ROM SIZE: %X\n", ctr_cart_interface_disk_size(&cart));
	printf("Finished dumping, hopefully.: %d\n %d %d\n", open_res, write_res, bw);
*/
	mmcdevice dev1= *getMMCDevice(1);
	mmcdevice dev0= *getMMCDevice(0);
	printf("SDHandle: %d\n", dev0.isSDHC);
	printf("SDHandle: %d\n", dev1.isSDHC);

	printf("Trying timer stuff\n");
	ctr_timer_disable_irq(CTR_TIMER0);
	uint16_t starting_timer = ctr_timer_get_value(CTR_TIMER0);
	ctr_timer_set_prescaler(CTR_TIMER0, CTR_TIMER_DIV1);
	ctr_timer_set_count_up(CTR_TIMER0, false);
	ctr_timer_disable(CTR_TIMER0);
	ctr_timer_set_value(CTR_TIMER0, 0);

	printf("Timer value: %u\n", starting_timer);
	printf("Timer reg: %08X\n", *(uint32_t*)CTR_TIMER_REG_BASE);
	printf("Timer value delta: %u\n", ctr_timer_get_value(CTR_TIMER0) - starting_timer);

	ctr_timer_enable(CTR_TIMER0);

	printf("Timer effective fq: %u\n", ctr_timer_get_effective_frequency(CTR_TIMER0));
	printf("Timer reg: %08X\n", *(uint32_t*)CTR_TIMER_REG_BASE);
	printf("Timer value delta: %u\n", ctr_timer_get_value(CTR_TIMER0) - starting_timer);
	printf("Timer value delta: %u\n", ctr_timer_get_value(CTR_TIMER0) - starting_timer);

	ctr_timer_set_prescaler(CTR_TIMER0, CTR_TIMER_DIV64);

	printf("Timer effective fq: %u\n", ctr_timer_get_effective_frequency(CTR_TIMER0));
	printf("Timer reg: %08X\n", *(uint32_t*)CTR_TIMER_REG_BASE);
	printf("Timer value delta: %u\n", ctr_timer_get_value(CTR_TIMER0) - starting_timer);
	printf("Timer value delta: %u\n", ctr_timer_get_value(CTR_TIMER0) - starting_timer);

	ctr_timer_set_prescaler(CTR_TIMER0, CTR_TIMER_DIV256);

	printf("Timer effective fq: %u\n", ctr_timer_get_effective_frequency(CTR_TIMER0));
	printf("Timer reg: %08X\n", *(uint32_t*)CTR_TIMER_REG_BASE);
	printf("Timer value delta: %u\n", ctr_timer_get_value(CTR_TIMER0) - starting_timer);
	printf("Timer value delta: %u\n", ctr_timer_get_value(CTR_TIMER0) - starting_timer);

	ctr_timer_set_prescaler(CTR_TIMER0, CTR_TIMER_DIV1024);

	printf("Timer effective fq: %u\n", ctr_timer_get_effective_frequency(CTR_TIMER0));
	printf("Timer value delta: %u\n", ctr_timer_get_value(CTR_TIMER0) - starting_timer);
	printf("Timer reg: %08X\n", *(uint32_t*)CTR_TIMER_REG_BASE);
	printf("Timer value delta: %u\n", ctr_timer_get_value(CTR_TIMER0) - starting_timer);
	printf("Timer value delta: %u\n", ctr_timer_get_value(CTR_TIMER0) - starting_timer);

	input_wait();
	printf("Testing clock\n");
	ctr_irq_initialize();
	ctr_system_clock clock;
	ctr_system_clock_initialize(&clock, CTR_TIMER0);
	ctr_irq_master_enable();

	ctr_gfx_screen_set_pixel(&top_screen, 0 + 100, 0, 0xFF00FFu);
	ctr_gfx_screen_set_pixel(&top_screen, 2 + 100, 0, 0xFF00FFu);
	ctr_gfx_screen_set_pixel(&top_screen, 4 + 100, 0, 0xFF00FFu);
	ctr_gfx_screen_set_pixel(&top_screen, 0 + 100, 2, 0xFF00FFu);
	ctr_gfx_screen_set_pixel(&top_screen, 0 + 100, 4, 0xFF00FFu);
	ctr_gfx_screen_set_pixel(&top_screen, 2 + 100, 2, 0xFF00FFu);
	ctr_gfx_screen_set_pixel(&top_screen, 4 + 100, 2, 0xFF00FFu);
	ctr_gfx_screen_set_pixel(&top_screen, 2 + 100, 4, 0xFF00FFu);
	ctr_gfx_screen_set_pixel(&top_screen, 4 + 100, 4, 0xFF00FFu);

	for (size_t i = 0; i < 3; ++i)
	{
		uint64_t start = ctr_system_clock_get_ms(&clock);
		uint64_t ms = start;
		while (ms - start < 1000)
		{
			ms = ctr_system_clock_get_ms(&clock);
		}
		printf("second: %d\n", i);
	}


	ctr_gfx_screen_set_pixel(&bottom_screen, 0 + 100, 0, 0xFF00FFu);
	ctr_gfx_screen_set_pixel(&bottom_screen, 2 + 100, 0, 0xFF00FFu);
	ctr_gfx_screen_set_pixel(&bottom_screen, 4 + 100, 0, 0xFF00FFu);
	ctr_gfx_screen_set_pixel(&bottom_screen, 0 + 100, 2, 0xFF00FFu);
	ctr_gfx_screen_set_pixel(&bottom_screen, 0 + 100, 4, 0xFF00FFu);
	ctr_gfx_screen_set_pixel(&bottom_screen, 2 + 100, 2, 0xFF00FFu);
	ctr_gfx_screen_set_pixel(&bottom_screen, 4 + 100, 2, 0xFF00FFu);
	ctr_gfx_screen_set_pixel(&bottom_screen, 2 + 100, 4, 0xFF00FFu);
	ctr_gfx_screen_set_pixel(&bottom_screen, 4 + 100, 4, 0xFF00FFu);


	uint8_t bitmap_data[][3] = {{0xFF, 0xFF, 0xFF}, { 0xFF, 0x00, 0x81 }, {0x80, 0x01, 0x01}, {0xFF, 0xFF, 0xFF}};
	ctr_gfx_bitmap bitmap = { 20, 4, bitmap_data };
	ctr_gfx_screen_draw_bitmap(&top_screen, 0, 0, 0xFF00FF, &bitmap);
	ctr_gfx_screen_draw_bitmap(&top_screen, 20, 4, 0x00FFFF, &bitmap);
	ctr_gfx_screen_draw_bitmap(&top_screen, 40, 8, 0x0000FF, &bitmap);
	ctr_gfx_screen_draw_bitmap(&top_screen, 20, 14, 0x00FF00, &bitmap);
	ctr_gfx_screen_draw_bitmap(&top_screen, 0, 20, 0xFF0000, &bitmap);
	ctr_gfx_screen_draw_bitmap(&bottom_screen, 200, 200, 0xFF0000, &bitmap);

	printf("Testing aes\n");

	char input_buffer[0x10 * 0x20000] = { 1,2,3,4,5,6 };
	for (size_t i = 0; i < sizeof(input_buffer); ++i)
	{
		input_buffer[i] = i;
	}
	char output_buffer[0x10 * 0x20000];
	char output_buffer2[0x10 * 0x20000];
	char output_buffer3[0x10 * 0x20000];
	uint8_t ctr[16] = {0};
	uint8_t ctr2[16] = {0};

	set_ctr(ctr);
	aes_decrypt(input_buffer, output_buffer, 5, AES_CTR_MODE);
	set_ctr(ctr);
	aes_decrypt(output_buffer, output_buffer2, 5, AES_CTR_MODE);
	printf("Comparing aes results: %d\n", memcmp(input_buffer, output_buffer2, 0x10*4));
	printf("Comparing aes results: %d\n", memcmp(input_buffer, output_buffer2, 0x10*5));

	memset(ctr, 0, sizeof(ctr));
	memset(ctr2, 0, sizeof(ctr2));
	ctr_decrypt(input_buffer, output_buffer, 0x20000, AES_CTR_MODE, ctr);
	set_ctr(ctr2);
	aes_decrypt(input_buffer, output_buffer2, 0x20000, AES_CTR_MODE);

	printf("Comparing mid aes results: %d\n", memcmp(output_buffer, output_buffer2, sizeof(output_buffer)));

	memset(ctr, 0, sizeof(ctr));
	ctr_decrypt(input_buffer, output_buffer, 0x20000, AES_CTR_MODE, ctr);
	memset(ctr2, 0, sizeof(ctr2));
	ctr_decrypt(output_buffer, output_buffer3, 0x20000, AES_CTR_MODE, ctr2);
	printf("Comparing aes results: %d\n", memcmp(input_buffer, output_buffer3, 0x10*4));
	printf("Comparing aes results: %d\n", memcmp(input_buffer, output_buffer3, sizeof(output_buffer)));

	memset(ctr2, 0, sizeof(ctr2));
	set_ctr(ctr2);
	aes_decrypt(input_buffer, output_buffer, 0x20000, AES_CTR_MODE);
	set_ctr(ctr2);
	aes_decrypt(output_buffer, output_buffer2, 0x20000, AES_CTR_MODE);
	printf("Comparing aes results: %d\n", memcmp(input_buffer, output_buffer2, 0x10*4));
	printf("Comparing aes results: %d\n", memcmp(input_buffer, output_buffer2, sizeof(output_buffer)));
	printf("Comparing aes results: %d\n", memcmp(output_buffer2, output_buffer3, sizeof(output_buffer)));

	alignas(4) uint8_t test_key[16] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
	alignas(4) uint8_t test_key_i[16] = { 0x3c, 0x4f, 0xcf, 0x09, 0x88, 0x15, 0xf7, 0xab, 0xa6, 0xd2, 0xae, 0x28, 0x16, 0x15, 0x7e, 0x2b };
	alignas(4) uint8_t test_iv[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
	alignas(4) uint8_t cipher_cbc[32] = { 0x76, 0x49, 0xab, 0xac, 0x81, 0x19, 0xb2, 0x46, 0xce, 0xe9, 0x8e, 0x9b, 0x12, 0xe9, 0x19, 0x7d,
		0x50, 0x86, 0xcb, 0x9b, 0x50, 0x72, 0x19, 0xee, 0x95, 0xdb, 0x11, 0x3a, 0x91, 0x76, 0x78, 0xb2};
	alignas(4) uint8_t cipher_ecb[32] = { 0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60, 0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97,
		0xf5, 0xd3, 0xd5, 0x85, 0x03, 0xb9, 0x69, 0x9d, 0xe7, 0x85, 0x89, 0x5a, 0x96, 0xfd, 0xba, 0xaf};
	alignas(4) uint8_t outtext[16] = { 0x6b, 0xc0, 0xbc, 0xe1, 0x2a, 0x45, 0x99, 0x91, 0xe1, 0x34, 0x74, 0x1a, 0x7f, 0x9e, 0x19, 0x25};
	alignas(4) uint8_t plaintext[32] = { 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
		0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac ,0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51};
	alignas(4) uint8_t iv[16];
	alignas(4) uint8_t zero[16] = { 0 };
	memcpy(iv, test_iv, AES_BLOCK_SIZE);

	setup_aeskey(0x11, test_key_i);
	use_aeskey(0x11);

	cbc_decrypt(cipher_cbc, output_buffer, 0x2, AES_CNT_CBC_DECRYPT_MODE, iv);
	printf("Comparing cbc results: %d\n", memcmp(output_buffer, plaintext, AES_BLOCK_SIZE * 2));
	for (size_t i = 0; i < 32; ++i)
	{
		printf("%02X ", plaintext[i]);
	}
	printf("\n");
	for (size_t i = 0; i < 32; ++i)
	{
		printf("%02X ", output_buffer[i]);
	}

	ecb_decrypt(cipher_ecb, output_buffer, 0x2, AES_CNT_ECB_DECRYPT_MODE);
	printf("Comparing ecb results: %d\n", memcmp(output_buffer, plaintext, AES_BLOCK_SIZE * 2));
	memcpy(iv, test_iv, AES_BLOCK_SIZE);
	cbc_decrypt(plaintext, output_buffer, 0x2, AES_CNT_CBC_ENCRYPT_MODE, iv);
	printf("Comparing cbc results: %d\n", memcmp(output_buffer, cipher_cbc, AES_BLOCK_SIZE * 2));
	aes_decrypt(plaintext, output_buffer, 0x2, AES_CNT_ECB_ENCRYPT_MODE);
	printf("Comparing ecb results: %d\n", memcmp(output_buffer, cipher_ecb, AES_BLOCK_SIZE * 2));


	for (size_t i = 0; i < 0x1FFFF * 16; ++i)
	{
		output_buffer[i] = i;
	}
	memcpy(iv, test_iv, AES_BLOCK_SIZE);
	cbc_decrypt(output_buffer, output_buffer2, 0x1FFFF, AES_CNT_CBC_DECRYPT_MODE, iv);

	ctr_sd_interface sd;
	ctr_fatfs_initialize(NULL, NULL, NULL, &sd);
	FATFS fs3;
	FIL dump;
	f_mount(&fs3, "SD:", 0);
	f_open(&dump, "SD:/dump.cbc", FA_WRITE | FA_READ  | FA_CREATE_ALWAYS);
	int br;
	f_write(&dump, output_buffer2, 0x1FFFF * 16, &br);
	f_close(&dump);

	input_wait();
	ctr_system_poweroff();
	return 0;
}

