/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */

#ifndef CTR_HEADERS_H_
#define CTR_HEADERS_H_

#include <ctr_core/ctr_core_headers.h>

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**	@brief Struct representing the NCSD header for carts.
 */
typedef ctr_core_ncsd_cart_header ctr_ncsd_cart_header;

/**	@brief Struct representing the NCCH header.
 */
typedef ctr_core_ncch_header ctr_ncch_header;

/**	@brief Loads the given ctr_ncsd_cart_header with the data from the actual
 *		NCSD cart header in memory.
 *
 *	@param[out] header Struct to load.
 *	@param[in] data Pointer to NCSD in memory to load into header struct.
 *	@param[in] data_size Length of the buffer in memory with the NCSD data. This
 *		function expects this number to be greater than or equal to 0x200, else
 *		the function refuses to load the struct, even partially.
 *
 *	@post If data_size >= 0x200, header is updated to include the parsed data
 *		from the NCSD header in memory, else nothing happens.
 */
void ctr_ncsd_header_load(ctr_ncsd_cart_header *header, const uint8_t *data, size_t data_size);

/**	@brief Loads the given ctr_ncch_header with the data from the actual NCCH
 *		header in memory.
 *
 *	@param[out] header Struct to load.
 *	@param[in] data Pointer to NCCH in memory to load into header struct.
 *	@param[in] data_size Length of the buffer in memory with the NCCH data. This
 *		function expects this number to be greater than or equal to 0x200, else
 *		the function refuses to load the struct, even partially.
 *
 *	@post If data_size >= 0x200, header is updated to include the parsed data
 *		from the NCSD header in memory, else nothing happens.
 */
void ctr_ncch_header_load(ctr_ncch_header *header, const uint8_t *data, size_t data_size);

#ifdef __cplusplus
}
#endif

#endif//CTR_HEADERS_H_

