#include "memory_control_tests.h"
#include <ctr9/ctr_memory.h>
#include <string.h>

//FIXME Need to make unit tests take into acconut when being run in an N3DS, since some region and size commands are impacted.

static bool memory_control_test1(void *ctx)
{
	if (CTR_MEMORY_ITCM_REGION != ctr_memory_get_region(0x0))
		return false;
	if (CTR_MEMORY_ARM9_REGION != ctr_memory_get_region(0x08000000))
		return false;
	if (CTR_MEMORY_ARM9_REGION != ctr_memory_get_region(0x080FFFFF))
		return false;
	if (CTR_MEMORY_UNKNOWN_REGION != ctr_memory_get_region(0x08100000))
		return false;
	if (CTR_MEMORY_IO_REGION != ctr_memory_get_region(0x10000000))
		return false;
	if (CTR_MEMORY_IO_REGION != ctr_memory_get_region(0x17FFFFFF))
		return false;
	if (CTR_MEMORY_VRAM_REGION != ctr_memory_get_region(0x18000000))
		return false;
	if (CTR_MEMORY_VRAM_REGION != ctr_memory_get_region(0x185FFFFF))
		return false;
	if (CTR_MEMORY_UNKNOWN_REGION != ctr_memory_get_region(0x18600000))
		return false;
	if (CTR_MEMORY_DSP_REGION != ctr_memory_get_region(0x1FF00000))
		return false;
	if (CTR_MEMORY_DSP_REGION != ctr_memory_get_region(0x1FF7FFFF))
		return false;
	if (CTR_MEMORY_AXI_WRAM_REGION != ctr_memory_get_region(0x1FF80000))
		return false;
	if (CTR_MEMORY_AXI_WRAM_REGION != ctr_memory_get_region(0x1FFFFFFF))
		return false;
	if (CTR_MEMORY_FCRAM_REGION != ctr_memory_get_region(0x20000000))
		return false;
	if (CTR_MEMORY_FCRAM_REGION != ctr_memory_get_region(0x27FFFFFF))
		return false;
	//if (CTR_MEMORY__REGION != ctr_memory_get_region(0x28000000))
	//	return false;
	if (CTR_MEMORY_DTCM_REGION != ctr_memory_get_region(0x30000000))
		return false;
	if (CTR_MEMORY_DTCM_REGION != ctr_memory_get_region(0x30003FFF))
		return false;
	if (CTR_MEMORY_UNKNOWN_REGION != ctr_memory_get_region(0x30004FFF))
		return false;
	if (CTR_MEMORY_BOOTROM_REGION != ctr_memory_get_region(0xFFFF0000))
		return false;
	if (CTR_MEMORY_BOOTROM_REGION != ctr_memory_get_region(0xFFFFFFFF))
		return false;

	return true;
}

static bool memory_control_test2(void *ctx)
{
	ctr_memory_set_dtcm_state(false);
	if (ctr_memory_dtcm_state())
		return false;

	ctr_memory_set_itcm_state(false);
	if (ctr_memory_itcm_state())
		return false;

	ctr_memory_set_dtcm_state(true);
	if (!ctr_memory_dtcm_state())
		return false;

	ctr_memory_set_itcm_state(true);
	if (!ctr_memory_itcm_state())
		return false;

	return true;
}

static bool memory_control_test3(void *ctx)
{
	if (CTR_MEMORY_TCM_64MB != ctr_memory_itcm_size())
		return false;
	if (CTR_MEMORY_TCM_16KB != ctr_memory_dtcm_size())
		return false;

	ctr_memory_set_itcm_size(CTR_MEMORY_TCM_128MB);
	if (CTR_MEMORY_TCM_128MB != ctr_memory_itcm_size())
		return false;

	ctr_memory_set_dtcm_size(CTR_MEMORY_TCM_32KB);
	if (CTR_MEMORY_TCM_32KB != ctr_memory_dtcm_size())
		return false;

	ctr_memory_set_dtcm_size(CTR_MEMORY_TCM_16KB);
	ctr_memory_set_itcm_size(CTR_MEMORY_TCM_64MB);

	return true;
}

static bool memory_control_test4(void *ctx)
{
	if (ctr_memory_dtcm_address() != 0x30000000)
		return false;

	ctr_memory_set_dtcm_address(0x45600000);
	if (ctr_memory_dtcm_address() != 0x45600000)
		return false;

	ctr_memory_set_dtcm_address(0x30000000);

	return true;
}

static bool memory_control_test5(void *ctx)
{
	//if (ctr_memory_fcram_size() != 0x08000000)
	//	return false;
	//if (ctr_memory_arm9_size() != 0x00100000)
	//	return false;
	return true;
}

void memory_control_tests_initialize(ctr_unit_tests *memory_control_tests, ctr_unit_test *funcs, size_t number_of_funcs, void *memory_control_ctx)
{
	ctr_unit_tests_initialize(memory_control_tests, "ctr io memory control tests", funcs, 5);
	ctr_unit_tests_add_test(memory_control_tests, (ctr_unit_test){ "memory region check", memory_control_ctx, memory_control_test1 });
	ctr_unit_tests_add_test(memory_control_tests, (ctr_unit_test){ "TCM state check", memory_control_ctx, memory_control_test2 });
	ctr_unit_tests_add_test(memory_control_tests, (ctr_unit_test){ "TCM size check", memory_control_ctx, memory_control_test3 });
	ctr_unit_tests_add_test(memory_control_tests, (ctr_unit_test){ "DTCM base address check", memory_control_ctx, memory_control_test4 });
	ctr_unit_tests_add_test(memory_control_tests, (ctr_unit_test){ "Memory size checks", memory_control_ctx, memory_control_test5 });
}

