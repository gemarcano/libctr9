/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

#include <ctr9/ctr_memory.h>
#include <ctr9/ctr_system.h>
#include <stdint.h>
#include <stdbool.h>

uint32_t ctr_memory_get_itcm_register_(void);
uint32_t ctr_memory_get_dtcm_register_(void);
void ctr_memory_set_itcm_register_(uint32_t reg);
void ctr_memory_set_dtcm_register_(uint32_t reg);
bool ctr_memory_itcm_state_(void);
bool ctr_memory_dtcm_state_(void);
void ctr_memory_enable_itcm_(void);
void ctr_memory_disable_itcm_(void);
void ctr_memory_enable_dtcm_(void);
void ctr_memory_disable_dtcm_(void);

ctr_memory_tcm_size ctr_memory_itcm_size(void)
{
	uint32_t reg = ctr_memory_get_itcm_register_();
	reg >>= 1;
	reg &= 0x1F;

	return (ctr_memory_tcm_size)reg;
}

ctr_memory_tcm_size ctr_memory_dtcm_size(void)
{
	uint32_t reg = ctr_memory_get_dtcm_register_();
	reg >>= 1;
	reg &= 0x1F;

	return (ctr_memory_tcm_size)reg;
}

uintptr_t ctr_memory_dtcm_base_address(void)
{
	uint32_t reg = ctr_memory_get_itcm_register_();
	reg &= 0xFFFFFFC0u;
	return reg;
}

#define CFG_EXTMEMCNT9 0x10000200u

uintptr_t ctr_memory_arm9_size(void)
{
	if ((*((uint32_t*)CFG_EXTMEMCNT9)) & 0x1)
		return CTR_ARM9_MEM_N3DS_SIZE;
	else
		return CTR_ARM9_MEM_O3DS_SIZE;
}

uintptr_t ctr_memory_fcram_size(void)
{
	switch (ctr_get_system_type())
	{
		case SYSTEM_N3DS:
			return CTR_FCRAM_N3DS_SIZE;
		case SYSTEM_O3DS:
		default:
			return CTR_FCRAM_O3DS_SIZE;
	}
}

void ctr_memory_set_itcm_size(ctr_memory_tcm_size size)
{
	uint32_t reg = ctr_memory_get_itcm_register_();
	reg &= (0x1F << 1);

	size >>= 9;
	reg |= 0x3E & (size << 1);

	ctr_memory_set_itcm_register_(reg);
}

void ctr_memory_set_dtcm_size(ctr_memory_tcm_size size)
{
	uint32_t reg = ctr_memory_get_dtcm_register_();
	reg &= (0x1F << 1);

	size >>= 9;
	reg |= 0x3E & (size << 1);

	ctr_memory_set_dtcm_register_(reg);
}

void ctr_memory_set_dtcm_base_address(uintptr_t base_address)
{
	uint32_t reg = ctr_memory_get_dtcm_register_();
	reg &= 0x30u;
	base_address &= 0xFFFFFFC0;
	reg |= base_address;
	ctr_memory_set_dtcm_register_(reg);
}

bool ctr_memory_itcm_state(void)
{
	return ctr_memory_itcm_state_();
}

bool ctr_memory_dtcm_state(void)
{
	return ctr_memory_dtcm_state_();
}

void ctr_memory_set_itcm_state(bool state)
{
	if (state)
	{
		ctr_memory_enable_itcm_();
	}
	else
	{
		ctr_memory_disable_itcm_();
	}
}

void ctr_memory_set_dtcm_state(bool state)
{
	if (state)
	{
		ctr_memory_enable_dtcm_();
	}
	else
	{
		ctr_memory_disable_dtcm_();
	}
}

#define CHECK_REGION(LOC, START, SIZE, REG) \
	region_start = START;\
	region_end = region_start + SIZE;\
	if (LOC >= region_start && LOC < region_end)\
	{\
		return REG;\
	}

ctr_memory_region ctr_memory_get_region(uintptr_t location)
{
	uintptr_t region_start, region_end;

	if (ctr_memory_itcm_state())
	{
		CHECK_REGION(location, CTR_ITCM_ADDRESS, ctr_memory_itcm_size(), CTR_MEMORY_ITCM_REGION);
	}

	if (ctr_memory_dtcm_state())
	{
		CHECK_REGION(location, ctr_memory_dtcm_base_address(), ctr_memory_dtcm_size(), CTR_MEMORY_DTCM_REGION);
	}

	CHECK_REGION(location, CTR_ARM9_MEM_ADDRESS, ctr_memory_arm9_size(), CTR_MEMORY_ARM9_REGION);
	CHECK_REGION(location, CTR_IO_MEM_ADDRESS, CTR_IO_MEM_SIZE, CTR_MEMORY_IO_REGION);
	CHECK_REGION(location, CTR_VRAM_ADDRESS, CTR_VRAM_SIZE, CTR_MEMORY_VRAM_REGION);
	CHECK_REGION(location, CTR_DSP_MEM_ADDRESS, CTR_DSP_MEM_SIZE, CTR_MEMORY_DSP_REGION);
	CHECK_REGION(location, CTR_AXI_WRAM_ADDRESS, CTR_AXI_WRAM_SIZE, CTR_MEMORY_AXI_WRAM_REGION);
	CHECK_REGION(location, CTR_FCRAM_ADDRESS, ctr_memory_fcram_size(), CTR_MEMORY_FCRAM_REGION);
	CHECK_REGION(location, CTR_BOOTROM_ADDRESS, CTR_BOOTROM_SIZE, CTR_MEMORY_BOOTROM_REGION);

	return CTR_MEMORY_UNKNOWN_REGION;
}

