/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */

#ifndef CTR_SCREEN_H_
#define CTR_SCREEN_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**	@brief Enum describing screen configurations.
 */
typedef enum
{
	CTR_SCREEN_NONE = 0,
	CTR_SCREEN_TOP = 1,
	CTR_SCREEN_BOTTOM = 2,
	CTR_SCREEN_BOTH = 3
} ctr_screen_enum;

/**	@brief Enables the backlight for the screens specified.
 *
 *	@param[in] aScreens Screen backlights to enable.
 *
 *	@post Backlight for the specified screens will be enabled.
 */
void ctr_screen_enable_backlight(ctr_screen_enum aScreens);

/**	@brief Disables the backlight for the screens specified.
 *
 *	@param[in] aScreens Screen backlights to disable.
 *
 *	@post Backlight for the specified screens will be disabled.
 */
void ctr_screen_disable_backlight(ctr_screen_enum aScreens);

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
} ctr_screen_pixel;

/**	@brief Represents a single 3DS screen.
 */
typedef struct
{
	uint8_t *framebuffer;
	size_t width;
	size_t height;
	size_t pixel_size;
	ctr_screen_pixel format;
} ctr_screen;

//FIXME currently bitmaps must start at the beginning of a byte
/**	@brief Represents a single bitmap entity.
 */
typedef struct
{
	size_t width, height;
	void *data;
} ctr_screen_bitmap;

extern ctr_screen ctr_screen_top, ctr_screen_bottom;

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
void ctr_screen_initialize(ctr_screen *screen, uint8_t *framebuffer, size_t width, size_t height, ctr_screen_pixel format);

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
uint32_t ctr_screen_get_pixel(ctr_screen *screen, size_t x, size_t y);

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
void ctr_screen_set_pixel(ctr_screen *screen, size_t x, size_t y, uint32_t pixel);

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
void ctr_screen_draw_bitmap(ctr_screen *screen, size_t x, size_t y, uint32_t pixel, ctr_screen_bitmap *bitmap);

#ifdef __cplusplus
}
#endif

#endif//CTR_SCREEN_H_

#ifdef __cplusplus
extern "C" {
#endif

