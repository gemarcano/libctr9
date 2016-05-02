/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs	 (C)ChaN, 2016	         */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be	     */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting	     */
/* storage control modules to the FatFs module with a defined API.	     */
/*-----------------------------------------------------------------------*/

// Implementation for 3DS stuff was originally written by Gabriel Marcano.
// License for this particular file is the same as fatfs, but note that it
// is (most likely) linked statically with libctr9, which is GPL 2 or later.

#include <ctr9/io/fatfs/diskio.h>		/* FatFs lower layer API */
#include <ctr9/io.h>
#include <ctr9/ctr_system.h>
#include <ctr9/io/fatfs/ctr_fatfs_disk.h>

/* Definitions of physical drive number for each drive */
#define CTRNAND	0
#define TWL		1
#define TWLP	2
#define SD		3
#define DISK0	4
#define DISK1	5
#define DISK2	6
#define DISK3	7
#define DISK4	8
#define DISK5	9

static ctr_fatfs_disk ctr_fatfs_disks[10] = {0};

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive number to identify the drive */
)
{
	return ctr_fatfs_disks[pdrv].status;
}


/*-----------------------------------------------------------------------*/
/* Initialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive number to identify the drive */
)
{
	DSTATUS stat = STA_NOINIT;
	ctr_fatfs_disk *disk = &ctr_fatfs_disks[pdrv];

	stat = (disk->io && disk->sectors) ?
		disk->status = 0 : (disk->status |= STA_NOINIT);

	return stat;
}


/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive number to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res = RES_PARERR;
	int result;

	ctr_fatfs_disk *disk = &ctr_fatfs_disks[pdrv];

	if (!(disk->status & STA_NOINIT))
	{
		result = ctr_io_read_sector(disk->io, buff, count * 0x200, sector + disk->sector_offset, count);
		res = result ? RES_ERROR : RES_OK;
	}

	return res;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive number to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res;
	int result;

	switch (pdrv) {
	case CTRNAND :
		// translate the arguments here

		//result = ATA_disk_write(buff, sector, count);

		// translate the reslut code here

	case TWL :
	case TWLP :
	default:
		break;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/


DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive number (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res = RES_PARERR;

	ctr_fatfs_disk *disk = &ctr_fatfs_disks[pdrv];
	switch (cmd)
	{
	case GET_SECTOR_COUNT:
		if (!(disk->status & STA_NOINIT))
		{
			*(DWORD*)buff = disk->sectors;
			return RES_OK;
		}
		return RES_NOTRDY;
		
	case GET_SECTOR_SIZE:
		if (!(disk->status & STA_NOINIT))
		{
			*(WORD*)buff = ctr_io_sector_size(disk->io);
			return RES_OK;
		}
		return RES_NOTRDY;
	case GET_BLOCK_SIZE:
		*(DWORD*)buff = 1; //Not sure what the block size for SD is...
	case CTRL_TRIM:
	case CTRL_SYNC:
		return RES_OK;
	case CTR_SETUP_DISK:
		{
			ctr_setup_disk_parameters *params = (ctr_setup_disk_parameters*)buff;
			ctr_fatfs_disk_initialize(disk, params->io, params->sector_offset, params->sectors);
		}
		return RES_OK;
	default:
		return RES_PARERR;
	}
	return RES_NOTRDY;
}

