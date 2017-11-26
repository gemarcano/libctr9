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

#endif

#endif//CTR_NAND_CRYPTO_INTERFACE_H_

