#pragma once

#include "../Shared/Base.h"

#include <gtest/gtest.h>

void TestEncrypt(const EncryptBase& base, size_t block_size);

void TestDecrypt(const EncryptBase& base, size_t block_size);

template<class T>
void TestCrypt()
{
	// Test keys
	{
		EXPECT_THROW(T(""), typename T::Exception);

		EXPECT_THROW(T("A"), typename T::Exception);

		EXPECT_NO_THROW(T(std::string(T::k_min_key_size, 'A')));

		EXPECT_THROW(T(std::string(T::k_min_key_size + 1, 'A')), typename T::Exception);
	}

	const std::string key = std::string(T::k_min_key_size, 'A');
	const std::string input(64, 'A');

	{
		T algorithmn(key);

		std::string ciphertext = algorithmn.Encrypt(input);
		std::string plaintext = algorithmn.Decrypt(ciphertext);

		EXPECT_STREQ(input.c_str(), plaintext.c_str());

		TestEncrypt(algorithmn, T::k_block_size);

		TestDecrypt(algorithmn, T::k_block_size);
	}

	printf("%s passed\n", typeid(T).name());
}

template<class T>
void TestCryptGroups()
{
	const std::string key = std::string(T::k_min_key_size, 'A');

	// Test group size
	{
		EXPECT_THROW(T(key, 0), typename T::Exception);

		EXPECT_NO_THROW(T(key, 1));

		EXPECT_NO_THROW(T(key, 17));

		EXPECT_THROW(T(key, SIZE_MAX), typename T::Exception);
	}

	TestCrypt<T>();
}

template<class T>
void TestCryptGroupsThreads()
{
	const std::string key = std::string(T::k_min_key_size, 'A');

	// Test thread_count
	{
		EXPECT_NO_THROW(T(key, 1, 0), typename T::Exception);

		EXPECT_NO_THROW(T(key, 1, 1));

		EXPECT_NO_THROW(T(key, 1, 17));

		EXPECT_NO_THROW(T(key, 1, SIZE_MAX), typename T::Exception);
	}

	TestCryptGroups<T>();
}

template<class Main, class... Others>
void TestVersions()
{
	const std::string key(Main::k_min_key_size, 'B');

	const std::string input(16, 'A');

	const Main main(key);

	std::string main_enc = main.Encrypt(input);

	std::string main_dec = main.Decrypt(main_enc);

	auto test = [&main_enc, &main_dec, &input](const EncryptBase& alg)
	{
		std::string other_enc = alg.Encrypt(input);

		EXPECT_EQ(main_enc, other_enc);

		std::string other_dec = alg.Decrypt(other_enc);

		EXPECT_EQ(main_dec, other_dec);
	};

	(test(Others(key)), ...);

	std::string names;
	(names.append(typeid(Others).name()), ...);

	printf("[%s, %s] passed\n", typeid(Main).name(), names.c_str());
}