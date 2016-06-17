#include <ctr/printf.h>
#include <ctr9/ctr_system.h>
#include <ctr9/ctr_interrupt.h>
#include <ctr/hid.h>

#ifdef __thumb__
//#error
#endif

void abort_interrupt(const uint32_t *registers)
{
	uint32_t cpsr = registers[0];
	uint32_t lr = registers[14];
	const uint32_t *r = registers+1;

	printf("\n\nDATA ABORT:\n");

	printf("CPSR: 0x%08X\n", cpsr);
	printf("Abort address: 0x%08X\n", lr - 8);
	for (size_t i = 0; i < 13; ++i)
	{
		printf("r%d: 0x%08X\n", i, r[i]);
	}
	input_wait();
	ctr_system_poweroff();
}
