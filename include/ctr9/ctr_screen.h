/*******************************************************************************
 * Copyright (C) 2016, 2017 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */

#ifndef CTR_SCREEN_H_
#define CTR_SCREEN_H_

#include <ctr_core/ctr_core_screen.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**	@brief Enum describing screen configurations.
 */
typedef ctr_core_screen_enum ctr_screen_enum;

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
typedef ctr_core_screen_pixel ctr_screen_pixel;

/**	@brief Represents a single 3DS screen.
 */
typedef ctr_core_screen ctr_screen;

//These are defined and declared in libctr_core
//extern ctr_screen ctr_screen_top, ctr_screen_bottom;

/**	@brief Creates a screen object.
 *
 *	@param[in] framebuffer Pointer to framebuffer in memory.
 *	@param[in] width Width of the framebuffer in pixels.
 *	@param[in] height Height of the framebuffer in pixels.
 *	@param[in] format Format of the pixels in the framebuffer.
 *
 *	@returns The created screen object.
 */
ctr_screen *ctr_screen_initialize(uint8_t *framebuffer, size_t width, size_t height, ctr_screen_pixel format);

/**	@brief Destroys the given screen object.
 *
 *	@brief screen Screen object to destroy.
 *
 *	@post Screen object has been destroyed, releasing resources used by it.
 */
void ctr_screen_destroy(ctr_screen *screen);

#ifdef __cplusplus
}

#include <ctr_core/ctr_core_screen.h>

namespace ctr9
{
	template<class Pixel, size_t Width, size_t Height>
	using screen = ctr_core::screen<Pixel, Width, Height>;

	typedef ctr_core::generic_screen generic_screen;
}

#endif

#endif//CTR_SCREEN_H_

