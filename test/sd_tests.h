#ifndef CTR_SD_TESTS_H_
#define CTR_SD_TESTS_H_

#include "test.h"
#include <ctr9/io.h>
#include <stddef.h>

typedef struct
{
	char *buffer;
	size_t buffer_size;
	ctr_sd_interface *io;
} sd_test_data;

#ifdef __cplusplus
extern "C" {
#endif

void sd_tests_initialize(ctr_unit_tests *sd_tests, ctr_unit_test *funcs, size_t number_of_funcs, void *sd_ctx);

#ifdef __cplusplus
}
#endif

#endif//CTR_SD_TESTS_H_

