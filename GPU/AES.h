#pragma once

#include "../Shared/Base.h"

#include "Api.h"

#include <bitset>
#include <string>

namespace cuda::aes
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

		GPU_API AES(const std::string& key);

		GPU_API ~AES();

		GPU_API std::string GetName() const override;

		GPU_API void EncryptInPlace(std::string& input) const override;

		GPU_API void DecryptInPlace(std::string& input) const override;

	private:
		unsigned int m_rounds;

		unsigned char* m_subkeys;
	};
}