#include "nand_tests.h"
#include <ctr9/io.h>

#include <string.h>

static bool nand_test1(void *ctx)
{
	nand_test_data *data = ctx;

	data->nand_io = ctr_nand_interface_initialize();

	return data->nand_io != NULL;
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

#include "test.h"

void nand_tests_initialize(ctr_unit_tests *nand_tests, ctr_unit_test *funcs, size_t number_of_funcs, void *nand_ctx)
{
	ctr_unit_tests_initialize(nand_tests, "NAND tests", funcs, 11);
	ctr_unit_tests_add_test(nand_tests, (ctr_unit_test){ "ctr_nand_initialize", nand_ctx, nand_test1 } );
	ctr_unit_tests_add_test(nand_tests, (ctr_unit_test){ "ctr_nand_read", nand_ctx, nand_test2 } );
	ctr_unit_tests_add_test(nand_tests, (ctr_unit_test){ "ctr_nand_read_sector", nand_ctx, nand_test3 } );
	ctr_unit_tests_add_test(nand_tests, (ctr_unit_test){ "ctr_nand compare", nand_ctx, nand_test4 } );
	ctr_unit_tests_add_test(nand_tests, (ctr_unit_test){ "ctr_nand_read test zero", nand_ctx, nand_test5 } );
	ctr_unit_tests_add_test(nand_tests, (ctr_unit_test){ "ctr_nand_disk_size", nand_ctx, nand_test6 } );
	ctr_unit_tests_add_test(nand_tests, (ctr_unit_test){ "ctr_nand_sector_size", nand_ctx, nand_test7 } );
	ctr_unit_tests_add_test(nand_tests, (ctr_unit_test){ "ctr_nand_read larger than a sector", nand_ctx, nand_test8 } );
	ctr_unit_tests_add_test(nand_tests, (ctr_unit_test){ "ctr_nand_write", nand_ctx, nand_test9 } );
	ctr_unit_tests_add_test(nand_tests, (ctr_unit_test){ "ctr_nand_write 2048 across sectors", nand_ctx, nand_test10 } );
	ctr_unit_tests_add_test(nand_tests, (ctr_unit_test){ "ctr_nand_write 2048 across sectors", nand_ctx, nand_test11 } );
}
