#include <3ds9/io.h>
#include <3ds9/ctr_nand_interface.h>
#include <3ds9/ctr_nand_crypto_interface.h>

#include <string.h>
#include <stdbool.h>
#include <ctr/console.h>
#include <ctr/printf.h>
#include <ctr/draw.h>
#include <ctr/headers.h>
#include <ctr/hid.h>

#include <3ds9/i2c.h>

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

static bool nand_ctrnand_test1(void *ctx)
{
	nand_crypto_test_data *data = ctx;

	int res = ctr_nand_crypto_interface_initialize(&data->io, 0x04, &data->lower_io->base);

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

int main()
{
	draw_init((draw_s*)0x23FFFE00);
	console_init(0xFFFFFF, 0);
	draw_clear_screen(SCREEN_TOP, 0xAAAAAA);
	printf("UNIT TESTING\n");

	char buffer[0x1000] = {0};
	nand_test_data nand_ctx = {buffer, sizeof(buffer), {{0}} };
	nand_crypto_test_data nand_crypto_ctx = {buffer, sizeof(buffer), {{0}}, &nand_ctx.nand_io};

	ctr_unit_test nand_tests_f[8];
	ctr_unit_tests nand_tests;
	ctr_unit_tests_initialize(&nand_tests, "NAND tests", nand_tests_f, 8);
	ctr_unit_tests_add_test(&nand_tests, (ctr_unit_test){ "ctr_nand_initialize", &nand_ctx, nand_test1 } );
	ctr_unit_tests_add_test(&nand_tests, (ctr_unit_test){ "ctr_nand_read", &nand_ctx, nand_test2 } );
	ctr_unit_tests_add_test(&nand_tests, (ctr_unit_test){ "ctr_nand_read_sector", &nand_ctx, nand_test3 } );
	ctr_unit_tests_add_test(&nand_tests, (ctr_unit_test){ "ctr_nand compare", &nand_ctx, nand_test4 } );
	ctr_unit_tests_add_test(&nand_tests, (ctr_unit_test){ "ctr_nand_read test zero", &nand_ctx, nand_test5 } );
	ctr_unit_tests_add_test(&nand_tests, (ctr_unit_test){ "ctr_nand_disk_size", &nand_ctx, nand_test6 } );
	ctr_unit_tests_add_test(&nand_tests, (ctr_unit_test){ "ctr_nand_sector_size", &nand_ctx, nand_test7 } );
	ctr_unit_tests_add_test(&nand_tests, (ctr_unit_test){ "ctr_nand_read larger than a sector", &nand_ctx, nand_test8 } );

	ctr_unit_test nand_crypto_tests_f[4];
	ctr_unit_tests nand_crypto_tests;
	ctr_unit_tests_initialize(&nand_crypto_tests, "NAND crypto tests", nand_crypto_tests_f, 4);
	ctr_unit_tests_add_test(&nand_crypto_tests, (ctr_unit_test){ "ctr_nand_crypto_initialize", &nand_crypto_ctx, nand_ctrnand_test1 });
	ctr_unit_tests_add_test(&nand_crypto_tests, (ctr_unit_test){ "ctr_nand_crypto_read", &nand_crypto_ctx, nand_ctrnand_test3 });
	ctr_unit_tests_add_test(&nand_crypto_tests, (ctr_unit_test){ "ctr_nand_crypto_read_sector", &nand_crypto_ctx, nand_ctrnand_test2 });
	ctr_unit_tests_add_test(&nand_crypto_tests, (ctr_unit_test){ "ctr_nand_crypto_read compare", &nand_crypto_ctx, nand_ctrnand_test4 });

	int res = ctr_execute_unit_tests(&nand_tests);
	res = ctr_execute_unit_tests(&nand_crypto_tests);

	ctr_sd_interface sd_io;
	ctr_sd_interface_initialize(&sd_io);

	ctr_sd_interface_destroy(&sd_io);
	ctr_nand_interface_destroy(&nand_ctx.nand_io);

	printf("Press any key to continue...\n");
	input_wait();

	i2cWriteRegister(I2C_DEV_MCU, 0x20, 1);

	while (*REG_HID & HID_START)
	{
	//	input_wait();
		printf("%d\n", i2cReadRegister(I2C_DEV_MCU, 0x10) & 0x4);
	}

	return 0;
}

