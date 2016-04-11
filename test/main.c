#include <3ds9/io.h>

#include <string.h>
#include <stdbool.h>
#include <ctr/console.h>
#include <ctr/printf.h>

void report_error(bool assert, const char *message)
{
	if (!assert)
	{
		printf(message);
	}
}

int main()
{
	console_init(0xFFFFFF, 0);

	ctr_nand_interface nand_io;
	int res = ctr_nand_interface_initialize(&nand_io);

	if (!res)
	{
		unsigned char buffer[256] = {0};
		res = ctr_io_read(&nand_io, buffer, sizeof(buffer), 0x100, 4);
		report_error(res == 0, "ctr_io_read failed!!!");
		report_error(strcmp(buffer, "NCSD"), "1: NCSD header not found or bug!!!");
		res = ctr_io_read_sector(&nand_io, buffer, sizeof(buffer), 0, 1);
		report_error(res == 0, "ctr_io_read_sector failed!!!");
		report_error(strncmp(buffer, "NCSD", 4), "2: NCSD header not found or bug!!!");
	}
	else
	{//An error has occurred

	}

	ctr_sd_interface sd_io;
	ctr_sd_interface_initialize(&sd_io);

	return 0;
}

