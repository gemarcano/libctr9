#include <ctr9/ctr_screen.h>
#include <ctr_core/ctr_core_screen.h>
#include <ctr9/i2c.h>

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

void ctr_screen_clear(ctr_screen *screen, uint32_t pixel)
{
	auto &scr = *reinterpret_cast<ctr_core::generic_screen*>(screen);
	scr.clear(scr.get_pixel(0, 0) = pixel);
}

uint32_t ctr_screen_get_pixel(ctr_screen *screen, size_t x, size_t y)
{
	auto &scr = *reinterpret_cast<ctr_core::generic_screen*>(screen);
	uint32_t result = 0;
	for (std::size_t i = 0; i < scr.pixel_size(); ++i)
	{
		result |= static_cast<std::uint32_t>(scr.get_pixel(x, y)[i]) << (CHAR_BIT * i);
	}
	return result;
}

void ctr_screen_set_pixel(ctr_screen *screen, size_t x, size_t y, uint32_t pixel)
{
	auto &scr = *reinterpret_cast<ctr_core::generic_screen*>(screen);
	scr.get_pixel(x, y) = pixel;
}

void ctr_screen_draw_bitmap(ctr_screen *screen, size_t x, size_t y, uint32_t pixel, ctr_screen_bitmap *bitmap)
{
	ctr_core_screen_draw_bitmap(screen, x, y, pixel, bitmap);
}

