/*******************************************************************************
 * Copyright (C) 2016, 2017 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

#include <ctr9/io/ctr_file_interface.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include <stdalign.h>
#include <limits.h>

//FIXME these are unique per instance... or are they?

ctr_file_interface *ctr_file_interface_initialize(FILE *file)
{
	return reinterpret_cast<ctr_file_interface*>(new (std::nothrow) ctr9::file_interface(file));
}

void ctr_file_interface_destroy(ctr_file_interface *io)
{
	ctr9::file_interface *io_ = reinterpret_cast<ctr9::file_interface*>(io);
	delete io_;
}

namespace ctr9
{
	static int set_position(FILE *file, uint64_t position)
	{
		if (fseek(file, 0, SEEK_SET)) return -1;
		while (position > INT_MAX)
		{
			int pos = INT_MAX;
			if (fseek(file, pos, SEEK_CUR)) return -1;
			position -= INT_MAX;
		}

		if (fseek(file, (int)position, SEEK_CUR)) return -1;
		return 0;
	}

	file_interface::file_interface(FILE *file)
	:file_(file)
	{
		struct stat st;
		fstat(fileno(file), &st);
	}

	int file_interface::read(void *buffer, std::size_t buffer_size, std::uint64_t position, std::size_t count)
	{
		int result = 0;
		if (count)
		{
			size_t readable = count < buffer_size ? count : buffer_size;
			result = set_position(file_, position);
			if (result == 0)
			{
				size_t amount_read = fread(buffer, readable, 1, file_);
				if (amount_read != 1)
					result = -1; //FIXME is this what I want if we read less than we could?
			}
		}

		return result;
	}

	int file_interface::write(const void *buffer, std::size_t buffer_size, std::uint64_t position)
	{
		int result = 0;
		if (buffer_size)
		{
			result = set_position(file_, position);

			if (result == 0)
			{
				size_t amount_written = fwrite(buffer, buffer_size, 1, file_);
				if (amount_written != 1)
				{
					result = -1; //FIXME, is this what I want if we write less than we could?
				}
			}
		}

		return result;
	}

	int file_interface::read_sector(void *buffer, std::size_t buffer_size, std::size_t sector, std::size_t count)
	{
		return read(buffer, buffer_size, sector, count);
	}

	int file_interface::write_sector(const void *buffer, std::size_t buffer_size, std::size_t sector)
	{
		return write(buffer, buffer_size, sector);
	}

	constexpr std::size_t file_interface::sector_size()
	{
		return 1u;
	}

	std::uint64_t file_interface::disk_size() const
	{
		struct stat st;
		fstat(fileno(file_), &st);

		//I DO want to explicitly change the sign of the result, a negative size
		//makes no sense, so I will use the negative bit as part of the unsigned
		//number, &st);
		return (size_t)(st.st_size);
	}
}

