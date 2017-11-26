#ifndef CTR_NAND_TESTS_H_
#define CTR_NAND_TESTS_H_

#include <ctr9/io/ctr_nand_interface.h>
#include "test.h"

#include <stddef.h>


typedef struct
{
	char *buffer;
	size_t buffer_size;
	ctr_nand_interface *nand_io;

} nand_test_data;

#ifdef __cplusplus
extern "C" {
#endif

void nand_tests_initialize(ctr_unit_tests *nand_test, ctr_unit_test *funcs, size_t number_of_funcs, void *nand_ctx);

#ifdef __cplusplus
}
#endif

#endif//CTR_NAND_TESTS_H_

