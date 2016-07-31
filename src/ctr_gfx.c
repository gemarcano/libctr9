#include <ctr9/ctr_gfx.h>

void ctr_gfx_screen_initialize(ctr_gfx_screen *screen, uint8_t *framebuffer, size_t width, size_t height, ctr_gfx_pixel format)
{
	screen->framebuffer = framebuffer;
	screen->width = width;
	screen->height = height;
	screen->format = format;

	switch (format)
	{
		case CTR_GFX_PIXEL_RGBA8:
			screen->pixel_size = 4;
			break;
		case CTR_GFX_PIXEL_RGB565:
			screen->pixel_size = 2;
			break;
		case CTR_GFX_PIXEL_A1_RGB5:
			screen->pixel_size = 2;
			break;
		case CTR_GFX_PIXEL_RGBA4:
			screen->pixel_size = 2;
			break;
		case CTR_GFX_PIXEL_RGB8:
		default:
			screen->pixel_size = 3;
			break;
	}
}

void ctr_gfx_initialize(ctr_gfx *gfx, ctr_gfx_screen *top, ctr_gfx_screen *bot)
{
	gfx->top_screen = *top;
	gfx->bottom_screen = *bot;
	gfx->color = 0xFFFFFF;
}

static inline uint8_t *ctr_gfx_screen_get_pixel_ptr(ctr_gfx_screen *screen, size_t x, size_t y)
{
	return screen->framebuffer + ((x * screen->height) + ((screen->height - 1 - y))) * screen->pixel_size;
}

static inline uint32_t ctr_gfx_screen_get_pixel(ctr_gfx_screen *screen, size_t x, size_t y)
{
	uint32_t result = 0;
	uint8_t *pixel = ctr_gfx_screen_get_pixel_ptr(screen, x, y);
	for (size_t i = 0; i < screen->pixel_size; ++i)
	{
		result |= (uint32_t)pixel[i] << (i*8u);
	}
	return result;
}

static inline void ctr_gfx_screen_set_pixel(ctr_gfx_screen *screen, size_t x, size_t y, uint32_t pixel)
{
	uint8_t *p = ctr_gfx_screen_get_pixel_ptr(screen, x, y);
	for (size_t i = 0; i < screen->pixel_size; ++i)
	{
		p[i] = pixel >> (i*8u);
	}
}

uint32_t ctr_gfx_get_top_pixel(ctr_gfx *gfx, size_t x, size_t y)
{
	return ctr_gfx_screen_get_pixel(&gfx->top_screen, x, y);
}

uint32_t ctr_gfx_get_bottom_pixel(ctr_gfx *gfx, size_t x, size_t y)
{
	return ctr_gfx_screen_get_pixel(&gfx->bottom_screen, x, y);
}

void ctr_gfx_set_top_pixel(ctr_gfx *gfx, size_t x, size_t y, uint32_t pixel)
{
	ctr_gfx_screen_set_pixel(&gfx->top_screen, x, y, pixel);
}

void ctr_gfx_set_bottom_pixel(ctr_gfx *gfx, size_t x, size_t y, uint32_t pixel)
{
	ctr_gfx_screen_set_pixel(&gfx->bottom_screen, x, y, pixel);
}

static inline void ctr_gfx_screen_draw_bitmap(ctr_gfx_screen *screen, size_t x, size_t y, uint32_t pixel, ctr_gfx_bitmap *bitmap)
{
	if (bitmap->width && bitmap->height)
	{
		uint8_t *data = bitmap->data;
		size_t width_bytes = bitmap->width / 8u;
		if (bitmap->width % 8) width_bytes++;

		for (size_t i = 0; i < bitmap->width; ++i)
		{
			size_t byte = i/8;
			size_t bit = 7 - i%8;
			for (size_t j = bitmap->height-1; j < bitmap->height; --j)
			{
				if (data[byte + width_bytes * j] & (1u << bit))
				{
					ctr_gfx_screen_set_pixel(screen, x + i, y + j, pixel);
				}
			}
		}
	}
}

void ctr_gfx_top_draw_bitmap(ctr_gfx *gfx, size_t x, size_t y, ctr_gfx_bitmap *bitmap)
{
	ctr_gfx_screen_draw_bitmap(&gfx->top_screen, x, y, gfx->color, bitmap);
}

void ctr_gfx_bottom_draw_bitmap(ctr_gfx *gfx, size_t x, size_t y, ctr_gfx_bitmap *bitmap)
{
	ctr_gfx_screen_draw_bitmap(&gfx->bottom_screen, x, y, gfx->color, bitmap);
}

