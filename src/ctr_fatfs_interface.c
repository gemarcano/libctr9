/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

#include <ctr9/io/ctr_fatfs_interface.h>
#include <ctr9/io/fatfs/ff.h>
#include <string.h>

#include <stdalign.h>

//FIXME these are unique per instance... or are they?

static const ctr_io_interface fatfs_base =
{
	ctr_fatfs_interface_read,
	ctr_fatfs_interface_write,
	ctr_fatfs_interface_read_sector,
	ctr_fatfs_interface_write_sector,
	ctr_fatfs_interface_disk_size,
	ctr_fatfs_interface_sector_size
};

int ctr_fatfs_interface_initialize(ctr_fatfs_interface *io, FIL *file)
{
	io->base = fatfs_base;
	io->file = file;
	return f_size(file) < 512; //FIXME is this good enough?
}

void ctr_fatfs_interface_destroy(ctr_fatfs_interface *io)
{
	*io = (ctr_fatfs_interface){0};
}

int ctr_fatfs_interface_read(void *io, void *buffer, size_t buffer_size, size_t position, size_t count)
{
	FRESULT result = FR_OK;
	if (count)
	{
		ctr_fatfs_interface *fatfs_io = io;
		size_t readable = count < buffer_size ? count : buffer_size;
		UINT read;
		result = f_lseek(fatfs_io->file, position);
		if (result == FR_OK)
		{
			result = f_read(fatfs_io->file, buffer, readable, &read);
		}
	}

	return result != FR_OK;
}

int ctr_fatfs_interface_write(void *io, const void *buffer, size_t buffer_size, size_t position)
{
	FRESULT result = FR_OK;
	if (buffer_size)
	{
		ctr_fatfs_interface *fatfs_io = io;
		UINT written;
		result = f_lseek(fatfs_io->file, position);

		if (result == FR_OK)
		{
			result = f_write(fatfs_io->file, buffer, buffer_size, &written);
		}
	}

	return result != FR_OK;
}

int ctr_fatfs_interface_read_sector(void *io, void *buffer, size_t buffer_size, size_t sector, size_t count)
{
	FRESULT result = FR_OK;
	if (count)
	{
		ctr_fatfs_interface *fatfs_io = io;
		size_t readable = count < buffer_size/512 ? count : buffer_size/512;
		UINT read;
		result = f_lseek(fatfs_io->file, sector * 512);
		if (result == FR_OK)
		{
			result = f_read(fatfs_io->file, buffer, readable * 512, &read);
		}
	}

	return result != FR_OK;
}

int ctr_fatfs_interface_write_sector(void *io, const void *buffer, size_t buffer_size, size_t sector)
{
	FRESULT result = FR_OK;
	if (buffer_size/512)
	{
		ctr_fatfs_interface *fatfs_io = io;
		UINT written;
		result = f_lseek(fatfs_io->file, sector * 512);

		if (result == FR_OK)
		{
			size_t writeable = buffer_size - buffer_size % 512;
			result = f_write(fatfs_io->file, buffer, writeable , &written);
		}
	}

	return result != FR_OK;
}

size_t ctr_fatfs_interface_disk_size(void *io)
{
	ctr_fatfs_interface *fatfs_io = io;
	return f_size(fatfs_io->file);
}

size_t ctr_fatfs_interface_sector_size(void *io)
{
	return 512;
}

