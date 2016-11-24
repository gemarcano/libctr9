#include "memory_tests.h"
#include <string.h>

memory_test_data memory_test_data_initialize(char *buffer, size_t buffer_size)
{
	memory_test_data data;
	data.buffer = buffer;
	data.buffer_size = buffer_size;
	return data;
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

void memory_tests_initialize(ctr_unit_tests *memory_tests, ctr_unit_test *funcs, size_t number_of_funcs, void *memory_ctx)
{
	ctr_unit_tests_initialize(memory_tests, "ctr io memory tests", funcs, 5);
	ctr_unit_tests_add_test(memory_tests, (ctr_unit_test){ "memory_initialize", memory_ctx, memory_test1 });
	ctr_unit_tests_add_test(memory_tests, (ctr_unit_test){ "memory_read_sector", memory_ctx, memory_test2 });
	ctr_unit_tests_add_test(memory_tests, (ctr_unit_test){ "memory_read", memory_ctx, memory_test3 });
	ctr_unit_tests_add_test(memory_tests, (ctr_unit_test){ "memory_write_sector", memory_ctx, memory_test4 });
	ctr_unit_tests_add_test(memory_tests, (ctr_unit_test){ "memory_write", memory_ctx, memory_test5 });
}

