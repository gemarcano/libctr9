/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */

#ifndef CTR_MEMORY_H_
#define CTR_MEMORY_H_

#include <stdint.h>
#include <stdbool.h>

#define CTR_ITCM_ADDRESS 0x00000000u

#define CTR_ARM9_MEM_ADDRESS 0x08000000u
#define CTR_ARM9_MEM_O3DS_SIZE 0x00100000u
#define CTR_ARM9_MEM_N3DS_SIZE 0x00180000u //N3DS only, relies on a config register for extra 0x00080000 bytes

#define CTR_IO_MEM_ADDRESS 0x10000000u
#define CTR_IO_MEM_SIZE 0x08000000u

#define CTR_VRAM_ADDRESS 0x18000000u
#define CTR_VRAM_SIZE 0x00600000u

#define CTR_DSP_MEM_ADDRESS 0x1FF00000u
#define CTR_DSP_MEM_SIZE 0x00080000u

#define CTR_AXI_WRAM_ADDRESS 0x1FF80000u
#define CTR_AXI_WRAM_SIZE 0x00080000u

#define CTR_FCRAM_ADDRESS 0x20000000u
#define CTR_FCRAM_O3DS_SIZE 0x08000000u
#define CTR_FCRAM_N3DS_SIZE 0x10000000u

#define CTR_BOOTROM_ADDRESS 0xFFFF0000u
#define CTR_BOOTROM_SIZE 0x00010000u

typedef enum
{
	CTR_MEMORY_ITCM_REGION,
	CTR_MEMORY_DTCM_REGION,
	CTR_MEMORY_ARM9_REGION,
	CTR_MEMORY_IO_REGION,
	CTR_MEMORY_VRAM_REGION,
	CTR_MEMORY_DSP_REGION,
	CTR_MEMORY_AXI_WRAM_REGION,
	CTR_MEMORY_FCRAM_REGION,
	CTR_MEMORY_BOOTROM_REGION,
	CTR_MEMORY_UNKNOWN_REGION
} ctr_memory_region;

typedef enum
{
	CTR_MEMORY_TCM_4KB = 0x3,
	CTR_MEMORY_TCM_8KB = 0x4,
	CTR_MEMORY_TCM_16KB = 0x5,
	CTR_MEMORY_TCM_32KB = 0x6,
	CTR_MEMORY_TCM_64KB = 0x7,
	CTR_MEMORY_TCM_128KB = 0x8,
	CTR_MEMORY_TCM_256KB = 0x9,
	CTR_MEMORY_TCM_512KB = 0xA,
	CTR_MEMORY_TCM_1MB = 0xB,
	CTR_MEMORY_TCM_2MB = 0xC,
	CTR_MEMORY_TCM_4MB = 0xF,
	CTR_MEMORY_TCM_8MB = 0xE,
	CTR_MEMORY_TCM_16MB = 0xF,
	CTR_MEMORY_TCM_32MB = 0x10,
	CTR_MEMORY_TCM_64MB = 0x11,
	CTR_MEMORY_TCM_128MB = 0x12,
	CTR_MEMORY_TCM_256MB = 0x13,
	CTR_MEMORY_TCM_512MB = 0x14,
	CTR_MEMORY_TCM_1GB = 0x15,
	CTR_MEMORY_TCM_2GB = 0x16,
	CTR_MEMORY_TCM_4GB = 0x17,

} ctr_memory_tcm_size;

/**	@brief Returns the size of ITCM.
 *
 *	@returns The size of ITCM.
 */
ctr_memory_tcm_size ctr_memory_itcm_size(void);

/**	@brief Returns the size of DTCM.
 *
 *	@returns The size of DTCM.
 */
ctr_memory_tcm_size ctr_memory_dtcm_size(void);

/**	@brief Returns the address where DTCM is/would be loaded to.
 *
 *	@returns The address where DTCM is/would be loaded to.
 */
uintptr_t ctr_memory_dtcm_address(void);

/**	@brief Returns the size of the ARM9 internal memory.
 *
 *	@returns The size of the ARM9 internal memory.
 */
uintptr_t ctr_memory_arm9_size(void);

/**	@brief Returns the size of the FCRAM.
 *
 * 	@returns The size of the FCRAM.
 */
uintptr_t ctr_memory_fcram_size(void);

/**	@Brief Sets the size of the ITCM.
 *
 *	@param[in] size The desired size of the ITCM.
 *
 *	@post The ITCM size is set to the desired value. Note it must be one of the
 *		ctr_memory_tcm_size enums, else behavior is undefined.
 */
void ctr_memory_set_itcm_size(ctr_memory_tcm_size size);

/**	@Brief Sets the size of the DTCM.
 *
 *	@param[in] size The desired size of the DTCM.
 *
 *	@post The DTCM size is set to the desired value. Note it must be one of the
 *		ctr_memory_tcm_size enums, else behavior is undefined.
 */
void ctr_memory_set_dtcm_size(ctr_memory_tcm_size size);

/**	@brief Sets the base address of the DTCM.
 *
 *	@param[in] base_address The address to set the DTCM base to. The lower 12
 *		bits of the address are ignored.
 *
 *	@post The DTCM base address is set to the value specified, with the lower 12
 *		bits of the given value ignored.
 */
void ctr_memory_set_dtcm_base_address(uintptr_t base_address);

/**	@brief Returns the DTCM base address.
 *
 *	@returns The DTCM base address.
 */
uintptr_t ctr_memory_dtcm_base_address(void);

/**	@brief Returns whether ITCM is enabled or not.
 *
 *	@returns True if ITCM is enabled, false otherwise.
 */
bool ctr_memory_itcm_state(void);

/**	@brief Returns whether DTCM is enabled or not.
 *
 *	@returns True if DTCM is enabled, false otherwise.
 */
bool ctr_memory_dtcm_state(void);

/**	@brief Sets the state of the ITCM.
 *
 *	@param[in] state State to set the ITCM to.
 *
 *	@post The ITCM is enabled if the state is set to true, disabled if set to
 *		false.
 */
void ctr_memory_set_itcm_state(bool state);

/**	@brief Sets the state of the DTCM.
 *
 *	@param[in] state State to set the DTCM to.
 *
 *	@post The DTCM is enabled if the state is set to true, disabled if set to
 *		false.
 */
void ctr_memory_set_dtcm_state(bool state);

/**	@brief Returns the region the given location in memory is found in.
 *
 *	@param[in] location Memory address to check in which region of memory it
 *		resides within.
 *
 *	@returns The region the given location in memory resides within.
 *
 */
ctr_memory_region ctr_memory_get_region(uintptr_t location);

#endif//CTR_MEMORY_H_

