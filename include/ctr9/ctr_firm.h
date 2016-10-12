#ifndef CTR_FIRM_H_
#define CTR_FIRM_H_

#include <stdint.h>

typedef struct
{
	uint32_t offset;
	uint32_t load_address;
	uint32_t size;
	uint32_t type;
	uint8_t sha256hash[0x20];
} ctr_firm_section_header;

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

void ctr_firm_section_header_load(ctr_firm_section_header *header, void *data);
void ctr_firm_header_load(ctr_firm_header *header, void *data);
void ctr_arm9bin_header_load(ctr_arm9bin_header *header, void *data);

#endif//TR_FIRM_H_

