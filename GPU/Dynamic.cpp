#include "Dynamic.h"

namespace aes::dynamic
{
	DynamicTripleDES::DynamicTripleDES(const std::string& key) :
		EncryptBase(key),
		m_des_thread(key),
		m_des_gpu(key)
	{}

	DynamicTripleDES::~DynamicTripleDES()
	{}

	std::string DynamicTripleDES::GetName() const
	{
		char buffer[24];
		sprintf_s(buffer, "Triple DES Dynamic");
		return buffer;
	}

	void DynamicTripleDES::EncryptInPlace(std::string& input) const
	{
		if (input.size() == 0 || input.size() % k_block_size != 0)
		{
			throw Exception{};
		}

		if (input.size() > 2048)
		{
			m_des_gpu.EncryptInPlace(input);
		}
		else
		{
			m_des_thread.EncryptInPlace(input);
		}
	}

	void DynamicTripleDES::DecryptInPlace(std::string& input) const
	{
		if (input.size() == 0 || input.size() % k_block_size != 0)
		{
			throw Exception{};
		}

		if (input.size() > 2048)
		{
			m_des_gpu.DecryptInPlace(input);
		}
		else
		{
			m_des_thread.DecryptInPlace(input);
		}
	}

	DynamicAES::DynamicAES(const std::string& key) :
		EncryptBase(key),
		m_aes_cpu(key),
		m_aes_thread(key),
		m_aes_gpu(key)
	{}

	DynamicAES::~DynamicAES()
	{}

	std::string DynamicAES::GetName() const
	{
		char buffer[16];
		sprintf_s(buffer, "AES Dynamic");
		return buffer;
	}

	void DynamicAES::EncryptInPlace(std::string& input) const
	{
		if (input.size() == 0 || input.size() % k_block_size != 0)
		{
			throw Exception{};
		}

		if (input.size() > 16384)
		{
			m_aes_gpu.EncryptInPlace(input);
		}
		else if (input.size() > 128)
		{
			m_aes_thread.EncryptInPlace(input);
		}
		else
		{
			m_aes_cpu.EncryptInPlace(input);
		}
	}

	void DynamicAES::DecryptInPlace(std::string& input) const
	{
		if (input.size() == 0 || input.size() % k_block_size != 0)
		{
			throw Exception{};
		}

		if (input.size() > 16384)
		{
			m_aes_gpu.DecryptInPlace(input);
		}
		else if (input.size() > 128)
		{
			m_aes_thread.DecryptInPlace(input);
		}
		else
		{
			m_aes_cpu.DecryptInPlace(input);
		}
	}
}