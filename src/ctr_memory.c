/*******************************************************************************
 * Copyright (C) 2016, 2017 Gabriel Marcano
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
int ctr_memory_itcm_state_(void);
int ctr_memory_dtcm_state_(void);
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
	//FIXME doesn't this depend on a register also?
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
	reg &= ~(0x1Fu << 1);
	reg |= 0x3E & (size << 1);
	ctr_memory_set_itcm_register_(reg);
}

void ctr_memory_set_dtcm_size(ctr_memory_tcm_size size)
{
	uint32_t reg = ctr_memory_get_dtcm_register_();
	reg &= ~(0x1Fu << 1);
	reg |= 0x3E & (size << 1);
	ctr_memory_set_dtcm_register_(reg);
}

uintptr_t ctr_memory_dtcm_address(void)
{
	uint32_t reg = ctr_memory_get_dtcm_register_();
	return reg & ~(uintptr_t)0xFFFu;
}

void ctr_memory_set_dtcm_address(uintptr_t base_address)
{
	uint32_t reg = ctr_memory_get_dtcm_register_();
	reg &= 0x3Fu;
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
	region_end = region_start + (SIZE - 1);\
	if (LOC >= region_start && LOC <= region_end)\
	{\
		return REG;\
	}

#define CHECK_TCM_REGION(LOC, START, SIZE_1, REG) \
	region_start = START;\
	region_end = 0xFFFFFFFF - START >= SIZE_1 ? START + SIZE_1 : 0xFFFFFFFF;\
	if (LOC >= region_start && LOC <= region_end)\
	{\
		return REG;\
	}

static uintptr_t tcm_size_map[] ={
	0xFFF,
	0x1FFF,
	0x3FFF,
	0x7FFF,
	0xFFFF,
	0x1FFFF,
	0x3FFFF,
	0x7FFFF,
	0xFFFFF,
	0x1FFFFF,
	0x3FFFFF,
	0x7FFFFF,
	0xFFFFFF,
	0x1FFFFFF,
	0x3FFFFFF,
	0x7FFFFFF,
	0xFFFFFFF,
	0x1FFFFFFF,
	0x3FFFFFFF,
	0x7FFFFFFF,
	0xFFFFFFFF
};

#include <stdio.h>

ctr_memory_region ctr_memory_get_region(uintptr_t location)
{
	uintptr_t region_start, region_end;

	if (ctr_memory_itcm_state())
	{
		CHECK_TCM_REGION(location, CTR_ITCM_ADDRESS, tcm_size_map[ctr_memory_itcm_size()-3], CTR_MEMORY_ITCM_REGION);
	}

	if (ctr_memory_dtcm_state())
	{
		CHECK_TCM_REGION(location, ctr_memory_dtcm_address(), tcm_size_map[ctr_memory_dtcm_size()-3], CTR_MEMORY_DTCM_REGION);
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

