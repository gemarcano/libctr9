#include <ctr9/aes.h>

namespace ctr9
{
	ecb_crypto::ecb_crypto(std::uint32_t mode)
	:mode_(mode & ~(7u << 27))
	{}

	void ecb_crypto::encrypt(const void *in, void *out, size_t blocks)
	{
		ecb_decrypt(in, out, blocks, mode_ | AES_ECB_ENCRYPT_MODE);
	}

	void ecb_crypto::decrypt(const void *in, void *out, size_t blocks)
	{
		ecb_decrypt(in, out, blocks, mode_ | AES_ECB_DECRYPT_MODE);
	}

	constexpr std::size_t ecb_crypto::block_size()
	{
		return aes_block_size();
	}

	ctr_crypto::ctr_crypto(std::uint32_t mode)
	:mode_(mode & ~(7u << 27))
	{}

	void ctr_crypto::set_ctr(const std::array<std::uint8_t, aes_block_size()>& ctr)
	{
		ctr_ = ctr;
	}

	void ctr_crypto::encrypt(const void *in, void *out, size_t blocks)
	{
		ctr_decrypt(in, out, blocks, mode_ | AES_CTR_ENCRYPT_MODE, ctr_.data());
	}

	void ctr_crypto::decrypt(const void *in, void *out, size_t blocks)
	{
		ctr_decrypt(in, out, blocks, mode_ | AES_CTR_DECRYPT_MODE, ctr_.data());
	}

	constexpr std::size_t ctr_crypto::block_size()
	{
		return aes_block_size();
	}

	cbc_crypto::cbc_crypto(std::uint32_t mode)
	:mode_(mode & ~(7u << 27))
	{}

	void cbc_crypto::set_ctr(const std::array<std::uint8_t, aes_block_size()>& ctr)
	{
		ctr_ = ctr;
	}

	void cbc_crypto::encrypt(const void *in, void *out, size_t blocks)
	{
		cbc_decrypt(in, out, blocks, mode_ | AES_CBC_ENCRYPT_MODE, ctr_.data());
	}

	void cbc_crypto::decrypt(const void *in, void *out, size_t blocks)
	{
		cbc_decrypt(in, out, blocks, mode_ | AES_CBC_DECRYPT_MODE, ctr_.data());
	}

	constexpr std::size_t cbc_crypto::block_size()
	{
		return aes_block_size();
	}

	ccm_crypto::ccm_crypto(std::uint32_t mode)
	:mode_(mode & ~(7u << 27))
	{}

	void ccm_crypto::set_ctr(const std::array<std::uint8_t, aes_block_size()>& ctr)
	{
		ctr_ = ctr;
	}

	void ccm_crypto::encrypt(const void *in, void *out, size_t blocks)
	{
		ccm_decrypt(in, out, blocks, mode_ | AES_CCM_ENCRYPT_MODE, ctr_.data());
	}

	void ccm_crypto::decrypt(const void *in, void *out, size_t blocks)
	{
		ccm_decrypt(in, out, blocks, mode_ | AES_CCM_DECRYPT_MODE, ctr_.data());
	}

	constexpr std::size_t ccm_crypto::block_size()
	{
		return aes_block_size();
	}

	ecb_generic_crypto::ecb_generic_crypto(ecb_crypto &crypto)
	:crypto_(crypto)
	{}

	void ecb_generic_crypto::encrypt(const void *in, void *out, size_t blocks)
	{
		crypto_.encrypt(in, out, blocks);
	}

	void ecb_generic_crypto::decrypt(const void *in, void *out, size_t blocks)
	{
		crypto_.decrypt(in, out, blocks);
	}

	void ecb_generic_crypto::set_ctr(const std::array<std::uint8_t, aes_block_size()>&)
	{}

	std::size_t ecb_generic_crypto::block_size() const
	{
		return crypto_.block_size();
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


	template<class CtrCrypto>
	constexpr std::size_t ctr_disk_crypto_impl<CtrCrypto>::block_size()
	{
		return aes_block_size();
	}

	template<>
	void ctr_disk_crypto_impl<ctr_crypto>::encrypt(const void *in, void *out, size_t blocks, size_t block_position)
	{}

	template<>
	void ctr_disk_crypto_impl<ctr_crypto>::decrypt(const void *in, void *out, size_t blocks, size_t block_position)
	{}

	template<>
	void ctr_disk_crypto_impl<cbc_crypto>::encrypt(const void *in, void *out, size_t blocks, size_t block_position)
	{}

	template<>
	void ctr_disk_crypto_impl<cbc_crypto>::decrypt(const void *in, void *out, size_t blocks, size_t block_position)
	{}

	template<>
	void ctr_disk_crypto_impl<ccm_crypto>::encrypt(const void *in, void *out, size_t blocks, size_t block_position)
	{}

	template<>
	void ctr_disk_crypto_impl<ccm_crypto>::decrypt(const void *in, void *out, size_t blocks, size_t block_position)
	{}

	generic_disk_crypto::generic_disk_crypto(generic_crypto& crypto, const std::array<std::uint8_t, aes_block_size()>& ctr)
	:crypto_(crypto), ctr_(ctr)
	{}

	void generic_disk_crypto::encrypt(const void *in, void *out, size_t blocks, size_t block_position)
	{
		auto ctr = ctr_;
		add_ctr(ctr.data(), block_position);
		crypto_.set_ctr(ctr);
		crypto_.encrypt(in, out, blocks);
	}

	void generic_disk_crypto::decrypt(const void *in, void *out, size_t blocks, size_t block_position)
	{
		auto ctr = ctr_;
		add_ctr(ctr.data(), block_position);
		crypto_.set_ctr(ctr);
		crypto_.decrypt(in, out, blocks);
	}

	std::size_t generic_disk_crypto::block_size() const
	{
		return crypto_.block_size();
	}
}

