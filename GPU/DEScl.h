#pragma once

#include "../Shared/Base.h"

#include <string>

#include "OpenCL.h"

#ifdef DLL_IMPORT
#define GPU_API __declspec(dllimport)
#else
#define GPU_API __declspec(dllexport)
#endif

namespace opencl::des
{
	class DES : public EncryptBase
	{
	public:
		static constexpr size_t k_block_size = 8;
		static constexpr size_t k_min_key_size = 8;

		struct Exception {};

		struct RoundKey
		{
			unsigned char bytes[8][4] = {};
		};

		GPU_API DES(const std::string& key, size_t group_size = 1);

		GPU_API ~DES();

		GPU_API void EncryptInPlace(std::string& input) const override;

		GPU_API void DecryptInPlace(std::string& input) const override;

	private:
		unsigned int m_rounds;

		Program m_program;

		Program::FunctionID m_enc_id;
		Program::FunctionID m_dec_id;
	};

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

		GPU_API TripleDES(const std::string& key, size_t group_size = 1);

		GPU_API ~TripleDES();

		GPU_API void EncryptInPlace(std::string& input) const override;

		GPU_API void DecryptInPlace(std::string& input) const override;

	private:
		unsigned int m_rounds;

		Program m_program;

		Program::FunctionID m_enc_id;
		Program::FunctionID m_dec_id;
	};
}