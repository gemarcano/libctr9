/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

#include <ctr9/io/ctr_sd_interface.h>
#include <ctr9/io/sdmmc/sdmmc.h>
#include <string.h>


static const ctr_io_interface sd_base = 
{
	ctr_sd_interface_read,
	ctr_sd_interface_write,
	ctr_sd_interface_read_sector,
	ctr_sd_interface_write_sector,
	ctr_sd_interface_disk_size,
	ctr_sd_interface_sector_size
};

int ctr_sd_interface_initialize(ctr_sd_interface *io)
{
	io->base = sd_base;
	InitSD();
	return SD_Init();
}

void ctr_sd_interface_destroy(ctr_sd_interface *io)
{
	
}

int ctr_sd_interface_read(void *ctx, void *buffer, size_t buffer_size, size_t position, size_t count)
{
	int res = 0;
	if (count && buffer_size) 
	{   
		size_t total_readable = count < buffer_size ? count : buffer_size;
		
		uint8_t *dest = buffer;
		uint8_t buf[0x200u];
		const size_t base_sector = position / 0x200u;
		
		size_t bytes_read = 0;
		size_t sectors_read = 0;
		
		//Section 1: read first sector to extract the right number of bytes from it
		const size_t start_location = position % 0x200u;
		res |= sdmmc_sdcard_readsectors(base_sector, 1, buf);
		if (res) return res;
		
		sectors_read++;
		
		size_t section_readable = 0x200u - start_location;
		if (section_readable > total_readable)
		{
			section_readable = total_readable;
		}
		
		memcpy(dest, &buf[start_location], section_readable);
		bytes_read += section_readable;
		
		//Section 2: read all sectors until the last one
		section_readable = (total_readable - bytes_read);
		size_t mid_sectors = section_readable / 0x200;
		
		if (mid_sectors)
		{   
			res |= sdmmc_sdcard_readsectors(base_sector + sectors_read, mid_sectors, dest + bytes_read);
			if (res) return res;
			sectors_read += mid_sectors;
			bytes_read += mid_sectors * 0x200u;
		}
		
		//Section 3: read last sector to extract the right number of bytes from it
		section_readable = total_readable - bytes_read;
		if (!res && section_readable)
		{   
			res |= sdmmc_sdcard_readsectors(base_sector + sectors_read, 1, buf);
			if (res) return res;
			memcpy(dest + bytes_read, buf, section_readable);
		}
	}
	return res;
}

int ctr_sd_interface_write(void *ctx, const void *buffer, size_t buffer_size, size_t position)
{
    int res = 0;
    if (buffer_size)
    {
        const uint8_t *source = buffer;
        uint8_t buf[0x200u];
        const size_t base_sector = position / 0x200u;
        const size_t start_location = position % 0x200u;

        size_t bytes_written = 0;
        size_t sectors_written = 0;

        //Section 1: read first sector to write back after adding the data to the sector
        res |= sdmmc_sdcard_readsectors(base_sector, ++sectors_written, buf);
        if (res) return res;

        const size_t writeable = 0x200u - start_location;
        bytes_written += writeable < buffer_size ? writeable : buffer_size;

        memcpy(buf + start_location, source, bytes_written);
        res |= sdmmc_sdcard_writesectors(base_sector, sectors_written, buf);
        if (res) return res;

        const size_t mid_sectors = (buffer_size-bytes_written) / 0x200u;
        //Section 2: write all sectors until the last one
        if (mid_sectors)
        {
            res |= sdmmc_sdcard_writesectors(base_sector + sectors_written, mid_sectors, source + bytes_written);
            if (res) return res;
            sectors_written += mid_sectors;
            bytes_written += mid_sectors * 0x200u;
        }

        //Section 3: read last sector to write back after adding the last bytes from the buffer
        if (bytes_written != buffer_size)
        {
            res |= sdmmc_sdcard_readsectors(base_sector + sectors_written, 1, buf);
            if (res) return res;
            memcpy(buf, source + bytes_written, buffer_size - bytes_written);
            sdmmc_sdcard_writesectors(base_sector + sectors_written, 1, buf);
        }
    }
    return res;
}

int ctr_sd_interface_read_sector(void *ctx, void *buffer, size_t buffer_size, size_t sector, size_t count)
{
    int res = 0;
    size_t read_size = (buffer_size / 512) < count ? buffer_size / 512 : count;
    if (read_size)
    {
        //What if read_size == 0?
        res = sdmmc_sdcard_readsectors(sector, read_size, (uint8_t*) buffer);
    }
    return res;
}

int ctr_sd_interface_write_sector(void *ctx, const void *buffer, size_t buffer_size, size_t sector)
{
    size_t write_size = (buffer_size / 512);
    int res;
    if (write_size)
    {
        res = sdmmc_sdcard_writesectors(sector, write_size, (const uint8_t*)buffer);
    }
    else
    {
        res = 1; //FIXME standardize return
    }
    return res;
}

size_t ctr_sd_interface_disk_size(void *ctx)
{
	return getMMCDevice(1)->total_size * 512u;
}

size_t ctr_sd_interface_sector_size(void *ctx)
{
	return 512u;
}

