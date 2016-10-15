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

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**	@brief Struct representing the NCSD header for carts.
 */
typedef struct
{
	uint8_t signature[0x100]; //rsa 2048 signature of a sha256 of the header
	uint8_t magic[4]; //Should be the string NCSD
	uint32_t media_size; //Size of NCSD image, in media units
	uint8_t media_id[8]; //or title id (CTR-[etc])
	uint8_t partition_type[8]; //0 == none, 1 == normal, 3 == FIRM, 4 == AGB_FIRM save
	uint8_t partition_crypt[8]; //one byte per partition in the table
	struct
	{
		uint32_t media_offset; //in media units
		uint32_t media_length; //in media units
	} partition_offset_length_table[8];

	uint8_t exheader_sha256[0x20]; //sha256 of exheader
	uint32_t header_size;
	uint32_t sector_zero_offset;
	uint8_t partition_id_table[8][8];
	uint8_t partition_flags[8];
	uint8_t reserved1[0x20];
	uint8_t reserved2[0xE];
	uint8_t anti_piracy_; //This is a guess on the function of this thing, based on 3dbrew
	uint8_t save_crypto_extra; //This is somewhat documented in Parition Flags for 9.6+

} ctr_ncsd_cart_header;

/**	@brief Struct representing the NCCH header.
 */
typedef struct
{
	uint8_t signature[0x100]; //rsa 2048 signature of a sha256 of the header
	uint8_t magic[4];
	uint32_t content_size; //in media units
	uint8_t partition_id[8];
	uint8_t maker_code[2];
	uint8_t version[2];
	uint32_t hash_content_lock_verify;
	uint8_t program_id[8];
	uint8_t reserved1[0x10];
	uint8_t logo_region_sha256[0x20]; //used 5.0.0-11+
	uint8_t product_code[0x10];
	uint8_t exheader_sha256[0x20]; //(SHA256 of 2x Alignment Size, beginning at 0x0 of ExHeader)
	uint32_t exheader_size; //in bytes
	uint8_t reserved2[4];
	uint8_t flags[8];
	uint32_t plain_offset_region; //in media units
	uint32_t plain_region_size; //in media units
	uint32_t logo_offset_region; //in media units
	uint32_t logo_region_size; //in media units
	uint32_t exefs_offset_region; //in media units
	uint32_t exefs_region_size; //in media units
	uint32_t exefs_hash_region_size; //in media units
	uint8_t reserved3[4];
	uint32_t romfs_offset_region; //in media units
	uint32_t romfs_region_size; //in media units
	uint32_t romfs_hash_region_size; //in media units
	uint8_t reserved4[4];
	uint8_t exefs_superblock_sha256[0x20]; //starting at 0x0 of the ExeFS over the number of media units specified in the ExeFS hash region size
	uint8_t romfs_superblock_sha256[0x20]; //starting at 0x0 of the RomFS over the number of media units specified in the RomFS hash region size
} ctr_ncch_header;

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

