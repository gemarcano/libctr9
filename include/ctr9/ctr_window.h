/*******************************************************************************
 * Copyright (C) 2016, 2017 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */

#ifndef CTR_WINDOW_H_
#define CTR_WINDOW_H_

#include <ctr_core/ctr_core_window.h>
#include <ctr9/ctr_screen.h>

#ifdef __cplusplus
extern "C" {
#endif

/**	@brief Represents a single window within the screen.
 */
typedef ctr_core_window ctr_window;

/**	@brief Creates a new window object.
 *
 *	@param[in] parent Parent screen, to hold the window.
 *	@param[in] x X position in parent of the window.
 *	@param[in] y Y position in parent of the window.
 *	@param[in] width Width of the window in pixels.
 *	@param[in] height Height of the window in pixels.
 *
 *	@returns A new window object.
 */
ctr_window *ctr_window_initialize(ctr_screen *parent, size_t x, size_t y, size_t width, size_t height);

/**	@brief Destroys the given window object.
 *
 *	@param[in] window Window object to destroy.
 *
 *	@post Resources held by window object are freed. Any further use will lead
 *		to undefined behavior.
 */
void ctr_window_destroy(ctr_window *window);

#ifdef __cplusplus
}

namespace ctr9
{
	template<class Parent, std::size_t Width, std::size_t Height>
	using window = ctr_core::window<Parent, Width, Height>;

	typedef ctr_core::generic_window generic_window;
}

#endif

#endif//CTR_WINDOW_H_

