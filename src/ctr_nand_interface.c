/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

#include <ctr9/io/ctr_nand_interface.h>
#include <ctr9/io/ctr_sdmmc_implementation.h>
#include <ctr9/io/sdmmc/sdmmc.h>
#include <string.h>

static const ctr_io_interface nand_base =
{
	ctr_nand_interface_read,
	ctr_nand_interface_write,
	ctr_nand_interface_read_sector,
	ctr_nand_interface_write_sector,
	ctr_nand_interface_disk_size,
	ctr_nand_interface_sector_size
};

int ctr_nand_interface_initialize(ctr_nand_interface *io)
{
	io->base = nand_base;
	InitSD();
	return Nand_Init();
}

void ctr_nand_interface_destroy(ctr_nand_interface *io)
{
	*io = (ctr_nand_interface){0};
}

int ctr_nand_interface_read(void *io, void *buffer, size_t buffer_size, uint64_t position, size_t count)
{
	return ctr_sdmmc_implementation_read(buffer, buffer_size, position, count, sdmmc_nand_readsectors);
}

int ctr_nand_interface_write(void *io, const void *buffer, size_t buffer_size, uint64_t position)
{
	return ctr_sdmmc_implementation_write(buffer, buffer_size, position, sdmmc_nand_readsectors, sdmmc_nand_writesectors);
}

int ctr_nand_interface_read_sector(void *io, void *buffer, size_t buffer_size, size_t sector, size_t count)
{
	return ctr_sdmmc_implementation_read_sector(buffer, buffer_size, sector, count, sdmmc_nand_readsectors);
}

int ctr_nand_interface_write_sector(void *io, const void *buffer, size_t buffer_size, size_t sector)
{
	return ctr_sdmmc_implementation_write_sector(buffer, buffer_size, sector, sdmmc_nand_writesectors);
}

uint64_t ctr_nand_interface_disk_size(void *io)
{
	return getMMCDevice(0)->total_size * (uint64_t)512u;
}

size_t ctr_nand_interface_sector_size(void *io)
{
	return 512u;
}

