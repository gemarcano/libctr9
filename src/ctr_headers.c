#include <ctr9/ctr_headers.h>

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#define COPY_ENTRY(HDR_DST, DATA, OFFSET) \
	memcpy(&HDR_DST, (DATA) + (OFFSET), sizeof(HDR_DST)); \
		OFFSET += sizeof(HDR_DST)

void ctr_ncsd_header_load(ctr_ncsd_cart_header *header, const uint8_t *data, size_t data_size)
{
	if (data_size < 0x200) return;

	size_t offset = 0;
	COPY_ENTRY(header->signature, data, offset);
	COPY_ENTRY(header->magic, data, offset);
	COPY_ENTRY(header->media_size, data, offset);
	COPY_ENTRY(header->media_id, data, offset);
	COPY_ENTRY(header->partition_type, data, offset);
	COPY_ENTRY(header->partition_crypt, data, offset);

	for (size_t i = 0; i < 8; ++i)
	{
		COPY_ENTRY(header->partition_offset_length_table[i].media_offset, data, offset);
		COPY_ENTRY(header->partition_offset_length_table[i].media_length, data, offset);
	}

	COPY_ENTRY(header->exheader_sha256, data, offset);
	COPY_ENTRY(header->header_size, data, offset);
	COPY_ENTRY(header->sector_zero_offset, data, offset);
	COPY_ENTRY(header->partition_flags, data, offset);
	COPY_ENTRY(header->partition_id_table, data, offset);
	COPY_ENTRY(header->reserved1, data, offset);
	COPY_ENTRY(header->reserved2, data, offset);
	header->anti_piracy_ = data[0x1FE];
	header->save_crypto_extra = data[0x1FF];
}

void ctr_ncch_header_load(ctr_ncch_header *header, const uint8_t *data, size_t data_size)
{
	if (data_size < 0x200) return;
	size_t offset = 0;

	COPY_ENTRY(header->signature, data, offset);
	COPY_ENTRY(header->magic, data, offset);
	COPY_ENTRY(header->content_size, data, offset);
	COPY_ENTRY(header->partition_id, data, offset);
	COPY_ENTRY(header->maker_code, data, offset);
	COPY_ENTRY(header->version, data, offset);
	COPY_ENTRY(header->hash_content_lock_verify, data, offset);
	COPY_ENTRY(header->program_id, data, offset);
	COPY_ENTRY(header->reserved1, data, offset);
	COPY_ENTRY(header->logo_region_sha256, data, offset);
	COPY_ENTRY(header->product_code, data, offset);
	COPY_ENTRY(header->exheader_sha256, data, offset);
	COPY_ENTRY(header->exheader_size, data, offset);
	COPY_ENTRY(header->reserved2, data, offset);
	COPY_ENTRY(header->flags, data, offset);
	COPY_ENTRY(header->plain_offset_region, data, offset);
	COPY_ENTRY(header->plain_region_size, data, offset);
	COPY_ENTRY(header->logo_offset_region, data, offset);
	COPY_ENTRY(header->logo_region_size, data, offset);
	COPY_ENTRY(header->exefs_offset_region, data, offset);
	COPY_ENTRY(header->exefs_region_size, data, offset);
	COPY_ENTRY(header->exefs_hash_region_size, data, offset);
	COPY_ENTRY(header->reserved3, data, offset);
	COPY_ENTRY(header->romfs_offset_region, data, offset);
	COPY_ENTRY(header->romfs_region_size, data, offset);
	COPY_ENTRY(header->romfs_hash_region_size, data, offset);
	COPY_ENTRY(header->reserved4, data, offset);
	COPY_ENTRY(header->exefs_superblock_sha256, data, offset);
	COPY_ENTRY(header->romfs_superblock_sha256, data, offset);
}

