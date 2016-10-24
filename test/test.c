#include "test.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

bool ctr_execute_unit_test(ctr_unit_test *test)
{
	bool result;
	result = test->function(test->data);
	if (!result)
	{
		printf("Executing unit test: %s\n", test->name);
		printf("  Result: %s\n", result ? "SUCCESS" : "FAILURE");
	}
	return result;
}

bool ctr_execute_unit_tests(ctr_unit_tests *tests)
{
	bool result = true;
	printf("Executing test group: %s\n", tests->group_name);

	for (size_t i = 0; i < tests->test_count; ++i)
	{
		if (!ctr_execute_unit_test(&(tests->test[i])) && result)
		{
			result = false;
		}
	}
	printf("Done test group: %s\n  result: %s\n", tests->group_name, result ? "SUCCESS" : "FAILURE");
	return result;
}

void ctr_unit_tests_initialize(ctr_unit_tests *tests, const char *name, ctr_unit_test *entry_buffer, size_t max_entry_count)
{
	tests->group_name = name;
	tests->test = entry_buffer;
	tests->test_count = 0;
	tests->max_count = max_entry_count;
}

void ctr_unit_tests_add_test(ctr_unit_tests *tests, ctr_unit_test test)
{
	if (tests->test_count < tests->max_count)
		tests->test[tests->test_count++] = test;
}


