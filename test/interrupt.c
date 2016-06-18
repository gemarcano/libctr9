#include <ctr/printf.h>
#include <ctr9/ctr_system.h>
#include <ctr9/ctr_interrupt.h>
#include <ctr/hid.h>

#ifdef __thumb__
//#error
#endif

void abort_interrupt(uint32_t *registers)
{
	uint32_t cpsr = registers[0];
	uint32_t sp = registers[1];
	uint32_t lr = registers[2];
	const uint32_t *r = registers+3;

	printf("\n\nDATA ABORT:\n");

	printf("CPSR: 0x%08X\n", cpsr);
	printf("SP: 0x%08X\n", sp);
	printf("Abort address: 0x%08X\n", lr - 8);
	for (size_t i = 0; i < 13; ++i)
	{
		printf("r%d: 0x%08X\n", i, r[i]);
	}
	input_wait();

	if (cpsr & 0x20)
	{
		registers[2] -= 6;
		printf("Ret. Thumb: 0x%08X\n", registers[2]);
	}
	else
	{
		registers[2] -= 4;
		printf("Ret. ARM: 0x%08X\n", registers[2]);
	}

}
