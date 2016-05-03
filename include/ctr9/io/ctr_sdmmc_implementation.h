/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */

#ifndef CTR_SDMMC_IMPLEMENTATION_H_
#define CTR_SDMMC_IMPLEMENTATION_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*sdmmc_readsectors)(uint32_t sector_no, uint32_t numsectors, uint8_t *out);
typedef int (*sdmmc_writesectors)(uint32_t sector_no, uint32_t numsectors, const uint8_t *in);
			

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
inline int ctr_sdmmc_implementation_read(void *ctx, void *buffer, size_t buffer_size, size_t position, size_t count, sdmmc_readsectors read);

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
inline int ctr_sdmmc_implementation_write(void *ctx, const void *buffer, size_t buffer_size, size_t position, sdmmc_readsectors read, sdmmc_writesectors write);

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
inline int ctr_sdmmc_implementation_read_sector(void *ctx, void *buffer, size_t buffer_size, size_t position, size_t count, sdmmc_readsectors read);

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
inline int ctr_sdmmc_implementation_write_sector(void *ctx, const void *buffer, size_t buffer_size, size_t position, sdmmc_writesectors write);

#include "ctr_sdmmc_implementation.c"

#ifdef __clusplus
}
#endif

#endif//CTR_SDMMC_IMPLEMENTATION_H_

