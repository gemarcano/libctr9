/*******************************************************************************
 * Copyright (C) 2016, 2017 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */

#include <ctr9/io/ctr_disks.h>
#include <ctr9/io/ctr_io_interface.h>
#include <ctr9/io/ctr_nand_interface.h>
#include <ctr9/io/ctr_nand_crypto_interface.h>
#include <ctr9/io/ctr_sd_interface.h>

int ctr_disks_initialize(
	ctr_nand_interface *nand_io,
	ctr_nand_crypto_interface *ctr_io,
	ctr_nand_crypto_interface *twl_io,
	ctr_sd_interface *sd_io)
{
	int result = 0;
	if (nand_io)
	{
		result |= ctr_nand_interface_initialize(nand_io);
		if (!result)
		{
			if (ctr_io)
			{
				uint8_t encryption_type = 0;
				result |= ctr_io_read(nand_io, &encryption_type, sizeof(encryption_type), 0x118 + 0x4, sizeof(encryption_type));
				uint8_t keyslot;
				switch (encryption_type)
				{
				default: //default to the o3DS keyslot FIXME is this a good default behavior?
				case 0x02:
					keyslot = 0x04;
					break;
				case 0x03:
					keyslot = 0x05;
					break;
				}
				result |= ctr_nand_crypto_interface_initialize(ctr_io, keyslot, NAND_CTR, (ctr_io_interface*)nand_io);
			}

			if (twl_io)
			{
				result |= ctr_nand_crypto_interface_initialize(twl_io, 0x03, NAND_TWL, (ctr_io_interface*)nand_io);
			}
		}
	}
	else if (ctr_io || twl_io)
	{
		result |= -1;
	}

	if (sd_io)
	{
		result |= ctr_sd_interface_initialize(sd_io);
	}

	return result;
}

