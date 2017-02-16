/*******************************************************************************
 * Copyright (C) 2016, 2017 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

#include <ctr9/io/ctr_nand_interface.h>
#include <ctr9/io/ctr_sdmmc_implementation.hpp>
#include <ctr9/io/sdmmc/sdmmc.h>
#include <cstdint>
#include <cstddef>
#include <new>

ctr_nand_interface *ctr_nand_interface_initialize()
{
	return reinterpret_cast<ctr_nand_interface*>(new (std::nothrow) ctr9::nand_interface());
}

void ctr_nand_interface_destroy(ctr_nand_interface *io)
{
	ctr9::nand_interface *io_ = reinterpret_cast<ctr9::nand_interface*>(io);
	delete io_;
}

namespace ctr9
{
	nand_interface::nand_interface()
	{
		InitSD();
	}

	int nand_interface::read(void *buffer, std::size_t buffer_size, std::uint64_t position, std::size_t count)
	{
		return ctr9::ctr_sdmmc_implementation_read(buffer, buffer_size, position, count, sdmmc_nand_readsectors);
	}

	int nand_interface::write(const void *buffer, std::size_t buffer_size, std::uint64_t position)
	{
		return ctr9::ctr_sdmmc_implementation_write(buffer, buffer_size, position, sdmmc_nand_readsectors, sdmmc_nand_writesectors);
	}

	int nand_interface::read_sector(void *buffer, std::size_t buffer_size, std::size_t sector, std::size_t count)
	{
		return ctr9::ctr_sdmmc_implementation_read_sector(buffer, buffer_size, sector, count, sdmmc_nand_readsectors);
	}

	int nand_interface::write_sector(const void *buffer, std::size_t buffer_size, std::size_t sector)
	{
		return ctr9::ctr_sdmmc_implementation_write_sector(buffer, buffer_size, sector, sdmmc_nand_writesectors);
	}

	std::uint64_t nand_interface::disk_size() const
	{
		return getMMCDevice(0)->total_size * (std::uint64_t)512u;
	}

	constexpr std::size_t nand_interface::sector_size()
	{
		return 512u;
	}
}

