#include <ctr9/ctr_screen.h>
#include <ctr_core/ctr_core_screen.h>

void ctr_screen_enable_backlight(ctr_screen_enum aScreens)
{
	ctr_core_screen_enable_backlight(aScreens);
}

void ctr_screen_disable_backlight(ctr_screen_enum aScreens)
{
	ctr_core_screen_disable_backlight(aScreens);
}

ctr_screen *ctr_screen_initialize(uint8_t *framebuffer, size_t width, size_t height, ctr_screen_pixel format)
{
	return ctr_core_screen_initialize(framebuffer, width, height, format);
}

void ctr_screen_destroy(ctr_screen *screen)
{
	ctr_core_screen_destroy(screen);
}

