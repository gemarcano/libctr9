#ifndef CTR_DISKS_H_
#define CTR_DISKS_H_

#include <ctr9/io/ctr_nand_interface.h>
#include <ctr9/io/ctr_nand_crypto_interface.h>
#include <ctr9/io/ctr_sd_interface.h>

/**	@brief Convenience function to help initialize the IO interfaces commonly
 *		used in 3DS applications.
 *
 *	@param[out] nand_io Pointer to nand_io to initialize. This will set up the
 *		interface to access NAND generally. If NULL, this parameter is ignored.
 *		Note that if this parameter is NULL, ctr_io and twl_io must also be
 *		NULL since they depend on nand_io.
 *	@param[out] ctr_io Pointer to nand_io to initialize. This will set up the
 *		interface for CTRNAND decryption and encryption, using nand_io as the
 *		base layer. If NULL, this parameter is ignored.
 *	@param[out] twl_io Pointer to nand_io to initialize. This will set up the
 *		interface for TWL* decryption and encryption, using nand_io as the base
 *		layer. If NULL, this parameter is ignored.
 *	@param[out] sd_io Pointer to nand_io to initialize. This will set up the
 *		interface to access the SD card generally. If NULL, this parameter is
 *		ignored.
 *
 *	@return 0 on success, anything else if there was a problem initializing any
 *		of the layers requested.
 */
int ctr_disks_initialize(
	ctr_nand_interface *nand_io,
	ctr_nand_crypto_interface *ctr_io,
	ctr_nand_crypto_interface *twl_io,
	ctr_sd_interface *sd_io);

#endif//CTR_DISKS_H_

