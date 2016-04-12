#include <3ds9/io.h>

#include <string.h>
#include <stdbool.h>
#include <ctr/console.h>
#include <ctr/printf.h>
#include <ctr/draw.h>
#include <ctr/headers.h>
#include <ctr/hid.h>

static void report_error(bool assert, const char *message)
{
	if (!assert)
	{
		printf(message);
		printf("\n");
	}
}

int main()
{
	draw_init((draw_s*)0x23FFFE00);
	console_init(0xFFFFFF, 0);
	draw_clear_screen(SCREEN_TOP, 0xAAAAAA);
	printf("TESTING\n");

	ctr_nand_interface nand_io;
	int res = ctr_nand_interface_initialize(&nand_io);

	printf("TESTING2\n");
	if (!res)
	{
		char buffer[1024] = {0};
		res = ctr_io_read(&nand_io, buffer, sizeof(buffer), 0x100, 4);

		printf("TESTING3\n");
		report_error(res == 0, "ctr_io_read failed!!!");
	
		printf("TESTING4\n");
		report_error(strcmp(buffer, "NCSD") == 0, "1: NCSD header not found or bug!!!");
		printf(buffer);
		printf("\n");
	
		printf("TESTING5\n");
		res = ctr_io_read_sector(&nand_io, buffer, sizeof(buffer), 0, 1);
	
		printf("TESTING6\n");
		report_error(res == 0, "ctr_io_read_sector failed!!!");
	
		printf("TESTING7\n");
		report_error(strncmp(buffer+0x100, "NCSD", 4) == 0, "2: NCSD header not found or bug!!!");
		buffer[0x105] = '\0';
		printf(buffer+0x100);
		printf("\n");

		printf("TESTING8\n");

		res = ctr_io_read(&nand_io, buffer+0x100, sizeof(buffer), 0, 512);
		report_error(res == 0, "ctr_io_read failed!!!");

		report_error(strncmp(buffer+0x100, buffer, 512) == 0, "3: NCSD header not found or bug!!!");
		

	}
	else
	{//An error has occurred

	}

	ctr_sd_interface sd_io;
	ctr_sd_interface_initialize(&sd_io);

	ctr_sd_interface_destroy(&sd_io);
	ctr_nand_interface_destroy(&nand_io);

	printf("Press any key to continue...\n");
	input_wait();

	return 0;
}

