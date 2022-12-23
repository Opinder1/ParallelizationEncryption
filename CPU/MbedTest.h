#pragma once

#include "Base.h"

#include "mbedtls/des.h"

#include "mbedtls/aes.h"

#include <string>
#include <string>

namespace mbed
{
	class DES : public EncryptBase
	{
	public:
		static constexpr size_t k_block_size = 8;
		static constexpr size_t k_min_key_size = 8;

		struct Exception {};

	public:
		DES(const std::string& key);

		~DES();

		static int Test();

		std::string Encrypt(const std::string& input) const override;

		std::string Decrypt(const std::string& input) const override;

	protected:
		mbedtls_des_context m_enc_ctx;
		mbedtls_des_context m_dec_ctx;
	};

	class DESParallel : public DES
	{
	public:
		DESParallel(const std::string& key, size_t group_size = 1);

		std::string Encrypt(const std::string& input) const override;

		std::string Decrypt(const std::string& input) const override;
	};

	class TripleDES : public EncryptBase
	{
	public:
		static constexpr size_t k_block_size = 8;
		static constexpr size_t k_min_key_size = 8 * 3;

		struct Exception {};

	public:
		TripleDES(const std::string& key);

		~TripleDES();

		std::string Encrypt(const std::string& input) const override;

		std::string Decrypt(const std::string& input) const override;

	protected:
		mbedtls_des3_context m_enc_ctx;
		mbedtls_des3_context m_dec_ctx;
	};

	class TripleDESParallel : public TripleDES
	{
	public:
		TripleDESParallel(const std::string& key, size_t group_size = 1);

		std::string Encrypt(const std::string& input) const override;

		std::string Decrypt(const std::string& input) const override;
	};

	class AES : public EncryptBase
	{
	public:
		static constexpr size_t k_block_size = 16;
		static constexpr size_t k_min_key_size = 16;

		struct Exception {};

	public:
		AES(const std::string& key);

		~AES();

		static int Test();

		std::string Encrypt(const std::string& input) const override;

		std::string Decrypt(const std::string& input) const override;

	protected:
		mbedtls_aes_context m_ctx;
	};

	class AESParallel : public AES
	{
	public:
		AESParallel(const std::string& key, size_t group_size = 1);

		std::string Encrypt(const std::string& input) const override;

		std::string Decrypt(const std::string& input) const override;

	private:
		size_t m_group_size;
	};
}