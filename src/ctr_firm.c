#include <ctr9/ctr_firm.h>
#include <string.h>

#define LOAD(O,E,A) \
memcpy(&((O)->E), A, sizeof((O)->E)); \
A += sizeof((O)->E);

void ctr_firm_section_header_load(ctr_firm_section_header *header, const void *data)
{
	const uint8_t *d = data;
	LOAD(header, offset, d);
	LOAD(header, load_address, d);
	LOAD(header, size, d);
	LOAD(header, type, d);
	LOAD(header, sha256hash, d);
}

void ctr_firm_header_load(ctr_firm_header *header, const void *data)
{
	const uint8_t *d = data;
	LOAD(header, magic, d);
	LOAD(header, reserved1, d);
	LOAD(header, arm11_entry, d);
	LOAD(header, arm9_entry, d);
	LOAD(header, reserved2, d);
	for (size_t i = 0; i < 4; ++i)
	{
		ctr_firm_section_header_load(&(header->section_headers[i]), d);
		d += sizeof(ctr_firm_section_header);
	}
	LOAD(header, rsa2048signature, d);
}

void ctr_arm9bin_header_load(ctr_arm9bin_header *header, const void *data)
{
	const uint8_t *d = data;
	LOAD(header, enc_keyx, d);
	LOAD(header, keyy, d);
	LOAD(header, ctr, d);
	LOAD(header, ascii_size, d);
	LOAD(header, unknown, d);
	LOAD(header, control_block, d);
	LOAD(header, hardware_debug, d);
	LOAD(header, enc_key16x, d);
}

