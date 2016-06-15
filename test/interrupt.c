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
	//printf("We hit an abort interrupt!\n");
	//printf("press any key to reset");
	//input_wait();
	ctr_system_poweroff();
}
