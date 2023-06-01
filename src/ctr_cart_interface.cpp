#include <ctr9/io/ctr_cart_interface.h>
#include <ctr9/ctr_headers.h>
#include <ctr9/gamecart/protocol.h>
#include <ctr9/gamecart/command_ctr.h>
#include <ctr9/io/ctr_io_implementation.hpp>

#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#define CFG_CARDCONF2 (*(volatile uint8_t*)0x10000010)

bool ctr_cart_inserted(void)
{
	return !(CFG_CARDCONF2 & 0x1);
}

ctr_cart_interface *ctr_cart_interface_initialize()
{
	return reinterpret_cast<ctr_cart_interface*>(new (std::nothrow) ctr9::cart_interface());
}

void ctr_cart_interface_destroy(ctr_cart_interface *cart)
{
	delete reinterpret_cast<ctr9::cart_interface*>(cart);
}

namespace ctr9
{
	raw_cart_interface::raw_cart_interface()
	{
		if (!ctr_cart_inserted())
		{
			//FIXME what do?
		}

		Cart_Init();
		CTR_CmdReadHeader(ncch_raw.data());
		ctr_ncch_header_load(&ncch_header, ncch_raw.data(), 0x200);

		cart_id = Cart_GetID();
		uint32_t buffer[0x200/4];
		memcpy(buffer, ncch_raw.data(), 0x200);
		Cart_Secure_Init(buffer, sec_keys);
		//this function doesn't care about alignment. it optimizes if it is aligned,
		//else it does the right thing for unaligned access
		CTR_CmdReadData(0, 0x200, 1, (uint8_t*)buffer);
		ctr_ncsd_header_load(&ncsd_header, (uint8_t*)buffer, 0x200);

		media_unit_size = 0x200 * (1u << ncsd_header.partition_flags[6]);
	}

	int raw_cart_interface::read(void* buffer, size_t buffer_size, uint64_t position, size_t count)
	{
		return ctr_io_implementation_read(*this, buffer, buffer_size, position, count);
	}

	int raw_cart_interface::read_sector(void* buffer, size_t buffer_size, size_t sector, size_t count)
	{
		if (count && ctr_cart_inserted())
		{
			const uint32_t unit_size = media_unit_size;
			const size_t count_read = buffer_size / unit_size < count ? buffer_size / unit_size : count;
			//this function doesn't care about alignment. it optimizes if it is aligned,
			//else it does the right thing for unaligned access
			CTR_CmdReadData(sector, unit_size, count_read, buffer);
		}
		return 0;
	}

	int raw_cart_interface::write(const void *buffer, size_t buffer_size, uint64_t position)
	{
		return 1; //Can't write
	}

	int raw_cart_interface::write_sector(const void *buffer, size_t buffer_size, size_t sector)
	{
		return 1; //Can't write
	}

	size_t raw_cart_interface::sector_size() const
	{
		return media_unit_size;
	}

	uint64_t raw_cart_interface::disk_size() const
	{
		return (uint64_t)ncsd_header.media_size * media_unit_size;
	}
	
	//Helper functions for handling the logical reading of cart stuff.
	//Cart has 3 sections: pre ncch header, ncch header, post
	//pre and post just read from the cart.
	//ncch header reads from the backed up ncch header.
	//
	//The helper functions check their respective regions, so just pass in
	//sectors_to_read as the full amount of sectors left to read, and the
	//helper function will use a MIN function to cut the number of sectors
	//to read to either the number of sectors left, or the number of sectors between
	//the end and the current sector.
	//

	static inline size_t pre_header_read(
		void *buffer,
		size_t ncch_start,
		size_t unit_size,
		size_t current_sector,
		size_t sectors_to_read);

	static inline size_t header_ncch_read(
		void *buffer,
		size_t ncch_start,
		size_t unit_size,
		size_t current_sector,
		size_t sectors_to_read,
		raw_cart_interface& cart);

	static inline size_t header_ff_read(
		void *buffer,
		size_t ncch_end,
		size_t unit_size,
		size_t current_sector,
		size_t sectors_to_read,
		raw_cart_interface& cart);

	static inline size_t header_read(
		void *buffer,
		size_t ncch_start,
		size_t ncch_end,
		size_t unit_size,
		size_t current_sector,
		size_t sectors_to_read,
		raw_cart_interface& cart);

	#define MIN(A, B) (A) < (B) ? (A) : (B)
	#define MAX(A, B) (A) > (B) ? (A) : (B)

	cart_interface::cart_interface()
	{}

	int cart_interface::read_sector(void* buffer, size_t buffer_size, size_t sector, size_t count)
	{
		const uint32_t unit_size = cart.sector_size();
		size_t sectors_left = MIN(buffer_size / unit_size, count);

		if (sectors_left && ctr_cart_inserted())
		{
			const size_t ncch_start = 0x1000/unit_size;
			const size_t ncch_end = 0x4000/unit_size;
			size_t current_sector = sector;

			//section 1: pre header data
			if (sectors_left && current_sector < ncch_start)
			{
				size_t sectors = pre_header_read(buffer, ncch_start, unit_size, current_sector, sectors_left);
				current_sector += sectors;
				buffer = (uint8_t*)buffer + sectors * unit_size;
				sectors_left -= sectors;
			}

			//section 2: header data
			if (sectors_left && current_sector >= ncch_start && current_sector < ncch_end)
			{
				size_t sectors = header_read(buffer, ncch_start, ncch_end, unit_size, current_sector, sectors_left, cart);
				current_sector += sectors;
				buffer = (uint8_t*)buffer + sectors * unit_size;
				sectors_left -= sectors;
			}

			//section 3: post header data
			if (sectors_left)
			{
				CTR_CmdReadData(current_sector, unit_size, sectors_left, buffer);
			}
		}

		return 0;
	}

	int cart_interface::read(void* buffer, size_t buffer_size, uint64_t position, size_t count)
	{
		return ctr_io_implementation_read(*this, buffer, buffer_size, position, count);
	}
	
	static inline size_t pre_header_read(
		void *buffer,
		size_t ncch_start,
		size_t unit_size,
		size_t current_sector,
		size_t sectors_to_read)

	{
		const size_t pre_header_count = MIN(sectors_to_read, ncch_start - current_sector);
		if (pre_header_count)
		{
			CTR_CmdReadData(current_sector, unit_size, pre_header_count, buffer);
		}
		return pre_header_count;
	}

	static inline size_t header_ncch_read(
		void *buffer,
		size_t ncch_start,
		size_t unit_size,
		size_t current_sector,
		size_t sectors_to_read,
		raw_cart_interface& cart)

	{
		const size_t ncch_count = MIN(sectors_to_read, ncch_start + 0x200/unit_size - current_sector);
		if (ncch_count)
		{
			memcpy(buffer, cart.get_raw_ncch().data() + (current_sector - ncch_start)/unit_size,
				ncch_count * unit_size);
		}
		return ncch_count;
	}

	static inline size_t header_ff_read(
		void *buffer,
		size_t ncch_end,
		size_t unit_size,
		size_t current_sector,
		size_t sectors_to_read,
		raw_cart_interface& cart)

	{
		const size_t ff_count = MIN(sectors_to_read, ncch_end - current_sector);
		if (ff_count)
		{
			memset(buffer, 0xFF, ff_count * unit_size);
		}
		return ff_count;
	}

	static inline size_t header_read(
		void *buffer,
		size_t ncch_start,
		size_t ncch_end,
		size_t unit_size,
		size_t current_sector,
		size_t sectors_to_read,
		raw_cart_interface& cart)

	{
		const size_t header_count = MIN(sectors_to_read, ncch_end - current_sector);
		if (header_count)
		{
			//There are two subsections
			//ncch
			if (current_sector < (ncch_start + 0x200/unit_size))
			{
				size_t sectors = header_ncch_read(
					buffer, ncch_start, unit_size, current_sector, sectors_to_read, cart);
				current_sector += sectors;
				buffer = (uint8_t*)buffer + sectors * unit_size;
				sectors_to_read -= sectors;
			}

			//0xFF
			if (sectors_to_read && current_sector < ncch_end)
			{
				header_ff_read(buffer, ncch_end, unit_size, current_sector, sectors_to_read, cart);
			}
		}
		return header_count;
	}

}



