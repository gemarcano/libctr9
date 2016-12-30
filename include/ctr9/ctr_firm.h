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

#include <ctr_core/ctr_core_firm.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**	@brief Represents the information found in the firm header describing
 *		section information.
 */
typedef ctr_core_firm_section_header ctr_firm_section_header;

/**	@brief Represents the information found in the firm header.
 */
typedef ctr_core_firm_header ctr_firm_header;

/**	@brief Represents the information found in the arm9bin header.
 */
typedef ctr_core_arm9bin_header ctr_arm9bin_header;

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

