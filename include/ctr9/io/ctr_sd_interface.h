/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */

#ifndef CTR_SD_INTRFACE_H_
#define CTR_SD_INTRFACE_H_

#include "ctr_io_interface.h"
#include "sdmmc/sdmmc.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief io interface object for accessing the SD card.
 */
typedef struct
{
	ctr_io_interface base;
} ctr_sd_interface;

//FIXME is it safe to have multiple SD card objects be initialized at the same time?
/** @brief Initialize the given SD io interface object.
 *
 *  @param[out] io SD io interface to initialize.
 *
 *	@returns 0 on success, anything else on a failure. Failures can stem from
 *		the SD card not being inserted, or some more severe underlying problem.
 */
int ctr_sd_interface_initialize(ctr_sd_interface *io);

/** @brief Destroys the given SD io interface object.
 *
 *  @param[in,out] io SD io interface to deinitialize.
 *
 *  @post The io interface has been destroyed and cannot be used for accessing
 *      the SD card without being re-initialized.
 */
void ctr_sd_interface_destroy(ctr_sd_interface *io);

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
int ctr_sd_interface_read(void *ctx, void *buffer, size_t buffer_size, size_t position, size_t count);

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
int ctr_sd_interface_write(void *ctx, const void *buffer, size_t buffer_size, size_t position);

/** @brief Reads sectors from the given io interface.
 *
 *  SD card access uses 512 byte sectors.
 *
 *  @param[in,out] io The io interface to use for reading.
 *  @param[out] buffer Pointer to the buffer. 
 *  @param[in] buffer_size The size of the buffer in bytes.
 *  @param[in] sector Sector position in the io interface to read from.
 *  @param[in] count The number of sectors to read.
 *
 *  @returns 0 upon success, anything else means an error.
 */
int ctr_sd_interface_read_sector(void *ctx, void *buffer, size_t buffer_size, size_t position, size_t count);

/** @brief Writes sectors from the given io interface.
 *
 *  SD card access uses 512 byte sectors.
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
int ctr_sd_interface_write_sector(void *ctx, const void *buffer, size_t buffer_size, size_t position);

/** @brief Returns the size of the underlying disk for the given io interface.
 *
 *  @returns The size of the NAND as reported by it.
 */
size_t ctr_sd_interface_disk_size(void *ctx);

/** @brief Returns the size of the sectors used by the io interface, which is
 *      512 bytes for SD cards.
 *
 *  @returns 512 bytes as the sector size for SD cards.
 */
size_t ctr_sd_interface_sector_size(void *ctx);

#ifdef __clusplus
}
#endif

#endif//CTR_SD_INTRFACE_H_

