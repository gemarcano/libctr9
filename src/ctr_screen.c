#include <ctr9/ctr_screen.h>
#include <ctr9/i2c.h>

void ctr_screen_enable_backlight(ctr_screen_enum aScreens)
{
	ctr_core_screen_enable_backlight(aScreens);
}

void ctr_screen_disable_backlight(ctr_screen_enum aScreens)
{
	ctr_core_screen_disable_backlight(aScreens);
}

void ctr_screen_initialize(ctr_screen *screen, uint8_t *framebuffer, size_t width, size_t height, ctr_screen_pixel format)
{
	ctr_core_screen_initialize(screen, framebuffer, width, height, format);
}

void ctr_screen_clear(ctr_screen *screen, uint32_t pixel)
{
	ctr_core_screen_clear(screen, pixel);
}

uint32_t ctr_screen_get_pixel(ctr_screen *screen, size_t x, size_t y)
{
	return ctr_core_screen_get_pixel(screen, x, y);
}

void ctr_screen_set_pixel(ctr_screen *screen, size_t x, size_t y, uint32_t pixel)
{
	ctr_core_screen_set_pixel(screen, x, y, pixel);
}

void ctr_screen_draw_bitmap(ctr_screen *screen, size_t x, size_t y, uint32_t pixel, ctr_screen_bitmap *bitmap)
{
	ctr_core_screen_draw_bitmap(screen, x, y, pixel, bitmap);
}

