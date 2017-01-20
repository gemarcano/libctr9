/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

#include <ctr9/io.h>

/*	This example goes over how to initialize the basic IO infrastructure to
 *	access raw NAND.
 */

int main()
{
	//All of the IO subsystem is built around the idea of IO objects that hold
	//the state of the IO subsystems, and IO functions that act on these
	//objects.
	//In this case, since this is used for raw NAND access, only the nand io
	//interface is needed. Make sure to initialize the object before calling any
	//ctr_io_* functions with it.
	ctr_nand_interface nand_io;
	ctr_nand_interface_initialize(&nand_io);

	//Allocate a small buffer to read to
	unsigned char buffer[0x200] = { 0 };

	//The IO subsystem implements polymorphism, so any io interface can be used
	//as an input to the ctr_io_* functions.

	//Read the size of the buffer into the buffer we allocated earlier, starting
	//from the beginning of NAND. Effectively, read the NCSD header.
	ctr_io_read(&nand_io, buffer, sizeof(buffer), 0x0, sizeof(buffer));

	//Destroy the io interface object when done with it.
	ctr_nand_interface_destroy(&nand_io);
	return 0;
}

