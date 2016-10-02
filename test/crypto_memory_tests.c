#include "crypto_memory_tests.h"

#include "nand_crypto_tests.h"

#include <string.h>

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
	res = ctr_nand_crypto_interface_read(&data->io, stuff2, sizeof(stuff2), 10, sizeof(stuff2)-5);

	bool result = !memcmp(stuff, stuff2, 5);
	return result;
}

void crypto_memory_tests_initialize(ctr_unit_tests *crypto_memory_tests, ctr_unit_test *funcs, size_t number_of_funcs, void *crypto_memory_ctx)
{
	ctr_unit_tests_initialize(crypto_memory_tests, "ctr crypto io memory tests", funcs, 5);
	ctr_unit_tests_add_test(crypto_memory_tests, (ctr_unit_test){ "crypto memory_initialize", crypto_memory_ctx, crypto_memory_test1 });
	ctr_unit_tests_add_test(crypto_memory_tests, (ctr_unit_test){ "crypto memory_read_sector", crypto_memory_ctx, crypto_memory_test2 });
	ctr_unit_tests_add_test(crypto_memory_tests, (ctr_unit_test){ "crypto memory_read", crypto_memory_ctx, crypto_memory_test3 });
	ctr_unit_tests_add_test(crypto_memory_tests, (ctr_unit_test){ "crypto memory_write_sector", crypto_memory_ctx, crypto_memory_test4 });
	ctr_unit_tests_add_test(crypto_memory_tests, (ctr_unit_test){ "crypto memory_write", crypto_memory_ctx, crypto_memory_test5 });
}
