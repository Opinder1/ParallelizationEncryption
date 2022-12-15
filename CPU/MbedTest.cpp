#include "MbedTest.h"

#include "../Shared/Shared.h"

#include <omp.h>

#include <algorithm>

namespace mbed
{
	void ReverseCopy(const void* src, size_t src_size, void* dest, size_t dest_size)
	{
		for (size_t i = 0; i < src_size; i++)
		{
			((char*)dest)[dest_size - 1 - i] = ((char*)src)[i];
		}
	}

	DES::DES(const std::string& key) :
		EncryptBase(key)
	{
		mbedtls_des_init(&m_enc_ctx);
		mbedtls_des_init(&m_dec_ctx);

		if (mbedtls_des_setkey_enc(&m_enc_ctx, (const unsigned char*)key.data()) != 0)
		{
			throw;
		}

		if (mbedtls_des_setkey_dec(&m_dec_ctx, (const unsigned char*)key.data()) != 0)
		{
			throw;
		}
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
			if (mbedtls_des_crypt_ecb(const_cast<mbedtls_des_context*>(&m_enc_ctx), (const unsigned char*)&input[i], (unsigned char*)&output[i]) != 0)
			{
				throw;
			}
		}

		return output;
	}

	std::string DES::Decrypt(const std::string& input) const
	{
		std::string output(input.size(), '\0');

		for (size_t i = 0; i < input.size(); i += 8)
		{
			if (mbedtls_des_crypt_ecb(const_cast<mbedtls_des_context*>(&m_dec_ctx), (const unsigned char*)&input[i], (unsigned char*)&output[i]) != 0)
			{
				throw;
			}
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
			if (mbedtls_des_crypt_ecb(const_cast<mbedtls_des_context*>(&m_enc_ctx), (const unsigned char*)&input[i], (unsigned char*)&output[i]) != 0)
			{
				throw;
			}
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
			if (mbedtls_des_crypt_ecb(const_cast<mbedtls_des_context*>(&m_dec_ctx), (const unsigned char*)&input[i], (unsigned char*)&output[i]) != 0)
			{
				throw;
			}
		}

		return output;
	}

	TripleDES::TripleDES(const std::string& key) :
		EncryptBase(key)
	{
		mbedtls_des3_init(&m_enc_ctx);
		mbedtls_des3_init(&m_dec_ctx);

		if (mbedtls_des3_set3key_enc(&m_enc_ctx, (const unsigned char*)key.data()) != 0)
		{
			throw;
		}

		if (mbedtls_des3_set3key_dec(&m_dec_ctx, (const unsigned char*)key.data()) != 0)
		{
			throw;
		}
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
			if (mbedtls_des3_crypt_ecb(const_cast<mbedtls_des3_context*>(&m_enc_ctx), (const unsigned char*)&input[i], (unsigned char*)&output[i]) != 0)
			{
				throw;
			}
		}

		return output;
	}

	std::string TripleDES::Decrypt(const std::string& input) const
	{
		std::string output(input.size(), '\0');

		for (size_t i = 0; i < input.size(); i += 8)
		{
			if (mbedtls_des3_crypt_ecb(const_cast<mbedtls_des3_context*>(&m_dec_ctx), (const unsigned char*)&input[i], (unsigned char*)&output[i]) != 0)
			{
				throw;
			}
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
			if (mbedtls_des3_crypt_ecb(const_cast<mbedtls_des3_context*>(&m_enc_ctx), (const unsigned char*)&input[i], (unsigned char*)&output[i]) != 0)
			{
				throw;
			}
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
			if (mbedtls_des3_crypt_ecb(const_cast<mbedtls_des3_context*>(&m_dec_ctx), (const unsigned char*)&input[i], (unsigned char*)&output[i]) != 0)
			{
				throw;
			}
		}

		return output;
	}

	AES::AES(const std::string& key) :
		EncryptBase(key)
	{
		mbedtls_aes_init(&m_ctx);

		unsigned int bits;
		switch(key.size())
		{
		case 16:
			bits = 128;
			break;

		case 24:
			bits = 192;
			break;

		case 32:
			bits = 256;
			break;

		default:
			throw;
		}

		if (mbedtls_aes_setkey_enc(&m_ctx, (const unsigned char*)key.data(), bits) != 0)
		{
			throw;
		}

		if (mbedtls_aes_setkey_dec(&m_ctx, (const unsigned char*)key.data(), bits) != 0)
		{
			throw;
		}
	}

	AES::~AES()
	{
		mbedtls_aes_free(&m_ctx);
	}

	int AES::Test()
	{
		return mbedtls_aes_self_test(1);
	}

	std::string AES::Encrypt(const std::string& input) const
	{
		std::string output(input.size(), '\0');

		size_t offset = 0;
		unsigned char nonce_counter[16] = {};
		unsigned char stream_block[16] = {};

		if (mbedtls_aes_crypt_ctr(const_cast<mbedtls_aes_context*>(&m_ctx), input.size(), &offset, nonce_counter, stream_block, (const unsigned char*)input.data(), (unsigned char*)output.data()) != 0)
		{
			throw;
		}

		return output;
	}

	std::string AES::Decrypt(const std::string& input) const
	{
		std::string output(input.size(), '\0');

		size_t offset = 0;
		unsigned char nonce_counter[16] = {};
		unsigned char stream_block[16] = {};

		if (mbedtls_aes_crypt_ctr(const_cast<mbedtls_aes_context*>(&m_ctx), input.size(), &offset, nonce_counter, stream_block, (const unsigned char*)input.data(), (unsigned char*)output.data()) != 0)
		{
			throw;
		}

		return output;
	}

	AESParallel::AESParallel(const std::string& key, size_t group_size) :
		AES(key),
		m_group_size(group_size)
	{}

	std::string AESParallel::Encrypt(const std::string& input) const
	{
		mbedtls_aes_context last;

		std::string output(input.size(), '\0');

		int i;

#pragma omp parallel for num_threads(16)
		for (i = 0; i < input.size(); i += (int)m_group_size)
		{
			size_t offset = 0;
			unsigned char nonce_counter[16] = {};
			unsigned char stream_block[16] = {};

			int block_index = i / 16;
			// Uses the left size while mbedtls_aes_crypt_ctr uses the right side so they don't collide and each counter item is unique
			ReverseCopy(&block_index, sizeof(int), nonce_counter, 16);

			if (mbedtls_aes_crypt_ctr(const_cast<mbedtls_aes_context*>(&m_ctx), m_group_size, &offset, nonce_counter, stream_block, (const unsigned char*)&input[i], (unsigned char*)&output[i]) != 0)
			{
				throw;
			}
		}

		return output;
	}

	std::string AESParallel::Decrypt(const std::string& input) const
	{
		std::string output(input.size(), '\0');

		int i;

#pragma omp parallel for num_threads(16)
		for (i = 0; i < input.size(); i += (int)m_group_size)
		{
			size_t offset = 0;
			unsigned char nonce_counter[16] = {};
			unsigned char stream_block[16] = {};

			int block_index = i / 16;
			// Uses the left size while mbedtls_aes_crypt_ctr uses the right side so they don't collide and each counter item is unique
			ReverseCopy(&block_index, sizeof(int), nonce_counter, 16);

			if (mbedtls_aes_crypt_ctr(const_cast<mbedtls_aes_context*>(&m_ctx), m_group_size, &offset, nonce_counter, stream_block, (const unsigned char*)&input[i], (unsigned char*)&output[i]) != 0)
			{
				throw;
			}
		}

		return output;
	}
}