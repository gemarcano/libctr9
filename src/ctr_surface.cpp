#include <ctr9/ctr_surface.h>
#include <ctr9/ctr_screen.h>

#include <climits>

std::size_t ctr_surface_get_width(const void *surface)
{
	return ctr_core_surface_get_width(surface);
}

std::size_t ctr_surface_get_height(const void *surface)
{
	return ctr_core_surface_get_height(surface);
}

std::uint32_t ctr_surface_get_pixel(const void *surface, std::size_t x, std::size_t y)
{
	return ctr_core_surface_get_pixel(surface, x, y);
}

void ctr_surface_set_pixel(void *surface, std::size_t x, std::size_t y, std::uint32_t pixel)
{
	return ctr_core_surface_set_pixel(surface, x, y, pixel);
}

ctr_screen *ctr_surface_get_screen(void *surface)
{
	return ctr_core_surface_get_screen(surface);
}

void ctr_surface_clear(void *surface, std::uint32_t pixel)
{
	ctr_core_surface_clear(surface, pixel);
}

void ctr_surface_draw_bitmap(void *surface, std::size_t x, std::size_t y, std::uint32_t pixel, ctr_surface_bitmap *bitmap)
{
	ctr_core_surface_draw_bitmap(surface, x, y, pixel, bitmap);
}

