#include <ctr9/ctr_aes.h>
#include <type_traits>

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

	template<>
	void generic_crypto_ctr_impl<ecb_crypto>::set_ctr(const std::array<std::uint8_t, aes_block_size()>& ctr)
	{ /*Do nothing*/ }

	generic_crypto::generic_crypto(std::uint32_t mode)
	{
		crypto_ = new ecb_generic_crypto(mode); //FIXME, determine which crypto to use based on mode
	}

	generic_crypto::~generic_crypto()
	{
		delete crypto_;
	}

	void generic_crypto::encrypt(const void *in, void *out, size_t blocks)
	{
		crypto_->encrypt(in, out, blocks);
	}

	void generic_crypto::decrypt(const void *in, void *out, size_t blocks)
	{
		crypto_->decrypt(in, out, blocks);
	}

	void generic_crypto::set_ctr(const std::array<std::uint8_t, aes_block_size()>& ctr)
	{
		crypto_->set_ctr(ctr);
	}

	std::size_t generic_crypto::block_size() const
	{
		return crypto_->block_size();
	}

	//Disk section

	ecb_disk_crypto::ecb_disk_crypto(ecb_crypto &crypto)
	:crypto_(crypto)
	{}
	
	void ecb_disk_crypto::encrypt(const void *in, void *out, size_t blocks, size_t block_position)
	{}
	
	void ecb_disk_crypto::decrypt(const void *in, void *out, size_t blocks, size_t block_position)
	{}

	constexpr std::size_t ecb_disk_crypto::block_size()
	{
		return decltype(crypto_)::block_size();
	}

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
}

