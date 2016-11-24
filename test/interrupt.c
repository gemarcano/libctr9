#include "interrupt.h"

#include <ctr9/ctr_system.h>
#include <ctr9/ctr_interrupt.h>
#include <ctr9/ctr_hid.h>

#include <inttypes.h>
#include <stdio.h>

#ifdef __thumb__
//#error
#endif

static void print_all_registers(uint32_t *registers)
{
	uint32_t cpsr = registers[0];
	uint32_t sp = registers[1];
	uint32_t lr = registers[2];
	uint32_t ret = registers[3];
	const uint32_t *r = registers+4;

	printf("CPSR: 0x%08"PRIX32"\n", cpsr);
	printf("SP: 0x%08"PRIX32"\n", sp);
	printf("LR: 0x%08"PRIX32"\n", lr);
	printf("Abort address: 0x%08"PRIXPTR"\n", (uintptr_t)(ret - 8));
	for (size_t i = 0; i < 13; ++i)
	{
		printf("r%d: 0x%08"PRIX32"\n", i, r[i]);
	}
}

void abort_interrupt(uint32_t *registers)
{
	printf("\n\nDATA ABORT:\n");

	print_all_registers(registers);
	input_wait();

	uint32_t cpsr = registers[0];
	if (cpsr & 0x20)
	{
		registers[3] -= 6;
		printf("Ret. Thumb: 0x%08"PRIX32"\n", registers[3]);
	}
	else
	{
		registers[3] -= 4;
		printf("Ret. ARM: 0x%08"PRIX32"\n", registers[3]);
	}
}

void undefined_instruction(uint32_t *registers)
{
	printf("\n\nUNDEFINED INSTRUCTION:\n");

	print_all_registers(registers);
	input_wait();
	ctr_system_poweroff();
}

void prefetch_abort(uint32_t *registers)
{
	printf("\n\nPREFETCH ABORT:\n");

	print_all_registers(registers);
	input_wait();
	ctr_system_poweroff();
}

