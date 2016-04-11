#include <3ds9/ctr_io_interface.h>

int ctr_io_read(void *io, void *buffer, size_t buffer_size, size_t position, size_t count)
{
	ctr_io_interface *base = io;
	return base->read(io, buffer, buffer_size, position, count);
}

int ctr_io_write(void *io, const void *buffer, size_t buffer_size, size_t position)
{
	ctr_io_interface *base = io;
	return base->write(io, buffer, buffer_size, position);
}

int ctr_io_read_sector(void *io, void *buffer, size_t buffer_size, size_t sector, size_t count)
{
	ctr_io_interface *base = io;
	return base->read_sector(io, buffer, buffer_size, sector, count);
}

int ctr_io_write_sector(void *io, const void *buffer, size_t buffer_size, size_t sector)
{
	ctr_io_interface *base = io;
	return base->write_sector(io, buffer, buffer_size, sector);
}

size_t ctr_io_disk_size(void *io)
{
	ctr_io_interface *base = io;
	return base->disk_size(io);
}

size_t ctr_io_sector_size(void *io)
{
	ctr_io_interface *base = io;
	return base->sector_size(io);
}

