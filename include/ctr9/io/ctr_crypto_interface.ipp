#include <array>
#include <cstdint>
#include <cstddef>
#include <vector>
#include <cstring>
#include <type_traits>

#include <ctr9/ctr_aes.h>

namespace ctr9
{
	template<class Crypto, ctr_crypto_disk_type Type>
	storage_accessor<Crypto, Type>::storage_accessor(Crypto& crypto)
	:crypto_(crypto)
	{}

	template<class Crypto, ctr_crypto_disk_type Type>
	void storage_accessor<Crypto, Type>::input(const void *in, void *out, size_t blocks, size_t block_position)
	{
		if (Type == CTR_CRYPTO_ENCRYPTED)
			crypto_.encrypt(in, out, blocks, block_position);
		else
			crypto_.decrypt(in, out, blocks, block_position);
	}

	template<class Crypto, ctr_crypto_disk_type Type>
	void storage_accessor<Crypto, Type>::output(const void *in, void *out, size_t blocks, size_t block_position)
	{
		if (Type == CTR_CRYPTO_ENCRYPTED)
			crypto_.decrypt(in, out, blocks, block_position);
		else
			crypto_.encrypt(in, out, blocks, block_position);
	}
	
	template<class Crypto, ctr_crypto_disk_type Type>
	constexpr std::size_t storage_accessor<Crypto, Type>::block_size()
	{
		return std::decay_t<decltype(crypto_)>::block_size();
	}

	template<class IO, class CryptoDisk>
	crypto_interface<IO, CryptoDisk>::crypto_interface(uint8_t key_slot, CryptoDisk& crypto, IO& lower_io)
	:lower_io_(lower_io), crypto_(crypto), key_slot_(key_slot)
	{

	}

	template<class IO, class CryptoDisk>
	int crypto_interface<IO, CryptoDisk>::read(void *buffer, size_t buffer_size, uint64_t position, size_t count)
	{
		std::uint8_t *current = static_cast<std::uint8_t *>(buffer);
		const size_t block_size = crypto_.block_size();
		size_t left = count;
		uint64_t current_position = position;
		std::vector<std::uint8_t> chunk(block_size * 10); //FIXME make chunk-size aligned to block size

		size_t unaligned = position % block_size;
		size_t unaligned_offset = block_size - unaligned;
		if (unaligned)
		{
			int result = lower_io_.read(chunk.data(), chunk.size(), current_position - unaligned_offset, block_size);
			if (result)
			{
				return result;
			}
			crypto_.output(chunk.data(), chunk.data(), 1, current_position/block_size);
			std::memcpy(current, chunk.data()+unaligned_offset, unaligned);
			current += block_size - unaligned;
			left -= block_size - unaligned;
			current_position += block_size - unaligned;
		}
				
		for (; left > chunk.size(); left -= chunk.size(), current += chunk.size(), current_position += chunk.size())
		{
			int result = lower_io_.read(chunk.data(), chunk.size(), current_position, chunk.size());
			if (result)
			{
				return result;
			}
			crypto_.output(chunk.data(), current, chunk.size()/block_size, current_position/block_size);
			std::memcpy(current, chunk.data(), chunk.size());
		}

		if (left)
		{
			std::uint8_t rounded_to_block = left + (block_size % left);
			int result = lower_io_.read(chunk.data(), chunk.size(), current_position, rounded_to_block);
			if (result)
			{
				return result;
			}
			crypto_.output(chunk.data(), chunk.data(), rounded_to_block/block_size, current_position/block_size);
			std::memcpy(current, chunk.data(), left);
		}

		return 0;
	}

	template<class IO, class CryptoDisk>
	int crypto_interface<IO, CryptoDisk>::write(const void *buffer, size_t buffer_size, uint64_t position)
	{
		return -1;
	}

	template<class IO, class CryptoDisk>
	int crypto_interface<IO, CryptoDisk>::read_sector(void *buffer, size_t buffer_size, size_t sector, size_t count)
	{
		return -1;
	}

	template<class IO, class CryptoDisk>
	int crypto_interface<IO, CryptoDisk>::write_sector(const void *buffer, size_t buffer_size, size_t sector)
	{
		return -1;
	}

	template<class IO, class CryptoDisk>
	uint64_t crypto_interface<IO, CryptoDisk>::disk_size() const
	{
		return 0;
	}

	template<class IO, class CryptoDisk>
	size_t crypto_interface<IO, CryptoDisk>::sector_size() const
	{
		return 0;
	}

	template<class CryptoDisk, ctr_crypto_disk_type Type>
	generic_storage_accessor_impl<CryptoDisk, Type>::generic_storage_accessor_impl(CryptoDisk &disk)
	:disk_(disk)
	{}

	template<class CryptoDisk, ctr_crypto_disk_type Type>
	void generic_storage_accessor_impl<CryptoDisk, Type>::input(const void *in, void *out, size_t blocks, size_t block_position)
	{
		if (Type == CTR_CRYPTO_ENCRYPTED)
			disk_.encrypt(in, out, blocks, block_position);
		else
			disk_.decrypt(in, out, blocks, block_position);
	}

	template<class CryptoDisk, ctr_crypto_disk_type Type>
	void generic_storage_accessor_impl<CryptoDisk, Type>::output(const void *in, void *out, size_t blocks, size_t block_position)
	{
		if (Type == CTR_CRYPTO_ENCRYPTED)
			disk_.decrypt(in, out, blocks, block_position);
		else
			disk_.encrypt(in, out, blocks, block_position);
	}
	
	template<class CryptoDisk, ctr_crypto_disk_type Type>
	std::size_t generic_storage_accessor_impl<CryptoDisk, Type>::block_size() const
	{
		return disk_.block_size();;
	}
}

