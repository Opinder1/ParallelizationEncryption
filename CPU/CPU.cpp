#include "../Shared/Shared.h"

#include <gtest/gtest.h>

#include "MbedTest.h"
#include "DESv1.h"
#include "DESv2.h"
#include "AESv1.h"
#include "AESv2.h"
#include "AESv3.h"
#include "../GPU/AES.h"
#include "../GPU/AEScl.h"
#include "../GPU/DES.h"

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
		T algorithmn(key);

		std::string ciphertext = algorithmn.Encrypt(input);
		std::string plaintext = algorithmn.Decrypt(ciphertext);

		EXPECT_STREQ(input.c_str(), plaintext.c_str());

		TestEncrypt(algorithmn, T::k_block_size);

		TestDecrypt(algorithmn, T::k_block_size);
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

template<class Main, class... Others>
void TestVersions()
{
	const std::string key(Main::k_min_key_size, 'A');

	const std::string input(64, 'A');

	const Main main(key);

	std::string main_output = main.Encrypt(input);

	PrintHexStr("main", main_output);

	auto test = [&main_output, &input](const EncryptBase& alg)
	{
		std::string other_output = alg.Encrypt(input);

		PrintHexStr("other", other_output);

		EXPECT_EQ(main_output.c_str(), main_output.c_str());
	};

	(test(Others(key)), ...);
}

void TimeEncrypt(const EncryptBase& des, const std::string& in, std::string out)
{
	out = des.Encrypt(in);
	out = des.Decrypt(out);
}

int main()
{
	TestAESCL();
	return 0;

	//EXPECT_EQ(mbed::DES::Test(), 0);
	//EXPECT_EQ(mbed::AES::Test(), 0);

	TestCrypt<mbed::DES>();
	TestParallelCrypt<mbed::DESParallel>();

	TestCrypt<mbed::TripleDES>();
	TestParallelCrypt<mbed::TripleDESParallel>();

	TestCrypt<mbed::AES>();
	TestParallelCrypt<mbed::AESParallel>();

	TestCrypt<des::v2::DES>();
	TestParallelCrypt<des::v2::DESParallel>();

	TestCrypt<aes::v3::AES>();
	TestParallelCrypt<aes::v3::AESParallel>();

	TestParallelCrypt<aes::cuda::AES>();

	TestParallelCrypt<des::cuda::DES>();

	TestVersions<aes::v1::AES, aes::v2::AES, aes::v3::AES, aes::cuda::AES>();

	TestVersions<des::v1::DES, des::v2::DES, des::cuda::DES>();

	const std::string in(1024 * 1024 * 20, 'A');
	std::string out;

	{
		mbed::DESParallel des(BinToStr("1010101010111011000010010001100000100111001101101100110011011101"), 1);

		printf("Time DESParallel (mbed): %f\n", TimeFunc(10, TimeEncrypt, des, in, out));
	}

	{
		mbed::TripleDESParallel des(BinToStr("101010101011101100001001000110000010011100110110110011001101110110101010101110110000100100011000001001110011011011001100110111011010101010111011000010010001100000100111001101101100110011011101"), 1);

		printf("Time TripleDESParallel (mbed): %f\n", TimeFunc(10, TimeEncrypt, des, in, out));
	}

	{
		mbed::AESParallel aes(BinToStr("10101010101110110000100100011000001001110011011011001100110111011010101010111011000010010001100000100111001101101100110011011101"));

		printf("Time AESParallel (mbed): %f\n", TimeFunc(10, TimeEncrypt, aes, in, out));
	}

	{
		aes::v1::AESParallel aes(BinToStr("10101010101110110000100100011000001001110011011011001100110111011010101010111011000010010001100000100111001101101100110011011101"));

		printf("Time AESParallel (v1): %f\n", TimeFunc(10, TimeEncrypt, aes, in, out));
	}

	{
		aes::v2::AESParallel aes(BinToStr("10101010101110110000100100011000001001110011011011001100110111011010101010111011000010010001100000100111001101101100110011011101"));

		printf("Time AESParallel (v2): %f\n", TimeFunc(10, TimeEncrypt, aes, in, out));
	}

	{
		aes::v3::AESParallel aes(BinToStr("10101010101110110000100100011000001001110011011011001100110111011010101010111011000010010001100000100111001101101100110011011101"));

		printf("Time AESParallel (v3): %f\n", TimeFunc(10, TimeEncrypt, aes, in, out));
	}

	{
		aes::cuda::AES aes(BinToStr("10101010101110110000100100011000001001110011011011001100110111011010101010111011000010010001100000100111001101101100110011011101"));

		printf("Time AESParallel (gpu): %f\n", TimeFunc(10, TimeEncrypt, aes, in, out));
	}
}