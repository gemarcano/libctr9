/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */

#ifndef CTR_FIRM_H_
#define CTR_FIRM_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**	@brief Represents the information found in the firm header describing
 *		section information.
 */
typedef struct
{
	uint32_t offset;
	uint32_t load_address;
	uint32_t size;
	uint32_t type;
	uint8_t sha256hash[0x20];
} ctr_firm_section_header;

/**	@brief Represents the information found in the firm header.
 */
typedef struct
{
	uint32_t magic;
	uint8_t reserved1[4];
	uint32_t arm11_entry;
	uint32_t arm9_entry;
	uint8_t reserved2[0x30];
	ctr_firm_section_header section_headers[4];
	uint8_t rsa2048signature[0x100]; //signature of header's hash
} ctr_firm_header;

/**	@brief Represents the information found in the arm9bin header.
 */
typedef struct
{
	uint8_t enc_keyx[16];
	uint8_t keyy[16];
	uint8_t ctr[16];
	uint8_t ascii_size[8];
	uint8_t unknown[8]; //FIXME
	uint8_t control_block[16];
	uint8_t hardware_debug[16]; //9.5+
	uint8_t enc_key16x[16]; //9.5+
} ctr_arm9bin_header;

/**	@brief Loads the given firm section header structure with the values found
 *		in memory.
 *
 *	@param[out] header Header structure to fill.
 *	@param[in] data Raw firm section header data found in memory. Must be at
 *		least 0x30 in size.
 *
 *	@post The given firm section header struct has been filled out based on the
 *		data found in memory.
 */
void ctr_firm_section_header_load(ctr_firm_section_header *header, const void *data);

/**	@brief Loads the given firm header structure with the values found in
 *		memory.
 *
 *	@param[out] header Header structure to fill.
 *	@param[in] data Raw firm header data found in memory. Must be at least 0x200
 *		in size.
 *
 *	@post The given firm header struct has been filled out based on the data
 *		found in memory.
 */
void ctr_firm_header_load(ctr_firm_header *header, const void *data);

/**	@brief Loads the given arm9bin header structure with the values found in
 *		memory.
 *
 *	@param[out] header Header structure to fill.
 *	@param[in] data Raw arm9bin header data found in memory. Must be at least 0x200
 *		in size.
 *
 *	@post The given firm header struct has been filled out based on the data
 *		found in memory.
 */
void ctr_arm9bin_header_load(ctr_arm9bin_header *header, const void *data);

#ifdef __cplusplus
}
#endif

#endif//TR_FIRM_H_

