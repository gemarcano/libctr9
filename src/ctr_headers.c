#include <ctr9/ctr_headers.h>

#include <stdint.h>
#include <stddef.h>
#include <string.h>

void ctr_ncsd_header_load(ctr_ncsd_cart_header *header, const uint8_t *data, size_t data_size)
{
	ctr_core_ncsd_header_load(header, data, data_size);
}

void ctr_ncch_header_load(ctr_ncch_header *header, const uint8_t *data, size_t data_size)
{
	ctr_core_ncch_header_load(header, data, data_size);
}

