#include <array>
#include <cstdint>
#include <cstddef>

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

	template<class IO, class CryptoDisk>
	crypto_interface<IO, CryptoDisk>::crypto_interface(uint8_t key_slot, CryptoDisk& crypto, IO& lower_io)
	:lower_io_(lower_io), crypto_(crypto), key_slot_(key_slot)
	{

	}

	template<class IO, class CryptoDisk>
	int crypto_interface<IO, CryptoDisk>::read(void *buffer, size_t buffer_size, uint64_t position, size_t count)
	{
		return -1;
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
}

