/*******************************************************************************
 * Copyright (C) 2016, 2017 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */

#ifndef CTR_CART_H_
#define CTR_CART_H_

#include <ctr9/ctr_headers.h>
#include <ctr9/io/ctr_io_interface.h>

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief io interface object for accessing 3DSS game carts.
 */
typedef struct ctr_cart_interface ctr_cart_interface;

/**	@brief Initialize the given 3DS cart io interface object.
 *
 *	@param[out] io 3DS cart io interface to initialize.
 *
 *	@returns True on success, false on failure. This will fail if no cart is
 *		inserted.
 */
ctr_cart_interface *ctr_cart_interface_initialize(void);

/**	@brief Frees the resources held by the given 3DS cart interface object.
 *
 *	@param[in] io Interface to free.
 *
 *	@post Interface is freed and is no longer safe to use.
 */
void ctr_cart_interface_destroy(ctr_cart_interface *io);

/**	@brief Returns whether a cart is inserted or not.
 *
 *	@returns True if a cart is inserted, false otherwise.
 */
bool ctr_cart_inserted(void);

#ifdef __cplusplus
}

namespace ctr9
{
	
	class cart_interface
	{
	public:

		cart_interface();
		~cart_interface() = default;
		/**	@brief Reads sectors from the given io interface.
		 *
		 *	3DS carts define their own sector size. They refer to these as the unit
		 *	media size. Most, if not all carts seem to use 512 bytes.
		 *
		 *	This implementation automatically returns the NCCH header when reading carts
		 *	at 0x1000, and from 0x1200 to 0x2000 it returns 0xFF bytes.
		 *
		 *	@param[in,out] io The io interface to use for reading.
		 *	@param[out] buffer Pointer to the buffer.
		 *	@param[in] buffer_size The size of the buffer in bytes.
		 *	@param[in] sector Sector position in the io interface to read from.
		 *	@param[in] count The number of sectors to read.
		 *
		 *	@returns 0 upon success, anything else means an error.
		 */
		int read_sector(void *io, void* buffer, size_t buffer_size, size_t sector, size_t count);

		/**	@brief Reads bytes from the given io interface.
		 *
		 *	This implementation automatically returns the NCCH header when reading carts
		 *	at 0x1000, and from 0x1200 to 0x2000 it returns 0xFF bytes.
		 *
		 *	@param[in,out] io The io interface to use for reading.
		 *	@param[out] buffer Pointer to the buffer.
		 *	@param[in] buffer_size The size of the buffer in bytes.
		 *	@param[in] position Position/address in the io interface to read from.
		 *	@param[in] count The number of bytes to read.
		 *
		 *	@returns 0 upon success, anything else means an error.
		 */
		int read(void *io, void* buffer, size_t buffer_size, uint64_t position, size_t count);

		/** @brief Writes bytes to the given io interface. Not possible for carts.
		 *
		 *	It is not possible to write to a 3DS cart, so this function will always
		 *	return an error.
		 *
		 *  @param[in,out] io The io interface to use for writing.
		 *  @param[in] buffer Pointer to the buffer.
		 *  @param[in] buffer_size The size of the buffer, and the number of bytes to
		 *      write.
		 *  @param[in] position Position/address in the io interface to write to.
		 *
		 *  @returns Non-zero, error always.
		 */
		int noop_write(void *io, const void* buffer, size_t buffer_size, uint64_t position);

		/** @brief Writes sectors from the given io interface. Not possible for carts.
		 *
		 *	It is not possible to write to a 3DS cart, so this function will always
		 *	return an error.
		 *
		 *  @param[in,out] io The io interface to use for writing.
		 *  @param[in] buffer Pointer to the buffer.
		 *  @param[in] buffer_size The size of the buffer, and the number of bytes to
		 *      write. If the number is not a multiple of the sector size, this function
		 *      will only write all the full sectors it can, ignoring the end of the
		 *      buffer that doesn't fit a sector.
		 *  @param[in] sector Sector Position in the io interface to write to.
		 *
		 *  @returns Non-zero, error always.
		 */
		int noop_write_sector(void *io, const void* buffer, size_t buffer_size, size_t sector);

		/**	@brief Returns the size of the underlying disk for the given io interface.
		 *
		 *	@returns The size of the 3DS ROM as reported by it.
		 */
		uint64_t disk_size(void *io) const;

		/**	@brief Returns the size of the sectors used by the io interface. This is
		 *		cart dependent, althoug to date most, if not all carts report 512 bytes.
		 *
		 *	@returns The unit media size as reported by the 3DS cart, tends to be 512
		 *		bytes.
		 */
		static constexpr size_t sector_size(void *io);

		/**	@brief Reads bytes from the given io interface.
		 *
		 *	This implementation returns the raw data returned by the cart reading
		 *	system for data between 0x1000 to 0x4000, which tends to be
		 *	incomprehensible.
		 *
		 *	@param[in,out] io The io interface to use for reading.
		 *	@param[out] buffer Pointer to the buffer.
		 *	@param[in] buffer_size The size of the buffer in bytes.
		 *	@param[in] position Position/address in the io interface to read from.
		 *	@param[in] count The number of bytes to read.
		 *
		 *	@returns 0 upon success, anything else means an error.
		 */
		int ctr_cart_raw_interface_read(void *io, void* buffer, size_t buffer_size, uint64_t position, size_t count);

		/**	@brief Reads sectors from the given io interface.
		 *
		 *	3DS carts define their own sector size. They refer to these as the unit
		 *	media size. Most, if not all carts seem to use 512 bytes.
		 *
		 *	This implementation returns the raw data returned by the cart reading
		 *	system for data between 0x1000 to 0x4000, which tends to be
		 *	incomprehensible.
		 *
		 *	@param[in,out] io The io interface to use for reading.
		 *	@param[out] buffer Pointer to the buffer.
		 *	@param[in] buffer_size The size of the buffer in bytes.
		 *	@param[in] sector Sector position in the io interface to read from.
		 *	@param[in] count The number of sectors to read.
		 *
		 *	@returns 0 upon success, anything else means an error.
		 */
		int ctr_cart_raw_interface_read_sector(void *io, void* buffer, size_t buffer_size, size_t sector, size_t count);
	private:
		uint32_t cart_id;
		//From looking at  the cart code, the header may only be 0x200 in size...
		ctr_ncch_header ncch_header;
		uint8_t ncch_raw[0x200];
		ctr_ncsd_cart_header ncsd_header;
		uint32_t sec_keys[4];
		uint32_t media_unit_size;
	} ctr_cart_interface;

}

#endif

#endif//CTR_CART_H_

