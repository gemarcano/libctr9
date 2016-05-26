#include <ctr9/io/fatfs/ctr_fatfs_disk.h>

void ctr_fatfs_default_setup(ctr_nand_crypto_interface *ctr_io, ctr_nand_crypto_interface *twl_io, ctr_sd_interface *sd_io)
{
	ctr_setup_disk_parameters params;
	if (ctr_io)
	{
		params = (ctr_setup_disk_parameters){ctr_io, 0x0B930000/0x200, 0x2F5D0000/0x200};
	    disk_ioctl(0, CTR_SETUP_DISK, &params);
	}

	if (twl_io)
	{
		params = (ctr_setup_disk_parameters){twl_io, 0x00012E00/0x200, 0x08FB5200/0x200 };
		disk_ioctl(1, CTR_SETUP_DISK, &params);

		params = (ctr_setup_disk_parameters){twl_io, 0x09011A00/0x200, 0x020B6600/0x200};
		disk_ioctl(2, CTR_SETUP_DISK, &params);
	}

	if (sd_io)
	{
		params = (ctr_setup_disk_parameters){sd_io, 0, ctr_io_disk_size(sd_io)};
		disk_ioctl(3, CTR_SETUP_DISK, &params);
	}
}

void ctr_fatfs_disk_initialize(ctr_fatfs_disk *disk, void *io, size_t sector_offset, size_t sectors)
{
	size_t sector_size = ctr_io_sector_size(io);
	*disk = (ctr_fatfs_disk){io, sector_offset, sectors, STA_NOINIT};
}

void ctr_fatfs_disk_destroy(ctr_fatfs_disk *disk)
{
	*disk = (ctr_fatfs_disk){ NULL, 0, 0, STA_NOINIT};
}

