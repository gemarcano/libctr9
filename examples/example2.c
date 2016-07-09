/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

#include <ctr9/io.h>

/*	This example goes over how to manually gain en/decrypted access to NAND. It
 *	also serves as an example of how the layer concept for io interfaces work.
 */

int main()
{
	//Declare the io interface objects that will be used. For accessing the
	//encrypted data inside NAND, that means NAND itself must be initialized, as
	//well as the layers that will deal with the decryption of the data. In this
	//case, both a crypto layer for ctr and twl need to be declared.
	ctr_nand_interface nand_io;
	ctr_nand_crypto_interface ctr_io;
	ctr_nand_crypto_interface twl_io;

	ctr_nand_interface_initialize(&nand_io);

	//To initialize io crypto layer objects, they require knowing which keyslot
	//to use for en/decryption as well as which mode of operation. Also, since
	//these are mere layers they need an io interface object to wrap. Note that
	//these functions merely use the keyslots, but do not set them up. These
	//must be setup prior to using the crypto layer objects in ctr_io_*
	//functions.
	
	//For CTRNAND decryption, use keyslot 0x04, used for the o3DS, set the
	//mode for CTR, and pass in the nand object as the object to wrap in the
	//crypto layer.
	ctr_nand_crypto_interface_initialize(&ctr_io, 0x04, NAND_CTR, (ctr_io_interface*)&nand_io);

	//For TWLN decryption, use keyslot 0x03, used for the o3DS, set the
	//mode for TWL, and pass in the nand object as the object to wrap in the
	//crypto layer.

	ctr_nand_crypto_interface_initialize(&twl_io, 0x03, NAND_TWL, (ctr_io_interface*)&nand_io);

	//Setup buffers to copy data to
	unsigned char ctr_buffer[0x200] = { 0 };
	unsigned char twl_buffer[0x200] = { 0 };

	//Read the beginning of CTRNAND
	ctr_io_read(&ctr_io, ctr_buffer, sizeof(ctr_buffer), 0x0B930000, sizeof(ctr_buffer));

	//Read the beginning of TWLN
	ctr_io_read(&twl_io, twl_buffer, sizeof(twl_buffer), 0x00012E00, sizeof(twl_buffer));

	return 0;
}

