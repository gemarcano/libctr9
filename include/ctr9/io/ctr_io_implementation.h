/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */

#ifndef CTR_IO_IMPLEMENTATION_H_
#define CTR_IO_IMPLEMENTATION_H_

#ifdef __cplusplus
extern "C" {
#endif

/**	@brief Pointer to an io interface function for reading sectors.
 *
 *  @param[out] buffer Pointer to the buffer.
 *  @param[in] buffer_size The size of the buffer in bytes.
 *  @param[in] sector Sector position in the io interface to read from.
 *  @param[in] count The number of sectors to read.
 *  @param[in] read Pointer to sdmmc function to read sectors.
 *
 *  @returns 0 upon success, anything else means an error.
 */
typedef int (*ctr_io_implementation_read_sector_f)(void *io, void *buffer, size_t buffer_size, size_t sector, size_t count);

/**	@brief Pointer to an io interface function for writing to sectors.
 *
 *  @param[in] buffer Pointer to the buffer.
 *  @param[in] buffer_size The size of the buffer, and the number of bytes to
 *      write. If the number is not a multiple of the sector size, this function
 *      will only write all the full sectors it can, ignoring the end of the
 *      buffer that doesn't fit a sector.
 *  @param[in] sector Sector Position in the io interface to write to.
 *  @param[in] write Pointer to sdmmc function to write sectors.
 *
 *  @returns 0 upon success, anything else means an error.
 */

typedef int (*ctr_io_implementation_write_sector_f)(void *io, const void *buffer, size_t buffer_size, size_t sector);

/** @brief General implementation for reading bytes using a general io sector
 *      function.
 *
 *  @param[out] buffer Pointer to the buffer.
 *  @param[in] buffer_size The size of the buffer in bytes.
 *  @param[in] position Position/address in the io interface to read from.
 *  @param[in] count The number of bytes to read.
 *  @param[in] read Pointer to sdmmc function to read sectors.
 *
 *  @returns 0 upon success, anything else means an error.
 */
static inline int ctr_io_implementation_read(void *io, void *buffer, size_t buffer_size, uint64_t position, size_t count, ctr_io_implementation_read_sector_f read);

/** @brief General implementation for writing bytes using a general io sector
 *      function.
 *
 *  @param[in] buffer Pointer to the buffer.
 *  @param[in] buffer_size The size of the buffer, and the number of bytes to
 *      write.
 *  @param[in] position Position/address in the io interface to write to.
 *  @param[in] read Pointer to sdmmc function to read sectors.
 *  @param[in] write Pointer to sdmmc function to write sectors.
 *
 *  @returns 0 upon success, anything else means an error.
 */
static inline int ctr_io_implementation_write(void *io, const void *buffer, size_t buffer_size, uint64_t position, ctr_io_implementation_read_sector_f read, ctr_io_implementation_write_sector_f write);

#include "ctr_io_implementation.c"

#ifdef __cplusplus
}
#endif

#endif//CTR_IO_IMPLEMENTATION_H_

