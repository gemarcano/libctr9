/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */

#ifndef CTR_INTERRUPT_H_
#define CTR_INTERRUPT_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**	@brief Enumerations for the different ARM9 CPU exceptions
 *
 *	These enumerations can be used as indices to set handlers for the exceptions
 *	with ctr_interrupt_set.
 */
typedef enum
{
	CTR_INTERRUPT_RESET,
	CTR_INTERRUPT_UNDEF,
	CTR_INTERRUPT_SWI,
	CTR_INTERRUPT_PREABRT,
	CTR_INTERRUPT_DATABRT,
	CTR_INTERRUPT_IRQ,
	CTR_INTERRUPT_FIQ
} ctr_interrupt_enum;

/**	@brief Sets up the exception vectors in ITCM.
 *
 *	For this function to work, ITCM must be enabled and MPU must allow for
 *	writing and reading from 0x0000 to 0x8000.
 */
void ctr_interrupt_prepare(void);

/**	@brief Sets up a handler for the given exception.
 *
 *	When the user-provided handler is called when an exception is generated,
 *	the parameter given to the function is an array with the following data
 *	from before the exception:
 *		[ cpsr, sp, lr, return address, r0-r12 ]
 *
 *	Note that lr is the lr prior to running handler. The return address is what
 *	is stored by the CPU to LR when switching into the exception handler. Refer
 *	to the ARM documentation to see how much the return address was modified
 *	relative to the PC of when the exceptiin was triggered. Any changes to any
 *	of the values in the input array WILL be written back to the corresponding
 *	registers when the exception returns, if the handler returns. This can be
 *	used to skip the instruction that caused a data abort, for example, by
 *	editing the reutnr address to skip it. Note that for return address
 *	adjustments, the mode of execution when the exception was triggered is
 *	important. Check the T bit of cpsr in the input array to determine the mode
 *	of execution when the exception was thrown.
 *
 *	@param[in] interrupt_type Enumeration determining which exception to set up.
 *	@param[in] interrupt Handler to call when the exception is fired. The handler
 *		can be either Thumb or ARM code.
 */
void ctr_interrupt_set(ctr_interrupt_enum interrupt_type, void (*interrupt)(uint32_t*));

#ifdef __cplusplus
}
#endif

#endif//CTR_INTERRUPT_H_

