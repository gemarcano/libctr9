#include <ctr9/io/ctr_console.h>

#include <sys/iosupport.h>
#include <ctr9/io.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

//FIXME Freetype interactions are wonky. I'm not handling the cases where metrics can be negative due to changes in text orientation

#include <ctr9/ctr_freetype.h>

static ssize_t ctr_console_write_r(struct _reent *r, int fd, const char *ptr, size_t len);

static const devoptab_t tab =
{
	"console",
	0,
	NULL,
	NULL,
	ctr_console_write_r,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	0,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

static ctr_console console;

int ctr_console_initialize(const ctr_screen *screen)
{
	devoptab_list[STD_OUT] = &tab;
	devoptab_list[STD_ERR] = &tab;
	setvbuf(stdout, NULL , _IONBF, 0);
	setvbuf(stderr, NULL , _IONBF, 0);

	memset(&console, 0, sizeof(console));
	console.font_pt = 14;
	console.width = screen->width;
	console.height = screen->height;
	console.default_fg = console.fg = 0xFFFFFF;
	console.default_bg = console.bg = 0x000000;

	console.screen = *screen;

	return 0;
}

short ctr_console_get_char_width(char c)
{
	FTC_SBit bit = ctr_freetype_prepare_character(c);
	return bit->xadvance;
}

unsigned int ctr_console_get_char_height(void)
{//FIXME currently char height == line height... will this always be the case?
	FT_Face face = ctr_freetype_get_face();
	//This should always be positive...
	return (unsigned int)(face->size->metrics.height >> 6);
}

static void pixel_set(void *buffer, uint32_t pixel, size_t pixel_size, size_t count)
{
	for (size_t i = 0; i < count; ++i)
	{
		for (size_t j = 0; j < pixel_size; ++j)
			((char*)buffer)[i*pixel_size + j] = pixel >> (j*CHAR_BIT);
	}
}

static void draw_shift_up(void)
{
	// Buffer is bottom to top, left to right
	size_t pixel_size = console.screen.pixel_size;
	unsigned int line_height = ctr_console_get_char_height();
	size_t copy_col = (console.height - line_height) * pixel_size;

	for (size_t i = 0; i < console.width; ++i)
	{
		uint8_t *col = console.screen.framebuffer + i*console.screen.height*pixel_size;
		memmove(col + line_height*pixel_size, col, copy_col);
		pixel_set(col, console.bg, pixel_size, line_height);
	}
}

void ctr_console_draw(char c)
{
	//This console implementation assumes horizontal layout, always... at least for now
	//This means the xadvance should always be positive
	unsigned int cwidth = (unsigned int)ctr_console_get_char_width(c);
	unsigned int cheight = ctr_console_get_char_height();

	FTC_SBit bit = ctr_freetype_prepare_character(c);

	if (c == '\n' || (console.xpos + cwidth) > console.width)
	{
		console.xpos = 0;
		console.ypos += cheight;
	}

	if (console.ypos + cheight >= console.height)
	{
		draw_shift_up();
		console.ypos -= cheight;
	}

	if (!(c == '\r' || c == '\n'))
	{
		//Can I assume bit->top is always positive for horizontal layouts?
		size_t off = (ctr_console_get_char_height() - (unsigned int)(bit->top));
		ctr_freetype_draw(&console.screen, console.xpos, console.ypos + off, c, console.fg);
		console.xpos += cwidth;
	}

}

typedef enum
{
	CSI_INITIAL, CSI_PARAMETER, CSI_NEW_PARAMETER, CSI_DONE, CSI_ERR
} csi_state;

static inline bool iscommand(char c)
{
	return NULL != strchr("ABCDEFGHJKSTfmsu", c);
}

typedef struct
{
	const char *params[2]; //Not supporting more than 2 parameters, rest are ignored
	unsigned char command;

	size_t bytes_read;
} csi_data;

static csi_state do_csi_initial(csi_data *data, const char *str, size_t i)
{
	memset(data, 0, sizeof(*data));
	if (isdigit(str[i]))
	{
		data->params[0] = str+i;
		return CSI_PARAMETER;
	}
	else if (iscommand(str[i]))
	{
		data->command = str[i];
		return CSI_DONE;
	}

	return CSI_ERR;
}

static csi_state do_csi_parameter(csi_data *data, const char *str, size_t i)
{
	if (str[i] == ';')
	{
		return CSI_NEW_PARAMETER;
	}
	else if (isdigit(str[i]))
	{
		return CSI_PARAMETER;
	}
	else if (iscommand(str[i]))
	{
		data->command = str[i];
		return CSI_DONE;
	}

	return CSI_ERR;
}

static csi_state do_csi_new_parameter(csi_data *data, const char *str, size_t i)
{
	if (isdigit(str[i]))
	{
		if (data->params[1] == NULL)
		{
			data->params[1] = str + i;
		}
		return CSI_PARAMETER;
	}
	return CSI_ERR;
}

static csi_state do_csi_done(csi_data *data, const char *str, size_t i)
{
	return CSI_ERR;
}

typedef csi_state (*csi_func)(csi_data *data, const char *str, size_t i);

static const csi_func state_table[4] =
{
	do_csi_initial, do_csi_parameter, do_csi_new_parameter, do_csi_done
};

static csi_state csi_run(csi_state state, csi_data *data, const char *str, size_t i)
{
	return state_table[state](data, str, i);
}

static inline size_t extract_param(const char* param, size_t def)
{
	return (param) ?
		(size_t)strtol(param, NULL, 10) :
		def;
}

static uint32_t colors[8] = {
	0x00, 0xAA0000, 0x00AA00, 0x808000, 0x0000AA, 0xAA00AA, 0x00AAAA, 0xAAAAAA
};

static void csi_sgm(const csi_data *data)
{
	size_t param1 = extract_param(data->params[0], 0);
	size_t param2 = extract_param(data->params[1], 0);

	switch (param1)
	{
		case 0:
		{
			console.fg = console.default_fg;
			console.bg = console.default_bg;
		}
		break;

		case 7:
		{
			if (!console.negative)
			{
				uint32_t tmp = console.fg;
				console.fg = console.bg;
				console.bg = tmp;

				console.negative = true;
			}
		}
		break;

		case 27:
			if (console.negative)
			{
				uint32_t tmp = console.fg;
				console.fg = console.bg;
				console.bg = tmp;

				console.negative = false;
			}
		break;

		case 30: case 31: case 32: case 33: case 34: case 35: case 36: case 37:
			console.fg = colors[param1 - 30];
		break;

		case 40: case 41: case 42: case 43: case 44: case 45: case 46: case 47:
			console.bg = colors[param1 - 40];
		break;

		default:
		break;
	}
}

static void execute_command(const csi_data *data)
{
	switch (data->command)
	{
		case 'A':
		{
			uint16_t cheight = (size_t)ctr_console_get_char_height();
			size_t param = extract_param(data->params[0], 1);

			size_t pos = console.ypos > (param*cheight) ? console.ypos-(param*cheight) : 0;
			//console_adjust_cursor(console.xpos, pos);

		}
		break;
		case 'B':
		{
			uint16_t cheight = ctr_console_get_char_height();
			size_t param = extract_param(data->params[0], 1);

			size_t pos = console.ypos + param * cheight;
			if (pos > console.height) pos = console.height - cheight;
			//console_adjust_cursor(console.xpos, pos);
		}
		break;
		case 'C':
		{
			unsigned int cwidth = (unsigned int)ctr_console_get_char_width('T'); //uh,FIXME, I need to keep track of the text contents...
			size_t param = extract_param(data->params[0], 1);

			size_t pos = console.xpos + param*cwidth;
			if (pos > console.width) pos = console.width - cwidth;
			//console_adjust_cursor(pos, console.ypos);
		}
		break;

		case 'D':
		{
			unsigned int cwidth = (unsigned int)ctr_console_get_char_width('T'); //uh,FIXME, I need to keep track of the text contents...
			size_t param = extract_param(data->params[0], 1);

			size_t pos = console.xpos > (param*cwidth) ? console.xpos-(param*cwidth) : 0;
			//console_adjust_cursor(pos, console.ypos);
		}
		break;

		case 'E':
		{
			uint16_t cheight = ctr_console_get_char_height();
			size_t param = extract_param(data->params[0], 1);

			size_t pos = console.ypos + param*cheight;
			if (pos > console.height) pos = console.height - cheight;
			//console_adjust_cursor(0, pos);
		}
		break;

		case 'F':
		{
			uint16_t cheight = ctr_console_get_char_height();
			size_t param = extract_param(data->params[0], 1);

			size_t pos = console.ypos > param*cheight ? console.ypos - param*cheight : 0;
			//console_adjust_cursor(0, pos);
		}
		break;

		case 'G':
		{
			unsigned int cwidth = (unsigned int)ctr_console_get_char_width('T'); //uh,FIXME, I need to keep track of the text contents...
			size_t param = extract_param(data->params[0], 1);
			param -= 1; //Is this indexed 0 or 1 (currently assuming 1)

			size_t pos = param * cwidth < console.width  ? param*cwidth : console.width;
			//console_adjust_cursor(pos, 0);

		}
		break;

		case 'f':
		case 'H':
		{
			size_t param1 = extract_param(data->params[0], 1);
			size_t param2 = extract_param(data->params[1], 1);
			unsigned int cwidth = (unsigned int)ctr_console_get_char_width('T'); //uh,FIXME, I need to keep track of the text contents...
			uint16_t cheight = ctr_console_get_char_height();

			size_t posx = param1*cwidth < console.width ? param1*cwidth : console.width;
			size_t posy = param2*cheight < console.height ? param2*cheight : console.height;
			//console_adjust_cursor(posx, posy);
		}
		break;

		case 'J':
		{
			size_t param = extract_param(data->params[0], 0);

			if (param == 0)
			{
				//TODO
			}
			else if (param == 1)
			{
				//TODO
			}
			else if (param == 2)
			{
				//console_clear();
			}
		}
		break;

		case 'K':
		{
			size_t param = extract_param(data->params[0], 0);
			//TODO
		}
		break;

		case 'S':
		case 'T':
			break;

		case 'm':
			csi_sgm(data);
			break;

		case 's':
			console.saved_xpos = console.xpos;
			console.saved_ypos = console.ypos;
			break;

		case 'u':
			console.xpos = console.saved_xpos;
			console.ypos = console.saved_ypos;
			break;

		default:
			break;
	}
}

static size_t process_csi(const char *str, size_t len)
{
	csi_state state = CSI_INITIAL;
	csi_data data = {0};
	size_t i;
	for (i = 1;
		state != CSI_DONE &&
		state != CSI_ERR &&
		i < len; ++i)
	{
		state = csi_run(state, &data, str, i);
	}

	if (state == CSI_DONE)
	{
		execute_command(&data);
	}

	return i;
}

static ssize_t ctr_console_write_r(struct _reent *r, int fd, const char *ptr, size_t len)
{
	for (size_t i = 0; i < len; ++i)
	{
		if (ptr[i] == 0x1B)
		{
			if (i+1 != len && ptr[i+1] == '[')
			{
				++i;
				i += process_csi(ptr+i, len-i) - 1;
			}
			//else, just ignore the ESC character
		}
		else
		{
			ctr_console_draw(ptr[i]);
		}
	}
	return (ssize_t)len;
}

