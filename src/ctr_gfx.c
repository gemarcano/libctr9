#include <ctr9/ctr_gfx.h>

void ctr_gfx_initialize(ctr_gfx *gfx, void *top_fb, void *bottom_fb)
{
	gfx->top_framebuffer = top_fb;
	gfx->bottom_framebuffer = bottom_fb;
	gfx->color = 0xFFFFFF;
}

