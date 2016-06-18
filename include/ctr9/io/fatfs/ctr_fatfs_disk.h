/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */

#ifndef CTR_FATFS_DISK_H_
#define CTR_FATFS_DISK_H_

#include <ctr9/io/fatfs/diskio.h>
#include <ctr9/io/ctr_io_interface.h>
#include <ctr9/io/ctr_nand_crypto_interface.h>
#include <ctr9/io/ctr_sd_interface.h>

/**	@brief Structure holding information used by the fatfs implementation for
 *		keeping track of which io interfaces are used for which partition
 *		access.
 */
typedef struct
{
	ctr_io_interface *io;	//IO interface to use as disk
	size_t sector_offset;	//Offset to the desired fatfs partition start
	size_t sectors;			//Disk size in sectors
	DSTATUS status;			//Status of disk

} ctr_fatfs_disk;

/**	@brief Convenience function to initialize the fatfs system with the io
 *		interfaces required to mount CTRNAND, TWLN, TWLP, and SD.
 *
 *	@param[in,out] ctr_io A pointer to an initialized CTRNAND io layer.
 *	@param[in,out] twl_io A pointer to an initialized TWL io layer.
 *	@param[in,out] sd_io A pointer to an initialized SD io layer.
 *
 *	@post The underlying structures in fatfs have been updated (using the custom
 *		CTR_SETUP_DISK ioctl) to be able to mount CTRNAND, TWLN, TWLP, and SD.
 */
void ctr_fatfs_default_setup(ctr_nand_crypto_interface *ctr_io, ctr_nand_crypto_interface *twl_io, ctr_sd_interface *sd_io);

/**	@brief Initializes the ctr_fatfs_disk structure.
 *
 *	@param[in] disk Pointer to structure to initialize.
 *	@param[in,out] io Pointer to ctr_io_interface to use for the disk being
 *		initialized.
 *	@param[in] sector_offset Offset in sectors from the beginning of the given io
 *		interface to use as the beginning of the fatfs drive (beginning of FAT
 *		partition or MBR with FAT partition)
 *	@param sectors Size in sectors of the FATFS disk/partition.
 */
void ctr_fatfs_disk_initialize(ctr_fatfs_disk *disk, void *io, size_t sector_offset, size_t sectors);

/**	@brief Destroys/clears the ctr_fatfs_disk structure.
 */
void ctr_fatfs_disk_destroy(ctr_fatfs_disk *disk);

#endif//CTR_FATFS_DISK_H_

