#include "../Shared/Shared.h"

#include <gtest/gtest.h>

#include "MbedTest.h"
#include "DESv1.h"
#include "AESv1.h"
#include "AESv2.h"

void TestEncrypt(const EncryptBase& base, size_t block_size)
{
	EXPECT_ANY_THROW(base.Encrypt(""));

	EXPECT_ANY_THROW(base.Encrypt("A"));

	EXPECT_ANY_THROW(base.Encrypt(std::string(block_size + 1, 'a')));

	EXPECT_NO_THROW(base.Encrypt(std::string(block_size, 'a')));

	EXPECT_NO_THROW(base.Encrypt(std::string(block_size * 3, 'a')));
}

void TestDecrypt(const EncryptBase& base, size_t block_size)
{
	EXPECT_ANY_THROW(base.Decrypt(""));

	EXPECT_ANY_THROW(base.Decrypt("A"));

	EXPECT_ANY_THROW(base.Decrypt(std::string(block_size + 1, 'a')));

	EXPECT_NO_THROW(base.Decrypt(std::string(block_size, 'a')));

	EXPECT_NO_THROW(base.Decrypt(std::string(block_size * 3, 'a')));
}

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
		T des(key);

		std::string ciphertext = des.Encrypt(input);
		std::string plaintext = des.Decrypt(ciphertext);

		EXPECT_STREQ(input.c_str(), plaintext.c_str());

		TestEncrypt(des, T::k_block_size);

		TestDecrypt(des, T::k_block_size);
	}
}

template<class T>
void TestParallelCrypt()
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

void TimeEncrypt(const EncryptBase& des, const std::string& in, std::string out)
{
	out = des.Encrypt(in);
	out = des.Decrypt(out);
}

int main()
{
	//EXPECT_EQ(mbed::DES::Test(), 0);
	//EXPECT_EQ(mbed::AES::Test(), 0);

	TestCrypt<mbed::DES>();
	TestParallelCrypt<mbed::DESParallel>();

	TestCrypt<mbed::TripleDES>();
	TestParallelCrypt<mbed::TripleDESParallel>();

	TestCrypt<mbed::AES>();
	TestParallelCrypt<mbed::AESParallel>();

	TestCrypt<des::v1::DES>();
	TestParallelCrypt<des::v1::DESParallel>();

	TestCrypt<aes::v1::AES>();
	TestParallelCrypt<aes::v1::AESParallel>();

	TestCrypt<aes::v2::AES>();
	TestParallelCrypt<aes::v2::AESParallel>();

	{
		mbed::DES des(BinToStr("1010101010111011000010010001100000100111001101101100110011011101"));
		std::string in(1024 * 1024, 'A');
		std::string out;

		printf("Time DES (mbed): %f\n", TimeFunc(10, TimeEncrypt, des, in, out));
	}

	{
		mbed::DESParallel des(BinToStr("1010101010111011000010010001100000100111001101101100110011011101"), 1);
		std::string in(1024 * 1024, 'A');
		std::string out;

		printf("Time DESParallel (mbed): %f\n", TimeFunc(10, TimeEncrypt, des, in, out));
	}

	{
		mbed::TripleDES des(BinToStr("101010101011101100001001000110000010011100110110110011001101110110101010101110110000100100011000001001110011011011001100110111011010101010111011000010010001100000100111001101101100110011011101"));
		std::string in(1024 * 1024, 'A');
		std::string out;

		printf("Time TripleDES (mbed): %f\n", TimeFunc(10, TimeEncrypt, des, in, out));
	}

	{
		mbed::TripleDESParallel des(BinToStr("101010101011101100001001000110000010011100110110110011001101110110101010101110110000100100011000001001110011011011001100110111011010101010111011000010010001100000100111001101101100110011011101"), 1);
		std::string in(1024 * 1024, 'A');
		std::string out;

		printf("Time TripleDESParallel (mbed): %f\n", TimeFunc(10, TimeEncrypt, des, in, out));
	}

	{
		mbed::AES aes(BinToStr("10101010101110110000100100011000001001110011011011001100110111011010101010111011000010010001100000100111001101101100110011011101"));
		std::string in(1024 * 1024, 'A');
		std::string out;

		printf("Time AES (mbed): %f\n", TimeFunc(10, TimeEncrypt, aes, in, out));
	}

	{
		mbed::AESParallel aes(BinToStr("10101010101110110000100100011000001001110011011011001100110111011010101010111011000010010001100000100111001101101100110011011101"));
		std::string in(1024 * 1024, 'A');
		std::string out;

		printf("Time AESParallel (mbed): %f\n", TimeFunc(10, TimeEncrypt, aes, in, out));
	}
	
	{
		aes::v2::AES aes(BinToStr("10101010101110110000100100011000001001110011011011001100110111011010101010111011000010010001100000100111001101101100110011011101"));
		std::string in(1024 * 1024, 'A');
		std::string out;

		printf("Time AES (v2): %f\n", TimeFunc(10, TimeEncrypt, aes, in, out));
	}

	{
		aes::v2::AESParallel aes(BinToStr("10101010101110110000100100011000001001110011011011001100110111011010101010111011000010010001100000100111001101101100110011011101"));
		std::string in(1024 * 1024, 'A');
		std::string out;

		printf("Time AESParallel (v2): %f\n", TimeFunc(10, TimeEncrypt, aes, in, out));
	}
}