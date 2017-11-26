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

static const ctr_io_interface file_base =
{
	ctr_file_interface_read,
	ctr_file_interface_write,
	ctr_file_interface_read_sector,
	ctr_file_interface_write_sector,
	ctr_file_interface_disk_size,
	ctr_file_interface_sector_size
};

int ctr_file_interface_initialize(ctr_file_interface *io, FILE *file)
{
	io->base = file_base;
	io->file = file;
	struct stat st;
	fstat(fileno(file), &st);
	return st.st_size < 512; //FIXME is this good enough?
}

void ctr_file_interface_destroy(ctr_file_interface *io)
{
	*io = (ctr_file_interface){0};
}

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

int ctr_file_interface_read(void *io, void *buffer, size_t buffer_size, uint64_t position, size_t count)
{
	int result = 0;
	if (count)
	{
		ctr_file_interface *file_io = io;
		size_t readable = count < buffer_size ? count : buffer_size;
		result = set_position(file_io->file, position);
		if (result == 0)
		{
			size_t amount_read = fread(buffer, readable, 1, file_io->file);
			if (amount_read != 1)
				result = -1; //FIXME is this what I want if we read less than we could?
		}
	}

	return result;
}

int ctr_file_interface_write(void *io, const void *buffer, size_t buffer_size, uint64_t position)
{
	int result = 0;
	if (buffer_size)
	{
		ctr_file_interface *file_io = io;
		result = set_position(file_io->file, position);

		if (result == 0)
		{
			size_t amount_written = fwrite(buffer, buffer_size, 1, file_io->file);
			if (amount_written != 1)
			{
				result = -1; //FIXME, is this what I want if we write less than we could?
			}
		}
	}

	return result;
}

int ctr_file_interface_read_sector(void *io, void *buffer, size_t buffer_size, size_t sector, size_t count)
{
	int result = 0;
	if (count)
	{
		ctr_file_interface *file_io = io;
		size_t readable = count < buffer_size/512 ? count : buffer_size/512;
		result = set_position(file_io->file, sector * 512);
		if (result == 0)
		{
			size_t rb = fread(buffer, readable * 512, 1, file_io->file);
			if (rb != 1)
				result = -1; //FIXME see others
		}
	}

	return result;
}

int ctr_file_interface_write_sector(void *io, const void *buffer, size_t buffer_size, size_t sector)
{
	int result = 0;
	if (buffer_size/512)
	{
		ctr_file_interface *file_io = io;
		result = set_position(file_io->file, sector * 512);

		if (result == 0)
		{
			size_t writeable = buffer_size - buffer_size % 512;
			size_t wb = fwrite(buffer, writeable, 1, file_io->file);
			if (wb != 1)
				result = -1; //FIXME see others
		}
	}

	return result != 0;
}

uint64_t ctr_file_interface_disk_size(void *io)
{
	ctr_file_interface *file_io = io;
	struct stat st;
	fstat(fileno(file_io->file), &st);

	//I DO want to explicitly change the sign of the result, a negative size
	//makes no sense, so I will use the negative bit as part of the unsigned
	//number, &st);
	return (size_t)(st.st_size);
}

size_t ctr_file_interface_sector_size(void *io)
{
	return 512; //FIXME this isn't right always... is there a way to query fatfs about it?
}

