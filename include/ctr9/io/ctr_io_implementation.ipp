/*******************************************************************************
 * Copyright (C) 2016, 2017 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

#include "ctr_io_implementation.hpp"
#include <string.h>
#include <cstdint>
#include <cstdlib>
#include <new>

namespace ctr9
{
	template<class IO>
	int ctr_io_implementation_read(IO &io, void *buffer, size_t buffer_size, uint64_t position, size_t count)
	{
		int res = 0;
		if (count && buffer_size)
		{
			const size_t sector_size = io.sector_size();
			size_t total_readable = count < buffer_size ? count : buffer_size;

			uint8_t *dest = reinterpret_cast<uint8_t*>(buffer);
			uint8_t *buf = new (std::nothrow) std::uint8_t[sector_size]; //FIXME what if this returns NULL?
			if (!buf)
				return -1;

			const size_t base_sector = position / sector_size;

			size_t bytes_read = 0;
			size_t sectors_read = 0;

			//Section 1: read first sector to extract the right number of bytes from it
			const size_t start_location = position % sector_size;
			res |= io.read(buf, sector_size, base_sector, 1);
			if (res) return res;

			sectors_read++;

			size_t section_readable = sector_size - start_location;
			if (section_readable > total_readable)
			{
				section_readable = total_readable;
			}

			memcpy(dest, &buf[start_location], section_readable);
			bytes_read += section_readable;

			//Section 2: read all sectors until the last one
			section_readable = (total_readable - bytes_read);
			size_t mid_sectors = section_readable / sector_size;

			if (mid_sectors)
			{
				res |= io.read(dest + bytes_read, section_readable, base_sector + sectors_read, mid_sectors);
				if (res) return res;
				sectors_read += mid_sectors;
				bytes_read += mid_sectors * sector_size;
			}

			//Section 3: read last sector to extract the right number of bytes from it
			section_readable = total_readable - bytes_read;
			if (!res && section_readable)
			{
				res |= io.read(buf, sector_size, base_sector + sectors_read, 1);
				if (res) return res;
				memcpy(dest + bytes_read, buf, section_readable);
			}
		}
		return res;
	}

	template<class IO>
	int ctr_io_implementation_write(IO& io, const void *buffer, size_t buffer_size, uint64_t position)
	{
		int res = 0;
		if (buffer_size)
		{
			const size_t sector_size = io.sector_size();
			const uint8_t *source = reinterpret_cast<const std::uint8_t*>(buffer);
			uint8_t *buf = new (std::nothrow) std::uint8_t[sector_size]; //FIXME what if this returns NULL?
			if (!buf)
				return -1;
			const size_t base_sector = position / sector_size;
			const size_t start_location = position % sector_size;

			size_t bytes_written = 0;
			size_t sectors_written = 0;

			//Section 1: read first sector to write back after adding the data to the sector
			res |= io.read(buf, sector_size, base_sector, ++sectors_written);
			if (res) return res;

			const size_t writeable = sector_size - start_location;
			bytes_written += writeable < buffer_size ? writeable : buffer_size;

			memcpy(buf + start_location, source, bytes_written);
			res |= io.write(buf, sector_size, base_sector);
			if (res) return res;

			const size_t mid_sectors = (buffer_size-bytes_written) / sector_size;
			//Section 2: write all sectors until the last one
			if (mid_sectors)
			{
				res |= io.write(source + bytes_written, buffer_size-bytes_written, base_sector + sectors_written);
				if (res) return res;
				sectors_written += mid_sectors;
				bytes_written += mid_sectors * sector_size;
			}

			//Section 3: read last sector to write back after adding the last bytes from the buffer
			if (bytes_written != buffer_size)
			{
				res |= io.read(buf, sector_size, base_sector + sectors_written, 1);
				if (res) return res;
				memcpy(buf, source + bytes_written, buffer_size - bytes_written);
				io.write(buf, sector_size, base_sector + sectors_written);
			}
		}
		return res;
	}
}

