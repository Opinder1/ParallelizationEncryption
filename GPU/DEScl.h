#pragma once

#include "../Shared/Base.h"

#include "OpenCL.h"
#include "Api.h"

#include <string>

namespace opencl::des
{
	class TripleDES : public EncryptBase
	{
	public:
		static constexpr size_t k_block_size = 8;
		static constexpr size_t k_min_key_size = 24;

		struct Exception {};

		struct RoundKey
		{
			unsigned char bytes[8][4] = {};
		};

		GPU_API TripleDES(const std::string& key);

		GPU_API ~TripleDES();

		GPU_API std::string GetName() const override;

		GPU_API void EncryptInPlace(std::string& input) const override;

		GPU_API void DecryptInPlace(std::string& input) const override;

	private:
		Program m_program;

		Program::FunctionID m_enc_id;
		Program::FunctionID m_dec_id;
	};
}