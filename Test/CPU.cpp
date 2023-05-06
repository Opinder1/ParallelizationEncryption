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

#include "ErrorTest.h"
#include "TimeTest.h"

#include "../Shared/Shared.h"

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

	TestCryptGroups<mbed::DESParallel>();
	TestCryptGroups<mbed::TripleDESParallel>();
	TestCryptGroups<mbed::AESParallel>();
	TestCryptGroups<des::v2::DESParallel>();
	TestCryptGroupsThreads<des::v3::DESParallel>();
	TestCryptGroups<aes::v1::AESParallel>();
	TestCryptGroups<aes::v2::AESParallel>();
	TestCryptGroupsThreads<aes::v3::AESParallel>();

	TestCrypt<cuda::aes::AES>();
	TestCrypt<cuda::des::TripleDES>();

	TestCrypt<opencl::aes::AES>();
	TestCrypt<opencl::des::TripleDES>();

	TestVersions<des::v3::DES, des::v3::DESParallel>();
	TestVersions<des::v3::TripleDES, des::v3::TripleDESParallel, cuda::des::TripleDES, opencl::des::TripleDES>();

	TestVersions<aes::v3::AES, cuda::aes::AES, opencl::aes::AES>();
}

struct DataSizeSpeedsTest : TimeTest
{
	DataSizeSpeedsTest(size_t iterations, bool make_log_graph) :
		m_iterations(iterations),
		m_make_log_graph(make_log_graph)
	{}

	void ApplyAlgorithm(const EncryptBase& algorithm, std::vector<double>& x, std::vector<double>& y) override
	{
		for (size_t i = 0; i < m_iterations; i++)
		{
			size_t data_size = 16 * size_t(pow(i, 1.8) + 1);

			const std::string in(data_size, 'A');
			std::string out;

			double t = 1000.0f;
			
			for (size_t i = 0; i < 5; i++)
			{
				t = fmin(t, TimeEncrypt(1, algorithm, in, out));
			}

			printf("%zi ", i);

			x.push_back(double(data_size));

			y.push_back(t);
		}
		printf("\n");
	}

	virtual const char* GetXAxisName() override { return "data size(bytes)"; }

	virtual const char* GetYAxisName() override { return "time (s)"; }

	virtual bool ShouldMakeLogGraph() override { return m_make_log_graph; }

	virtual bool ShouldMakeXLog() override { return false; }

	size_t m_iterations;
	bool m_make_log_graph;
};

struct BytesPerSecondTest : TimeTest
{
	BytesPerSecondTest()
	{}

	void ApplyAlgorithm(const EncryptBase& algorithm, std::vector<double>& x, std::vector<double>& y)
	{
		for (size_t i = 0; i < 100; i++)
		{
			size_t data_size = 16 * size_t(pow(i, 1.8) + 1);

			const std::string in(data_size, 'A');
			std::string out;

			double t = 1000.0f;
			
			for (size_t i = 0; i < 5; i++)
			{
				t = fmin(t, TimeEncrypt(1, algorithm, in, out));
			}

			printf("%zi ", data_size);

			x.push_back(double(data_size));

			y.push_back(double(data_size / t));
		}
		printf("\n");
	}

	virtual const char* GetXAxisName() override { return "data size(bytes)"; }

	virtual const char* GetYAxisName() override { return "bytes per second(bytes/s)"; }

	virtual bool ShouldMakeLogGraph() override { return false; }

	virtual bool ShouldMakeXLog() override { return true; }
};

void RunAllAnalysis()
{
	const std::string deskey = BinToStr("101010101011101100001001000110000010011100110110110011001101110110101010101110110000100100011000001001110011011011001100110111011010101010111011000010010001100000100111001101101100110011011101");
	const std::string aeskey = BinToStr("10101010101110110000100100011000001001110011011011001100110111011010101010111011000010010001100000100111001101101100110011011010");

	{
		des::v3::TripleDES des1(deskey);
		des::v3::TripleDESParallel des2(deskey, 1, 12);
		cuda::des::TripleDES des3(deskey);
		opencl::des::TripleDES des4(deskey);

		aes::v3::AES aes1(aeskey);
		aes::v3::AESParallel aes2(aeskey, 1, 12);
		cuda::aes::AES aes3(aeskey);
		opencl::aes::AES aes4(aeskey);

		// Tests comparing different versions of Triple DES and AES
		TimeAndGraph("TDES", "Time to encrypt different amounts of data\\n with different parallelized versions of triple DES", { &des1, &des2, &des3, &des4 }, DataSizeSpeedsTest(300, true));

		TimeAndGraph("AES", "Time to encrypt different amounts of data\\n with different parallelized versions of AES", { &aes1, &aes2, &aes3, &aes4 }, DataSizeSpeedsTest(300, true));

		TimeAndGraph("TDES_vs_AES", "Comparing the time to encrypt different\\n amounts of data with GPU versions of triple DES and AES", { &des3, &des4, &aes3, &aes4 }, DataSizeSpeedsTest(300, true));

		// Testing the bytes per second of the GPU versions. This shows to what extent the Memory transfer is a bottleneck
		TimeAndGraph("TDES_BytesPerSecond", "The amount of bytes per second being\\n encrypted with different sizes of data for triple DES", { &des3, &des4 }, BytesPerSecondTest());

		TimeAndGraph("AES_BytesPerSecond", "The amount of bytes per second being\\n encrypted with different sizes of data for AES", { &aes3, &aes4 }, BytesPerSecondTest());
	}

	// Testing encrypting blocks in groups
	{
		des::v3::TripleDESParallel des1(deskey, 1);
		des::v3::TripleDESParallel des2(deskey, 4);
		des::v3::TripleDESParallel des3(deskey, 16);
		des::v3::TripleDESParallel des4(deskey, 64);

		TimeAndGraph("TDES_GroupSize", "Encrypting data blocks in different\\n sized groups for triple DES", {&des1, &des2, &des3, &des4}, DataSizeSpeedsTest(300, false));
	}

	{
		aes::v3::AESParallel aes1(aeskey, 1);
		aes::v3::AESParallel aes2(aeskey, 4);
		aes::v3::AESParallel aes3(aeskey, 16);
		aes::v3::AESParallel aes4(aeskey, 64);

		TimeAndGraph("AES_GroupSize", "Encrypting data blocks in different\\n sized groups for AES", {&aes1, &aes2, &aes3, &aes4}, DataSizeSpeedsTest(300, false));
	}

	// Testing encrypting with different CPU thread counts.
	{
		des::v3::TripleDESParallel des1(deskey, 1, 1);
		des::v3::TripleDESParallel des2(deskey, 1, 2);
		des::v3::TripleDESParallel des3(deskey, 1, 6);
		des::v3::TripleDESParallel des4(deskey, 1, 12);

		TimeAndGraph("TDES_ThreadCount", "Encrypting data with different numbers\\n of CPU cores for triple DES", { &des1, &des2, &des3, &des4 }, DataSizeSpeedsTest(300, false));
	}

	{
		aes::v3::AESParallel aes1(aeskey, 1, 1);
		aes::v3::AESParallel aes2(aeskey, 1, 2);
		aes::v3::AESParallel aes3(aeskey, 1, 6);
		aes::v3::AESParallel aes4(aeskey, 1, 12);

		TimeAndGraph("AES_ThreadCount", "Encrypting data with different numbers\\n of CPU cores for AES", { &aes1, &aes2, &aes3, &aes4 }, DataSizeSpeedsTest(300, false));
	}
}

int main()
{
	opencl::InitOpenCL();

	RunAllTests();

	RunAllAnalysis();

	opencl::ShutdownOpenCL();
}