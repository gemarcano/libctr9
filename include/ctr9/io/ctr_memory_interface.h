/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */

#ifndef CTR_MEMORY_INTERFACE_H_
#define CTR_MEMORY_INTERFACE_H_

#include "ctr_io_interface.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief io interface object for accessing the memory.
 */
typedef struct
{
	ctr_io_interface base;
	void *buffer;
	size_t buffer_size;
} ctr_memory_interface;

/** @brief Initialize the given memory io interface object.
 *
 *  @param[out] io memory io interface to initialize.
 *  @param[in] buffer Pointer to buffer to use as a disk.
 *  @param[in] buffer_size Size of the buffer to be used as a disk.
 *
 *	@post io is initialized and ready to be used in other ctr_memory_interface
 *		functions.
 */
void ctr_memory_interface_initialize(ctr_memory_interface *io, void *buffer, size_t buffer_size);

/** @brief Destroys the given memory io interface object.
 *
 *  @param[in,out] io memory io interface to deinitialize.
 *
 *  @post The io interface has been destroyed and cannot be used for accessing
 *      the memory without being re-initialized.
 */
void ctr_memory_interface_destroy(ctr_memory_interface *io);

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
int ctr_memory_interface_read(void *io, void *buffer, size_t buffer_size, uint64_t position, size_t count);

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
int ctr_memory_interface_write(void *io, const void *buffer, size_t buffer_size, uint64_t position);

/** @brief Reads sectors from the given io interface.
 *
 *  Memory has no concept of sectors, so logical "sectors" are 1 byte.
 *
 *  @param[in,out] io The io interface to use for reading.
 *  @param[out] buffer Pointer to the buffer.
 *  @param[in] buffer_size The size of the buffer in bytes.
 *  @param[in] sector Sector position in the io interface to read from.
 *  @param[in] count The number of sectors to read.
 *
 *  @returns 0 upon success, anything else means an error.
 */
int ctr_memory_interface_read_sector(void *io, void *buffer, size_t buffer_size, size_t sector, size_t count);

/** @brief Writes sectors from the given io interface.
 *
 *  Memory has no concept of sectors, so logical "sectors" are 1 byte.
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
int ctr_memory_interface_write_sector(void *io, const void *buffer, size_t buffer_size, size_t sectorn);

/** @brief Returns the size of the underlying disk for the given io interface.
 *
 *  @returns The size of the memory buffer used by this io interface.
 */
uint64_t ctr_memory_interface_disk_size(void *io);

/** @brief Returns the size of the sectors used by the io interface, which is
 *      1 bytes for memory.
 *
 *  @returns 1 byte as the sector size for memorys.
 */
size_t ctr_memory_interface_sector_size(void *io);

#ifdef __cplusplus
}
#endif

#endif//CTR_MEMORY_INTERFACE_H_

