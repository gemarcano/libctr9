/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

#include <ctr9/io.h>

/*	This example goes over how to manually setup fatfs.
 */

int main()
{
	//Declare the io interface objects that will be used, and initialize them.
	ctr_nand_interface nand_io;
	ctr_nand_crypto_interface ctr_io;
	
	ctr_nand_interface_initialize(&nand_io);

	//For CTRNAND decryption, use keyslot 0x04, used for the o3DS, set the
	//mode for CTR, and pass in the nand object as the object to wrap in the
	//crypto layer.
	ctr_nand_crypto_interface_initialize(&ctr_io, 0x04, NAND_CTR, (ctr_io_interface*)&nand_io);

	//fatfs uses the io interface subsystem to handle disks. By default, no
	//disks registed with fatfs. These need to be manually registered via a
	//custom IOCTL, CTR_SETUP_DISK. This IOCTL uses a special structure to tell
	//fatfs how to register the disk and where to find the fat partition.
	//ctr_setup_disk_parameters takes 3 fields:
	//	- A pointer to the ctr_* interface to use as a disk
	//	- The location, in sectors, of where the fatfs partition begins
	//	- The size, in sectors, of the fatfs partition.
	ctr_setup_disk_parameters params;
	params = (ctr_setup_disk_parameters){&ctr_io, 0x0B930000/0x200, 0x2F5D0000/0x200};

	//Tell fatfs to register this particular disk as disk 0.
	disk_ioctl(0, CTR_SETUP_DISK, &params);

	//At this point, it should be safe to use f_mount with disk 0
	FATFS fs;
	FIL file;

	f_mount(&fs, "0:", 0);
	//result should be FR_OK if everything worked as it should have.
	int result = f_open(&file, "0:/rw/sys/SecureInfo_A", FA_OPEN_EXISTING | FA_READ);

	//Note that it is safe to do reads with the ctr_io object even while it is
	//mounted.
	unsigned char buffer[0x200] = { 0 };
	//Result should also be zero
	result = ctr_io_read(&ctr_io, buffer, sizeof(buffer), 0x0B930000, sizeof(buffer));

	//unmount the filesystem
	f_mount(NULL, "0:", 1);

	return 0;
}

