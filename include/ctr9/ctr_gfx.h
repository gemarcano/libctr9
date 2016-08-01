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

/**	@brief Represents the different framebuffer formats supported by the 3DS
 *		GPU.
 */
typedef enum
{
	CTR_GFX_PIXEL_RGBA8,
	CTR_GFX_PIXEL_RGB8,
	CTR_GFX_PIXEL_RGB565,
	CTR_GFX_PIXEL_A1_RGB5,
	CTR_GFX_PIXEL_RGBA4
} ctr_gfx_pixel;

/**	@brief Represents a single 3DS screen.
 */
typedef struct
{
	uint8_t *framebuffer;
	size_t width;
	size_t height;
	size_t pixel_size;
	ctr_gfx_pixel format;
} ctr_gfx_screen;

//FIXME currently bitmaps must start at the beginning of a byte
/**	@brief Represents a single bitmap entity.
 */
typedef struct
{
	size_t width, height;
	void *data;
} ctr_gfx_bitmap;

/**	@brief Initializes the given screen.
 *
 *	@param[out] screen Screen structure to initialize.
 *	@param[in] framebuffer Pointer to framebuffer in memory.
 *	@param[in] width Width of the framebuffer in pixels.
 *	@param[in] height Height of the framebuffer in pixels.
 *	@param[in] format Format of the pixels in the framebuffer.
 *
 *	@post The screen object has been initialized and is ready for use.
 */
void ctr_gfx_screen_initialize(ctr_gfx_screen *screen, uint8_t *framebuffer, size_t width, size_t height, ctr_gfx_pixel format);

//FIXME these functions are bound to change. I can't say I'm happy about the API

/**	@brief Retrieves the pixel specified from the given screen.
 *
 *	Note that only the actual size of the pixel, according to the current pixel,
 *	format is guaranteed to be correct. Any bits beyond the actual pixel size
 *	are not guaranteed to be anything useful. E.g. when in one of the 16 bit
 *	modes, the upper 16 bits of the return can be garbage and not zero.
 *	Accessing a pixel outside of the framebuffer invokes undefined behavior.
 *
 *	@param[in] screen Screen to access. Must be initialized.
 *	@param[in] x X location of pixel.
 *	@param[in] y Y location of pixel.
 *
 *	@returns The pixel value at the specified location. The current screen
 *	format governs how many of the returned bits mean anything.
 */
uint32_t ctr_gfx_screen_get_pixel(ctr_gfx_screen *screen, size_t x, size_t y);

/**	@brief Sets the pixel to the value specified in the given screen.
 *
 *	Note that only the actual size of the pixel, according to the current pixel,
 *	format is used for setting the pixel value. Any bits beyond the actual pixel
 *	are ignored. Accessing a pixel outside of the framebuffer invokes undefined
 *	behavior.
 *
 *	@param[in] screen Screen to use for operation. Must be initialized.
 *	@param[in] x X location of pixel.
 *	@param[in] y Y location of pixel.
 *	@param[in] pixel Pixel value to set.
 *
 *	@post The pixel value specified has been set at the given position.
 */
void ctr_gfx_screen_set_pixel(ctr_gfx_screen *screen, size_t x, size_t y, uint32_t pixel);

/**	@brief Draws the given bitmap at the given location in the given screen.
 *
 *	The coordinates in the framebuffer given dictate where the upper left corner
 *	of the bitmap will be drawn. If the bitmap is placed in the framebuffer such
 *	that it will overflow outside of the framebuffer, this invokes undefined
 *	behavior.
 *
 *	@param[in] screen Screen to use for operation. Must be initialized.
 *	@param[in] x X location of pixel.
 *	@param[in] y Y location of pixel.
 *	@param[in] pixel Pixel to use as color when drawing bitmap.
 *	@param[in] bitmap Bitmap to draw.
 *
 *	@post The bitmap has been drawn at the given location.
 */
void ctr_gfx_screen_draw_bitmap(ctr_gfx_screen *screen, size_t x, size_t y, uint32_t pixel, ctr_gfx_bitmap *bitmap);

#ifdef __cplusplus
}
#endif

#endif//CTR_GFX_H_

