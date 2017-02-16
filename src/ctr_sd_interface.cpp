/*******************************************************************************
 * Copyright (C) 2016, 2017 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

#include <ctr9/io/ctr_sd_interface.h>
#include <ctr9/io/sdmmc/sdmmc.h>
#include <ctr9/io/ctr_sdmmc_implementation.hpp>
#include <new>
#include <cstdint>
#include <cstddef>

ctr_sd_interface *ctr_sd_interface_initialize(void)
{
	InitSD();
	if (!SD_Init())
		return reinterpret_cast<ctr_sd_interface*>(new (std::nothrow) ctr9::sd_interface());
	return NULL;
}

void ctr_sd_interface_destroy(ctr_sd_interface *io)
{
	ctr9::sd_interface *sd = reinterpret_cast<ctr9::sd_interface*>(io);
	delete sd;
}

namespace ctr9
{
	sd_interface::sd_interface()
	{
		InitSD();
		SD_Init();
	}

	int sd_interface::read(void *buffer, std::size_t buffer_size, std::uint64_t position, std::size_t count)
	{
		return ctr_sdmmc_implementation_read(buffer, buffer_size, position, count, sdmmc_sdcard_readsectors);
	}

	int sd_interface::write(const void *buffer, std::size_t buffer_size, std::uint64_t position)
	{
		return ctr_sdmmc_implementation_write(buffer, buffer_size, position, sdmmc_sdcard_readsectors, sdmmc_sdcard_writesectors);
	}

	int sd_interface::read_sector(void *buffer, std::size_t buffer_size, std::size_t sector, std::size_t count)
	{
		return ctr_sdmmc_implementation_read_sector(buffer, buffer_size, sector, count, sdmmc_sdcard_readsectors);
	}

	int sd_interface::write_sector(const void *buffer, std::size_t buffer_size, std::size_t sector)
	{
		return ctr_sdmmc_implementation_write_sector(buffer, buffer_size, sector, sdmmc_sdcard_writesectors);
	}

	std::uint64_t sd_interface::disk_size() const
	{
		return getMMCDevice(1)->total_size * (std::uint64_t)512u;
	}

	constexpr std::size_t sd_interface::sector_size()
	{
		return 512u;
	}
}

bool ctr_sd_interface_inserted(void)
{
	return sdmmc_sd_inserted();
}

