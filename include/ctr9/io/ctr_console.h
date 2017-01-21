/*******************************************************************************
 * Copyright (C) 2016, 2017 Gabriel Marcano
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
#include <ctr9/ctr_screen.h>
#include <ctr9/ctr_circular_buffer.h>

#include <ctr_core/ctr_core_console.h>

#ifdef __cplusplus
extern "C" {
#endif

/**	@brief Initializes the console subsystem.
 *
 *	@returns 0 on success, anything else on an error. This function should
 *		suceed. If it does not, this means that the underling libc and graphics
 *		handling for libctr9 is broken.
 */
int ctr_console_initialize(ctr_screen *screen);

typedef ctr_core_console ctr_console;

void ctr_console_draw(char c);
short ctr_console_get_char_width(char c);
unsigned int ctr_console_get_char_height(void);
void ctr_console_clear(void);

#ifdef __cplusplus
}
#endif

#endif//CTR_CONSOLE_H_

