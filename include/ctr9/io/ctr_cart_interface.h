/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

#ifndef CTR_CART_H_
#define CTR_CART_H_

#include <ctr9/ctr_headers.h>
#include <ctr9/io/ctr_io_interface.h>

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	ctr_io_interface base;
	
	uint32_t cart_id;
	//From looking at  the cart code, the header may only be 0x200 in size...
	ctr_ncch_header ncch_header;
	uint8_t ncch_raw[0x200];
	ctr_ncsd_header ncsd_header;
	uint32_t sec_keys[4];
	uint32_t media_unit_size;
} ctr_cart_interface;

bool ctr_cart_interface_initialize(ctr_cart_interface *cart);
bool ctr_cart_inserted(void);
int ctr_cart_interface_read_sector(void *io, void* buffer, size_t buffer_size, size_t sector, size_t count);
int ctr_cart_interface_read(void *io, void* buffer, size_t buffer_size, uint64_t position, size_t count);
int ctr_cart_interface_noop_write(void *io, const void* buffer, size_t buffer_size, uint64_t position);
int ctr_cart_interface_noop_write_sector(void *io, const void* buffer, size_t buffer_size, size_t sector);
uint64_t ctr_cart_interface_disk_size(void *io);
size_t ctr_cart_interface_sector_size(void *io);


int ctr_cart_raw_interface_read_sector(void *io, void* buffer, size_t buffer_size, size_t sector, size_t count);
int ctr_cart_raw_interface_read(void *io, void* buffer, size_t buffer_size, uint64_t position, size_t count);

#ifdef __cplusplus
}
#endif

#endif//CTR_CART_H_

