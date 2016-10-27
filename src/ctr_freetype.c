#include <ctr9/ctr_freetype.h>
#include <ft2build.h>
#include FT_FREETYPE_H

static FT_Library library;

int ctr_freetype_initialize(void)
{
	int err = FT_Init_FreeType(&library);
	return err;
}



