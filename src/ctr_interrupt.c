#include <ctr9/ctr_interrupt.h>

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


void ctr_interrupt_prepare(void)
{
	
}

