#ifndef CTR_CRYPTO_MEMORY_TESTS_H_
#define CTR_CRYPTO_MEMORY_TESTS_H_

#include "test.h"
#include <stddef.h>

void crypto_memory_tests_initialize(ctr_unit_tests *crypto_memory_tests, ctr_unit_test *funcs, size_t number_of_funcs, void *crypto_memory_ctx);

#endif//CTR_CRYPTO_MEMORY_TESTS_H_

