#include <ctr9/io/fatfs/ctr_fatfs_disk.h>

void ctr_fatfs_disk_initialize(ctr_fatfs_disk *disk, void *io, size_t sector_offset, size_t sectors)
{
	size_t sector_size = ctr_io_sector_size(io);
	*disk = (ctr_fatfs_disk){io, sector_offset, sectors, STA_NOINIT};
}

void ctr_fatfs_disk_destroy(ctr_fatfs_disk *disk)
{
	*disk = (ctr_fatfs_disk){ NULL, 0, 0, STA_NOINIT};
}

