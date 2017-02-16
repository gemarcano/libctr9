/*******************************************************************************
 * Copyright (C) 2016, 2017 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

#include <ctr9/io/ctr_io_interface.h>

int ctr_io_read(void *io, void *buffer, size_t buffer_size, uint64_t position, size_t count)
{
	ctr9::io_interface *io_ = reinterpret_cast<ctr9::io_interface*>(io);
	return io_->read(buffer, buffer_size, position, count);
}

int ctr_io_write(void *io, const void *buffer, size_t buffer_size, uint64_t position)
{
	ctr9::io_interface *io_ = reinterpret_cast<ctr9::io_interface*>(io);
	return io_->write(buffer, buffer_size, position);
}

int ctr_io_read_sector(void *io, void *buffer, size_t buffer_size, size_t sector, size_t count)
{
	ctr9::io_interface *io_ = reinterpret_cast<ctr9::io_interface*>(io);
	return io_->read_sector(buffer, buffer_size, sector, count);
}

int ctr_io_write_sector(void *io, const void *buffer, size_t buffer_size, size_t sector)
{
	ctr9::io_interface *io_ = reinterpret_cast<ctr9::io_interface*>(io);
	return io_->write_sector(buffer, buffer_size, sector);
}

uint64_t ctr_io_disk_size(void *io)
{
	ctr9::io_interface *io_ = reinterpret_cast<ctr9::io_interface*>(io);
	return io_->disk_size();
}

size_t ctr_io_sector_size(void *io)
{
	ctr9::io_interface *io_ = reinterpret_cast<ctr9::io_interface*>(io);
	return io_->sector_size();
}

