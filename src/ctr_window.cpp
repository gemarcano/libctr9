#include <ctr9/ctr_window.h>
#include <ctr9/ctr_screen.h>
#include <ctr_core/ctr_core_screen.h>
#include <ctr_core/ctr_core_window.h>

#include <climits>

ctr_window *ctr_window_initialize(ctr_screen *screen, std::size_t x, std::size_t y, std::size_t width, std::size_t height)
{
	return ctr_core_window_initialize(screen, x, y, width, height);
}

void ctr_window_destroy(ctr_window *window)
{
	ctr_core_window_destroy(window);
}

