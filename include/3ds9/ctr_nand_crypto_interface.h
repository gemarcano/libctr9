#ifndef CTR_NAND_CRYPTO_INTRFACE_H_
#define CTR_NAND_CRYPTO_INTRFACE_H_

#include "ctr_io_interface.h"
#include "sdmmc/sdmmc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	ctr_io_interface base;
	ctr_io_interface *lower_io;
} ctr_nand_crypto_interface;

int ctr_nand_crypto_interface_initialize(ctr_nand_crypto_interface *io, unsigned int slot, ctr_io_interface* lower_io);
void ctr_nand_crypto_interface_destroy(ctr_nand_crypto_interface *io);

int ctr_nand_crypto_interface_read(void *ctx, void *buffer, size_t buffer_size, size_t position, size_t count);
int ctr_nand_crypto_interface_write(void *ctx, const void *buffer, size_t buffer_size, size_t position);
int ctr_nand_crypto_interface_sector_read(void *ctx, void *buffer, size_t buffer_size, size_t position, size_t count);
int ctr_nand_crypto_interface_sector_write(void *ctx, const void *buffer, size_t buffer_size, size_t position);
size_t ctr_nand_crypto_interface_disk_size(void *ctx);
size_t ctr_nand_crypto_interface_sector_size(void *ctx);

#ifdef __clusplus
}
#endif

#endif//CTR_NAND_CRYPTO_INTRFACE_H_

