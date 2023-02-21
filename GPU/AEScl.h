#pragma once

#include "../Shared/Base.h"

#include <string>

#include "OpenCL.h"

#ifdef DLL_IMPORT
#define GPU_API __declspec(dllimport)
#else
#define GPU_API __declspec(dllexport)
#endif

namespace opencl::aes
{
	class AES : public EncryptBase
	{
	public:
		static constexpr size_t k_block_size = 16;
		static constexpr size_t k_min_key_size = 16;

		struct Exception {};

		struct RoundKey
		{
			unsigned char bytes[8][4] = {};
		};

		GPU_API AES(const std::string& key, size_t group_size = 1);

		GPU_API ~AES();

		GPU_API void EncryptInPlace(std::string& input) const;

		GPU_API void DecryptInPlace(std::string& input) const;

		GPU_API std::string Encrypt(const std::string& input) const override;

		GPU_API std::string Decrypt(const std::string& input) const override;

	private:
		unsigned int m_rounds;

		Program m_program;

		Program::FunctionID m_enc_id;
		Program::FunctionID m_dec_id;
	};
}