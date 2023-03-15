#pragma once

#include "../Shared/Base.h"

#include <string>

namespace des::v3
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

		DES(const std::string& key, size_t group_size = 1);

		void EncryptInPlace(std::string& input) const override;

		void DecryptInPlace(std::string& input) const override;

	protected:
		unsigned char m_enc_subkeys[96] = { 0 };
		unsigned char m_dec_subkeys[96] = { 0 };
	};

	class DESParallel : public DES
	{
	public:
		DESParallel(const std::string& key, size_t group_size = 1);

		void EncryptInPlace(std::string& input) const override;

		void DecryptInPlace(std::string& input) const override;
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

		TripleDES(const std::string& key, size_t group_size = 1);

		void EncryptInPlace(std::string& input) const override;

		void DecryptInPlace(std::string& input) const override;

	protected:
		unsigned char m_enc_subkeys[3][96] = { 0 };
		unsigned char m_dec_subkeys[3][96] = { 0 };
	};

	class TripleDESParallel : public TripleDES
	{
	public:
		TripleDESParallel(const std::string& key, size_t group_size = 1);

		void EncryptInPlace(std::string& input) const override;

		void DecryptInPlace(std::string& input) const override;
	};
}