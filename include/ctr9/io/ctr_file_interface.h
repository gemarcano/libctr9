/*******************************************************************************
 * Copyright (C) 2016, 2017 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */

#ifndef CTR_FILE_INTERFACE_H_
#define CTR_FILE_INTERFACE_H_

#include "ctr_io_interface.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**	@brief io interface object for accessing a file file as a disk.
 */
typedef struct
{
	ctr_io_interface base;
	FILE *file;
} ctr_file_interface;

/**	@brief Initialize the given file io interface object.
 *
 *	Note that any size file greater or equal than a single sector will be
 *	accepted by the constructor, but the io operations will ignore the trailing
 *	data on the file if the file size isn't divisible by the sector size. In
 *	other words, the effective size of the disk is fsize(file)/0x200,
 *	using integer arithmetic. In order for reads to work the file must have been
 *	opened using "rb", and for writes to work "r+b".
 *
 *	@param[out] io file io interface to initialize.
 *	@param[in,out] file File to set up as a disk.
 *
 *	@returns 0 on success, anything else on a failure.
 */
int ctr_file_interface_initialize(ctr_file_interface *io, FILE *file);

/**	@brief Destroys the given file io interface object.
 *
 *	@param[in,out] io file io interface to deinitialize.
 *
 *	@post The io interface has been destroyed and cannot be used for accessing
 *		the file file as a disk without being re-initialized.
 */
void ctr_file_interface_destroy(ctr_file_interface *io);

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
int ctr_file_interface_read(void *io, void *buffer, size_t buffer_size, uint64_t position, size_t count);

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
int ctr_file_interface_write(void *io, const void *buffer, size_t buffer_size, uint64_t position);

/** @brief Reads sectors from the given io interface.
 *
 *  file is configured to only use 512 byte sectors.
 *
 *  @param[in,out] io The io interface to use for reading.
 *  @param[out] buffer Pointer to the buffer.
 *  @param[in] buffer_size The size of the buffer in bytes.
 *  @param[in] sector Sector position in the io interface to read from.
 *  @param[in] count The number of sectors to read.
 *
 *  @returns 0 upon success, anything else means an error.
 */
int ctr_file_interface_read_sector(void *io, void *buffer, size_t buffer_size, size_t sector, size_t count);

/** @brief Writes sectors from the given io interface.
 *
 *	file is configured to only use 512 byte sectors.
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
int ctr_file_interface_write_sector(void *io, const void *buffer, size_t buffer_size, size_t sector);

/** @brief Returns the size of the underlying disk for the given io interface.
 *
 *  @returns The size of the file used as the backing for the given io
 *  	interface.
 */
uint64_t ctr_file_interface_disk_size(void *io);

/** @brief Returns the size of the sectors used by the io interface, which is
 *		512 bytes for file due to how it is configured.
 *
 *  @returns 512 bytes as the sector size.
 */
size_t ctr_file_interface_sector_size(void *io);

#ifdef __cplusplus
}
#endif

#endif//CTR_FILE_INTERFACE_H_

