#pragma once

#include "../Shared/Base.h"

#include "Api.h"

#include <bitset>
#include <string>

namespace aes::v1
{
	class AES : public EncryptBase
	{
	public:
		static constexpr size_t k_block_size = 16;
		static constexpr size_t k_min_key_size = 16;

		struct Exception {};

		struct RoundKey
		{
			std::bitset<128> bytes;
		};

		struct State
		{
			unsigned char bytes[4 * 4];
		};

		CPU_API AES(const std::string& key);

		CPU_API ~AES();

		CPU_API std::string GetName() const override;

		CPU_API void EncryptInPlace(std::string& input) const override;

		CPU_API void DecryptInPlace(std::string& input) const override;

	protected:
		void EncryptBlock(char block[16]) const;

		void DecryptBlock(char block[16]) const;

	private:
		void AddRoundKey(State& state, size_t index) const;

		void SubBytes(State& state) const;

		void ShiftRows(State& state) const;

		void MixColumns(State& state) const;

		void InvSubBytes(State& state) const;

		void InvShiftRows(State& state) const;

		void InvMixColumns(State& state) const;

	private:
		size_t m_key_bits;
		size_t m_rounds;

		RoundKey m_subkeys[15];
	};

	class AESParallel : public AES
	{
	public:
		CPU_API AESParallel(const std::string& key, size_t group_size = 1);

		CPU_API std::string GetName() const override;

		CPU_API void EncryptInPlace(std::string& input) const override;

		CPU_API void DecryptInPlace(std::string& input) const override;
	};
}