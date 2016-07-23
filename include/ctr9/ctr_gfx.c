/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */

#define CTR_TOP_WIDTH           400
#define CTR_TOP_HEIGHT          240
#define CTR_BOT_WIDTH           320
#define CTR_BOT_HEIGHT          240

static inline uint32_t ctr_gfx_get_top_pixel(ctr_gfx *gfx, size_t x, size_t y)
{
	uint32_t result;
	result = (uint32_t)(*gfx->top_framebuffer)[x][CTR_TOP_HEIGHT - 1 - y][0];
	result |= (uint32_t)(*gfx->top_framebuffer)[x][CTR_TOP_HEIGHT - 1 - y][1] << 8;
	result |= (uint32_t)(*gfx->top_framebuffer)[x][CTR_TOP_HEIGHT - 1 - y][2] << 16;
	return result;
}

static inline uint32_t ctr_gfx_get_bottom_pixel(ctr_gfx *gfx, size_t x, size_t y)
{
	uint32_t result;
	result = (uint32_t)(*gfx->bottom_framebuffer)[x][CTR_BOT_HEIGHT - 1 - y][0];
	result |= (uint32_t)(*gfx->bottom_framebuffer)[x][CTR_BOT_HEIGHT - 1 - y][1] << 8;
	result |= (uint32_t)(*gfx->bottom_framebuffer)[x][CTR_BOT_HEIGHT - 1 - y][2] << 16;
	return result;
}

static inline void ctr_gfx_set_top_pixel(ctr_gfx *gfx, size_t x, size_t y, uint32_t pixel)
{
	(*gfx->top_framebuffer)[x][CTR_TOP_HEIGHT - 1 - y][0] = pixel;
	(*gfx->top_framebuffer)[x][CTR_TOP_HEIGHT - 1 - y][1] = pixel >> 8;
	(*gfx->top_framebuffer)[x][CTR_TOP_HEIGHT - 1 - y][2] = pixel >> 16;
}

static inline void ctr_gfx_set_bottom_pixel(ctr_gfx *gfx, size_t x, size_t y, uint32_t pixel)
{
	(*gfx->bottom_framebuffer)[x][CTR_BOT_HEIGHT - 1 - y][0] = pixel;
	(*gfx->bottom_framebuffer)[x][CTR_BOT_HEIGHT - 1 - y][1] = pixel >> 8;
	(*gfx->bottom_framebuffer)[x][CTR_BOT_HEIGHT - 1 - y][2] = pixel >> 16;
}

static inline void ctr_gfx_top_draw_bitmap(ctr_gfx *gfx, size_t x, size_t y, ctr_gfx_bitmap *bitmap)
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
					ctr_gfx_set_top_pixel(gfx, x + i, y + j, gfx->color);
				}
			}
		}
	}
}

#undef CTR_TOP_WIDTH
#undef CTR_TOP_HEIGHT
#undef CTR_BOT_WIDTH
#undef CTR_BOT_HEIGHT

