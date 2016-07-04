/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

#include "ctr_io_implementation.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline int ctr_io_implementation_read(void *io, void *buffer, size_t buffer_size, uint64_t position, size_t count, ctr_io_implementation_read_sector_f read)
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
		res |= read(io, buf, sizeof(buf), base_sector, 1);
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
			res |= read(io, dest + bytes_read, section_readable, base_sector + sectors_read, mid_sectors);
			if (res) return res;
			sectors_read += mid_sectors;
			bytes_read += mid_sectors * 0x200u;
		}

		//Section 3: read last sector to extract the right number of bytes from it
		section_readable = total_readable - bytes_read;
		if (!res && section_readable)
		{
			res |= read(io, buf, sizeof(buf), base_sector + sectors_read, 1);
			if (res) return res;
			memcpy(dest + bytes_read, buf, section_readable);
		}
	}
	return res;
}

static inline int ctr_io_implementation_write(void *io, const void *buffer, size_t buffer_size, uint64_t position, ctr_io_implementation_read_sector_f read, ctr_io_implementation_write_sector_f write)
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
		res |= read(io, buf, sizeof(buf), base_sector, ++sectors_written);
		if (res) return res;

		const size_t writeable = 0x200u - start_location;
		bytes_written += writeable < buffer_size ? writeable : buffer_size;

		memcpy(buf + start_location, source, bytes_written);
		res |= write(io, buf, sizeof(buffer), base_sector);
		if (res) return res;

		const size_t mid_sectors = (buffer_size-bytes_written) / 0x200u;
		//Section 2: write all sectors until the last one
		if (mid_sectors)
		{
			res |= write(io, source + bytes_written, buffer_size-bytes_written, base_sector + sectors_written);
			if (res) return res;
			sectors_written += mid_sectors;
			bytes_written += mid_sectors * 0x200u;
		}

		//Section 3: read last sector to write back after adding the last bytes from the buffer
		if (bytes_written != buffer_size)
		{
			res |= read(io, buf, sizeof(buf), base_sector + sectors_written, 1);
			if (res) return res;
			memcpy(buf, source + bytes_written, buffer_size - bytes_written);
			write(io, buf, sizeof(buf), base_sector + sectors_written);
		}
	}
	return res;
}

#ifdef __cplusplus
}
#endif

