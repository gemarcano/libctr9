/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

#include <ctr9/io/ctr_sd_interface.h>
#include <ctr9/io/sdmmc/sdmmc.h>
#include <ctr9/io/ctr_sdmmc_implementation.h>
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
	*io = (ctr_sd_interface){0};	
}

int ctr_sd_interface_read(void *io, void *buffer, size_t buffer_size, size_t position, size_t count)
{
	return ctr_sdmmc_implementation_read(buffer, buffer_size, position, count, sdmmc_sdcard_readsectors);
}

int ctr_sd_interface_write(void *io, const void *buffer, size_t buffer_size, size_t position)
{
	return ctr_sdmmc_implementation_write(buffer, buffer_size, position, sdmmc_sdcard_readsectors, sdmmc_sdcard_writesectors);
}

int ctr_sd_interface_read_sector(void *io, void *buffer, size_t buffer_size, size_t sector, size_t count)
{
	return ctr_sdmmc_implementation_read_sector(buffer, buffer_size, sector, count, sdmmc_sdcard_readsectors);
}

int ctr_sd_interface_write_sector(void *io, const void *buffer, size_t buffer_size, size_t sector)
{
	return ctr_sdmmc_implementation_write_sector(buffer, buffer_size, sector, sdmmc_sdcard_writesectors);
}

size_t ctr_sd_interface_disk_size(void *io)
{
	return getMMCDevice(1)->total_size * 512u;
}

size_t ctr_sd_interface_sector_size(void *io)
{
	return 512u;
}

