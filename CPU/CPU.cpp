#include "../Shared/Shared.h"

#include <gtest/gtest.h>

#include "CPUSingle.h"
#include "MbedTest.h"

void TestDES()
{
	const std::string key = BinToStr("1010101010111011000010010001100000100111001101101100110011011101");
	const std::string input(64, 'A');

	std::string expected;

	{
		mbed::DES des(key);

		std::string ciphertext = des.Encrypt(input);
		std::string plaintext = des.Decrypt(ciphertext);

		expected = ciphertext;

		EXPECT_STREQ(input.c_str(), plaintext.c_str());
	}

	{
		mbed::DESParallel des(key);

		std::string ciphertext = des.Encrypt(input);
		std::string plaintext = des.Decrypt(ciphertext);

		EXPECT_STREQ(input.c_str(), plaintext.c_str());
		EXPECT_STREQ(ciphertext.c_str(), expected.c_str());
	}

	{
		des::v1::DES des(key);

		std::string ciphertext = des.Encrypt(input);
		std::string plaintext = des.Decrypt(ciphertext);

		expected = ciphertext;

		EXPECT_STREQ(input.c_str(), plaintext.c_str());
	}

	{
		des::v1::DESParallel des(key);

		std::string ciphertext = des.Encrypt(input);
		std::string plaintext = des.Decrypt(ciphertext);

		EXPECT_STREQ(input.c_str(), plaintext.c_str());
		EXPECT_STREQ(ciphertext.c_str(), expected.c_str());
	}
}

void TimeEncrypt(const EncryptBase& des, const std::string& in, std::string out)
{
	out = des.Encrypt(in);
	out = des.Decrypt(out);
}

int main()
{
	mbed::DES::Test();

	TestDES();

	{
		mbed::DES des(BinToStr("1010101010111011000010010001100000100111001101101100110011011101"));
		std::string in(1024 * 1024, 'A');
		std::string out;

		printf("Time DES (mbed): %f\n", TimeFunc(10, TimeEncrypt, des, in, out));
	}

	{
		mbed::DESParallel des(BinToStr("1010101010111011000010010001100000100111001101101100110011011101"));
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
		mbed::TripleDESParallel des(BinToStr("101010101011101100001001000110000010011100110110110011001101110110101010101110110000100100011000001001110011011011001100110111011010101010111011000010010001100000100111001101101100110011011101"));
		std::string in(1024 * 1024, 'A');
		std::string out;

		printf("Time TripleDESParallel (mbed): %f\n", TimeFunc(10, TimeEncrypt, des, in, out));
	}
}