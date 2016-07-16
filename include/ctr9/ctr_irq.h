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

void ctr_irq_master_disable(void);

void ctr_irq_master_enable(void);

void ctr_irq_critical_enter(void);
void ctr_irq_critical_leave(void);

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

void ctr_irq_handler(uint32_t *register_array);

void ctr_irq_initialize(void);

void ctr_irq_register(ctr_irq_enum irq, void (*handler)(void));

void ctr_irq_acknowledge(ctr_irq_enum irq);
void ctr_irq_enable(ctr_irq_enum irq);
void ctr_irq_disable(ctr_irq_enum irq);


#ifdef __cplusplus
}
#endif

#endif//CTR_INTERRUPT_H_

