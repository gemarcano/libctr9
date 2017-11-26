#include <ctr9/aes.h>

namespace ctr9
{
	constexpr std::size_t ecb_crypto::block_size()
	{
		return aes_block_size();
	}

	template<class Crypto>
	template<class... Args>
	generic_crypto_ctr_impl<Crypto>::generic_crypto_ctr_impl(Args&&... args)
	:crypto_(std::forward<Args&&>(args)...)
	{}
	
	template<class Crypto>
	void generic_crypto_ctr_impl<Crypto>::encrypt(const void *in, void *out, size_t blocks)
	{
		crypto_.encrypt(in, out, blocks);
	}
	
	template<class Crypto>
	void generic_crypto_ctr_impl<Crypto>::decrypt(const void *in, void *out, size_t blocks)
	{
		crypto_.decrypt(in, out, blocks);
	}
	
	template<class Crypto>
	void generic_crypto_ctr_impl<Crypto>::set_ctr(const std::array<std::uint8_t, aes_block_size()>& ctr)
	{
		crypto_.set_ctr(ctr);
	}

	template<class Crypto>
	std::size_t generic_crypto_ctr_impl<Crypto>::block_size() const
	{
		return crypto_.block_size();
	}

	
	constexpr std::size_t ecb_disk_crypto::block_size()
	{
		return decltype(crypto_)::block_size();
	}


	template<class CtrCrypto>
	constexpr std::size_t ctr_disk_crypto_impl<CtrCrypto>::block_size()
	{
		return aes_block_size();
	}

	
}

