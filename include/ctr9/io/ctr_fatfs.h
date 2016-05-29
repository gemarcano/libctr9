#ifndef CTR_FATFS_H_
#define CTR_FATFS_H_

#include <ctr9/io/ctr_nand_interface.h>
#include <ctr9/io/ctr_nand_crypto_interface.h>
#include <ctr9/io/ctr_sd_interface.h>

int ctr_fatfs_initialize(ctr_nand_interface *nand_io, ctr_nand_crypto_interface *ctr_io, ctr_nand_crypto_interface *twl_io, ctr_sd_interface *sd_io);

#endif//CTR_FATFS_H_

