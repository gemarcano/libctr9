#ifndef CTR_TWL_TESTS_H_
#define CTR_TWL_TESTS_H_

#include "test.h"
#include <ctr9/io.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void twl_tests_initialize(ctr_unit_tests *twl_tests, ctr_unit_test *funcs, size_t number_of_funcs, void *twl_ctx);

#ifdef __cplusplus
}
#endif

#endif//CTR_TWL_TESTS_H_

