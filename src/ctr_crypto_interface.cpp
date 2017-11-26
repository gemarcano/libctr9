/*******************************************************************************
 * Copyright (C) 2016, 2017 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

#include <ctr9/io/ctr_crypto_interface.h>
#include <ctr9/io/ctr_io_implementation.hpp>

#include <ctr9/io/sdmmc/sdmmc.h>
#include <string.h>
#include <ctr9/aes.h>
#include <ctr9/ctr_aes.h>
#include <ctr9/sha.h>

#include <stdalign.h>
#include <type_traits>

namespace ctr9
{
	ecb_disk_crypto_wrapper::ecb_disk_crypto_wrapper(ecb_crypto& crypto, const std::array<std::uint8_t, aes_block_size()>&)
	:crypto_(crypto)
	{}
	
	void ecb_disk_crypto_wrapper::encrypt(const void *in, void *out, size_t blocks, size_t block_position)
	{
		crypto_.encrypt(in, out, blocks, block_position);
	}
	
	void ecb_disk_crypto_wrapper::decrypt(const void *in, void *out, size_t blocks, size_t block_position)
	{
		crypto_.decrypt(in, out, blocks, block_position);
	}
	
	constexpr std::size_t ecb_disk_crypto_wrapper::block_size()
	{
		return std::decay_t<decltype(crypto_)>::block_size();
	}
}

namespace ctr9
{
	static ctr9::crypto_generic_interface *create_crypto_generic_interface(std::uint8_t key_slot, std::uint32_t mode, const std::array<std::uint8_t, aes_block_size()>& ctr, ctr_io_interface *lower_io)
	{
		//FIXME
		//Determine crypto type from mode

		/*
		 
		This is messy...

		So, the generic crypto interface needs a key_slot, an Accessor, and the lower layer
			The generic_accessor needs a generic_CryptoDisk
				The generic_CryptoDisk needs a generic_crypto and ctr
					generic_crypto needs the actual crypto class to use
						crypto class needs the mode of encryption

		GM - Remember, CryptoDisk isn't a disk, it merely holds logic for accessing a disk, namely tracks ctr.

		At the end of the day, who the heck owns all of this state? What is the state anyway?
		State:
			key_slot, mode, lower layer, ctr

		 * */
		ctr9::ecb_crypto ecb(mode);
		ctr9::ecb_generic_crypto crypto(ecb);
		ctr9::generic_disk_crypto disk(crypto, ctr);
		ctr9::generic_encrypted_storage_accessor acc(disk);
		ctr9::crypto_generic_interface *result = nullptr;// new (std::nothrow) ctr9::crypto_generic_interface(key_slot, acc, reinterpret_cast<ctr9::io_interface*>(lower_io));
		return result;
	}
}

ctr_crypto_interface *ctr_crypto_interface_initialize(uint8_t key_slot, uint32_t mode, ctr_crypto_disk_type disk_type, ctr_crypto_type type, uint8_t *ctr, void *lower_io)
{
	std::array<std::uint8_t, ctr9::aes_block_size()> ctr_;
	memcpy(&ctr_, ctr, sizeof(ctr_));
	ctr9::crypto_generic_interface *result = ctr9::create_crypto_generic_interface(key_slot, mode, ctr_, reinterpret_cast<ctr_io_interface*>(lower_io));
	return reinterpret_cast<ctr_crypto_interface*>(result);
}

void ctr_crypto_interface_destroy(ctr_crypto_interface *crypto_io)
{
	ctr9::crypto_generic_interface *crypto = reinterpret_cast<ctr9::crypto_generic_interface *>(crypto_io);
	delete crypto;
}

