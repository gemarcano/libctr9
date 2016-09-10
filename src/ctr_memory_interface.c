/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

#include <ctr9/io/ctr_memory_interface.h>
#include <ctr9/io/ctr_io_implementation.h>
#include <string.h>
#include <stdbool.h>

static const ctr_io_interface memory_base =
{
	ctr_memory_interface_read,
	ctr_memory_interface_write,
	ctr_memory_interface_read_sector,
	ctr_memory_interface_write_sector,
	ctr_memory_interface_disk_size,
	ctr_memory_interface_sector_size
};

void ctr_memory_interface_initialize(ctr_memory_interface *io, void *buffer, size_t buffer_size)
{
	io->base = memory_base;
	io->buffer = buffer;
	io->buffer_size = buffer_size;
}

void ctr_memory_interface_destroy(ctr_memory_interface *io)
{
	*io = (ctr_memory_interface){0};
}

int ctr_memory_interface_read(void *io, void *buffer, size_t buffer_size, uint64_t position, size_t count)
{
	return ctr_io_implementation_read(io, buffer, buffer_size, position, count, ctr_memory_interface_read_sector);
}

int ctr_memory_interface_write(void *io, const void *buffer, size_t buffer_size, uint64_t position)
{
	return ctr_io_implementation_write(io, buffer, buffer_size, position, ctr_memory_interface_read_sector, ctr_memory_interface_write_sector);
}

int ctr_memory_interface_read_sector(void *io, void *buffer, size_t buffer_size, size_t sector, size_t count)
{
	ctr_memory_interface *mem_io = io;
	size_t size_to_read = buffer_size < mem_io->buffer_size ? buffer_size : mem_io->buffer_size;
	size_to_read = size_to_read < count ? size_to_read : count;
	memmove(buffer, (char*)mem_io->buffer + sector, size_to_read);
	return 0;
}

int ctr_memory_interface_write_sector(void *io, const void *buffer, size_t buffer_size, size_t sector)
{
	ctr_memory_interface *mem_io = io;
	size_t size_to_write = buffer_size < mem_io->buffer_size ? buffer_size : mem_io->buffer_size;
	memmove((char*)mem_io->buffer + sector, buffer, size_to_write);
	return 0;
}

uint64_t ctr_memory_interface_disk_size(void *io)
{
	return ((ctr_memory_interface*)io)->buffer_size;
}

size_t ctr_memory_interface_sector_size(void *io)
{
	return 1u;
}

