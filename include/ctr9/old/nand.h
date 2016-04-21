#ifndef DS3_FATFS_NAND_H_
#define DS3_FATFS_NAND_H_

#include <stddef.h>
#include <stdint.h>

void nand_init(void);
int nand_read(size_t location, size_t size, uint8_t *dest);
int nand_write(size_t location, size_t size, const uint8_t *source);

#endif//DS3_FATFS_NAND_H_

