/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */

#ifndef CTR_CRYPTO_INTERFACE_H_
#define CTR_CRYPTO_INTERFACE_H_

#include "ctr_io_interface.h"

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**	@brief Represent the types of cryptographic AES modes that can be used with
 *		ctr_crypto_inteface.
 */
typedef enum
{
	CRYPTO_CCM, CRYPTO_CTR, CRYPTO_CBC, CRYPTO_ECB
} ctr_crypto_type;

/**	@brief Represent the mode of the underlying storage in a
 *		ctr_crypto_interface.
 *
 *	Encrypted means that the underlying storage is encrypted, while plaintext
 *	means that the underlying storage is stored plaintext. This affects what
 *	happens to the data with read and write functions. If the underlying storage
 *	is encrypted, reading decrypts it and writing encrypts the incoming data. If
 *	the underlying storage is plaintext, reading encrypts the outgoing data and
 *	writing decrypts it.
 */
typedef enum
{
	CTR_CRYPTO_ENCRYPTED, CTR_CRYPTO_PLAINTEXT
} ctr_crypto_disk_type;

/**	@brief Filter io interface to apply encryption while acting on the
 *		underlying interface.
 */
typedef struct ctr_crypto_interface ctr_crypto_interface;

struct ctr_crypto_interface
{
	ctr_io_interface base;
	ctr_io_interface *lower_io;

	uint8_t keySlot;
	uint8_t ctr[16];
	uint32_t input_mode;
	uint32_t output_mode;

	void (*advance_ctr_input)(ctr_crypto_interface *io, uint8_t *buffer, size_t buffer_size, size_t block, uint8_t *ctr);
	void (*advance_ctr_output)(ctr_crypto_interface *io, uint8_t *buffer, size_t buffer_size, size_t block, uint8_t *ctr);
	void (*crypto_input)(void* inbuf, void* outbuf, size_t size, uint32_t mode, uint8_t *ctr);
	void (*crypto_output)(void* inbuf, void* outbuf, size_t size, uint32_t mode, uint8_t *ctr);

	size_t block_size;
};

/**	@brief Initialize the given crypto io interface object.
 *
 *	@param[out] io IO crypto interface to initialize.
 *	@param[in] key_slot Nintendo 3DS AES key slot to use for CTR.
 *	@param[in] mode AES mode flags to use.
 *	@param[in] disk_type Encryption state of the underlying disk.
 *	@param[in] type AES mode to use for this interface.
 *	@param[in] ctr 16 byte array containing the initialization value/vector for
 *		the CTR or IV value to use with the selected AES mode.
 *	@param[in,out] lower_io io interface to use as NAND. The filter does not
 *		gain ownership of the lower_io interface, it merely uses it. This
 *		pointer must remain valid while the crypto io interface object is in
 *		use.
 *
 *	@post The io interface has been initialized and can be used for decrypting
 *		and encrypting.
 */
int ctr_crypto_interface_initialize(ctr_crypto_interface *crypto_io, uint8_t keySlot, uint32_t mode, ctr_crypto_disk_type disk_type, ctr_crypto_type type, uint8_t *ctr, ctr_io_interface *lower_io);

/** @brief Destroys the given crypto io interface object.
 *
 *  @param[in,out] io IO interface to deinitialize.
 *
 *  @post The io interface has been destroyed and cannot be used for decrypting
 *      without being re-initialized. The lower_io passed at construction/
 *      iniitalization remains valid.
 */
void ctr_crypto_interface_destroy(ctr_crypto_interface *io);

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
int ctr_crypto_interface_read(void *io, void *buffer, size_t buffer_size, uint64_t position, size_t count);

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
int ctr_crypto_interface_write(void *io, const void *buffer, size_t buffer_size, uint64_t position);

/** @brief Reads sectors from the given io interface.
 *
 *  Uses whatever the underlying io interface uses for sector size.
 *
 *  @param[in,out] io The io interface to use for reading.
 *  @param[out] buffer Pointer to the buffer.
 *  @param[in] buffer_size The size of the buffer in bytes.
 *  @param[in] sector Sector position in the io interface to read from.
 *  @param[in] count The number of sectors to read.
 *
 *  @returns 0 upon success, anything else means an error.
 */
int ctr_crypto_interface_read_sector(void *io, void *buffer, size_t buffer_size, size_t sector, size_t count);

/** @brief Writes sectors from the given io interface.
 *
 *  Uses whatever the underlying io interface uses for sector size.
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
int ctr_crypto_interface_write_sector(void *io, const void *buffer, size_t buffer_size, size_t sector);

/** @brief Returns the size of the underlying disk for the given io interface.
 *
 *  @returns The size of the underlying io interface as reported by it.
 */
uint64_t ctr_crypto_interface_disk_size(void *io);

/** @brief Returns the size of the sectors used by the underlying io interface.
 *
 *  @returns The size in bytes of the underlying io interface.
 */
size_t ctr_crypto_interface_sector_size(void *io);

#ifdef __cplusplus
}
#endif

#endif//CTR_CRYPTO_INTERFACE_H_

