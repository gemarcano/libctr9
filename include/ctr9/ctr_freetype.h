/*******************************************************************************
 * Copyright (C) 2016, 2017 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */

#ifndef CTR_FREETYPE_H_
#define CTR_FREETYPE_H_

#include <ctr9/ctr_screen.h>
#include <ctr_core/ctr_core_freetype.h>

#include <stdint.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_CACHE_H

#ifdef __cplusplus
extern "C" {
#endif

/**	@brief Initializes the freetype subsystem.
 */
int ctr_freetype_initialize(void);

FT_Face ctr_freetype_get_face(void);
FTC_SBit ctr_freetype_prepare_character(char c);
void ctr_freetype_draw(ctr_screen *screen, size_t x, size_t y, char c, uint32_t pixel, uint32_t bg);

#ifdef __cplusplus
}
#endif

#endif//CTR_FREETYPE_H_

