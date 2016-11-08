#include <ctr9/io/ctr_console.h>
#include <ctr9/io/ctr_drives.h>

#include <ctr/console.h>
#include <ctr/draw.h>

void __attribute__((weak)) ctr_libctr9_init(void);

void __attribute__((weak)) ctr_libctr9_init(void)
{
	draw_init((draw_s*)0x23FFFE00);
	console_init(0xFFFFFF, 0);
	ctr_console_initialize();
	ctr_drives_initialize();
}

