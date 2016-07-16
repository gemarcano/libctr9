/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */

#ifndef CTR_IRQ_H_
#define CTR_IRQ_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**	@brief Enable IRQs globally.
 */
void ctr_irq_master_disable(void);

/**	@brief Disable IRQs globally.
 */
void ctr_irq_master_enable(void);

/**	@brief Enters a critical section.
 *
 *	A critical section is defined as a section of code that cannot be
 *	interrupted by anything else. This allows for exclusive access to certain
 *	variables, for example, if they have to be shared between interrupts and
 *	non-interrupt code.
 *
 *	This implementation of critical sections is re-entrant, meaning it is safe
 *	to nest entries to a critical section. There is a limitation to how deep the
 *	nesting can be (currently 2^32-1). Exceeding the maximum level of nesting
 *	will yield undefined behavior.
 *
 *	@post A cricial section has been entered. No other IRQs can interrupt the
 *		currently executing code until the section is exited.
 */
void ctr_irq_critical_enter(void);

/**	@brief Exits a critical section.
 *
 *	If this function is called and execution is not in a critical section, this
 *	invokes undefined behavior.
 *
 *	@post A critical section could have been exited if the section isn't nested.
 *		In the case of nested critical sections, it merely keeps track of how
 *		nested it is. The section is exited when it determines there are no more
 *		nested sections to consider. If called when not in a critical section,
 *		it invokes undefined behavior.
 */
void ctr_irq_critical_exit(void);

/**	@brief Enums representing all the possible 3DS IRQ interrupts.
 */
typedef enum
{
	CTR_IRQ_DMAC_1_0 = 1,
	CTR_IRQ_DMAC_1_1 = 1,
	CTR_IRQ_DMAC_1_2 = 2,
	CTR_IRQ_DMAC_1_3 = 3,
	CTR_IRQ_DMAC_1_4 = 4,
	CTR_IRQ_DMAC_1_5 = 5,
	CTR_IRQ_DMAC_1_6 = 6,
	CTR_IRQ_DMAC_1_7 = 7,
	CTR_IRQ_TIMER_0 = 8,
	CTR_IRQ_TIMER_1 = 9,
	CTR_IRQ_TIMER_2 = 10,
	CTR_IRQ_TIMER_3 = 11,
	CTR_IRQ_PXI_SYNC = 12,
	CTR_IRQ_PXI_NOT_FULL = 13,
	CTR_IRQ_PXI_NOT_EMPTY = 15,
	CTR_IRQ_AES = 15,
	CTR_IRQ_SDIO_1 = 16,
	CTR_IRQ_SDIO_1_ASYNC = 17,
	CTR_IRQ_SDIO_3 = 18,
	CTR_IRQ_SDIO_3_ASYNC = 19,
	CTR_IRQ_DEBUG_RECV = 20,
	CTR_IRQ_DEBUG_SEND = 21,
	CTR_IRQ_RSA = 22,
	CTR_IRQ_CTR_CARD_1 = 23,
	CTR_IRQ_CTR_CARD_2 = 24,
	CTR_IRQ_CGC = 25,
	CTR_IRQ_GCC_DET = 26,
	CTR_IRQ_DS_CARD = 27,
	CTR_IRQ_DMAC_2 = 28,
	CTR_IRQ_DMAC_2_ABORT = 29
} ctr_irq_enum;

#define IRQ_IF_REG (*(volatile uint32_t*)0x10001004)
#define IRQ_IE_REG (*(volatile uint32_t*)0x10001000)

/**	@brief Initializes the IRQ subsystem.
 *
 *	Sets up the internal IRQ handler via ctr_interrupt support.
 *
 *	Whenever an IRQ is raised, the handler checks the 3DS IRQ
 *	registers to determine which ones need to be serviced, and
 *	then calls the registered IRQ handlers. If no handler is
 *	registered, nothing is executed.
 *
 *	@post The IRQ subsystem is initialized and handlers can be
 *		installed via ctr_irq_register.
 */
void ctr_irq_initialize(void);

/**	@brief Registers an IRQ handler.
 *
 *	This function registers an IRQ handler so that when the specified IRQ is
 *	triggered the handler is called. Note that acknowledging the IRQ is left
 *	to the handler.
 *
 *	@param[in] irq IRQ to register.
 *	@param[in] handler Function to call to handle the IRQ.
 *
 *	@post Handler is registered and will be called when an IRQ is raised and the
 *		specified IRQ bit in the 3DS IRQ registers is set.
 */
void ctr_irq_register(ctr_irq_enum irq, void (*handler)(void));

/**	@brief Acknowledges an IRQ.
 *
 *	This function acknowledges the given IRQ by writing to the corresponding bit
 *	on the 3DS IRQ status register (IRQ_IF). The interrupt pending bit will not
 *	be reset unless the interrupt is acknowledged. Acknowledging an interrupt is
 *	typically done once by the external IRQ handler once the condition that
 *	triggered the interrupt is addressed.
 *
 *	@param[in] irq IRQ to acknowledge.
 */
void ctr_irq_acknowledge(ctr_irq_enum irq);

/**	@brief Enables the given IRQ.
 *
 *	@param irq IRQ to enable.
 *
 *	@post The IRQ requested will now be able to trigger IRQs if a handler is
 *		registered.
 */
void ctr_irq_enable(ctr_irq_enum irq);

/**	@brief Disables the given IRQ.
 *
 *	@param irq IRQ to disable.
 *
 *	@post The IRQ requested will now not be able to trigger IRQs.
 */
void ctr_irq_disable(ctr_irq_enum irq);

#ifdef __cplusplus
}
#endif

#endif//CTR_INTERRUPT_H_

