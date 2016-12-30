#include <ctr9/ctr_freetype.h>
#include <ctr9/ctr_screen.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_CACHE_H

#include <stdint.h>

int ctr_freetype_initialize(void)
{
	return ctr_core_freetype_initialize();
}

FTC_SBit ctr_freetype_prepare_character(char c)
{
	return ctr_core_freetype_prepare_character(c);
}

void ctr_freetype_draw(ctr_screen *screen, size_t x, size_t y, char c, uint32_t pixel, uint32_t bg)
{
	ctr_core_freetype_draw(screen, x, y, c, pixel, bg);
}


FT_Face ctr_freetype_get_face(void)
{
	return ctr_core_freetype_get_face();
}

