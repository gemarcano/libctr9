#ifndef CTR_IO_INTERFACE_H_
#define CTR_IO_INTERFACE_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

/**	@brief Pointer to generic io interface function used to read.
 *
 *	@param
 *	@param
 *	@param
 *	@param
 *
 *	@returns 0 upon success, anything else means an error.
 */
typedef int (*ctr_io_interface_read)(void *ctx, void *buffer, size_t buffer_size, size_t position, size_t count);
typedef int (*ctr_io_interface_write)(void *ctx, const void *buffer, size_t buffer_size, size_t position);

typedef int (*ctr_io_interface_read_sector)(void *ctx, void *buffer, size_t buffer_size, size_t sector, size_t count);
typedef int (*ctr_io_interface_write_sector)(void *ctx, const void *buffer, size_t buffer_size, size_t sector);

typedef size_t (*ctr_io_interface_disk_size)(void *ctx);
typedef size_t (*ctr_io_interface_sector_size)(void *ctx);

typedef struct
{
	ctr_io_interface_read read;
	ctr_io_interface_write write;
	ctr_io_interface_read_sector read_sector;
	ctr_io_interface_write_sector write_sector;
	ctr_io_interface_disk_size disk_size;
	ctr_io_interface_sector_size sector_size;
} ctr_io_interface;

int ctr_io_read(void *io, void *buffer, size_t buffer_size, size_t position, size_t count);
int ctr_io_write(void *io, const void *buffer, size_t buffer_size, size_t position);
int ctr_io_read_sector(void *io, void *buffer, size_t buffer_size, size_t sector, size_t count);
int ctr_io_write_sector(void *io, const void *buffer, size_t buffer_size, size_t sector);
size_t ctr_io_disk_size(void *io);
size_t ctr_io_sector_size(void *io);

#ifdef __cplusplus
}
#endif//__cplusplus


#endif//CTR_IO_INTERFACE_H_

