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

		//FIXME read required keyslot from NAND
		if (nand_io && ctr_io)
			result |= ctr_nand_crypto_interface_initialize(ctr_io, 0x04, NAND_CTR, (ctr_io_interface*)nand_io);
		if (twl_io)
			result |= ctr_nand_crypto_interface_initialize(twl_io, 0x03, NAND_TWL, (ctr_io_interface*)nand_io);
	}
	else if (ctr_io || twl_io)
	{
		result |= -1;
	}

	if (sd_io)
		result |= ctr_sd_interface_initialize(sd_io);

	return result;
}

