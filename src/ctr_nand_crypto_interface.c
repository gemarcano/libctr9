#include <3ds9/ctr_nand_crypto_interface.h>
#include <3ds9/sdmmc/sdmmc.h>
#include <string.h>
#include <3ds9/aes.h>
#include <3ds9/sha.h>

#include <stdalign.h>

//FIXME these are unique per instamce... or are they?

static const ctr_io_interface nand_crypto_base =
{
	ctr_nand_crypto_interface_read,
	ctr_nand_crypto_interface_write,
	ctr_nand_crypto_interface_read_sector,
	ctr_nand_crypto_interface_write_sector,
	ctr_nand_crypto_interface_disk_size,
	ctr_nand_crypto_interface_sector_size
};

int ctr_nand_crypto_interface_initialize(ctr_nand_crypto_interface *io, uint8_t keySlot, ctr_io_interface *lower_io)
{
	io->base = nand_crypto_base;
	io->lower_io = lower_io;
	io->keySlot = keySlot;

	// STEP #1: Get NAND CID, set up TWL/CTR counter
	uint32_t NandCid[4];
	uint8_t shasum[32];

	sdmmc_get_cid(true, NandCid);
	sha_init(SHA256_MODE);
	sha_update((uint8_t*)NandCid, 16);
	sha_get(shasum);
	memcpy(io->CtrNandCtr, shasum, 16);
	sha_init(SHA1_MODE);
	sha_update((uint8_t*)NandCid, 16);
	sha_get(shasum);
	for(uint32_t i = 0; i < 16; i++) // little endian and reversed order
		io->TwlNandCtr[i] = shasum[15-i];

	return 0;
}


static void decryptNandSector(ctr_nand_crypto_interface *io, uint8_t* buffer, uint32_t sector, uint32_t count)
{
	uint32_t mode = (sector >= (0x0B100000u / 0x200)) ? AES_CNT_CTRNAND_MODE : AES_CNT_TWLNAND_MODE;
	alignas(32) uint8_t ctr[16];
	
	// copy NAND CTR and increment it
	memcpy(ctr, (sector >= (0x0B100000 / 0x200)) ? io->CtrNandCtr : io->TwlNandCtr, 16);
	add_ctr(ctr, sector * (0x200/0x10));
	
	// decrypt the data
	use_aeskey(io->keySlot);
	for (uint32_t s = 0; s < count; s++)
	{
		for (uint32_t b = 0x0; b < 0x200; b += 0x10, buffer += 0x10)
		{
			set_ctr(ctr);
			aes_decrypt((void*) buffer, (void*) buffer, 1, mode);
			add_ctr(ctr, 0x1);
		}
	}
} 

static void decryptNand(ctr_nand_crypto_interface *io, uint8_t* buffer, uint32_t location, uint32_t count)
{
	if (count)
	{
		uint32_t mode = (location >= 0x0B100000u) ? AES_CNT_CTRNAND_MODE : AES_CNT_TWLNAND_MODE;
		alignas(32) uint8_t ctr[16];
		alignas(32) uint8_t block_buffer[16];
		uint32_t amount_read = 0;

		use_aeskey(io->keySlot);
		
		// copy NAND CTR and increment it
		memcpy(ctr, (location >= 0x0B100000 ) ? io->CtrNandCtr : io->TwlNandCtr, 16);
		add_ctr(ctr, location / 0x10);

		//Three parts: First block, all blocks which are aligned, last block
		// First block always exists, may or may not be aligned.
		
		uint8_t block_offset = location & 0xF;
		size_t current_section_size = 16u - block_offset;
		if (current_section_size > count)
		{
			current_section_size = count;
		}

		memcpy(block_buffer + block_offset, buffer, current_section_size);

		set_ctr(ctr);
		aes_decrypt((void*) block_buffer, (void*) block_buffer, 1, mode);
		add_ctr(ctr, 0x1);


		memcpy(buffer, block_buffer + block_offset, current_section_size);

		amount_read = current_section_size;

		current_section_size = count - amount_read;
		current_section_size -= current_section_size % 16;

		// second set may or may not exists (only if count - (0xF - location & 0xF) != 0 and if count - (0xF - location & 0xF) >= 0x10)
		if (current_section_size)
		{
			for (size_t b = 0x0; b < current_section_size; b += 0x10)
			{
				set_ctr(ctr);
				aes_decrypt((void*) (buffer + amount_read + b), (void*) (buffer + amount_read + b), 1, mode);
				add_ctr(ctr, 0x1);
			}
		}

		amount_read += current_section_size;

		// last block may or may not exist. Only if (count - (0xF - location & 0xF)) & 0xF != 0
		current_section_size = (count - amount_read) % 16;
		if (current_section_size)
		{
			memcpy(block_buffer, buffer + amount_read, current_section_size);

			set_ctr(ctr);
			aes_decrypt((void*) block_buffer, (void*) block_buffer, 1, mode);
			add_ctr(ctr, 0x1);

			memcpy(buffer + amount_read, block_buffer, current_section_size);
		}
	}
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
	decryptNand(io, (uint8_t*)buffer, position, count);
	
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
	decryptNandSector(io, (uint8_t*)buffer, sector, count);
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
	ctr_nand_crypto_interface *io = ctx;
	return io->lower_io->disk_size(ctx);
}

size_t ctr_nand_crypto_interface_sector_size(void *ctx)
{
	ctr_nand_crypto_interface *io = ctx;
	return io->lower_io->sector_size(ctx);
}

