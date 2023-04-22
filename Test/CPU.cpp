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

	TestParallelCrypt<mbed::DESParallel>();
	TestParallelCrypt<mbed::TripleDESParallel>();
	TestParallelCrypt<mbed::AESParallel>();
	TestParallelCrypt<des::v2::DESParallel>();
	TestParallelCrypt<des::v3::DESParallel>();
	TestParallelCrypt<aes::v1::AESParallel>();
	TestParallelCrypt<aes::v2::AESParallel>();
	TestParallelCrypt<aes::v3::AESParallel>();

	TestParallelCrypt<cuda::aes::AES>();
	TestParallelCrypt<opencl::aes::AES>();

	TestVersions<des::v3::DES, des::v3::DESParallel>();
	TestVersions<des::v3::TripleDES, des::v3::TripleDESParallel, cuda::des::TripleDES, opencl::des::TripleDES>();

	TestVersions<aes::v3::AES, cuda::aes::AES, opencl::aes::AES>();
}

struct DataSizeSpeedsTest : TimeTest
{
	DataSizeSpeedsTest(size_t repeats, size_t iterations, bool make_log_graph) :
		m_repeats(repeats),
		m_iterations(iterations),
		m_make_log_graph(make_log_graph)
	{}

	void ApplyAlgorithm(const EncryptBase& algorithm, std::string& x, std::string& y) override
	{
		for (size_t i = 0; i < m_iterations; i++)
		{
			size_t data_size = 16 * (1 << i);

			const std::string in(data_size, 'A');
			std::string out;

			// Initial run to warm up GPU if being used
			TimeEncrypt(1, algorithm, in, out);

			double t = TimeEncrypt(m_repeats, algorithm, in, out) / float(m_repeats);

			printf("%zi ", i);

			x.append(std::to_string(data_size));
			x.append(",");

			y.append(std::to_string(t));
			y.append(",");
		}
		printf("\n");
	}

	virtual const char* GetXAxisName() override { return "data size(bytes)"; }

	virtual const char* GetYAxisName() override { return "time (s)"; }

	virtual bool ShouldMakeLogGraph() override { return m_make_log_graph; }

	virtual bool ShouldMakeXLog() override { return false; }

	size_t m_iterations;
	size_t m_repeats;
	bool m_make_log_graph;
};

struct BytesPerSecondTest : TimeTest
{
	BytesPerSecondTest(size_t repeats) :
		m_repeats(repeats)
	{}

	void ApplyAlgorithm(const EncryptBase& algorithm, std::string& x, std::string& y)
	{
		for (size_t i = 0; i < 24; i++)
		{
			size_t data_size = 16 * (1 << i);

			const std::string in(data_size, 'A');
			std::string out;

			TimeEncrypt(1, algorithm, in, out);

			double t = TimeEncrypt(m_repeats, algorithm, in, out) / float(m_repeats);

			printf("%zi ", data_size);

			x.append(std::to_string(data_size));
			x.append(",");

			y.append(std::to_string(data_size / t));
			y.append(",");
		}
		printf("\n");
	}

	virtual const char* GetXAxisName() override { return "data size(bytes)"; }

	virtual const char* GetYAxisName() override { return "bytes per second(bytes/s)"; }

	virtual bool ShouldMakeLogGraph() override { return false; }

	virtual bool ShouldMakeXLog() override { return true; }

	size_t m_repeats;
};

void RunAllAnalysis()
{
	const std::string deskey = BinToStr("101010101011101100001001000110000010011100110110110011001101110110101010101110110000100100011000001001110011011011001100110111011010101010111011000010010001100000100111001101101100110011011101");
	const std::string aeskey = BinToStr("10101010101110110000100100011000001001110011011011001100110111011010101010111011000010010001100000100111001101101100110011011010");

	{
		des::v3::TripleDES des1(deskey);
		des::v3::TripleDESParallel des2(deskey, 1);
		cuda::des::TripleDES des3(deskey, 1);
		opencl::des::TripleDES des4(deskey, 1);

		aes::v3::AES aes1(aeskey);
		aes::v3::AESParallel aes2(aeskey, 1);
		cuda::aes::AES aes3(aeskey, 1);
		opencl::aes::AES aes4(aeskey, 1);

		TimeAndGraph("DES", { &des1, &des2, &des3, &des4 }, DataSizeSpeedsTest(10, 20, true));

		TimeAndGraph("AES", { &aes1, &aes2, &aes3, &aes4 }, DataSizeSpeedsTest(10, 20, true));

		TimeAndGraph("DES and AES", { &des3, &des4, &aes3, &aes4 }, DataSizeSpeedsTest(20, 20, true));

		TimeAndGraph("DES overhead", { &des3, &des4 }, BytesPerSecondTest(10));

		TimeAndGraph("AES overhead", { &aes3, &aes4 }, BytesPerSecondTest(10));
	}

	{
		des::v3::TripleDESParallel des1(deskey, 1);
		des::v3::TripleDESParallel des2(deskey, 4);
		des::v3::TripleDESParallel des3(deskey, 16);
		des::v3::TripleDESParallel des4(deskey, 64);

		TimeAndGraph("DES CPU group size", { &des1, &des2, &des3, &des4 }, DataSizeSpeedsTest(10, 18, false));
	}

	{
		cuda::des::TripleDES des1(deskey, 1);
		cuda::des::TripleDES des2(deskey, 4);
		cuda::des::TripleDES des3(deskey, 16);
		cuda::des::TripleDES des4(deskey, 64);

		TimeAndGraph("DES CUDA group size", { &des1, &des2, &des3, &des4 }, DataSizeSpeedsTest(10, 18, false));
	}

	{
		opencl::des::TripleDES des1(deskey, 1);
		opencl::des::TripleDES des2(deskey, 4);
		opencl::des::TripleDES des3(deskey, 16);
		opencl::des::TripleDES des4(deskey, 64);

		TimeAndGraph("DES OpenCL group size", { &des1, &des2, &des3, &des4 }, DataSizeSpeedsTest(10, 18, false));
	}

	{
		des::v3::TripleDESParallel des1(deskey, 1, 1);
		des::v3::TripleDESParallel des2(deskey, 1, 2);
		des::v3::TripleDESParallel des3(deskey, 1, 6);
		des::v3::TripleDESParallel des4(deskey, 1, 12);

		TimeAndGraph("DES CPU group size", { &des1, &des2, &des3, &des4 }, DataSizeSpeedsTest(10, 16, false));
	}
}

int main()
{
	opencl::InitOpenCL();

	RunAllTests();

	RunAllAnalysis();

	opencl::ShutdownOpenCL();
}