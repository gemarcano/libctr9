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

/**	@brief Pointer to an sdmmc function for reading sectors.
 *
 *	Pointer to sdmmc function for reading sectors. A sector is defined by sdmmc
 *	to be 0x200 or 512 bytes.
 *
 *	@param[in] sector_no Sector to read.
 *	@param[in] numsectors The number of sectors to read.
 *	@param[out] out Pointer to buffer to store read data. The buffer must be
 *		able to hold the number of sectors requested.
 *
 *	@returns 0 on success, anything else indicates a failure.
 */
typedef int (*sdmmc_readsectors)(uint32_t sector_no, uint32_t numsectors, uint8_t *out);

/**	@brief Pointer to an sdmmc function for writing sectors.
 *
 *	Pointer to sdmmc function for writing sectors. A sector is defined by sdmmc
 *	to be 0x200 or 512 bytes.
 *
 *	@param[in] sector_no Sector to write.
 *	@param[in] numsectors The number of sectors to write.
 *	@param[out] in Pointer to buffer with data to write. The buffer must contain
 *		the specified number of sectors to write to prevent a buffer overflow.
 *
 *	@returns 0 on success, anything else indicates a failure.
 */
typedef int (*sdmmc_writesectors)(uint32_t sector_no, uint32_t numsectors, const uint8_t *in);

/** @brief General implementation for reading bytes using an sdmmc function.
 *
 *  @param[out] buffer Pointer to the buffer.
 *  @param[in] buffer_size The size of the buffer in bytes.
 *  @param[in] position Position/address in the io interface to read from.
 *  @param[in] count The number of bytes to read.
 *  @param[in] read Pointer to sdmmc function to read sectors.
 *
 *  @returns 0 upon success, anything else means an error.
 */
static inline int ctr_sdmmc_implementation_read(void *buffer, size_t buffer_size, uint64_t position, size_t count, sdmmc_readsectors read);

/** @brief General implementation for writing bytes using sdmmc functions.
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
static inline int ctr_sdmmc_implementation_write(const void *buffer, size_t buffer_size, uint64_t position, sdmmc_readsectors read, sdmmc_writesectors write);

/** @brief General implementation for reading sectors using an sdmmc function.
 *
 *  sdmmc defines a sector as 512 bytes.
 *
 *  @param[out] buffer Pointer to the buffer.
 *  @param[in] buffer_size The size of the buffer in bytes.
 *  @param[in] sector Sector position in the io interface to read from.
 *  @param[in] count The number of sectors to read.
 *  @param[in] read Pointer to sdmmc function to read sectors.
 *
 *  @returns 0 upon success, anything else means an error.
 */
static inline int ctr_sdmmc_implementation_read_sector(void *buffer, size_t buffer_size, size_t sector, size_t count, sdmmc_readsectors read);

/** @brief General implementation for writing sectors using an sdmmc function.
 *
 *  sdmmc defines a sector as 512 bytes.
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
static inline int ctr_sdmmc_implementation_write_sector(const void *buffer, size_t buffer_size, size_t sector, sdmmc_writesectors write);

#include "ctr_sdmmc_implementation.c"

#ifdef __cplusplus
}
#endif

#endif//CTR_SDMMC_IMPLEMENTATION_H_

