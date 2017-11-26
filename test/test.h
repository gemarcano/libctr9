#ifndef CTR9_UNIT_TEST_H_
#define CTR9_UNIT_TEST_H_

#include <stddef.h>
#include <stdbool.h>

typedef bool (*ctr_unit_test_f)(void* data);

typedef struct
{
	const char* name;
	void *data;
	ctr_unit_test_f function;
} ctr_unit_test;

typedef struct
{
	const char *group_name;
	ctr_unit_test *test;
	size_t test_count;
	size_t max_count;
} ctr_unit_tests;

#ifdef __cplusplus
extern "C" {
#endif

bool ctr_execute_unit_test(ctr_unit_test *test);

bool ctr_execute_unit_tests(ctr_unit_tests *tests);

void ctr_unit_tests_initialize(ctr_unit_tests *tests, const char *name, ctr_unit_test *entry_buffer, size_t max_entry_count);

void ctr_unit_tests_add_test(ctr_unit_tests *tests, ctr_unit_test test);

#ifdef __cplusplus
}
#endif

#endif//CTR9_UNIT_TEST_H_

