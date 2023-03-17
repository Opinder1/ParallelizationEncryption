#pragma once

#include "../Shared/Base.h"

#include "Api.h"

#include <bitset>
#include <string>

namespace aes::v3
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

		CPU_API AES(const std::string& key);

		CPU_API ~AES();

		CPU_API void EncryptInPlace(std::string& input) const override;

		CPU_API void DecryptInPlace(std::string& input) const override;

	protected:
		void EncryptBlockECB(unsigned char block[16]) const;

		void DecryptBlockECB(unsigned char block[16]) const;

		void EncryptBlockCTR(unsigned char block[16], size_t index) const;

		void DecryptBlockCTR(unsigned char block[16], size_t index) const;

	private:
		size_t m_key_bits;
		size_t m_rounds;

		unsigned char m_subkeys[15 * 4 * 4] = {};
	};

	class AESParallel : public AES
	{
	public:
		CPU_API AESParallel(const std::string& key, size_t group_size = 1);

		CPU_API void EncryptInPlace(std::string& input) const override;

		CPU_API void DecryptInPlace(std::string& input) const override;
	};
}