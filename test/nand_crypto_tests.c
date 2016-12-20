#include "nand_crypto_tests.h"
#include <ctr9/io.h>

#include <string.h>

nand_crypto_test_data nand_crypto_test_data_initialize(char *buffer, size_t buffer_size, ctr_io_interface *lower_io)
{
	nand_crypto_test_data data;
	data.buffer = buffer;
	data.buffer_size = buffer_size;
	data.lower_io = lower_io;
	return data;
}

static bool nand_ctrnand_test1(void *ctx)
{
	nand_crypto_test_data *data = ctx;

	int res = ctr_nand_crypto_interface_initialize(&data->io, 0x04, NAND_CTR, data->lower_io);

	return !res;
}

static bool nand_ctrnand_test2(void *ctx)
{
	nand_crypto_test_data *data = ctx;
	char *buffer = data->buffer;
	size_t buffer_size = data->buffer_size;

	uint32_t loc = 0x0B95CA00/ 0x200; //CTRNAND
	int res = ctr_io_read_sector(&data->io, buffer, buffer_size, loc, 1);

	return !res && strncmp("CTR", buffer+3, 3) == 0;
}

static bool nand_ctrnand_test3(void *ctx)
{
	nand_crypto_test_data *data = ctx;
	char *buffer = data->buffer;
	size_t buffer_size = data->buffer_size;

	uint32_t loc = 0x0B95CA00; //CTRNAND
	int res = ctr_io_read(&data->io, buffer, buffer_size, loc + 3, 3);

	return !res && strncmp("CTR", buffer, 3) == 0;
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

#include <stdio.h>

static bool nand_ctrnand_test7(void *ctx)
{
	FILE *file = fopen("CTRNAND:/dbs/certs.db", "rb");
	if (!file) return false;
	fclose(file);

	return true;
}
#include "test.h"

void nand_crypto_tests_initialize(ctr_unit_tests *nand_crypto_tests, ctr_unit_test *funcs, size_t number_of_funcs, void *nand_crypto_ctx)
{
	ctr_unit_tests_initialize(nand_crypto_tests, "NAND crypto tests", funcs, 7);
	ctr_unit_tests_add_test(nand_crypto_tests, (ctr_unit_test){ "ctr_nand_crypto_initialize", nand_crypto_ctx, nand_ctrnand_test1 });
	ctr_unit_tests_add_test(nand_crypto_tests, (ctr_unit_test){ "ctr_nand_crypto_read", nand_crypto_ctx, nand_ctrnand_test3 });
	ctr_unit_tests_add_test(nand_crypto_tests, (ctr_unit_test){ "ctr_nand_crypto_read_sector", nand_crypto_ctx, nand_ctrnand_test2 });
	ctr_unit_tests_add_test(nand_crypto_tests, (ctr_unit_test){ "ctr_nand_crypto_read compare", nand_crypto_ctx, nand_ctrnand_test4 });
	ctr_unit_tests_add_test(nand_crypto_tests, (ctr_unit_test){ "ctr_nand_crypto_write", nand_crypto_ctx, nand_ctrnand_test5 });
	ctr_unit_tests_add_test(nand_crypto_tests, (ctr_unit_test){ "ctr_nand_crypto_write_sector", nand_crypto_ctx, nand_ctrnand_test6 });
	ctr_unit_tests_add_test(nand_crypto_tests, (ctr_unit_test){ "ctr_nand_crypto_mount_file", nand_crypto_ctx, nand_ctrnand_test7 });
}
