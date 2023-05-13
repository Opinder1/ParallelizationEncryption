#pragma once

#include "../CPU/AESv3.h"
#include "../CPU/DESv3.h"
#include "AEScl.h"
#include "DEScl.h"

#include "Api.h"

namespace aes::dynamic
{
	class DynamicTripleDES : public EncryptBase
	{
	public:
		static constexpr size_t k_block_size = 16;
		static constexpr size_t k_min_key_size = 16;

		struct Exception {};

		GPU_API DynamicTripleDES(const std::string& key);

		GPU_API ~DynamicTripleDES();

		GPU_API std::string GetName() const override;

		GPU_API void EncryptInPlace(std::string& input) const override;

		GPU_API void DecryptInPlace(std::string& input) const override;

	private:
		des::v3::TripleDESParallel m_des_thread;
		opencl::des::TripleDES m_des_gpu;
	};

	class DynamicAES : public EncryptBase
	{
	public:
		static constexpr size_t k_block_size = 16;
		static constexpr size_t k_min_key_size = 16;

		struct Exception {};

		GPU_API DynamicAES(const std::string& key);

		GPU_API ~DynamicAES();

		GPU_API std::string GetName() const override;

		GPU_API void EncryptInPlace(std::string& input) const override;

		GPU_API void DecryptInPlace(std::string& input) const override;

	private:
		aes::v3::AES m_aes_cpu;
		aes::v3::AESParallel m_aes_thread;
		opencl::aes::AES m_aes_gpu;
	};
}