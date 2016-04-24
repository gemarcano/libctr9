/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

#ifndef CTR_NAND_INTRFACE_H_
#define CTR_NAND_INTRFACE_H_

#include "ctr_io_interface.h"
#include "sdmmc/sdmmc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	ctr_io_interface base;
} ctr_nand_interface;

int ctr_nand_interface_initialize(ctr_nand_interface *io);
void ctr_nand_interface_destroy(ctr_nand_interface *io);

int ctr_nand_interface_read(void *ctx, void *buffer, size_t buffer_size, size_t position, size_t count);
int ctr_nand_interface_write(void *ctx, const void *buffer, size_t buffer_size, size_t position);
int ctr_nand_interface_read_sector(void *ctx, void *buffer, size_t buffer_size, size_t position, size_t count);
int ctr_nand_interface_write_sector(void *ctx, const void *buffer, size_t buffer_size, size_t position);
size_t ctr_nand_interface_disk_size(void *ctx);
size_t ctr_nand_interface_sector_size(void *ctx);

#ifdef __clusplus
}
#endif

#endif//CTR_NAND_INTRFACE_H_

