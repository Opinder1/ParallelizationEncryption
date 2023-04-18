#include "../Shared/Shared.h"

#include <gtest/gtest.h>

#include "../CPU/MbedTest.h"
#include "../CPU/DESv2.h"
#include "../CPU/DESv3.h"
#include "../CPU/AESv1.h"
#include "../CPU/AESv2.h"
#include "../CPU/AESv3.h"
#include "../GPU/AES.h"
#include "../GPU/AEScl.h"
#include "../GPU/DES.h"
#include "../GPU/DEScl.h"

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

	printf("%s passed\n", typeid(T).name());
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

	printf("%s passed\n", typeid(T).name());
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

template<class F, class... Args>
inline float TimeFunc(size_t num, const F& function, Args&... args)
{
	auto t1 = std::chrono::steady_clock::now();

	for (size_t i = 0; i < num; i++)
	{
		function(args...);
	}

	auto t2 = std::chrono::steady_clock::now();

	auto time_nano = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1);
	auto time_sec = time_nano.count() / 1000000000.0f;

	return time_sec;
}

void TimeEncrypt(const EncryptBase& alg, const std::string& in, std::string& out)
{
	out = alg.Encrypt(in);
	out = alg.Decrypt(out);
};

void RunAllTests()
{
	TestCrypt<mbed::DES>();
	TestCrypt<mbed::TripleDES>();
	TestCrypt<mbed::AES>();
	TestCrypt<des::v2::DES>();
	TestCrypt<des::v3::DES>();
	TestCrypt<aes::v1::AES>();
	TestCrypt<aes::v2::AES>();
	TestCrypt<aes::v3::AES>();

	TestParallelCrypt<mbed::DESParallel>();
	TestParallelCrypt<mbed::TripleDESParallel>();
	TestParallelCrypt<mbed::AESParallel>();
	TestParallelCrypt<des::v2::DESParallel>();
	TestParallelCrypt<des::v3::DESParallel>();
	TestParallelCrypt<aes::v1::AESParallel>();
	TestParallelCrypt<aes::v2::AESParallel>();
	TestParallelCrypt<aes::v3::AESParallel>();

	TestParallelCrypt<cuda::aes::AES>();
	TestParallelCrypt<cuda::des::DES>();
	TestParallelCrypt<opencl::aes::AES>();
	TestParallelCrypt<opencl::des::DES>();

	TestVersions<des::v3::DES, des::v3::DESParallel, cuda::des::DES, opencl::des::DES>();
	TestVersions<des::v3::TripleDES, des::v3::TripleDESParallel, cuda::des::TripleDES, opencl::des::TripleDES>();

	TestVersions<aes::v3::AES, cuda::aes::AES, opencl::aes::AES>();
}

void TimeAndGraph(const char* name, const std::vector<EncryptBase*> algorithms)
{
	printf(Format("TimeAndGraph %s\n", name).c_str());

	std::string command;
	command.append("python -c \"");
	command.append("from matplotlib import pyplot;");

	for (auto algorithm : algorithms)
	{
		printf("%s\n", typeid(*algorithm).name());

		std::string x = "[";
		std::string y = "[";

		for (size_t i = 0; i < 20; i++)
		{
			size_t data_size = 16 * (1 << i);

			const std::string in(data_size, 'A');
			std::string out;

			TimeFunc(1, TimeEncrypt, *algorithm, in, out);

			float t = TimeFunc(1, TimeEncrypt, *algorithm, in, out) / 10.0f;

			x.append(std::to_string(t));
			x.append(",");

			y.append(std::to_string(data_size));
			y.append(",");
		}

		x.append("],");
		y.append("],");

		command.append("pyplot.plot(");
		command.append(y);
		command.append(x);
		command.append("label='");
		command.append(typeid(*algorithm).name());
		command.append("');");
	}

	command.append("pyplot.legend();");
	command.append(Format("pyplot.title('%s algorithm speed based on data size input');", name));
	command.append("pyplot.xlabel('data size (bytes)');");
	command.append("pyplot.ylabel('time (s)');");
	command.append("pyplot.xscale('log');");
	command.append("pyplot.yscale('log');");
	//command.append("pyplot.xlim(16, 1024*1024*4);");
	//command.append("pyplot.ylim(0.00001, 1);");
	command.append("pyplot.show();\"");

	// Easy way of using matplotlib with c++ for testing purposes
	system(command.c_str());
}

void RunAllAnalysis()
{
	const std::string key1 = BinToStr("101010101011101100001001000110000010011100110110110011001101110110101010101110110000100100011000001001110011011011001100110111011010101010111011000010010001100000100111001101101100110011011101");

	des::v3::TripleDES des1(key1);
	des::v3::TripleDESParallel des2(key1, 1);
	cuda::des::TripleDES des3(key1, 1);
	opencl::des::TripleDES des4(key1, 1);

	TimeAndGraph("DES", { &des1, &des2, &des3, &des4 });

	const std::string key2 = BinToStr("10101010101110110000100100011000001001110011011011001100110111011010101010111011000010010001100000100111001101101100110011011010");

	aes::v3::AES aes1(key2);
	aes::v3::AESParallel aes2(key2, 1);
	cuda::aes::AES aes3(key2, 1);
	opencl::aes::AES aes4(key2, 1);

	TimeAndGraph("AES", { &aes1, &aes2, &aes3, &aes4 });
}

int main()
{
	opencl::InitOpenCL();

	RunAllTests();

	RunAllAnalysis();

	opencl::ShutdownOpenCL();
}