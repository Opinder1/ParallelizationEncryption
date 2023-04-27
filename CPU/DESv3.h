#pragma once

#include "../Shared/Base.h"

#include "Api.h"

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

		CPU_API DES(const std::string& key);

		CPU_API std::string GetName() const override;

		CPU_API void EncryptInPlace(std::string& input) const override;

		CPU_API void DecryptInPlace(std::string& input) const override;

	private:
		void Crypt(const unsigned char subkeys[96], std::string& input) const;

	protected:
		unsigned char m_enc_subkeys[96] = { 0 };
		unsigned char m_dec_subkeys[96] = { 0 };
	};

	class DESParallel : public DES
	{
	public:
		CPU_API DESParallel(const std::string& key, size_t group_size = 1, size_t thread_count = 1);

		CPU_API std::string GetName() const override;

		CPU_API void EncryptInPlace(std::string& input) const override;

		CPU_API void DecryptInPlace(std::string& input) const override;

	private:
		void Crypt(const unsigned char subkeys[96], std::string& input) const;

	private:
		size_t m_group_size;
		int m_thread_count;
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

		CPU_API TripleDES(const std::string& key);

		CPU_API std::string GetName() const override;

		CPU_API void EncryptInPlace(std::string& input) const override;

		CPU_API void DecryptInPlace(std::string& input) const override;

	private:
		void Crypt(const unsigned char* subkeys[3], std::string& input) const;

	protected:
		unsigned char m_enc_subkeys[3][96] = { 0 };
		unsigned char m_dec_subkeys[3][96] = { 0 };
	};

	class TripleDESParallel : public TripleDES
	{
	public:
		CPU_API TripleDESParallel(const std::string& key, size_t group_size = 1, size_t thread_count = 1);

		CPU_API std::string GetName() const override;

		CPU_API void EncryptInPlace(std::string& input) const override;

		CPU_API void DecryptInPlace(std::string& input) const override;

	private:
		void Crypt(const unsigned char* subkeys[3], std::string& input) const;

	private:
		size_t m_group_size;
		int m_thread_count;
	};
}