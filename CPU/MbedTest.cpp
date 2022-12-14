#include "MbedTest.h"

#include "../Shared/Shared.h"

#include <omp.h>

#include <algorithm>

namespace mbed
{
	DES::DES(const std::string& key) :
		EncryptBase(key)
	{
		mbedtls_des_init(&m_enc_ctx);
		mbedtls_des_init(&m_dec_ctx);

		mbedtls_des_setkey_enc(&m_enc_ctx, (const unsigned char*)key.data());

		mbedtls_des_setkey_dec(&m_dec_ctx, (const unsigned char*)key.data());
	}

	DES::~DES()
	{
		mbedtls_des_free(&m_enc_ctx);
		mbedtls_des_free(&m_dec_ctx);
	}

	int DES::Test()
	{
		return mbedtls_des_self_test(1);
	}

	std::string DES::Encrypt(const std::string& input) const
	{
		std::string output(input.size(), '\0');

		for (size_t i = 0; i < input.size(); i += 8)
		{
			mbedtls_des_crypt_ecb(const_cast<mbedtls_des_context*>(&m_enc_ctx), (const unsigned char*)&input[i], (unsigned char*)&output[i]);
		}

		return output;
	}

	std::string DES::Decrypt(const std::string& input) const
	{
		std::string output(input.size(), '\0');

		for (size_t i = 0; i < input.size(); i += 8)
		{
			mbedtls_des_crypt_ecb(const_cast<mbedtls_des_context*>(&m_dec_ctx), (const unsigned char*)&input[i], (unsigned char*)&output[i]);
		}

		return output;
	}

	DESParallel::DESParallel(const std::string& key) :
		DES(key)
	{}

	std::string DESParallel::Encrypt(const std::string& input) const
	{
		std::string output(input.size(), '\0');

		int i;

#pragma omp parallel for num_threads(16)
		for (i = 0; i < input.size(); i += 8)
		{
			mbedtls_des_crypt_ecb(const_cast<mbedtls_des_context*>(&m_enc_ctx), (const unsigned char*)&input[i], (unsigned char*)&output[i]);
		}

		return output;
	}

	std::string DESParallel::Decrypt(const std::string& input) const
	{
		std::string output(input.size(), '\0');

		int i;

#pragma omp parallel for num_threads(16)
		for (i = 0; i < input.size(); i += 8)
		{
			mbedtls_des_crypt_ecb(const_cast<mbedtls_des_context*>(&m_dec_ctx), (const unsigned char*)&input[i], (unsigned char*)&output[i]);
		}

		return output;
	}

	TripleDES::TripleDES(const std::string& key) :
		EncryptBase(key)
	{
		mbedtls_des3_init(&m_enc_ctx);
		mbedtls_des3_init(&m_dec_ctx);

		mbedtls_des3_set3key_enc(&m_enc_ctx, (const unsigned char*)key.data());

		mbedtls_des3_set3key_dec(&m_dec_ctx, (const unsigned char*)key.data());
	}

	TripleDES::~TripleDES()
	{
		mbedtls_des3_free(&m_enc_ctx);
		mbedtls_des3_free(&m_dec_ctx);
	}

	std::string TripleDES::Encrypt(const std::string& input) const
	{
		std::string output(input.size(), '\0');

		for (size_t i = 0; i < input.size(); i += 8)
		{
			mbedtls_des3_crypt_ecb(const_cast<mbedtls_des3_context*>(&m_enc_ctx), (const unsigned char*)&input[i], (unsigned char*)&output[i]);
		}

		return output;
	}

	std::string TripleDES::Decrypt(const std::string& input) const
	{
		std::string output(input.size(), '\0');

		for (size_t i = 0; i < input.size(); i += 8)
		{
			mbedtls_des3_crypt_ecb(const_cast<mbedtls_des3_context*>(&m_dec_ctx), (const unsigned char*)&input[i], (unsigned char*)&output[i]);
		}

		return output;
	}

	TripleDESParallel::TripleDESParallel(const std::string& key) :
		TripleDES(key)
	{}

	std::string TripleDESParallel::Encrypt(const std::string& input) const
	{
		std::string output(input.size(), '\0');

		int i;

#pragma omp parallel for num_threads(16)
		for (i = 0; i < input.size(); i += 8)
		{
			mbedtls_des3_crypt_ecb(const_cast<mbedtls_des3_context*>(&m_enc_ctx), (const unsigned char*)&input[i], (unsigned char*)&output[i]);
		}

		return output;
	}

	std::string TripleDESParallel::Decrypt(const std::string& input) const
	{
		std::string output(input.size(), '\0');

		int i;

#pragma omp parallel for num_threads(16)
		for (i = 0; i < input.size(); i += 8)
		{
			mbedtls_des3_crypt_ecb(const_cast<mbedtls_des3_context*>(&m_dec_ctx), (const unsigned char*)&input[i], (unsigned char*)&output[i]);
		}

		return output;
	}
}