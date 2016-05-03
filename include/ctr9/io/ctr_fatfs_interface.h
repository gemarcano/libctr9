/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */

#ifndef CTR_FATFS_INTERFACE_H_
#define CTR_FATFS_INTERFACE_H_

#include "ctr_io_interface.h"
#include "sdmmc/sdmmc.h"

#include "fatfs/ff.h"

#ifdef __cplusplus
extern "C" {
#endif

/**	@brief io interface object for accessing NAND.
 */
typedef struct
{
	ctr_io_interface base;
	FIL *file;
} ctr_fatfs_interface;

//FIXME is it safe to call this function multiple times? as in, if we have multiple NAND objects, is it safe?
/**	@brief Initialize the given NAND io interface object.
 *
 *	@param[out] io NAND io interface to initialize.
 *
 *	@returns 0 on success, anything else on a failure. Unless something has gone
 *		horribly wrong with the NAND subsystem, this function shouldn't fail.
 */
int ctr_fatfs_interface_initialize(ctr_fatfs_interface *io, FIL *file);

/**	@brief Destroys the given NAND io interface object.
 *
 *	@param[in,out] io NAND io interface to deinitialize.
 *
 *	@post The io interface has been destroyed and cannot be used for accessing
 *		NAND without being re-initialized.
 */
void ctr_fatfs_interface_destroy(ctr_fatfs_interface *io);

/** @brief Reads bytes from the given io interface.
 *
 *  @param[in,out] io The io interface to use for reading.
 *  @param[out] buffer Pointer to the buffer.
 *  @param[in] buffer_size The size of the buffer in bytes.
 *  @param[in] position Position/address in the io interface to read from.
 *  @param[in] count The number of bytes to read.
 *
 *  @returns 0 upon success, anything else means an error.
 */
int ctr_fatfs_interface_read(void *io, void *buffer, size_t buffer_size, size_t position, size_t count);

/** @brief Writes bytes to the given io interface.
 *
 *  @param[in,out] io The io interface to use for writing.
 *  @param[in] buffer Pointer to the buffer.
 *  @param[in] buffer_size The size of the buffer, and the number of bytes to
 *      write.
 *  @param[in] position Position/address in the io interface to write to.
 *
 *  @returns 0 upon success, anything else means an error.
 */
int ctr_fatfs_interface_write(void *io, const void *buffer, size_t buffer_size, size_t position);

/** @brief Reads sectors from the given io interface.
 *
 *  NAND uses 512 byte sectors.
 *
 *  @param[in,out] io The io interface to use for reading.
 *  @param[out] buffer Pointer to the buffer.
 *  @param[in] buffer_size The size of the buffer in bytes.
 *  @param[in] sector Sector position in the io interface to read from.
 *  @param[in] count The number of sectors to read.
 *
 *  @returns 0 upon success, anything else means an error.
 */
int ctr_fatfs_interface_read_sector(void *io, void *buffer, size_t buffer_size, size_t position, size_t count);

/** @brief Writes sectors from the given io interface.
 *
 *	NAND uses 512 byte sectors.
 *
 *  @param[in,out] io The io interface to use for writing.
 *  @param[in] buffer Pointer to the buffer.
 *  @param[in] buffer_size The size of the buffer, and the number of bytes to
 *      write. If the number is not a multiple of the sector size, this function
 *      will only write all the full sectors it can, ignoring the end of the
 *      buffer that doesn't fit a sector.
 *  @param[in] sector Sector Position in the io interface to write to.
 *
 *  @returns 0 upon success, anything else means an error.
 */
int ctr_fatfs_interface_write_sector(void *io, const void *buffer, size_t buffer_size, size_t position);

/** @brief Returns the size of the underlying disk for the given io interface.
 *
 *  @returns The size of the NAND as reported by it.
 */
size_t ctr_fatfs_interface_disk_size(void *io);

/** @brief Returns the size of the sectors used by the io interface, which is
 *		512 bytes for NAND.
 *
 *  @returns 512 bytes as the sector size for NAND.
 */
size_t ctr_fatfs_interface_sector_size(void *io);

#ifdef __clusplus
}
#endif

#endif//CTR_FATFS_INTERFACE_H_

