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

namespace ctr9
{
	class aes_hardware
	{
	public:
		std::uint8_t write_fifo_count() const;
		void write_fifo_count(std::uint8_t count);
		std::uint8_t read_fifo_count();
		void read_fifo_count(std::uint8_t count);
		void flush_write_fifo();
		void flush_read_fifo();
		std::uint8_t write_fifo_dma_size();
		void write_fifo_dma_size(std::uint8_t size);
		std::uint8_t read_fifo_dma_size();
		void read_fifo_dma_size(std::uint8_t size);
		std::uint8_t mac_size();
		void mac_size(std::uint8_t size);
		bool mac_status();
		void mac_status(bool status);
		bool output_endian();
		void output_endian(bool big_endian);
		bool input_endian();
		void input_endian(bool big_endian);
		bool output_word_order();
		void output_word_order(bool normal);
		bool input_word_order();
		void input_word_order(bool normal);
		void update_keyslot();
		void mode(uint8_t mode);
		bool interrupt_enable();
		bool interrupt_enable(bool enable);
		bool start();
		void start(bool start);
	};

	constexpr std::size_t aes_block_size() {return AES_BLOCK_SIZE;}

	
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

	class generic_crypto
	{
	public:
		virtual ~generic_crypto() = default;
		virtual void encrypt(const void *in, void *out, size_t blocks);
		virtual void decrypt(const void *in, void *out, size_t blocks);
		virtual void set_ctr(const std::array<std::uint8_t, aes_block_size()>& ctr);
		virtual std::size_t block_size() const;
	};

	class ecb_generic_crypto : public generic_crypto
	{
	public:
		ecb_generic_crypto(ecb_crypto& crypto);
		virtual void encrypt(const void *in, void *out, size_t blocks) override;
		virtual void decrypt(const void *in, void *out, size_t blocks) override;
		virtual void set_ctr(const std::array<std::uint8_t, aes_block_size()>& ctr) override;
		virtual std::size_t block_size() const override;
	private:
		ecb_crypto& crypto_;
	};

	template<class Crypto>
	class generic_crypto_ctr_impl : public generic_crypto
	{
	public:
		template<class... Args>
		generic_crypto_ctr_impl(Args&&... args);
		virtual void encrypt(const void *in, void *out, size_t blocks) override;
		virtual void decrypt(const void *in, void *out, size_t blocks) override;
		virtual void set_ctr(const std::array<std::uint8_t, aes_block_size()>& ctr) override;
		virtual std::size_t block_size() const override;
	private:
		Crypto& crypto_;
	};
	
	typedef generic_crypto_ctr_impl<ctr_disk_crypto> ctr_generic_crypto;
	typedef generic_crypto_ctr_impl<cbc_disk_crypto> cbc_generic_crypto;
	typedef generic_crypto_ctr_impl<ccm_disk_crypto> ccm_generic_crypto;

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
