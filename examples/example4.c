/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

#include <ctr9/io.h>

/*	This example goes over how to initialize the fatfs subsystem using a helper
 *	function that does away with having to deal with the fatfs IOCTLs directly.
 */

int main()
{
	//Prepare the io interfaces that will be initialized by the helper function.
	ctr_nand_interface nand_io;
	ctr_nand_crypto_interface ctr_io;
	ctr_nand_crypto_interface twl_io;
	ctr_sd_interface sd_io;

	//This is the helper function to initialize fatfs. It does two things,
	//actually: it initializes each interface object given to it, and then
	//hooks them up with fatfs. It is intelligent enough to select the keys for
	//CTRNAND based on the NCSD header of NAND (so it selects 0x04 for o3DS,
	//0x05 for the N3DS). Any parameters passed in as NULL are ignored and not
	//initialized. Trying to mount a disk related with an uninitialized
	//component should error out with a disk not ready error.
	ctr_fatfs_initialize(&nand_io, &ctr_io, &twl_io, &sd_io);

	FATFS fatfs;

	//The default setup for fatfs sets up CTRNAND as disk 0, TWLN as disk 1,
	//TWLP as disk 2, and SD as disk 3. Either numbers or the names as listed
	//below can be used to address the fatfs partitions.
	f_mount(&fatfs, "CTRNAND:", 0);
	f_mount(&fatfs, "TWLN:", 0);
	f_mount(&fatfs, "TWLP:", 0);
	f_mount(&fatfs, "SD:", 0);

	//Now it is possible to access any files in any of those partitions via
	//f_open calls, etc.

	return 0;
}
