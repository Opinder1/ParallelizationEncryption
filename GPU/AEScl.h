#pragma once

#include "../Shared/Base.h"

#include "OpenCL.h"
#include "Api.h"

#include <string>

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

		GPU_API void EncryptInPlace(std::string& input) const override;

		GPU_API void DecryptInPlace(std::string& input) const override;

	private:
		unsigned int m_rounds;

		Program m_program;

		Program::FunctionID m_enc_id;
		Program::FunctionID m_dec_id;
	};
}