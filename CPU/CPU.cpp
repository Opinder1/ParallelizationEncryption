#include "../Shared/Shared.h"

#include <gtest/gtest.h>

#include "MbedTest.h"
#include "DESv1.h"
#include "DESv2.h"
#include "DESv3.h"
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
	const std::string key(Main::k_min_key_size, 'B');

	const std::string input(64, 'A');

	const Main main(key);

	std::string main_enc = main.Encrypt(input);
	
	std::string main_dec = main.Decrypt(main_enc);

	auto test = [&main_enc, &main_dec, &input](const EncryptBase& alg)
	{
		std::string other_enc = alg.Encrypt(input);

		EXPECT_STREQ(main_enc.c_str(), other_enc.c_str());

		std::string other_dec = alg.Decrypt(other_enc);

		EXPECT_STREQ(main_dec.c_str(), other_dec.c_str());
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
	opencl::InitOpenCL();

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

	TestCrypt<des::v2::DES>();
	TestParallelCrypt<des::v2::DESParallel>();

	TestCrypt<des::v3::DES>();
	TestParallelCrypt<des::v3::DESParallel>();

	TestCrypt<aes::v3::AES>();
	TestParallelCrypt<aes::v3::AESParallel>();

	TestParallelCrypt<cuda::aes::AES>();

	TestParallelCrypt<cuda::des::DES>();

	TestParallelCrypt<opencl::aes::AES>();

	//TestParallelCrypt<opencl::des::DES>();

	TestVersions<des::v1::DES, des::v2::DES, des::v3::DES, cuda::des::DES>();

	TestVersions<aes::v1::AES, aes::v2::AES, aes::v3::AES, cuda::aes::AES, opencl::aes::AES>();

	const std::string in(1024 * 1024 * 20, 'A');
	std::string out;

	{
		des::v1::DESParallel des(BinToStr("10101010101110110000100100011000001001110011011011001100110111"), 1);

		printf("Time TripleDESParallel (mbed): %f\n", TimeFunc(1, TimeEncrypt, des, in, out));
	}

	{
		des::v2::DESParallel des(BinToStr("10101010101110110000100100011000001001110011011011001100110111"), 1);

		printf("Time TripleDESParallel (mbed): %f\n", TimeFunc(1, TimeEncrypt, des, in, out));
	}

	{
		des::v3::DESParallel des(BinToStr("10101010101110110000100100011000001001110011011011001100110111"), 1);

		printf("Time TripleDESParallel (mbed): %f\n", TimeFunc(1, TimeEncrypt, des, in, out));
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
		cuda::aes::AES aes(BinToStr("10101010101110110000100100011000001001110011011011001100110111011010101010111011000010010001100000100111001101101100110011011101"));

		printf("Time AESParallel (cuda): %f\n", TimeFunc(10, TimeEncrypt, aes, in, out));
	}

	{
		opencl::aes::AES aes(BinToStr("10101010101110110000100100011000001001110011011011001100110111011010101010111011000010010001100000100111001101101100110011011101"));

		printf("Time AESParallel (opencl): %f\n", TimeFunc(10, TimeEncrypt, aes, in, out));
	}

	opencl::ShutdownOpenCL();
}