/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */

#ifndef CTR_GFX_H_
#define CTR_GFX_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	uint8_t (*top_framebuffer)[400][240][3];
	uint8_t (*bottom_framebuffer)[320][240][3];
	uint32_t color;
} ctr_gfx;

typedef struct
{
	size_t width, height;
	void *data;

} ctr_gfx_bitmap;

void ctr_gfx_initialize(ctr_gfx *gfx, void *top_fb, void *bottom_fb);

static inline uint32_t ctr_gfx_get_top_pixel(ctr_gfx *gfx, size_t x, size_t y);
static inline uint32_t ctr_gfx_get_bottom_pixel(ctr_gfx *gfx, size_t x, size_t y);
static inline void ctr_gfx_set_top_pixel(ctr_gfx *gfx, size_t x, size_t y, uint32_t pixel);
static inline void ctr_gfx_set_bottom_pixel(ctr_gfx *gfx, size_t x, size_t y, uint32_t pixel);
static inline void ctr_gfx_top_draw_bitmap(ctr_gfx *gfx, size_t x, size_t y, ctr_gfx_bitmap *bitmap);

typedef enum
{
	CTR_GFX_SCREEN_TOP, CTR_GFX_SCREEN_BOTTOM
} ctr_gfx_screen;

void ctr_gfx_draw_line(ctr_gfx_screen aScreen, size_t x1, size_t y1, size_t x2, size_t y2, uint32_t aPixel);

#ifdef __cplusplus
}
#endif

#include "ctr_gfx.c"

#endif//CTR_GFX_H_

