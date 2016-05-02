#ifndef CTR_FATFS_DISK_H_
#define CTR_FATFS_DISK_H_

#include <ctr9/io/fatfs/diskio.h>
#include <ctr9/io/ctr_io_interface.h>

typedef struct
{
	ctr_io_interface *io;
	size_t sector_offset;
	size_t sectors;
	DSTATUS status;

} ctr_fatfs_disk;

void ctr_fatfs_disk_initialize(ctr_fatfs_disk *disk, void *io, size_t sector_offset, size_t sectors);

void ctr_fatfs_disk_destroy(ctr_fatfs_disk *disk);

#endif//CTR_FATFS_DISK_H_

