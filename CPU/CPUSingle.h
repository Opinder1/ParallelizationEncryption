#pragma once

#include "Base.h"

#include <bitset>
#include <string>

namespace des::v1
{
	class DES : public EncryptBase
	{
	public:
		struct RoundKey
		{
			std::bitset<48> bytes;
		};

	public:
		DES(const std::string& key);

		~DES();

		std::string Encrypt(const std::string& input) const override;

		std::string Decrypt(const std::string& input) const override;

	private:
		std::string Crypt(const std::string& input, const DES::RoundKey keys[16]) const;

	protected:
		std::bitset<64> CryptBlock(std::bitset<64> block, const DES::RoundKey keys[16]) const;

	protected:
		RoundKey m_enc_keys[16];
		RoundKey m_dec_keys[16];
	};

	class DESParallel : public DES
	{
	public:
		DESParallel(const std::string& key);

		std::string Encrypt(const std::string& input) const override;

		std::string Decrypt(const std::string& input) const override;

	private:
		std::string Crypt(const std::string& input, const DES::RoundKey keys[16]) const;
	};
}