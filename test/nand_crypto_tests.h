#ifndef CTR_NAND_CRYPTO_TESTS_H_
#define CTR_NAND_CRYPTO_TESTS_H_

#include "test.h"
#include <ctr9/io.h>
#include <stddef.h>

typedef struct
{
	char *buffer;
	size_t buffer_size;
	ctr_nand_crypto_interface *io;
	ctr_io_interface *lower_io;
} nand_crypto_test_data;

#ifdef __cplusplus
extern "C" {
#endif

nand_crypto_test_data nand_crypto_test_data_initialize(char *buffer, size_t buffer_size, ctr_io_interface *lower_io);

void nand_crypto_tests_initialize(ctr_unit_tests *nand_crypto_test, ctr_unit_test *funcs, size_t number_of_funcs, void *nand_crypto_ctx);

#ifdef __cplusplus
}
#endif

#endif//CTR_NAND_CRYPTO_TESTS_H_

