#ifndef CTR_MEMORY_CONTROL_TESTS_H_
#define CTR_MEMORY_CONTROL_TESTS_H_

#include "test.h"
#include <ctr9/io.h>
#include <stddef.h>

typedef struct
{
	char *buffer;
	size_t buffer_size;

	uint8_t data[0x1000];
	ctr_memory_interface mem_io;
} memory_control_test_data;

void memory_tests_initialize(ctr_unit_tests *memory_tests, ctr_unit_test *funcs, size_t number_of_funcs, void *memory_ctx);

#endif//CTR_MEMORY_CONTROL_TESTS_H_

