#ifndef CTR_SD_INTRFACE_H_
#define CTR_SD_INTRFACE_H_

#include "ctr_io_interface.h"
#include "sdmmc/sdmmc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	ctr_io_interface base;
} ctr_sd_interface;

int ctr_sd_interface_initialize(ctr_sd_interface *io);
void ctr_sd_interface_destroy(ctr_sd_interface *io);

int ctr_sd_interface_read(void *ctx, void *buffer, size_t buffer_size, size_t position, size_t count);
int ctr_sd_interface_write(void *ctx, const void *buffer, size_t buffer_size, size_t position);
int ctr_sd_interface_read_sector(void *ctx, void *buffer, size_t buffer_size, size_t position, size_t count);
int ctr_sd_interface_write_sector(void *ctx, const void *buffer, size_t buffer_size, size_t position);
size_t ctr_sd_interface_disk_size(void *ctx);
size_t ctr_sd_interface_sector_size(void *ctx);

#ifdef __clusplus
}
#endif

#endif//CTR_SD_INTRFACE_H_

