#include "DES.h"

#ifdef __CUDA_ARCH__
#define CONSTANT __constant__
#define DEVICE __device__
#else
#define CONSTANT
#define DEVICE
#endif

#include "cuda_runtime.h"
#include "device_launch_parameters.h"

namespace cuda::des
{
	CONSTANT unsigned char key_perm_l[56] = {
		3, 2, 1, 0, 3, 2, 1, 0,
		3, 2, 1, 0, 7, 6, 5, 4,
		3, 2, 1, 0, 7, 6, 5, 4,
		7, 6, 5, 4, 7, 6, 5, 4,
		7, 6, 5, 4, 3, 2, 1, 0,
		7, 6, 5, 4, 3, 2, 1, 0,
		7, 6, 5, 4, 3, 2, 1, 0,
	};

	CONSTANT unsigned char key_perm_r[56] = {
		8, 8, 8, 8, 16, 16, 16, 16,
		4, 4, 4, 4, 8, 8, 8, 8,
		2, 2, 2, 2, 4, 4, 4, 4,
		16, 16, 16, 16, 2, 2, 2, 2,
		32, 32, 32, 32, 32, 32, 32, 32,
		64, 64, 64, 64, 64, 64, 64, 64,
		128, 128, 128, 128, 128, 128, 128, 128,
	};

	CONSTANT unsigned char left_round_perm_l[24] = {
		3, 0, 1, 0, 3, 2, 1, 0,
		1, 0, 2, 1, 2, 2, 1, 0,
		1, 2, 1, 2, 0, 0, 0, 3,
	};

	CONSTANT unsigned char left_round_perm_r[24] = {
		64, 1, 1, 2, 32, 16, 8, 64,
		2, 4, 8, 64, 2, 32, 16, 16,
		4, 128, 32, 1, 128, 8, 32, 16,
	};

	CONSTANT unsigned char right_round_perm_l[24] = {
		4, 6, 5, 5, 6, 4, 3, 3,
		6, 5, 4, 5, 5, 6, 4, 6,
		5, 6, 3, 4, 5, 6, 3, 4,
	};

	CONSTANT unsigned char right_round_perm_r[24] = {
		64, 8, 4, 64, 64, 16, 8, 1,
		32, 8, 128, 1, 16, 128, 2, 1,
		128, 16, 2, 8, 2, 2, 4, 1,
	};

	CONSTANT unsigned char initial_perm_l[64] = {
		0, 1, 2, 3, 4, 5, 6, 7,
		0, 1, 2, 3, 4, 5, 6, 7,
		0, 1, 2, 3, 4, 5, 6, 7,
		0, 1, 2, 3, 4, 5, 6, 7,
		0, 1, 2, 3, 4, 5, 6, 7,
		0, 1, 2, 3, 4, 5, 6, 7,
		0, 1, 2, 3, 4, 5, 6, 7,
		0, 1, 2, 3, 4, 5, 6, 7,
	};

	CONSTANT unsigned char initial_perm_r[64] = {
		64, 64, 64, 64, 64, 64, 64, 64,
		16, 16, 16, 16, 16, 16, 16, 16,
		4, 4, 4, 4, 4, 4, 4, 4,
		1, 1, 1, 1, 1, 1, 1, 1,
		128, 128, 128, 128, 128, 128, 128, 128,
		32, 32, 32, 32, 32, 32, 32, 32,
		8, 8, 8, 8, 8, 8, 8, 8,
		2, 2, 2, 2, 2, 2, 2, 2,
	};

	CONSTANT unsigned char final_perm_l[64] = {
		3, 7, 2, 6, 1, 5, 0, 4,
		3, 7, 2, 6, 1, 5, 0, 4,
		3, 7, 2, 6, 1, 5, 0, 4,
		3, 7, 2, 6, 1, 5, 0, 4,
		3, 7, 2, 6, 1, 5, 0, 4,
		3, 7, 2, 6, 1, 5, 0, 4,
		3, 7, 2, 6, 1, 5, 0, 4,
		3, 7, 2, 6, 1, 5, 0, 4,
	};

	CONSTANT unsigned char final_perm_r[64] = {
		1, 1, 1, 1, 1, 1, 1, 1,
		2, 2, 2, 2, 2, 2, 2, 2,
		4, 4, 4, 4, 4, 4, 4, 4,
		8, 8, 8, 8, 8, 8, 8, 8,
		16, 16, 16, 16, 16, 16, 16, 16,
		32, 32, 32, 32, 32, 32, 32, 32,
		64, 64, 64, 64, 64, 64, 64, 64,
		128, 128, 128, 128, 128, 128, 128, 128,
	};

	CONSTANT unsigned char expansion_l[48] = {
		0, 0, 0, 0, 0, 0, 0, 3,
		1, 1, 1, 0, 1, 0, 0, 0,
		2, 1, 1, 1, 1, 1, 1, 1,
		2, 2, 2, 2, 2, 2, 2, 1,
		3, 3, 3, 2, 3, 2, 2, 2,
		0, 3, 3, 3, 3, 3, 3, 3,
	};

	CONSTANT unsigned char expansion_r[48] = {
		8, 16, 8, 16, 32, 64, 128, 1,
		32, 64, 128, 1, 128, 1, 2, 4,
		128, 1, 2, 4, 8, 16, 8, 16,
		8, 16, 8, 16, 32, 64, 128, 1,
		32, 64, 128, 1, 128, 1, 2, 4,
		128, 1, 2, 4, 8, 16, 8, 16,
	};

	CONSTANT unsigned char pbox_l[32] = {
		2, 3, 1, 3, 2, 2, 0, 1,
		1, 3, 2, 0, 3, 2, 1, 0,
		1, 0, 3, 3, 1, 2, 0, 0,
		3, 0, 1, 2, 0, 3, 1, 2,
	};

	CONSTANT unsigned char pbox_r[32] = {
		128, 16, 16, 8, 8, 16, 2, 1,
		64, 2, 64, 8, 64, 2, 2, 128,
		128, 32, 32, 1, 4, 1, 1, 64,
		128, 16, 32, 4, 4, 4, 8, 32,
	};

	CONSTANT unsigned char sbox[2][4][4][16] = {
		14,	4,	13,	1,	2,	15,	11,	8,	3,	10,	6,	12,	5,	9,	0,	7,
		0,	15,	7,	4,	14,	2,	13,	1,	10,	6,	12,	11,	9,	5,	3,	8,
		4,	1,	14,	8,	13,	6,	2,	11,	15,	12,	9,	7,	3,	10,	5,	0,
		15,	12,	8,	2,	4,	9,	1,	7,	5,	11,	3,	14,	10,	0,	6,	13,

		15,	1,	8,	14,	6,	11,	3,	4,	9,	7,	2,	13,	12,	0,	5,	10,
		3,	13,	4,	7,	15,	2,	8,	14,	12,	0,	1,	10,	6,	9,	11,	5,
		0,	14,	7,	11,	10,	4,	13,	1,	5,	8,	12,	6,	9,	3,	2,	15,
		13,	8,	10,	1,	3,	15,	4,	2,	11,	6,	7,	12,	0,	5,	14,	9,

		10,	0,	9,	14,	6,	3,	15,	5,	1,	13,	12,	7,	11,	4,	2,	8,
		13,	7,	0,	9,	3,	4,	6,	10,	2,	8,	5,	14,	12,	11,	15,	1,
		13,	6,	4,	9,	8,	15,	3,	0,	11,	1,	2,	12,	5,	10,	14,	7,
		1,	10,	13,	0,	6,	9,	8,	7,	4,	15,	14,	3,	11,	5,	2,	12,

		7,	13,	14,	3,	0,	6,	9,	10,	1,	2,	8,	5,	11,	12,	4,	15,
		13,	8,	11,	5,	6,	15,	0,	3,	4,	7,	2,	12,	1,	10,	14,	9,
		10,	6,	9,	0,	12,	11,	7,	13,	15,	1,	3,	14,	5,	2,	8,	4,
		3,	15,	0,	6,	10,	1,	13,	8,	9,	4,	5,	11,	12,	7,	2,	14,

		2,	12,	4,	1,	7,	10,	11,	6,	8,	5,	3,	15,	13,	0,	14,	9,
		14,	11,	2,	12,	4,	7,	13,	1,	5,	0,	15,	10,	3,	9,	8,	6,
		4,	2,	1,	11,	10,	13,	7,	8,	15,	9,	12,	5,	6,	3,	0,	14,
		11,	8,	12,	7,	1,	14,	2,	13,	6,	15,	0,	9,	10,	4,	5,	3,

		12,	1,	10,	15,	9,	2,	6,	8,	0,	13,	3,	4,	14,	7,	5,	11,
		10,	15,	4,	2,	7,	12,	9,	5,	6,	1,	13,	14,	0,	11,	3,	8,
		9,	14,	15,	5,	2,	8,	12,	3,	7,	0,	4,	10,	1,	13,	11,	6,
		4,	3,	2,	12,	9,	5,	15,	10,	11,	14,	1,	7,	6,	0,	8,	13,

		4,	11,	2,	14,	15,	0,	8,	13,	3,	12,	9,	7,	5,	10,	6,	1,
		13,	0,	11,	7,	4,	9,	1,	10,	14,	3,	5,	12,	2,	15,	8,	6,
		1,	4,	11,	13,	12,	3,	7,	14,	10,	15,	6,	8,	0,	5,	9,	2,
		6,	11,	13,	8,	1,	4,	10,	7,	9,	5,	0,	15,	14,	2,	3,	12,

		13,	2,	8,	4,	6,	15,	11,	1,	10,	9,	3,	14,	5,	0,	12,	7,
		1,	15,	13,	8,	10,	3,	7,	4,	12,	5,	6,	11,	0,	14,	9,	2,
		7,	11,	4,	1,	9,	12,	14,	2,	0,	6,	10,	13,	15,	3,	5,	8,
		2,	1,	14,	7,	4,	10,	8,	13,	15,	12,	9,	0,	3,	5,	6,	11,
	};

	DEVICE void Permute(const unsigned char* set, unsigned char* out, const unsigned char* table_div, const unsigned char* table_modshift, size_t table_size)
	{
		for (size_t i = 0; i < table_size / 8; i++)
		{
			out[i] = (!!(set[table_div[0]] & table_modshift[0]) << 0) |
				(!!(set[table_div[1]] & table_modshift[1]) << 1) |
				(!!(set[table_div[2]] & table_modshift[2]) << 2) |
				(!!(set[table_div[3]] & table_modshift[3]) << 3) |
				(!!(set[table_div[4]] & table_modshift[4]) << 4) |
				(!!(set[table_div[5]] & table_modshift[5]) << 5) |
				(!!(set[table_div[6]] & table_modshift[6]) << 6) |
				(!!(set[table_div[7]] & table_modshift[7]) << 7);

			table_div += 8;
			table_modshift += 8;
		}
	}

	void RotateLeft1Bit(unsigned char* set, size_t bit_start, size_t bit_size)
	{
		// First byte that we are modifying
		unsigned char byte_start = unsigned char(bit_start / 8);

		// First bit of first byte we are modifying
		unsigned char start_bit_index = bit_start % 8;

		unsigned char end_bit_index = ((bit_start + bit_size) % 8);

		// Saving the first bit that will be cut off
		bool first = (set[byte_start] >> start_bit_index) & 1;

		size_t i = byte_start;

		{
			// Mask to make sure we don't effect the bits outside the range
			unsigned char mask = 0 - (1 << start_bit_index);

			// Shift the bits in our range
			set[i] = ((set[i] >> 1) & mask) | (set[i] & ~mask);

			// Copy the first bit from the next byte to the last bit in this byte
			set[i] |= (set[i + 1] & 1) << 7;
		}

		for (i++; i < byte_start + (bit_size / 8); i++)
		{
			// Shift all bits in this byte
			set[i] = set[i] >> 1;

			// Copy the first bit from the next byte to the last bit in this byte
			set[i] |= (set[i + 1] & 1) << 7;
		}

		{
			// Mask to make sure we don't effect the bits outside the range
			unsigned char mask = end_bit_index ? (0 - (1 << end_bit_index)) : 0;

			// Shift the bits in our range
			set[i] = ((set[i] & ~mask) >> 1) | (set[i] & mask);

			// Copy the first bit from the first byte to the last bit in this byte
			set[i] |= first << ((end_bit_index + 7) % 8);
		}
	}

	void RotateLeft2Bit(unsigned char* set, size_t bit_start, size_t bit_size)
	{
		// First byte that we are modifying
		size_t byte_start = bit_start / 8;

		// First bit of first byte we are modifying
		unsigned char start_bit_index = bit_start % 8;

		unsigned char end_bit_index = ((bit_start + bit_size) % 8);

		// Saving the first 2 bits thats will be cut off
		unsigned char first = (set[byte_start] >> (bit_start % 8)) & 3;

		size_t i = byte_start;

		{
			// Mask to make sure we don't effect the bits outside the range
			unsigned char mask = 0 - (1 << start_bit_index);

			// Shift the bits in our range
			set[i] = ((set[i] >> 2) & mask) | (set[i] & ~mask);

			// Copy the first bit from the next byte to the last bit in this byte
			set[i] |= (set[i + 1] & 3) << 6;
		}

		for (i++; i < byte_start + (bit_size / 8); i++)
		{
			set[i] = set[i] >> 2;

			set[i] |= (set[i + 1] & 3) << 6;
		}

		{
			// Mask to make sure we don't effect the bits outside the range
			unsigned char mask = end_bit_index ? (0 - (1 << end_bit_index)) : 0;

			// Shift the bits in our range
			set[i] = ((set[i] & ~mask) >> 2) | (set[i] & mask);

			// Copy the first bit from the first byte to the last bit in this byte
			set[i] |= first << ((end_bit_index + 6) % 8);
		}
	}

	__forceinline__ __device__ unsigned char V(unsigned char byte, unsigned char from, unsigned char to)
	{
		from = 7 - from;

		if (to == from)
		{
			return byte & (1 << from);
		}
		else if (to > from)
		{
			return (byte & (1 << from)) << (to - from);
		}
		else
		{
			return (byte & (1 << from)) >> (from - to);
		}
	}

	__device__ void Compress(const unsigned char input[6], unsigned char output[4])
	{
		for (size_t i = 0; i < 2; i++)
		{
			const unsigned char* in = input + (i * 3);

			unsigned char* out = output + (i * 2);

			{
				unsigned char row = V(in[0], 0, 1) | V(in[0], 5, 0);
				unsigned char column = V(in[0], 1, 3) | V(in[0], 2, 2) | V(in[0], 3, 1) | V(in[0], 4, 0);

				unsigned char val = sbox[i][0][row][column];

				out[0] |= val << 4;
			}

			{
				unsigned char row = V(in[0], 6, 1) | V(in[1], 3, 0);
				unsigned char column = V(in[0], 7, 3) | V(in[1], 0, 2) | V(in[1], 1, 1) | V(in[1], 2, 0);

				unsigned char val = sbox[i][1][row][column];

				out[0] |= val;
			}

			{
				unsigned char row = V(in[1], 4, 1) | V(in[2], 1, 0);
				unsigned char column = V(in[1], 5, 3) | V(in[1], 6, 2) | V(in[1], 7, 1) | V(in[2], 0, 0);

				unsigned char val = sbox[i][2][row][column];

				out[1] |= val << 4;
			}

			{
				unsigned char row = V(in[2], 2, 1) | V(in[2], 7, 0);
				unsigned char column = V(in[2], 3, 3) | V(in[2], 4, 2) | V(in[2], 5, 1) | V(in[2], 6, 0);

				unsigned char val = sbox[i][3][row][column];

				out[1] |= val;
			}
		}
	}

	__device__ void CryptBlock(const unsigned char* subkeys, unsigned char* block)
	{
		unsigned char temp[8] = { 0 };

		Permute(block, temp, initial_perm_l, initial_perm_r, 64);

		unsigned char* left = temp;
		unsigned char* right = temp + 4;

		for (size_t key = 0; key < 16; key++)
		{
			const unsigned char* round_key = subkeys + (key * 6);

			unsigned char temp2[6] = { 0 };

			// Mangle
			{
				Permute(right, temp2, expansion_l, expansion_r, 48);

				for (size_t i = 0; i < 6; i++)
				{
					temp2[i] ^= round_key[i];
				}

				unsigned char temp3[4] = { 0 };

				Compress(temp2, temp3);

				Permute(temp3, temp2, pbox_l, pbox_r, 32);
			}

			for (size_t i = 0; i < 4; i++)
			{
				temp2[i] ^= left[i];
			}

			if (key < 15)
			{
				for (size_t i = 0; i < 4; i++)
				{
					left[i] = right[i];
				}

				for (size_t i = 0; i < 4; i++)
				{
					right[i] = temp2[i];
				}
			}
			else
			{
				for (size_t i = 0; i < 4; i++)
				{
					left[i] = temp2[i];
				}
			}
		}

		Permute(temp, block, final_perm_l, final_perm_r, 64);
	}

	__global__ void CryptBlocks(const unsigned char* subkeys, unsigned char* block)
	{
		block += (threadIdx.x * 8);

		CryptBlock(subkeys + (96 * 0), block);
	}

	__global__ void TripleCryptBlocksOld(const unsigned char* subkeys, unsigned char* block)
	{
		block += (threadIdx.x * 8);

		CryptBlock(subkeys + (96 * 0), block);
		CryptBlock(subkeys + (96 * 1), block);
		CryptBlock(subkeys + (96 * 2), block);
	}

	__global__ void TripleCryptBlocks(const unsigned char* subkeys, unsigned char* block, unsigned int group_size, unsigned int num_blocks)
	{
		unsigned int group = threadIdx.x;
		size_t group_start = group * group_size;
		size_t group_end = __min(group_start + group_size, num_blocks);

		for (size_t block_id = group_start; block_id < group_end; block_id++)
		{
			CryptBlock(subkeys + (96 * 0), block + (block_id * 8));
			CryptBlock(subkeys + (96 * 1), block + (block_id * 8));
			CryptBlock(subkeys + (96 * 2), block + (block_id * 8));
		}
	}

	TripleDES::TripleDES(const std::string& key, size_t group_size) :
		EncryptBase(key),
		m_group_size(group_size)
	{
		if (group_size == 0 || group_size > (SIZE_MAX / k_block_size))
		{
			throw Exception{};
		}

		if (key.size() != k_min_key_size)
		{
			throw Exception{};
		}

		if (cudaSetDevice(0) != cudaSuccess)
		{
			fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
			throw Exception{};
		}

		if (cudaMalloc(&m_subkeys, 6 * 96) != cudaSuccess)
		{
			fprintf(stderr, "cudaMalloc failed!");
			throw Exception{};
		}

		unsigned char subkeys[6][96] = { 0 };

		const unsigned char enc_pos[3] = { 0, 4, 2 };
		const unsigned char dec_pos[3] = { 3, 1, 5 };

		for (size_t k = 0; k < 3; k++)
		{
			unsigned char* enc_subkeys = subkeys[enc_pos[k]];
			unsigned char* dec_subkeys = subkeys[dec_pos[k]] + (6 * 15);

			unsigned char temp[7] = { 0 };
			Permute((const unsigned char*)key.data() + (k * 8), temp, key_perm_l, key_perm_r, 56);

			for (size_t i = 0; i < 16; i++)
			{
				switch (i)
				{
				case 0:
				case 1:
				case 8:
				case 15:
					RotateLeft1Bit(temp, 0, 28);
					RotateLeft1Bit(temp, 28, 28);
					break;

				default:
					RotateLeft2Bit(temp, 0, 28);
					RotateLeft2Bit(temp, 28, 28);
					break;
				}

				Permute(temp, enc_subkeys, left_round_perm_l, left_round_perm_r, 24);

				Permute(temp, enc_subkeys + 3, right_round_perm_l, right_round_perm_r, 24);

				Permute(temp, dec_subkeys, left_round_perm_l, left_round_perm_r, 24);

				Permute(temp, dec_subkeys + 3, right_round_perm_l, right_round_perm_r, 24);

				enc_subkeys += 6;
				dec_subkeys -= 6;
			}
		}

		if (cudaMemcpy(m_subkeys, subkeys, 6 * 96, cudaMemcpyHostToDevice) != cudaSuccess)
		{
			fprintf(stderr, "cudaMemcpy failed!");
			throw Exception{};
		}
	}

	TripleDES::~TripleDES()
	{
		cudaFree(m_subkeys);
	}

	std::string TripleDES::GetName() const
	{
		char buffer[32];
		sprintf_s(buffer, "Triple DES CPU %zi per group", m_group_size);
		return buffer;
	}

	void TripleDES::EncryptInPlace(std::string& input) const
	{
		if (input.size() == 0 || input.size() % k_block_size != 0)
		{
			throw Exception{};
		}

		unsigned char* mem = 0;

		if (cudaMalloc(&mem, input.size()) != cudaSuccess)
		{
			fprintf(stderr, "cudaMalloc failed!");
			throw Exception{};
		}

		if (cudaMemcpy(mem, input.data(), input.size(), cudaMemcpyHostToDevice) != cudaSuccess)
		{
			fprintf(stderr, "cudaMemcpy failed!");
			throw Exception{};
		}

		unsigned int num_blocks = (unsigned int)input.size() / k_block_size;
		unsigned int num_iterations = (num_blocks + m_group_size - 1) / m_group_size;
		TripleCryptBlocks<<<1, num_iterations>>>(m_subkeys, mem, m_group_size, num_blocks);

		if (cudaMemcpy(input.data(), mem, input.size(), cudaMemcpyDeviceToHost) != cudaSuccess)
		{
			fprintf(stderr, "cudaMemcpy failed!");
			throw Exception{};
		}

		if (cudaFree(mem) != cudaSuccess)
		{
			fprintf(stderr, "cudaFree failed!");
			throw Exception{};
		}
	}

	void TripleDES::DecryptInPlace(std::string& input) const
	{
		if (input.size() == 0 || input.size() % k_block_size != 0)
		{
			throw Exception{};
		}

		unsigned char* mem = 0;

		if (cudaMalloc(&mem, input.size()) != cudaSuccess)
		{
			fprintf(stderr, "cudaMalloc failed!");
			throw Exception{};
		}

		if (cudaMemcpy(mem, input.data(), input.size(), cudaMemcpyHostToDevice) != cudaSuccess)
		{
			fprintf(stderr, "cudaMemcpy failed!");
			throw Exception{};
		}

		unsigned int num_blocks = (unsigned int)input.size() / k_block_size;
		unsigned int num_iterations = (num_blocks + m_group_size - 1) / m_group_size;
		TripleCryptBlocks<<<1, num_iterations>>>(m_subkeys + (96 * 3), mem, m_group_size, num_blocks);

		if (cudaMemcpy(input.data(), mem, input.size(), cudaMemcpyDeviceToHost) != cudaSuccess)
		{
			fprintf(stderr, "cudaMemcpy failed!");
			throw Exception{};
		}

		if (cudaFree(mem) != cudaSuccess)
		{
			fprintf(stderr, "cudaFree failed!");
			throw Exception{};
		}
	}
}