#include <ctr9/io/ctr_console.h>
#include <ctr9/io/ctr_drives.h>
#include <ctr9/ctr_screen.h>
#include <ctr9/ctr_freetype.h>
#include <ctr9/ctr_pxi.h>
#include <ctr9/ctr_interrupt.h>

typedef struct draw_s
{
	void* top_left;
	void* top_right;
	void* sub;
} draw_s;

void __attribute__((weak)) ctr_libctr9_init(void);

void __attribute__((weak)) ctr_libctr9_init(void)
{
	ctr_pxi_initialize();
	// Pointers to the framebuffers for the top and bottom screens
	void* top, *bottom;
	// Get them from ARM11 via PXI
	while (!ctr_pxi_pop(&top));
	while (!ctr_pxi_pop(&bottom));
	ctr_screen_initialize(&ctr_screen_top, top, 400, 240, CTR_GFX_PIXEL_RGB8);
	ctr_screen_initialize(&ctr_screen_bottom, bottom, 320, 240, CTR_GFX_PIXEL_RGB8);
	ctr_console_initialize(&ctr_screen_bottom);

	ctr_freetype_initialize();
	ctr_drives_initialize();
}

