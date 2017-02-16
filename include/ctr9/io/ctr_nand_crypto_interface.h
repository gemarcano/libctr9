/*******************************************************************************
 * Copyright (C) 2016, 2017 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */

#ifndef CTR_NAND_CRYPTO_INTERFACE_H_
#define CTR_NAND_CRYPTO_INTERFACE_H_

#include "ctr_crypto_interface.h"
#include "ctr_io_interface.h"
#include "sdmmc/sdmmc.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**	@brief Represents the type of underlying partitions to access in NAND.
 */
typedef enum
{
	NAND_CTR, NAND_TWL
} ctr_nand_crypto_type;

/**	@brief Filter io interface to apply encryption while reading NAND.
 */
typedef struct ctr_nand_crypto_interface ctr_nand_crypto_interface;

/**	@brief Initialize the given NAND crypto io interface object.
 *
 *	@param[in] key_slot Nintendo 3DS AES key slot to use for CTR.
 *	@param[in] crypto_type Type of the system to taylor encryption to. CTR and
 *		TWL sections handle encryption slightly differently.
 *	@param[in,out] lower_io io interface to use as NAND. The filter does not
 *		gain ownership of the lower_io interface, it merely uses it. This
 *		pointer must remain valid while the crypto io interface object is in
 *		use.
 *
 *	@returns The initialized io interface that can be used for decrypting
 *      NAND, NULL on an error.
 */
ctr_nand_crypto_interface *ctr_nand_crypto_interface_initialize(uint8_t key_slot, ctr_nand_crypto_type crypto_type, void* lower_io);

/** @brief Destroys the given NAND crypto io interface object.
 *
 *  @param[in,out] io NAND io interface to deinitialize.
 *
 *  @post The io interface has been destroyed and cannot be used for decrypting
 *      NAND without being re-initialized. The lower_io passed at construction/
 *      iniitalization remains valid.
 */
void ctr_nand_crypto_interface_destroy(ctr_nand_crypto_interface *io);

#ifdef __cplusplus
}

namespace ctr9
{
	template<class IO>	
	class nand_crypto_interface
	{
	public:
		
		nand_crypto_interface(uint8_t key_slot, ctr_nand_crypto_type crypto_type, IO& lower_io);

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
		 *  Uses whatever the underlying io interface uses for sector size.
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
		 *  Uses whatever the underlying io interface uses for sector size.
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
		 *  @returns The size of the underlying io interface as reported by it.
		 */
		uint64_t disk_size() const;

		/** @brief Returns the size of the sectors used by the underlying io interface.
		 *
		 *  @returns The size in bytes of the underlying io interface.
		 */
		constexpr size_t sector_size() const;
	};

	typedef io_interface_impl<nand_crypto_interface<io_interface&>> nand_crypto_generic_interface;
}

#endif

#endif//CTR_NAND_CRYPTO_INTERFACE_H_

