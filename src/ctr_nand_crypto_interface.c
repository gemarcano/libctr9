#include <3ds9/ctr_nand_crypto_interface.h>
#include <3ds9/sdmmc/sdmmc.h>
#include <string.h>

static const ctr_io_interface nand_crypto_base =
{
	ctr_nand_crypto_interface_read,
	ctr_nand_crypto_interface_write,
	ctr_nand_crypto_interface_read_sector,
	ctr_nand_crypto_interface_write_sector,
	ctr_nand_crypto_interface_disk_size,
	ctr_nand_crypto_interface_sector_size
};

int ctr_nand_crypto_interface_initialize(ctr_nand_crypto_interface *io, unsigned int slot, ctr_io_interface *lower_io)
{
	io->base = nand_crypto_base;
	io->lower_io = lower_io;
	return 0;
}

void ctr_nand_crypto_interface_destroy(ctr_nand_crypto_interface *io)
{
	*io = (ctr_nand_crypto_interface){0};
}

int ctr_nand_crypto_interface_read(void *ctx, void *buffer, size_t buffer_size, size_t position, size_t count)
{
	ctr_nand_crypto_interface* io = ctx;
	int res = io->lower_io->read(ctx, buffer, buffer_size, position, count);
	//FIXME check res
	
	//we now have raw data, apply crypto
	
	return res;
}

int ctr_nand_crypto_interface_write(void *ctx, const void *buffer, size_t buffer_size, size_t position)
{
	ctr_nand_crypto_interface* io = ctx;
	int res = io->lower_io->write(ctx, buffer, buffer_size, position);
	//FIXME check res
	
	//we now have raw data, apply crypto

	return res;
}

int ctr_nand_crypto_interface_read_sector(void *ctx, void *buffer, size_t buffer_size, size_t sector, size_t count)
{
	ctr_nand_crypto_interface* io = ctx;
	int res = io->lower_io->read_sector(ctx, buffer, buffer_size, sector, count);
	//FIXME check res
	
	//we now have raw data, apply crypto

	return res;
}

int ctr_nand_crypto_interface_write_sector(void *ctx, const void *buffer, size_t buffer_size, size_t sector)
{
	ctr_nand_crypto_interface* io = ctx;
	int res = io->lower_io->write_sector(ctx, buffer, buffer_size, sector);
	//FIXME check res
	
	//we now have raw data, apply crypto

	return res;
}

size_t ctr_nand_crypto_interface_disk_size(void *ctx)
{
	ctr_nand_crypto_interface* io = ctx;
	return io->lower_io->disk_size(ctx);
}

size_t ctr_nand_crypto_interface_sector_size(void *ctx)
{
	return 512;
}

