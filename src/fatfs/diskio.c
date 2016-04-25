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
#include <ctr9/ctr_system.h>

/* Definitions of physical drive number for each drive */
#define CTRNAND	0
#define TWL		1
#define TWLP	2
#define SD		3
#define EMU1	4

static ctr_sd_interface *sd_disk;

static struct
{
	ctr_nand_crypto_interface *io;
	DSTATUS status;
} ctrnand_state;

static struct
{
	ctr_sd_interface *io;
	DSTATUS status;
} sd_state;

void disk_prepare(BYTE pdrv, void *io)
{
	
	switch (pdrv) {
	case CTRNAND :
		ctrnand_state.io = (ctr_nand_crypto_interface*)io;
		ctrnand_state.status = STA_NOINIT;
		break;
	case TWL :
	case TWLP :
		break;
	case SD :
		sd_state.io = (ctr_sd_interface*)io;
		ctrnand_state.status = STA_NOINIT;
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
	DSTATUS stat = STA_NOINIT;

	switch (pdrv) {
	case CTRNAND :
		stat = ctrnand_state.status;
		break;
	case TWL :
	case TWLP :
		break;
	case SD :
		stat = sd_state.status;
		break;
	case EMU1 :
	default:
		break;
	}
	return stat;
}


/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = STA_NOINIT;

	switch (pdrv) {
	case CTRNAND :
		if (ctrnand_state.io && ctr_io_disk_size(ctrnand_state.io))
		{
			//FIXME check if we can read a known text
			stat = ctrnand_state.status = 0;
		}
		else
		{
			stat = ctrnand_state.status |= STA_NOINIT;
		}
		break;
	case TWL :
	case TWLP:
		break;
	case SD:
		if (sd_state.io && ctr_io_disk_size(sd_state.io))
		{
			stat = sd_state.status = 0;
		}
		else
		{
			stat = sd_state.status |= STA_NOINIT;
		}
		break;
	case EMU1:
	default :
		break;
	}
	return stat;
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
	DRESULT res = RES_PARERR;
	int result;

	switch (pdrv) {
	case CTRNAND :
		// translate the arguments here
		if (!(ctrnand_state.status & STA_NOINIT))
		{
			result = ctr_io_read_sector(ctrnand_state.io, buff, count*512, sector + CTRNAND_LOCATION/0x200, count);
			res = result ? RES_ERROR : RES_OK;
		}
		else
		{
			result = RES_NOTRDY;
		}
		return RES_OK;

	case TWL :
	case TWLP:
	case SD:
		if (!(sd_state.status & STA_NOINIT))
		{
			result = ctr_io_read_sector(sd_state.io, buff, count*512, sector, count);
			res = result ? RES_ERROR : RES_OK;
		}
		else
		{
			result = RES_NOTRDY;
		}
	case EMU1:
	default:
		break;
	}

	return res;
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

static inline size_t nand_sector_count_handler(BYTE partition)
{
	switch (partition)
	{
		case CTRNAND:
			return (ctr_get_system_type() == SYSTEM_O3DS) ? 
				0x2F5D0000/0x200 : 0x41ED0000/0x200;
		case TWL:
			return 0x00012E00/0x200;
		case TWLP:
			return 0x09011A00/0x200;
		default:
			return 0;
	}
}

static inline DRESULT nand_handle_ioctl(BYTE cmd, void *buff, BYTE partition)
{
	switch (cmd)
	{
	case GET_SECTOR_COUNT:
		{
			if (!(ctrnand_state.status & STA_NOINIT))
			{
				*(DWORD*)buff = nand_sector_count_handler(partition);
				return RES_OK;
			}
			return RES_NOTRDY;
		}
	case GET_SECTOR_SIZE:
		if (!(ctrnand_state.status & STA_NOINIT))
		{
			*(WORD*)buff = ctr_io_sector_size(ctrnand_state.io);
			return RES_OK;
		}
		return RES_NOTRDY;
	case GET_BLOCK_SIZE:
		*(DWORD*)buff = 1; //Not sure what the block size for NAND is...
	case CTRL_TRIM:
	case CTRL_SYNC:
		return RES_OK;
	default:
		return RES_PARERR;
	}
}

static inline DRESULT sd_handle_ioctl(BYTE cmd, void *buff)
{
	switch (cmd)
	{
	case GET_SECTOR_COUNT:
		{
			if (!(sd_state.status & STA_NOINIT))
			{
				size_t size = ctr_io_disk_size(sd_state.io);
				*(DWORD*)buff = size / ctr_io_sector_size(sd_state.io);
				return RES_OK;
			}
			return RES_NOTRDY;
		}
	case GET_SECTOR_SIZE:
		if (!(sd_state.status & STA_NOINIT))
		{
			*(WORD*)buff = ctr_io_sector_size(sd_state.io);
			return RES_OK;
		}
		return RES_NOTRDY;
	case GET_BLOCK_SIZE:
		*(DWORD*)buff = 1; //Not sure what the block size for SD is...
	case CTRL_TRIM:
	case CTRL_SYNC:
		return RES_OK;
	default:
		return RES_PARERR;
	}
	return RES_NOTRDY;
}

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res = RES_PARERR;

	switch (pdrv) {
	case CTRNAND :
	case TWL :
	case TWLP :
		return nand_handle_ioctl(cmd, buff, pdrv);

	case SD:	
		return sd_handle_ioctl(cmd, buff);

	default:
		break;
	}

	return RES_PARERR;
}

