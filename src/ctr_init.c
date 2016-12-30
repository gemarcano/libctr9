#include <ctr9/io/ctr_console.h>
#include <ctr9/io/ctr_drives.h>
#include <ctr9/ctr_screen.h>
#include <ctr9/ctr_freetype.h>
#include <ctr_core/ctr_core_pxi.h>

typedef struct draw_s
{
	void* top_left;
	void* top_right;
	void* sub;
} draw_s;

void __attribute__((weak)) ctr_libctr9_init(void);

void __attribute__((weak)) ctr_libctr9_init(void)
{
	ctr_core_pxi_change_base((volatile uint32_t*)0x10008000);
	draw_s *cakehax_fbs = (draw_s*)0x23FFFE00;
	ctr_screen_initialize(&ctr_screen_top, cakehax_fbs->top_left, 400, 240, CTR_GFX_PIXEL_RGB8);
	ctr_screen_initialize(&ctr_screen_bottom, cakehax_fbs->sub, 320, 240, CTR_GFX_PIXEL_RGB8);
	ctr_console_initialize(&ctr_screen_top);

	ctr_freetype_initialize();
	ctr_drives_initialize();
}

