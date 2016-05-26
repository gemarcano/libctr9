#include <3ds9/nand.h>
#include <3ds9/sdmmc/sdmmc.h>

#include <stdint.h>
#include <string.h>

void nand_init(void)
{
	sdmmc_sdcard_init();
}

int nand_read(size_t location, size_t size, uint8_t *dest)
{
	if (size)
	{
		uint8_t buffer[0x200u];
		const size_t base_sector = location / 0x200u;
		const size_t start_location = location % 0x200u;

		size_t bytes_read = 0;
		size_t sectors_read = 0;

		//read first sector to extract the right number of bytes from it
		sdmmc_nand_readsectors(base_sector, ++sectors_read, buffer);

		const size_t readable = 0x200u - start_location;
		bytes_read += readable < size ? readable : size;

		memcpy(dest, &buffer[start_location], bytes_read);

		//read all sectors until the last one
		const size_t mid_sectors = (size-bytes_read)/0x200u;
		if (mid_sectors)
		{
			sdmmc_nand_readsectors(base_sector + sectors_read, mid_sectors, dest + bytes_read);
			sectors_read += mid_sectors;
			bytes_read += mid_sectors * 0x200u;
		}

		if (bytes_read != size)
		{
			//read last sector to extract the right number of bytes from it
			sdmmc_nand_readsectors(base_sector + sectors_read, 1, buffer);
			memcpy(dest + bytes_read, buffer, size - bytes_read);
		}
	}
	return 0;
}

int nand_write(size_t location, size_t size, const uint8_t *source)
{
	if (size)
	{
		uint8_t buffer[0x200u];
		const size_t base_sector = location / 0x200u;
		const size_t start_location = location % 0x200u;

		size_t bytes_written = 0;
		size_t sectors_written = 0;

		//read first sector to extract the right number of bytes from it
		sdmmc_nand_readsectors(base_sector, ++sectors_written, buffer);

		const size_t writeable = 0x200u - start_location;
		bytes_written += writeable < size ? writeable : size;

		memcpy(buffer + start_location, source, bytes_written);
		sdmmc_nand_writesectors(base_sector, sectors_written, buffer);

		const size_t mid_sectors = (size-bytes_written)/0x200u;
		//read all sectors until the last one
		if (mid_sectors)
		{
			sdmmc_nand_writesectors(base_sector + sectors_written, mid_sectors, source + bytes_written);
			sectors_written += mid_sectors;
			bytes_written += mid_sectors * 0x200u;
		}

		if (bytes_written != size)
		{
			//read last sector to extract the right number of bytes from it
			sdmmc_nand_readsectors(base_sector + sectors_written, 1, buffer);
			memcpy(buffer, source + bytes_written, size - bytes_written);
			sdmmc_nand_writesectors(base_sector + sectors_written, 1, buffer);
		}
	}
	return 0;
}

