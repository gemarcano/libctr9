/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */

#include <ctr9/io/fatfs/ctr_fatfs.h>
#include <ctr9/io/fatfs/ctr_fatfs_disk.h>
#include <ctr9/io/ctr_nand_interface.h>
#include <ctr9/io/ctr_nand_crypto_interface.h>
#include <ctr9/io/ctr_sd_interface.h>
#include <ctr9/io/ctr_disks.h>

int ctr_fatfs_initialize(
	ctr_nand_interface *nand_io,
	ctr_nand_crypto_interface *ctr_io,
	ctr_nand_crypto_interface *twl_io,
	ctr_sd_interface *sd_io)
{
	int result = ctr_disks_initialize(nand_io, ctr_io, twl_io, sd_io);
	ctr_fatfs_default_setup(ctr_io, twl_io, sd_io);
	return result;
}

int ctr_fatfs_internal_initialize(
	ctr_nand_interface *nand_io,
	ctr_nand_crypto_interface *ctr_io,
	ctr_nand_crypto_interface *twl_io)
{
	int result = ctr_disks_initialize(nand_io, ctr_io, twl_io, NULL);
	ctr_fatfs_default_setup(ctr_io, twl_io, NULL);
	return result;
}

int ctr_fatfs_sd_initialize(ctr_sd_interface *sd_io)
{
	int result = ctr_disks_initialize(NULL, NULL, NULL, sd_io);
	ctr_fatfs_default_setup(NULL, NULL, sd_io);
	return result;
}

