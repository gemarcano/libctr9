/*******************************************************************************
 * Copyright (C) 2016, 2017 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

#ifndef CTR_IO_INTERFACE_IPP_
#define CTR_IO_INTERFACE_IPP_

#include <cstddef>
#include <cstdint>
#include <utility>

namespace ctr9
{
	template<class IO>
	template<class... Args>
	io_interface_impl<IO>::io_interface_impl(Args&&... args)
	:io_(std::forward<Args>(args)...)
	{}

	template<class IO>
	int io_interface_impl<IO>::read(void *buffer, std::size_t buffer_size, std::uint64_t position, std::size_t count)
	{
		return io_.read(buffer, buffer_size, position, count);
	}

	template<class IO>
	int io_interface_impl<IO>::write(const void *buffer, std::size_t buffer_size, std::uint64_t position)
	{
		return io_.write(buffer, buffer_size, position);
	}

	template<class IO>
	int io_interface_impl<IO>::read_sector(void *buffer, std::size_t buffer_size, std::size_t sector, std::size_t count)
	{
		return io_.read_sector(buffer, buffer_size, sector, count);
	}

	template<class IO>
	int io_interface_impl<IO>::write_sector(const void *buffer, std::size_t buffer_size, std::size_t sector)
	{
		return io_.write_sector(buffer, buffer_size, sector);
	}

	template<class IO>
	std::uint64_t io_interface_impl<IO>::disk_size() const
	{
		return io_.disk_size();
	}

	template<class IO>
	std::size_t io_interface_impl<IO>::sector_size() const
	{
		return io_.sector_size();
	}
}

#endif//CTR_IO_INTERFACE_IPP_

