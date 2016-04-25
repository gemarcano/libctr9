/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */ 

#ifndef CTR_IO_INTERFACE_H_
#define CTR_IO_INTERFACE_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

/**	@brief Pointer to an io interface function used to read.
 *
 *	@param[in,out] io The io interface to use for reading.
 *	@param[out] buffer Pointer to the buffer. 
 *	@param[in] buffer_size The size of the buffer in bytes.
 *	@param[in] position Position/address in the io interface to read from.
 *	@param[in] count The number of bytes to read.
 *
 *	@returns 0 upon success, anything else means an error.
 */
typedef int (*ctr_io_interface_read)(void *io, void *buffer, size_t buffer_size, size_t position, size_t count);

/**	@brief Pointer to an io interface function used to write.
 *
 *	@param[in,out] io The io interface to use for writing.
 *	@param[in] buffer Pointer to the buffer.
 *	@param[in] buffer_size The size of the buffer, and the number of bytes to
 *		write.
 *	@param[in] position Position/address in the io interface to write to.
 *
 *	@returns 0 upon success, anything else means an error.
 */
typedef int (*ctr_io_interface_write)(void *io, const void *buffer, size_t buffer_size, size_t position);

/**	@brief Pointer to an io interface function used to read sectors.
 *
 *	The definition of a sector is left to the io interface being used. Refer to
 *	ctr_io_sector_size as a way to poll how many bytes constitute a sector.
 *
 *	@param[in,out] io The io interface to use for reading.
 *	@param[out] buffer Pointer to the buffer. 
 *	@param[in] buffer_size The size of the buffer in bytes.
 *	@param[in] sector Sector position in the io interface to read from.
 *	@param[in] count The number of sectors to read.
 *
 *	@returns 0 upon success, anything else means an error.
 */
typedef int (*ctr_io_interface_read_sector)(void *io, void *buffer, size_t buffer_size, size_t sector, size_t count);

/**	@brief Pointer to an io interface function used to write sectors.
 *
 *	@param[in,out] io The io interface to use for writing.
 *	@param[in] buffer Pointer to the buffer.
 *	@param[in] buffer_size The size of the buffer, and the number of bytes to
 *		write. If the number is not a multiple of the sector size, this function
 *		will only write all the full sectors it can, ignoring the end of the
 *		buffer that doesn't fit a sector.
 *	@param[in] sector Sector Position in the io interface to write to.
 *
 *	@returns 0 upon success, anything else means an error.
 */
typedef int (*ctr_io_interface_write_sector)(void *io, const void *buffer, size_t buffer_size, size_t sector);

/**	@brief Pointer to an io interface function used to get the size of the disk.
 *
 *	@returns The size of the io interface's underlying disk storage in bytes.
 */
typedef size_t (*ctr_io_interface_disk_size)(void *io);

/**	@brief Pointer to an io interface function used to get the sector size in
 *		bytes.
 *
 *	@returns The size of the io interface's sector size in bytes.
 */
typedef size_t (*ctr_io_interface_sector_size)(void *io);

/**	@brief Virtual table for io interface dispatching.
 */
typedef struct
{
	ctr_io_interface_read read;
	ctr_io_interface_write write;
	ctr_io_interface_read_sector read_sector;
	ctr_io_interface_write_sector write_sector;
	ctr_io_interface_disk_size disk_size;
	ctr_io_interface_sector_size sector_size;
} ctr_io_interface;

/**	@brief Reads bytes from the given io interface.
 *
 *	This function uses the virtual table in the io interface given to call the
 *	correct function to handle the read request.
 *
 *	@param[in,out] io The io interface to use for reading.
 *	@param[out] buffer Pointer to the buffer. 
 *	@param[in] buffer_size The size of the buffer in bytes.
 *	@param[in] position Position/address in the io interface to read from.
 *	@param[in] count The number of bytes to read.
 *
 *	@returns 0 upon success, anything else means an error.
 */
int ctr_io_read(void *io, void *buffer, size_t buffer_size, size_t position, size_t count);

/**	@brief Writes bytes to the given io interface.
 *
 *	This function uses the virtual table in the io interface given to call the
 *	correct function to handle the write request.
 *
 *	@param[in,out] io The io interface to use for writing.
 *	@param[in] buffer Pointer to the buffer.
 *	@param[in] buffer_size The size of the buffer, and the number of bytes to
 *		write.
 *	@param[in] position Position/address in the io interface to write to.
 *
 *	@returns 0 upon success, anything else means an error.
 */
int ctr_io_write(void *io, const void *buffer, size_t buffer_size, size_t position);

/**	@brief Reads sectors from the given io interface.
 *
 *	This function uses the virtual table in the io interface given to call the
 *	correct function to handle the read sectors request.
 *
 *	The definition of a sector is left to the io interface being used. Refer to
 *	ctr_io_sector_size as a way to poll how many bytes constitute a sector.
 *
 *	@param[in,out] io The io interface to use for reading.
 *	@param[out] buffer Pointer to the buffer. 
 *	@param[in] buffer_size The size of the buffer in bytes.
 *	@param[in] sector Sector position in the io interface to read from.
 *	@param[in] count The number of sectors to read.
 *
 *	@returns 0 upon success, anything else means an error.
 */
int ctr_io_read_sector(void *io, void *buffer, size_t buffer_size, size_t sector, size_t count);

/**	@brief Writes sectors from the given io interface.
 *
 *	This function uses the virtual table in the io interface given to call the
 *	correct function to handle the write sectors request.
 *
 *	@param[in,out] io The io interface to use for writing.
 *	@param[in] buffer Pointer to the buffer.
 *	@param[in] buffer_size The size of the buffer, and the number of bytes to
 *		write. If the number is not a multiple of the sector size, this function
 *		will only write all the full sectors it can, ignoring the end of the
 *		buffer that doesn't fit a sector.
 *	@param[in] sector Sector Position in the io interface to write to.
 *
 *	@returns 0 upon success, anything else means an error.
 */
int ctr_io_write_sector(void *io, const void *buffer, size_t buffer_size, size_t sector);

/**	@brief Returns the size of the underlying disk for the given io interface.
 *
 *	This function uses the virtual table in the io interface given to call the
 *	correct function to handle the read request.
 *
 *	@returns The size of the io interface's underlying disk storage in bytes.
 */
size_t ctr_io_disk_size(void *io);

/**	@brief Returns the size of the sectors used by the io interface.
 *
 *	This function uses the virtual table in the io interface given to call the
 *	correct function to handle the read request.
 *
 *	@returns The size of the io interface's sector size in bytes.
 */
size_t ctr_io_sector_size(void *io);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif//CTR_IO_INTERFACE_H_

