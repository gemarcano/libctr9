/*******************************************************************************
 * Copyright (C) 2016, 2017 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

#include <ctr9/ctr_irq.h>
#include <ctr9/ctr_interrupt.h>
#include <ctr9/ctr_system.h>
#include <stdint.h>
#include <stddef.h>

__attribute__((noinline))
void ctr_irq_master_disable(void)
{
	asm volatile(
		"mrs r0, cpsr \n\t"
		"orr r0, r0, #0x80 \n\t" //disable IRQ, bit 7, active low
		"msr cpsr_c, r0 \n\t"
		::: "r0"
	);
}

__attribute__((noinline))
void ctr_irq_master_enable(void)
{
	asm volatile (
		"mrs r0, cpsr \n\t"
		"bic r0, r0, #0x80 \n\t" //enable IRQ, bit 7, active low
		"msr cpsr_c, r0 \n\t"
		::: "r0"
	);

}

static uint32_t critical_count = 0;
static uint32_t saved_status = 0;

__attribute__((noinline))
void ctr_irq_critical_enter(void)
{
	uint32_t status = 0;
	asm volatile(
		"mrs r0, cpsr \n\t"
		"mov %0, r0 \n\t"
		"orr r0, r0, #0x80 \n\t" //disable IRQ, bit 7, active low
		"msr cpsr_c, r0 \n\t"
		:"+r"(status)
		:: "r0"
	);

	ctr_irq_master_disable();
	if (!critical_count++)
	{
		saved_status = status;
	}
}

__attribute__((noinline))
void ctr_irq_critical_exit(void)
{
	if (!--critical_count)
	{
		uint32_t status = saved_status & 0x80;
		saved_status = status;
		asm volatile(
			"mrs r0, cpsr \n\t"
			"bic r0, r0, #0x80 \n\t" //enable IRQ, bit 7, active low
			"add r0, r0, %0 \n\t" //Restore saved
			"msr cpsr_c, r0 \n\t"
			::"r"(status)
			: "r0"
		);
	}

}

typedef void (*ctr_interrupt_handler)(uint32_t *register_array, void *data);

static void (*ctr_irq_handlers[29])(void*) = { NULL };
static void *ctr_irq_data[29] = { NULL };

static void ctr_irq_handler(uint32_t *register_array, void *data)
{
	//Check IRQ_IF
	uint32_t pending = IRQ_IF_REG;
	for (size_t i = 0; i < 30; ++i)
	{
		if (pending & (1u << i) && ctr_irq_handlers[i])
		{
			ctr_irq_handlers[i](ctr_irq_data[i]);
		}
	}
	register_array[3] -= 4;
}

void ctr_irq_initialize(void)
{
	ctr_interrupt_set(CTR_INTERRUPT_IRQ, ctr_irq_handler, NULL);
}

void ctr_irq_register(ctr_irq_enum irq, void (*handler)(void*), void *data)
{
	if (irq < 29u)
	{
		ctr_irq_handlers[irq] = handler;
	}
}

void ctr_irq_acknowledge(ctr_irq_enum irq)
{
	if (irq < 29u)
	{
		IRQ_IF_REG = 1u << irq;
	}
}

void ctr_irq_enable(ctr_irq_enum irq)
{
	if (irq < 29u)
	{
		IRQ_IE_REG |= 1u << irq;
	}
}

void ctr_irq_disable(ctr_irq_enum irq)
{
	if (irq < 29u)
	{
		IRQ_IE_REG &= ~(1u << irq);
	}
}

