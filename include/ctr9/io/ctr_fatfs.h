/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */

#ifndef CTR_FATFS_H_
#define CTR_FATFS_H_

#include <ctr9/io/ctr_nand_interface.h>
#include <ctr9/io/ctr_nand_crypto_interface.h>
#include <ctr9/io/ctr_sd_interface.h>

/**	@brief Initializes the entire IO subsystem.
 *
 *	This is a helper function that takes in pointers to all necessary io
 *	interfaces to hook up to the fatfs layer, and initializes them and prepares
 *	fatfs for usage.
 *
 *	@param[in,out] nand_io NAND io interface to initialize. May be used
 *		independetly after initialization so long as it is not being
 *		simultaneously used by fatfs.
 *	@param[in,out] ctr_io CTRNAND io crypto interface to initialize. May be used
 *		independetly after initialization so long as it is not being
 *		simultaneously used by fatfs.
 *	@param[in,out] twl_io TWL io crypto interface to initialize. May be used
 *		independetly after initialization so long as it is not being
 *		simultaneously used by fatfs.
 *	@param[in,out] sd_io SD card io interface to initialize. May be used
 *		independetly after initialization so long as it is not being
 *		simultaneously used by fatfs.
 *
 *	@post All structures passed in are initialized and the fatfs subsystem has
 *		been prepared for usage.
 */
int ctr_fatfs_initialize(
	ctr_nand_interface *nand_io,
	ctr_nand_crypto_interface *ctr_io,
	ctr_nand_crypto_interface *twl_io,
	ctr_sd_interface *sd_io);

#endif//CTR_FATFS_H_

