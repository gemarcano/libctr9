/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */

#ifndef CTR_CONSOLE_H_
#define CTR_CONSOLE_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**	@brief Initializes the console subsystem.
 *
 *	@returns 0 on success, anything else on an error. This function should
 *		suceed. If it does not, this means that the underling libc and graphics
 *		handling for libctr9 is broken.
 */
int ctr_console_initialize(void);

typedef struct
{
	unsigned int width;
	unsigned int height;

	unsigned int font_pt;

	uint32_t fg;
	uint32_t bg;

	uint32_t default_fg;
	uint32_t default_bg;

	uint16_t xpos;
	uint16_t ypos;

	uint16_t saved_xpos;
	uint16_t saved_ypos;

	bool negative;

} ctr_console;

void ctr_console_draw(char c);
uint16_t ctr_console_get_char_width(void);
uint16_t ctr_console_get_char_height(void);

#ifdef __cplusplus
}
#endif

#endif//CTR_CONSOLE_H_

