#pragma once

#include "../Shared/Base.h"

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

		AES(const std::string& key);

		~AES();

		std::string Encrypt(const std::string& input) const override;

		std::string Decrypt(const std::string& input) const override;

	protected:
		void EncryptBlock(unsigned char block[16]) const;

		void DecryptBlock(unsigned char block[16]) const;

	private:
		size_t m_key_bits;
		size_t m_rounds;

		unsigned char m_subkeys[15 * 4 * 4] = {};
	};

	class AESParallel : public AES
	{
	public:
		AESParallel(const std::string& key, size_t group_size = 1);

		std::string Encrypt(const std::string& input) const override;

		std::string Decrypt(const std::string& input) const override;
	};
}