#include <ctr9/io.h>

int main()
{
	ctr_nand_interface nand_io;
	ctr_nand_interface_initialize(&nand_io);

	unsigned char buffer[0x200] = { 0 };

	ctr_io_read(&nand_io, buffer, sizeof(buffer), 0x0, sizeof(buffer));

	ctr_nand_interface_destroy(&nand_io);
	return 0;
}

