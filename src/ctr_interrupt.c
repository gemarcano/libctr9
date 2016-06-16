#include <ctr9/ctr_interrupt.h>
#include <stdint.h>

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
#define ACCESS_FUNCTION_PTR(x) (*((interrupt_function*)(x)))

void ctr_interrupt_set(ctr_interrupt_enum interrupt_type, void (*interrupt)(void))
{
	switch (interrupt_type)
	{
		case CTR_INTERRUPT_RESET:
			ACCESS_FUNCTION_PTR(0x01FF8024) = interrupt;
			break;
		case CTR_INTERRUPT_UNDEF:
			ACCESS_FUNCTION_PTR(0x01FF802C) = interrupt;
			break;
		case CTR_INTERRUPT_SWI:
			ACCESS_FUNCTION_PTR(0x01FF8034) = interrupt;
			break;
		case CTR_INTERRUPT_PREABRT:
			ACCESS_FUNCTION_PTR(0x01FF803C) = interrupt;
			break;
		case CTR_INTERRUPT_DATABRT:
			ACCESS_FUNCTION_PTR(0x01FF8044) = interrupt;
			break;
		case CTR_INTERRUPT_IRQ:
			ACCESS_FUNCTION_PTR(0x01FF804C) = interrupt;
			break;
		case CTR_INTERRUPT_FIQ:
			ACCESS_FUNCTION_PTR(0x01FF8054) = interrupt;
			break;

		default:
			break; //Set nothing
	}
}

void ctr_interrupt_prepare(void)
{
	ACCESS_FUNCTION_PTR(0x01FF8020) = (interrupt_function)0xE51FF004;
	ACCESS_FUNCTION_PTR(0x01FF8028) = (interrupt_function)0xE51FF004;
	ACCESS_FUNCTION_PTR(0x01FF8030) = (interrupt_function)0xE51FF004;
	ACCESS_FUNCTION_PTR(0x01FF8038) = (interrupt_function)0xE51FF004;
	ACCESS_FUNCTION_PTR(0x01FF8040) = (interrupt_function)0xE51FF004;
	ACCESS_FUNCTION_PTR(0x01FF8048) = (interrupt_function)0xE51FF004;
	ACCESS_FUNCTION_PTR(0x01FF8050) = (interrupt_function)0xE51FF004;

	ACCESS_FUNCTION_PTR(0x01FF8024) = (interrupt_function)0xEAFFFFFE;
	ACCESS_FUNCTION_PTR(0x01FF802C) = (interrupt_function)0xEAFFFFFE;
	ACCESS_FUNCTION_PTR(0x01FF8034) = (interrupt_function)0xEAFFFFFE;
	ACCESS_FUNCTION_PTR(0x01FF803C) = (interrupt_function)0xEAFFFFFE;
	ACCESS_FUNCTION_PTR(0x01FF8044) = (interrupt_function)0xEAFFFFFE;
	ACCESS_FUNCTION_PTR(0x01FF804C) = (interrupt_function)0xEAFFFFFE;
	ACCESS_FUNCTION_PTR(0x01FF8054) = (interrupt_function)0xEAFFFFFE;

	ACCESS_FUNCTION_PTR(0x01FF8000) = (interrupt_function)0xEA000006u;
	ACCESS_FUNCTION_PTR(0x01FF8004) = (interrupt_function)0xEA000007u;
	ACCESS_FUNCTION_PTR(0x01FF8008) = (interrupt_function)0xEA000008u;
	ACCESS_FUNCTION_PTR(0x01FF800C) = (interrupt_function)0xEA000009u;
	ACCESS_FUNCTION_PTR(0x01FF8010) = (interrupt_function)0xEA00000Au;
	ACCESS_FUNCTION_PTR(0x01FF8014) = (interrupt_function)0xEAFFFFFEu;
	ACCESS_FUNCTION_PTR(0x01FF8018) = (interrupt_function)0xEA00000Au;
	ACCESS_FUNCTION_PTR(0x01FF801C) = (interrupt_function)0xEA00000Bu;

	asm volatile (
		"mrc p15, 0, r0, c1, c0, 0 \n\t"
		"bic r0, #(1<<13) \n\t"
		"mcr p15, 0, r0, c1, c0, 0 \n\t"
	);
}

