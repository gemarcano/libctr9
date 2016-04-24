/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include <ctr9/io/fatfs/diskio.h>		/* FatFs lower layer API */
#include <ctr9/io.h>

/* Definitions of physical drive number for each drive */
#define CTRNAND	0
#define TWL		1
#define TWLP	2
#define SD		3
#define EMU1	4

static ctr_nand_crypto_interface *ctrnand;
static ctr_sd_interface *sd_disk;

void disk_prepare(BYTE pdrv, void *io)
{
	
	switch (pdrv) {
	case CTRNAND :
		ctrnand = (ctr_nand_crypto_interface*)io;
		break;
	case TWL :
	case TWLP :
	case SD :
		sd_disk = (ctr_sd_interface*)io;
		break;
	case EMU1 :
	default:
		break;
	}
}


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	int result;

	switch (pdrv) {
	case CTRNAND :

		return STA_NOINIT;

	case TWL :
	case TWLP :
	case SD :
		
		return 0;
	case EMU1 :
	default:
		break;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	int result;

	switch (pdrv) {
	case CTRNAND :
		if (ctrnand) return 0;
		return STA_NOINIT;
		// translate the reslut code here
	case TWL :
	case TWLP:
	case SD:
		if (sd_disk) return 0;
		return STA_NOINIT;
	case EMU1:
	default :
		break;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

#define CTRNAND_LOCATION 0x0B930000

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res;
	int result;

	switch (pdrv) {
	case CTRNAND :
		// translate the arguments here
		result = ctr_io_read_sector(ctrnand, buff, count*512, sector + CTRNAND_LOCATION/0x200, count);

		return RES_OK;

	case TWL :
	case TWLP:
	case EMU1:
	default:
		break;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
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

		return RES_OK;

	case TWL :
	case TWLP :
	case EMU1:
	default:
		break;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	int result;

	switch (pdrv) {
	case CTRNAND :

		// Process of the command for the ATA drive

		return res;

	case TWL :

		// Process of the command for the MMC/SD card

		return res;

	case TWLP :

		// Process of the command the USB drive

		return res;
	default:
		break;
	}

	return RES_PARERR;
}

