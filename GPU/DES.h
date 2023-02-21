#pragma once

#include "../Shared/Base.h"

#include <bitset>
#include <string>

#ifdef DLL_IMPORT
#define GPU_API __declspec(dllimport)
#else
#define GPU_API __declspec(dllexport)
#endif

namespace cuda::des
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

		GPU_API void EncryptInPlace(std::string& input) const;

		GPU_API void DecryptInPlace(std::string& input) const;

		GPU_API std::string Encrypt(const std::string& input) const override;

		GPU_API std::string Decrypt(const std::string& input) const override;

	private:
		unsigned int m_rounds;

		unsigned char* m_subkeys;
	};
}