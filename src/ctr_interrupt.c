/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

#include <ctr9/ctr_interrupt.h>
#include <ctr9/ctr_system.h>
#include <ctr9/ctr_cache.h>
#include <stdint.h>

void ctr_interrupt_reset_veneer(void);
void ctr_interrupt_undef_veneer(void);
void ctr_interrupt_swi_veneer(void);
void ctr_interrupt_preabrt_veneer(void);
void ctr_interrupt_databrt_veneer(void);
void ctr_interrupt_irq_veneer(void);
void ctr_interrupt_fiq_veneer(void);

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
	ACCESS_FUNCTION_PTR(0x00008020) = (interrupt_function)0xE51FF004;
	ACCESS_FUNCTION_PTR(0x00008028) = (interrupt_function)0xE51FF004;
	ACCESS_FUNCTION_PTR(0x00008030) = (interrupt_function)0xE51FF004;
	ACCESS_FUNCTION_PTR(0x00008038) = (interrupt_function)0xE51FF004;
	ACCESS_FUNCTION_PTR(0x00008040) = (interrupt_function)0xE51FF004;
	ACCESS_FUNCTION_PTR(0x00008048) = (interrupt_function)0xE51FF004;
	ACCESS_FUNCTION_PTR(0x00008050) = (interrupt_function)0xE51FF004;

	//Pointers to veneers, which then would load the actual user provide
	//handlers
	ACCESS_FUNCTION_PTR(0x00008024) = ctr_interrupt_reset_veneer;
	ACCESS_FUNCTION_PTR(0x0000802C) = ctr_interrupt_undef_veneer;
	ACCESS_FUNCTION_PTR(0x00008034) = ctr_interrupt_swi_veneer;
	ACCESS_FUNCTION_PTR(0x0000803C) = ctr_interrupt_preabrt_veneer;
	ACCESS_FUNCTION_PTR(0x00008044) = ctr_interrupt_databrt_veneer;
	ACCESS_FUNCTION_PTR(0x0000804C) = ctr_interrupt_irq_veneer;
	ACCESS_FUNCTION_PTR(0x00008054) = ctr_interrupt_fiq_veneer;

	//Actual exception handling code. Jumps to the secondary handler.
	ACCESS_FUNCTION_PTR(0x00008000) = (interrupt_function)0xEA000006u;
	ACCESS_FUNCTION_PTR(0x00008004) = (interrupt_function)0xEA000007u;
	ACCESS_FUNCTION_PTR(0x00008008) = (interrupt_function)0xEA000008u;
	ACCESS_FUNCTION_PTR(0x0000800C) = (interrupt_function)0xEA000009u;
	ACCESS_FUNCTION_PTR(0x00008010) = (interrupt_function)0xEA00000Au;
	ACCESS_FUNCTION_PTR(0x00008014) = (interrupt_function)0xEAFFFFFEu;
	ACCESS_FUNCTION_PTR(0x00008018) = (interrupt_function)0xEA00000Au;
	ACCESS_FUNCTION_PTR(0x0000801C) = (interrupt_function)0xEA00000Bu;

	ctr_cache_clean_data_range((void*)0x8000, (void*)0x10000);
	ctr_cache_flush_instruction_range((void*)0x0, (void*)0x8000);
	ctr_cache_drain_write_buffer();

	//switch to low vectors
	asm volatile (
		"mrc p15, 0, r0, c1, c0, 0 \n\t"
		"bic r0, #(1<<13) \n\t"
		"mcr p15, 0, r0, c1, c0, 0 \n\t"
	);
}

