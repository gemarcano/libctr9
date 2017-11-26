#ifndef CTR_AES_H_
#define CTR_AES_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}

#include <ctr9/aes.h>

#include <cstddef>
#include <cstdint>
#include <array>


namespace ctr9
{
	constexpr std::size_t aes_block_size()
	{
		return AES_BLOCK_SIZE;
	}

	// C++ crypto classes
	class ecb_crypto
	{
	public:
		ecb_crypto(std::uint32_t mode);
		void encrypt(const void *in, void *out, size_t blocks);
		void decrypt(const void *in, void *out, size_t blocks);
		static constexpr std::size_t block_size();
	private:
		std::uint32_t mode_;
	};

	class ctr_crypto
	{
	public:
		ctr_crypto(std::uint32_t mode);
		void set_ctr(const std::array<std::uint8_t, aes_block_size()>& ctr);
		void encrypt(const void *in, void *out, size_t blocks);
		void decrypt(const void *in, void *out, size_t blocks);
		static constexpr std::size_t block_size();

	private:
		std::uint32_t mode_;
		std::array<std::uint8_t, aes_block_size()> ctr_;
	};

	class cbc_crypto
	{
	public:
		cbc_crypto(std::uint32_t mode);
		void set_ctr(const std::array<std::uint8_t, aes_block_size()>& ctr);
		void encrypt(const void *in, void *out, size_t blocks);
		void decrypt(const void *in, void *out, size_t blocks);
		static constexpr std::size_t block_size();

	private:
		std::uint32_t mode_;
		std::array<std::uint8_t, aes_block_size()> ctr_;
	};

	class ccm_crypto
	{
	public:
		ccm_crypto(std::uint32_t mode);
		void set_ctr(const std::array<std::uint8_t, aes_block_size()>& ctr);
		void encrypt(const void *in, void *out, size_t blocks);
		void decrypt(const void *in, void *out, size_t blocks);
		static constexpr std::size_t block_size();

	private:
		std::uint32_t mode_;
		std::array<std::uint8_t, aes_block_size()> ctr_;
	};

	//Disk crypto classes ---------------------------
	class ecb_disk_crypto
	{
	public:
		ecb_disk_crypto(ecb_crypto &crypto);
		void encrypt(const void *in, void *out, size_t blocks, size_t block_position);
		void decrypt(const void *in, void *out, size_t blocks, size_t block_position);
		static constexpr std::size_t block_size();

	private:
		ecb_crypto crypto_;
	};
	
	//Used to make an object with identical construction as all the other crypto ones
	class ecb_disk_crypto_wrapper
	{
	public:
		ecb_disk_crypto_wrapper(ecb_crypto& crypto, const std::array<std::uint8_t, aes_block_size()>&);
		void encrypt(const void *in, void *out, size_t blocks, size_t block_position);
		void decrypt(const void *in, void *out, size_t blocks, size_t block_position);
		static constexpr std::size_t block_size();
	private:
		ecb_disk_crypto crypto_;
	};

	template<class CtrCrypto>
	class ctr_disk_crypto_impl
	{
	public:
		ctr_disk_crypto_impl(CtrCrypto& crypto, const std::array<std::uint8_t, aes_block_size()>& base_ctr);
		void encrypt(const void *in, void *out, size_t blocks, size_t block_position);
		void decrypt(const void *in, void *out, size_t blocks, size_t block_position);
		static constexpr std::size_t block_size();

	private:
		CtrCrypto& crypto_;
		std::array<std::uint8_t, aes_block_size()> base_ctr_;
	};

	typedef ctr_disk_crypto_impl<ctr_crypto> ctr_disk_crypto;
	typedef ctr_disk_crypto_impl<cbc_crypto> cbc_disk_crypto;
	typedef ctr_disk_crypto_impl<ccm_crypto> ccm_disk_crypto;

	//Generic classes to help implement C API--------------------------------

	class generic_crypto_base
	{
	public:
		virtual ~generic_crypto_base() = default;
		virtual void encrypt(const void *in, void *out, size_t blocks) = 0;
		virtual void decrypt(const void *in, void *out, size_t blocks) = 0;
		virtual void set_ctr(const std::array<std::uint8_t, aes_block_size()>& ctr) = 0;
		virtual std::size_t block_size() const = 0;
	};

	template<class Crypto>
	class generic_crypto_ctr_impl : public generic_crypto_base
	{
	public:
		template<class... Args>
		generic_crypto_ctr_impl(Args&&... args);
		virtual void encrypt(const void *in, void *out, size_t blocks) override;
		virtual void decrypt(const void *in, void *out, size_t blocks) override;
		virtual void set_ctr(const std::array<std::uint8_t, aes_block_size()>& ctr) override;
		virtual std::size_t block_size() const override;
	private:
		Crypto crypto_;
	};
	
	typedef generic_crypto_ctr_impl<ecb_crypto> ecb_generic_crypto;
	typedef generic_crypto_ctr_impl<ctr_crypto> ctr_generic_crypto;
	typedef generic_crypto_ctr_impl<cbc_crypto> cbc_generic_crypto;
	typedef generic_crypto_ctr_impl<ccm_crypto> ccm_generic_crypto;
	
	class generic_crypto : public generic_crypto_base
	{
	public:
		generic_crypto(std::uint32_t mode);
		~generic_crypto();
		virtual void encrypt(const void *in, void *out, size_t blocks) override;
		virtual void decrypt(const void *in, void *out, size_t blocks) override;
		virtual void set_ctr(const std::array<std::uint8_t, aes_block_size()>& ctr) override;
		virtual std::size_t block_size() const override;
	private:
		generic_crypto_base *crypto_;
	};

	class generic_disk_crypto
	{
	public:
		generic_disk_crypto(generic_crypto& crypto, const std::array<std::uint8_t, aes_block_size()>& ctr);
		~generic_disk_crypto() = default;
		void encrypt(const void *in, void *out, size_t blocks, size_t block_position);
		void decrypt(const void *in, void *out, size_t blocks, size_t block_position);
		std::size_t block_size() const;
	private:
		generic_crypto& crypto_;
		std::array<std::uint8_t, aes_block_size()> ctr_;
	};
}

#include "ctr_aes.ipp"

#endif

#endif//CTR_AES_H_
