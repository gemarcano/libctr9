/*******************************************************************************
 * Copyright (C) 2016, 2017 Gabriel Marcano
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
typedef struct ctr_memory_interface ctr_memory_interface;

/** @brief Initialize the given memory io interface object.
 *
 *  @param[in] buffer Pointer to buffer to use as a disk.
 *  @param[in] buffer_size Size of the buffer to be used as a disk.
 *
 *	@return A non null pointer on success, null on failure (shouldn't fail).
 */
ctr_memory_interface *ctr_memory_interface_initialize(void *buffer, size_t buffer_size);

/** @brief Destroys the given memory io interface object.
 *
 *  @param[in,out] io memory io interface to deinitialize.
 *
 *  @post The io interface has been destroyed and cannot be used for accessing
 *      the memory without being re-initialized.
 */
void ctr_memory_interface_destroy(ctr_memory_interface *io);

#ifdef __cplusplus
}

namespace ctr9
{
	class memory_interface
	{
	public:
		/** @brief Reads bytes from the given io interface.
		 *
		 *  @param[out] buffer Pointer to the buffer.
		 *  @param[in] buffer_size The size of the buffer in bytes.
		 *  @param[in] position Position/address in the io interface to read from.
		 *  @param[in] count The number of bytes to read.
		 *
		 *  @returns 0 upon success, anything else means an error.
		 */
		int read(void *buffer, size_t buffer_size, uint64_t position, size_t count);

		/** @brief Writes bytes to the given io interface.
		 *
		 *  @param[in] buffer Pointer to the buffer.
		 *  @param[in] buffer_size The size of the buffer, and the number of bytes to
		 *      write.
		 *  @param[in] position Position/address in the io interface to write to.
		 *
		 *  @returns 0 upon success, anything else means an error.
		 */
		int write(const void *buffer, size_t buffer_size, uint64_t position);

		/** @brief Reads sectors from the given io interface.
		 *
		 *  Memory has no concept of sectors, so logical "sectors" are 1 byte.
		 *
		 *  @param[out] buffer Pointer to the buffer.
		 *  @param[in] buffer_size The size of the buffer in bytes.
		 *  @param[in] sector Sector position in the io interface to read from.
		 *  @param[in] count The number of sectors to read.
		 *
		 *  @returns 0 upon success, anything else means an error.
		 */
		int read_sector(void *buffer, size_t buffer_size, size_t sector, size_t count);

		/** @brief Writes sectors from the given io interface.
		 *
		 *  Memory has no concept of sectors, so logical "sectors" are 1 byte.
		 *
		 *  @param[in] buffer Pointer to the buffer.
		 *  @param[in] buffer_size The size of the buffer, and the number of bytes to
		 *      write. If the number is not a multiple of the sector size, this function
		 *      will only write all the full sectors it can, ignoring the end of the
		 *      buffer that doesn't fit a sector.
		 *  @param[in] sector Sector Position in the io interface to write to.
		 *
		 *  @returns 0 upon success, anything else means an error.
		 */
		int write_sector(const void *buffer, size_t buffer_size, size_t sectorn);

		/** @brief Returns the size of the underlying disk for the given io interface.
		 *
		 *  @returns The size of the memory buffer used by this io interface.
		 */
		uint64_t disk_size() const;

		/** @brief Returns the size of the sectors used by the io interface, which is
		 *      1 bytes for memory.
		 *
		 *  @returns 1 byte as the sector size for memorys.
		 */
		static constexpr size_t sector_size();

	private:
		void *buffer;
		size_t buffer_size;
	};

}

#endif

#endif//CTR_MEMORY_INTERFACE_H_

