#ifndef CTR_IO_INTERFACE_H_
#define CTR_IO_INTERFACE_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

typedef int (*ctr_io_interface_read)(void *ctx, void *buffer, size_t buffer_size, size_t position, size_t count);
typedef int (*ctr_io_interface_write)(void *ctx, const void *buffer, size_t buffer_size, size_t position);

typedef int (*ctr_io_interface_sector_read)(void *ctx, void *buffer, size_t buffer_size, size_t sector, size_t count);
typedef int (*ctr_io_interface_sector_write)(void *ctx, const void *buffer, size_t buffer_size, size_t sector);

typedef size_t (*ctr_io_interface_disk_size)(void *ctx);
typedef size_t (*ctr_io_interface_sector_size)(void *ctx);

typedef struct
{
	ctr_io_interface_read read;
	ctr_io_interface_write write;
	ctr_io_interface_sector_read sector_read;
	ctr_io_interface_sector_write sector_write;
	ctr_io_interface_disk_size disk_size;
	ctr_io_interface_sector_size sector_size;
} ctr_io_interface;

int ctr_io_read(void *io, void *buffer, size_t buffer_size, size_t position, size_t count);
int ctr_io_write(void *io, const void *buffer, size_t buffer_size, size_t position);
int ctr_io_sector_read(void *io, void *buffer, size_t buffer_size, size_t sector, size_t count);
int ctr_io_sector_write(void *io, const void *buffer, size_t buffer_size, size_t sector);
size_t ctr_io_disk_size(void *io);
size_t ctr_io_sector_size(void *io);

#ifdef __cplusplus
}
#endif//__cplusplus


#endif//DS39_IO_INTERFACE_H_

