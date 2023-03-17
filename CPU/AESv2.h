#pragma once

#include "../Shared/Base.h"

#include "Api.h"

#include <bitset>
#include <string>

namespace aes::v2
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
		void EncryptBlock(unsigned char block[16]) const;

		void DecryptBlock(unsigned char block[16]) const;

	private:
		void AddRoundKey(unsigned char state[16], size_t index) const;

		void SubBytes(unsigned char state[16]) const;

		void ShiftRows(unsigned char state[16]) const;

		void MixColumns(unsigned char state[4][4]) const;

		void InvSubBytes(unsigned char state[16]) const;

		void InvShiftRows(unsigned char state[16]) const;

		void InvMixColumns(unsigned char state[4][4]) const;

	private:
		size_t m_key_bits;
		size_t m_rounds;

		RoundKey m_subkeys[15];
	};

	class AESParallel : public AES
	{
	public:
		CPU_API AESParallel(const std::string& key, size_t group_size = 1);

		CPU_API void EncryptInPlace(std::string& input) const override;

		CPU_API void DecryptInPlace(std::string& input) const override;
	};
}