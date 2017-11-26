/*******************************************************************************
 * Copyright (C) 2016, 2017 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

#include <ctr9/io/ctr_memory_interface.h>
#include <ctr9/io/ctr_io_implementation.hpp>
#include <string.h>
#include <stdbool.h>

ctr_memory_interface *ctr_memory_interface_initialize(void *buffer, std::size_t buffer_size)
{
	return reinterpret_cast<ctr_memory_interface*>(new (std::nothrow) ctr9::memory_interface(static_cast<std::uint8_t*>(buffer), buffer_size));
}

void ctr_memory_interface_destroy(ctr_memory_interface *io)
{
	ctr9::memory_interface *io_ = reinterpret_cast<ctr9::memory_interface*>(io);
	delete io_;
}

namespace ctr9
{
	memory_interface::memory_interface(std::uint8_t *buffer, std::size_t buffer_size)
	:buffer_(buffer), buffer_size_(buffer_size)
	{}

	int memory_interface::read(void *buffer, std::size_t buffer_size, std::uint64_t position, std::size_t count)
	{
		//What if we want to read more than the buffer in size?
		memcpy(buffer, buffer_ + position, count);
		return 0;
	}

	int memory_interface::write(const void *buffer, std::size_t buffer_size, std::uint64_t position)
	{
		//FIXME what if we want to write more than the buffer in size?
		memcpy(buffer_ + position, buffer, buffer_size);
		return 0;
	}

	int memory_interface::read_sector(void *buffer, std::size_t buffer_size, std::size_t sector, std::size_t count)
	{
		return read(buffer, buffer_size, sector, count);
	}

	int memory_interface::write_sector(const void *buffer, std::size_t buffer_size, std::size_t sector)
	{
		return write(buffer, buffer_size, sector);
	}

	std::uint64_t memory_interface::disk_size() const
	{
		return buffer_size_;
	}

	constexpr std::size_t memory_interface::sector_size()
	{
		return 1u;
	}
}

