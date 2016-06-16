#include <ctr/printf.h>
#include <ctr9/ctr_system.h>
#include <ctr9/ctr_interrupt.h>
#include <ctr/hid.h>

#ifdef __thumb__
#error
#endif

__attribute__((interrupt("IRQ")))
void abort_interrupt()
{
	uint32_t lr = 0;
	asm volatile (
		"mov %0, lr \n\t"
		: "=r"(lr)
	);
	lr -= 4;

	printf("\n\nDATA ABORT:\n");
	printf("\n\nAddress: 0x%08X\n\n", lr);
	input_wait();
	//ctr_system_poweroff();
}
