/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */

#ifndef CTR_AESKEYDB_H_
#define CTR_AESKEYDB_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	uint8_t slot;
	char type;
	char id[10];
	uint8_t reserved[2];
	uint8_t is_devkit_key;
	uint8_t is_encrypted;
	uint8_t key[16];
} ctr_aesdb_entry;

/**	@brief Loads the given entry structure with the data found in memory at the
 *		given address.
 *
 *	@param[out] entry Entry structure to fill.
 *	@param[in] data Data in memory that contains the raw aesdb data. This must
 *		be at least 32 bytes in length.
 *
 *	@post The given entry will have its structure loaded based on the data found
 *		in memory.
 */
void ctr_aesdb_entry_load(ctr_aesdb_entry *entry, const void *data);

/**	@brief Applies the encryption algorithm to the key in the given entry, as
 *		designed by d0k3 in Decrypt9.
 *
 *	@param[in,out] entry Entry with the key to be processed through the
 *		algorithm.
 *
 *	@post The key inside the given entry has been processed through the
 *		encryption algorithm, and the encryption flag on the entry has been
 *		updated accordingly.
 */
void ctr_aesdb_entry_crypt_key(ctr_aesdb_entry *entry);

#ifdef __cplusplus
}
#endif

#endif//CTR_AESKEYDB_H_

