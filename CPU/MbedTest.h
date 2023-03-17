#pragma once

#include "../Shared/Base.h"

#include "Api.h"

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
		CPU_API DES(const std::string& key);

		CPU_API ~DES();

		CPU_API static int Test();

		CPU_API void EncryptInPlace(std::string& input) const override;

		CPU_API void DecryptInPlace(std::string& input) const override;

	protected:
		mbedtls_des_context m_enc_ctx;
		mbedtls_des_context m_dec_ctx;
	};

	class DESParallel : public DES
	{
	public:
		CPU_API DESParallel(const std::string& key, size_t group_size = 1);

		CPU_API void EncryptInPlace(std::string& input) const override;

		CPU_API void DecryptInPlace(std::string& input) const override;
	};

	class TripleDES : public EncryptBase
	{
	public:
		static constexpr size_t k_block_size = 8;
		static constexpr size_t k_min_key_size = 8 * 3;

		struct Exception {};

	public:
		CPU_API TripleDES(const std::string& key);

		CPU_API ~TripleDES();

		CPU_API void EncryptInPlace(std::string& input) const override;

		CPU_API void DecryptInPlace(std::string& input) const override;

	protected:
		mbedtls_des3_context m_enc_ctx;
		mbedtls_des3_context m_dec_ctx;
	};

	class TripleDESParallel : public TripleDES
	{
	public:
		CPU_API TripleDESParallel(const std::string& key, size_t group_size = 1);

		CPU_API void EncryptInPlace(std::string& input) const override;

		CPU_API void DecryptInPlace(std::string& input) const override;
	};

	class AES : public EncryptBase
	{
	public:
		static constexpr size_t k_block_size = 16;
		static constexpr size_t k_min_key_size = 16;

		struct Exception {};

	public:
		CPU_API AES(const std::string& key);

		CPU_API ~AES();

		CPU_API static int Test();

		CPU_API void EncryptInPlace(std::string& input) const override;

		CPU_API void DecryptInPlace(std::string& input) const override;

	protected:
		mbedtls_aes_context m_ctx;
	};

	class AESParallel : public AES
	{
	public:
		CPU_API AESParallel(const std::string& key, size_t group_size = 1);

		CPU_API void EncryptInPlace(std::string& input) const override;

		CPU_API void DecryptInPlace(std::string& input) const override;

	private:
		size_t m_group_size;
	};
}