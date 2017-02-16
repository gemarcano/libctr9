/*******************************************************************************
 * Copyright (C) 2016, 2017 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */

#ifndef CTR_NAND_INTERFACE_H_
#define CTR_NAND_INTERFACE_H_

#include "ctr_io_interface.h"
#include "sdmmc/sdmmc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**	@brief io interface object for accessing NAND.
 */
typedef struct ctr_nand_interface ctr_nand_interface;

/**	@brief Returns a newly initialized NAND io object.
 *
 *	@returns A non null pointer on success, null on failure. The returned
 *		pointer should be passed to the destroy pointer once it is no longer
 *		needed for anything to free the resources being used by the object.
 */
ctr_nand_interface *ctr_nand_interface_initialize(void);

/**	@brief Destroys the given NAND io interface object.
 *
 *	@param[in,out] io NAND io interface to deinitialize.
 *
 *	@post The io interface has been destroyed and cannot be used for accessing
 *		NAND without being re-initialized.
 */
void ctr_nand_interface_destroy(ctr_nand_interface *io);

#ifdef __cplusplus
}

namespace ctr9
{
	class nand_interface
	{
	public:

		nand_interface();
		~nand_interface() = default;
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
		 *  NAND uses 512 byte sectors.
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
		 *	NAND uses 512 byte sectors.
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
		int write_sector(const void *buffer, size_t buffer_size, size_t sector);

		/** @brief Returns the size of the underlying disk for the given io interface.
		 *
		 *  @returns The size of the NAND as reported by it.
		 */
		uint64_t disk_size() const;

		/** @brief Returns the size of the sectors used by the io interface, which is
		 *		512 bytes for NAND.
		 *
		 *  @returns 512 bytes as the sector size for NAND.
		 */
		static constexpr size_t sector_size();
	private:
	};

	typedef io_interface_impl<nand_interface> nand_generic_interface;
}

#endif

#endif//CTR_NAND_INTERFACE_H_

