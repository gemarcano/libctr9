#include <ctr9/ctr_freetype.h>
#include <ctr9/ctr_screen.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_CACHE_H

#include <stdint.h>

static FT_Library library;
static FTC_Manager manager;
static FTC_SBitCache sbcache;
static FTC_ImageCache icache;
static FTC_CMapCache ccache;
static FTC_ScalerRec scaler;

typedef struct
{
	const char* filepath;
	int index;
	FT_Face face;
} ctr_face;

static ctr_face faceid;

extern uint8_t ctr_font_data_begin;
extern long int ctr_font_data_size;

static FT_Error face_requester(FTC_FaceID face_id, FT_Library lib, FT_Pointer data, FT_Face *aface)
{
	ctr_face *face = (ctr_face*)face_id;
	if (face->filepath)
		return FT_New_Face(library, face->filepath, face->index, aface);
	else
		return FT_New_Memory_Face(library, &ctr_font_data_begin, ctr_font_data_size, face->index, aface);
}

int ctr_freetype_initialize(void)
{
	int err = FT_Init_FreeType(&library);
	if (err) return err;

	err = FTC_Manager_New(library, 0, 0, 0, face_requester, NULL, &manager);
	err = FTC_SBitCache_New(manager, &sbcache);
	err = FTC_CMapCache_New(manager, &ccache);
	err = FTC_ImageCache_New(manager, &icache);

	//FIXME hard-coded path
	faceid.filepath = NULL;
	faceid.index = 0;

	err = FTC_Manager_LookupFace(manager, (FTC_FaceID)&faceid, &faceid.face);

	FT_Face face = ctr_freetype_get_face();
	if (!face->num_fixed_sizes)
	{
		scaler.face_id = (FTC_FaceID)&faceid;
		scaler.height = 0;
		scaler.width = 8*64;
		scaler.pixel = 0;
		scaler.x_res = 95; //FIXME o3DS XL specific sizes
		scaler.y_res = 95;

		FT_Size size;
		err = FTC_Manager_LookupSize(manager, &scaler, &size);
	}
	else
	{
		scaler.face_id = (FTC_FaceID)&faceid;
		scaler.height = (unsigned)face->available_sizes[0].height;
		scaler.width = (unsigned)face->available_sizes[0].width;
		scaler.pixel = 1;
		scaler.x_res = 95; //FIXME o3DS XL specific sizes
		scaler.y_res = 95;

	}

	return err;
}

FTC_SBit ctr_freetype_prepare_character(char c)
{
	FT_Size size;
	size_t glyphIndex = FTC_CMapCache_Lookup(ccache, (FTC_FaceID)&faceid, 0, c);

	FTC_SBit bit;
	int err = FTC_SBitCache_LookupScaler(sbcache, &scaler, FT_LOAD_MONOCHROME | FT_LOAD_RENDER, glyphIndex, &bit, NULL);
	return bit;
}

void ctr_freetype_draw(ctr_screen *screen, size_t x, size_t y, char c, uint32_t pixel, uint32_t bg)
{
	FTC_SBit bit = ctr_freetype_prepare_character(c);
	for (size_t yy = 0; yy < bit->height; ++yy)
	{
		//FIXME assuming positive pitch...
		for (size_t bytes = 0; bytes < (size_t)bit->pitch; ++bytes)
		{
			uint8_t bits = bit->width - bytes*8 > 8 ? 8 : bit->width - bytes*8;
			for (size_t i = 0; i < bits; ++i)
			{
				uint8_t *buf = bit->buffer;
				if (buf[(size_t)bit->pitch * yy + bytes] & (0x80u >> i))
				{
					ctr_screen_set_pixel(screen, x+bytes*8+i, y+yy, pixel);
				}
				else
				{
					ctr_screen_set_pixel(screen, x+bytes*8+i, y+yy, bg);
				}
			}
		}
	}
}


FT_Face ctr_freetype_get_face(void)
{
	FT_Face face;
	int err = FTC_Manager_LookupFace(manager, (FTC_FaceID)&faceid, &face);
	return face;
}

