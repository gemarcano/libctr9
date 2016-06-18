/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */

#include <ctr9/ctr_interrupt.h>
#include <ctr9/ctr_system.h>
#include <stdint.h>

void ctr_interrupt_reset_veneer(void);
void ctr_interrupt_undef_veneer(void);
void ctr_interrupt_swi_veneer(void);
void ctr_interrupt_preabrt_veneer(void);
void ctr_interrupt_databrt_veneer(void);
void ctr_interrupt_irq_veneer(void);
void ctr_interrupt_fiq_veneer(void);

void ctr_interrupt_global_disable(void)
{
	asm volatile(
		"mrs r0, cpsr \n\t"
		"orr r0, r0, #0x80 \n\t" //disable IRQ, bit 7, active low
		"msr cpsr_c, r0 \n\t"
		::: "r0"
	);
}

void ctr_interrupt_global_enable(void)
{
	asm volatile (
		"mrs r0, cpsr \n\t"
		"bic r0, r0, #0x80 \n\t" //enable IRQ, bit 7, active low
		"msr cpsr_c, r0 \n\t"
		::: "r0"
	);

}

typedef void (*interrupt_function)(void);
typedef void (*ctr_interrupt_handler)(uint32_t* register_array);

ctr_interrupt_handler ctr_interrupt_handlers[7] = {0};

#define ACCESS_FUNCTION_PTR(x) (*((interrupt_function*)(x)))

void ctr_interrupt_set(ctr_interrupt_enum interrupt_type, ctr_interrupt_handler handler)
{
	switch (interrupt_type)
	{
		case CTR_INTERRUPT_RESET:
		case CTR_INTERRUPT_UNDEF:
		case CTR_INTERRUPT_SWI:
		case CTR_INTERRUPT_PREABRT:
		case CTR_INTERRUPT_DATABRT:
		case CTR_INTERRUPT_IRQ:
		case CTR_INTERRUPT_FIQ:
			ctr_interrupt_handlers[interrupt_type] = handler;
			break;

		default:
			break; //Set nothing
	}
}

void ctr_interrupt_prepare(void)
{
	//Secondary handler. Read payload address from next word
	ACCESS_FUNCTION_PTR(0x01FF8020) = (interrupt_function)0xE51FF004;
	ACCESS_FUNCTION_PTR(0x01FF8028) = (interrupt_function)0xE51FF004;
	ACCESS_FUNCTION_PTR(0x01FF8030) = (interrupt_function)0xE51FF004;
	ACCESS_FUNCTION_PTR(0x01FF8038) = (interrupt_function)0xE51FF004;
	ACCESS_FUNCTION_PTR(0x01FF8040) = (interrupt_function)0xE51FF004;
	ACCESS_FUNCTION_PTR(0x01FF8048) = (interrupt_function)0xE51FF004;
	ACCESS_FUNCTION_PTR(0x01FF8050) = (interrupt_function)0xE51FF004;

	//Pointers to veneers, which then would load the actual user provide
	//handlers
	ACCESS_FUNCTION_PTR(0x01FF8024) = ctr_interrupt_reset_veneer;
	ACCESS_FUNCTION_PTR(0x01FF802C) = ctr_interrupt_undef_veneer;
	ACCESS_FUNCTION_PTR(0x01FF8034) = ctr_interrupt_swi_veneer;
	ACCESS_FUNCTION_PTR(0x01FF803C) = ctr_interrupt_preabrt_veneer;
	ACCESS_FUNCTION_PTR(0x01FF8044) = ctr_interrupt_databrt_veneer;
	ACCESS_FUNCTION_PTR(0x01FF804C) = ctr_interrupt_irq_veneer;
	ACCESS_FUNCTION_PTR(0x01FF8054) = ctr_interrupt_fiq_veneer;

	//Actual exception handling code. Jumps to the secondary handler.
	ACCESS_FUNCTION_PTR(0x01FF8000) = (interrupt_function)0xEA000006u;
	ACCESS_FUNCTION_PTR(0x01FF8004) = (interrupt_function)0xEA000007u;
	ACCESS_FUNCTION_PTR(0x01FF8008) = (interrupt_function)0xEA000008u;
	ACCESS_FUNCTION_PTR(0x01FF800C) = (interrupt_function)0xEA000009u;
	ACCESS_FUNCTION_PTR(0x01FF8010) = (interrupt_function)0xEA00000Au;
	ACCESS_FUNCTION_PTR(0x01FF8014) = (interrupt_function)0xEAFFFFFEu;
	ACCESS_FUNCTION_PTR(0x01FF8018) = (interrupt_function)0xEA00000Au;
	ACCESS_FUNCTION_PTR(0x01FF801C) = (interrupt_function)0xEA00000Bu;

	ctr_flush_cache();

	//switch to low vectors
	asm volatile (
		"mrc p15, 0, r0, c1, c0, 0 \n\t"
		"bic r0, #(1<<13) \n\t"
		"mcr p15, 0, r0, c1, c0, 0 \n\t"
	);
}

