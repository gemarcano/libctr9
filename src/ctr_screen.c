#include <ctr9/ctr_screen.h>
#include <ctr9/i2c.h>

void ctr_screen_enable_backlight(ctr_screen_enum aScreens)
{
	int data = 0;
	if (aScreens & CTR_SCREEN_TOP)
	{
		data |= 1 << 5;
	}

	if (aScreens & CTR_SCREEN_BOTTOM)
	{
		data |= 1 << 3;
	}

	i2cWriteRegister(I2C_DEV_MCU, 0x22, data);
}

void ctr_screen_disable_backlight(ctr_screen_enum aScreens)
{
	int data = 0;
	if (aScreens & CTR_SCREEN_TOP)
	{
		data |= 1 << 4;
	}

	if (aScreens & CTR_SCREEN_BOTTOM)
	{
		data |= 1 << 2;
	}
	i2cWriteRegister(I2C_DEV_MCU, 0x22, data);
}

void ctr_screen_initialize(ctr_screen *screen, uint8_t *framebuffer, size_t width, size_t height, ctr_screen_pixel format)
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

static inline uint8_t *ctr_screen_get_pixel_ptr(ctr_screen *screen, size_t x, size_t y)
{
	return screen->framebuffer + ((x * screen->height) + ((screen->height - 1 - y))) * screen->pixel_size;
}

uint32_t ctr_screen_get_pixel(ctr_screen *screen, size_t x, size_t y)
{
	uint32_t result = 0;
	uint8_t *pixel = ctr_screen_get_pixel_ptr(screen, x, y);
	for (size_t i = 0; i < screen->pixel_size; ++i)
	{
		result |= (uint32_t)pixel[i] << (i*8u);
	}
	return result;
}

void ctr_screen_set_pixel(ctr_screen *screen, size_t x, size_t y, uint32_t pixel)
{
	uint8_t *p = ctr_screen_get_pixel_ptr(screen, x, y);
	for (size_t i = 0; i < screen->pixel_size; ++i)
	{
		p[i] = pixel >> (i*8u);
	}
}

void ctr_screen_draw_bitmap(ctr_screen *screen, size_t x, size_t y, uint32_t pixel, ctr_screen_bitmap *bitmap)
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
					ctr_screen_set_pixel(screen, x + i, y + j, pixel);
				}
			}
		}
	}
}

