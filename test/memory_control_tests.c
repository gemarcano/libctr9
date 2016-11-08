#include "memory_control_tests.h"
#include <ctr9/ctr_memory.h>
#include <string.h>


static bool memory_control_test1(void *ctx)
{
	ctr_memory_region region = ctr_memory_get_region(0x0);
	region = ctr_memory_get_region(0x08000000);
	region = ctr_memory_get_region(0x08FFFFFF);
	region = ctr_memory_get_region(0x08100000);
	region = ctr_memory_get_region(0x10000000);
	region = ctr_memory_get_region(0x17FFFFFF);
	region = ctr_memory_get_region(0x18000000);
	region = ctr_memory_get_region(0x185FFFFF);
	region = ctr_memory_get_region(0x18600000);
	region = ctr_memory_get_region(0x1FF00000);
	region = ctr_memory_get_region(0x1FF7FFFF);
	region = ctr_memory_get_region(0x1FF80000);
	region = ctr_memory_get_region(0x1FFFFFFF);
	region = ctr_memory_get_region(0x20000000);
	region = ctr_memory_get_region(0x27FFFFFF);
	region = ctr_memory_get_region(0x28000000);
	region = ctr_memory_get_region(0xFFF00000);
	region = ctr_memory_get_region(0xFFF03FFF);
	region = ctr_memory_get_region(0xFFF04FFF);
	region = ctr_memory_get_region(0xFFFF0000);
	region = ctr_memory_get_region(0xFFFFFFFF);
	

	return true;
}

static bool memory_control_test2(void *ctx)
{
	return true;
}

static bool memory_control_test3(void *ctx)
{
	return true;
}

static bool memory_control_test4(void *ctx)
{
	return false; 
}

static bool memory_control_test5(void *ctx)
{
	return false; 
}

void memory_control_tests_initialize(ctr_unit_tests *memory_control_tests, ctr_unit_test *funcs, size_t number_of_funcs, void *memory_control_ctx)
{
	ctr_unit_tests_initialize(memory_control_tests, "ctr io memory control tests", funcs, 5);
	ctr_unit_tests_add_test(memory_control_tests, (ctr_unit_test){ "memory_control_initialize", memory_control_ctx, memory_control_test1 });
	ctr_unit_tests_add_test(memory_control_tests, (ctr_unit_test){ "memory_control_read_sector", memory_control_ctx, memory_control_test2 });
	ctr_unit_tests_add_test(memory_control_tests, (ctr_unit_test){ "memory_control_read", memory_control_ctx, memory_control_test3 });
	ctr_unit_tests_add_test(memory_control_tests, (ctr_unit_test){ "memory_control_write_sector", memory_control_ctx, memory_control_test4 });
	ctr_unit_tests_add_test(memory_control_tests, (ctr_unit_test){ "memory_control_write", memory_control_ctx, memory_control_test5 });
}

