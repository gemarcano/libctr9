#ifndef CTR_CART_H_
#define CTR_CART_H_

#include <ctr9/ctr_headers.h>

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct
{
	//From looking at  the cart code, the header may only be 0x200 in size...
	ctr_ncch_header ncch_header;
	uint8_t ncch_raw[0x200];
	ctr_ncsd_header ncsd_header;
	uint32_t sec_keys[4];
	uint32_t media_unit_size;
} ctr_cart;

bool ctr_cart_initialize(ctr_cart *cart);
bool ctr_cart_inserted(void);
void ctr_cart_read_sector(ctr_cart *cart, void* buffer, size_t buffer_size, size_t sector, size_t count);
void ctr_cart_raw_read_sector(ctr_cart *cart, void* buffer, size_t buffer_size, size_t sector, size_t count);
int ctr_cart_read(ctr_cart *cart, void* buffer, size_t buffer_size, size_t location, size_t count);
int ctr_cart_raw_read(ctr_cart *cart, void* buffer, size_t buffer_size, size_t sector, size_t count);
size_t ctr_cart_rom_size(ctr_cart *cart);


#endif//CTR_CART_H_

