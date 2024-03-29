#include "DESv1.h"

#include "../Shared/Shared.h"

namespace
{
	unsigned char key_perm[56] = {
		56, 48, 40, 32, 24, 16, 8,	0,
		57, 49, 41, 33, 25, 17, 9,	1,
		58, 50, 42, 34, 26, 18, 10, 2,
		59, 51, 43, 35, 62, 54, 46, 38,
		30, 22, 14, 6,	61, 53, 45, 37,
		29, 21, 13, 5,	60, 52, 44, 36,
		28, 20, 12, 4,	27, 19, 11, 3,
	};

	unsigned char left_round_perm[24] = {
		13, 16, 10, 23, 0,	4,
		2,	27, 14, 5,	20,	9,
		22, 18, 11, 3,	25, 7,
		15, 6,	26,	19, 12, 1,
	};

	unsigned char right_round_perm[24] = {
		40, 51, 30, 36, 46, 54,
		29, 39, 50, 44, 32, 47,
		43, 48, 38, 55, 33, 52,
		45, 41, 49, 35, 28, 31,
	};

	unsigned char initial_perm[64] = {
		57, 49, 41, 33, 25, 17, 9,	1,
		59, 51, 43, 35, 27, 19, 11, 3,
		61, 53, 45, 37, 29, 21, 13, 5,
		63, 55, 47, 39, 31, 23, 15, 7,
		56, 48, 40, 32, 24, 16, 8,	0,
		58, 50, 42, 34, 26, 18, 10, 2,
		60, 52, 44, 36, 28, 20, 12, 4,
		62, 54, 46, 38, 30, 22, 14, 6,
	};

	unsigned char final_perm[64] = {
		39, 7,	47,	15,	55, 23, 63, 31,
		38, 6,	46,	14,	54, 22, 62, 30,
		37, 5,	45,	13,	53, 21, 61, 29,
		36, 4,	44,	12,	52, 20, 60, 28,
		35, 3,	43, 11,	51, 19, 59, 27,
		34, 2,	42, 10,	50, 18, 58, 26,
		33, 1,	41, 9,	49, 17, 57, 25,
		32, 0,	40, 8,	48, 16, 56, 24,
	};

	unsigned char expansion[48] = {
		31, 0,	1,	2,	3,	4,	3,	4,
		5,	6,	7,	8,	7,	8,	9,	10,
		11, 12, 11, 12, 13, 14, 15, 16,
		15, 16, 17, 18, 19, 20, 19, 20,
		21, 22, 23, 24, 23, 24, 25, 26,
		27, 28, 27, 28, 29, 30, 31, 0,
	};

	unsigned char sbox[8][4][16] = {
	{
		14,	4,	13,	1,	2,	15,	11,	8,	3,	10,	6,	12,	5,	9,	0,	7,
		0,	15,	7,	4,	14,	2,	13,	1,	10,	6,	12,	11,	9,	5,	3,	8,
		4,	1,	14,	8,	13,	6,	2,	11,	15,	12,	9,	7,	3,	10,	5,	0,
		15,	12,	8,	2,	4,	9,	1,	7,	5,	11,	3,	14,	10,	0,	6,	13,
	},
	{
		15,	1,	8,	14,	6,	11,	3,	4,	9,	7,	2,	13,	12,	0,	5,	10,
		3,	13,	4,	7,	15,	2,	8,	14,	12,	0,	1,	10,	6,	9,	11,	5,
		0,	14,	7,	11,	10,	4,	13,	1,	5,	8,	12,	6,	9,	3,	2,	15,
		13,	8,	10,	1,	3,	15,	4,	2,	11,	6,	7,	12,	0,	5,	14,	9,
	},
	{
		10,	0,	9,	14,	6,	3,	15,	5,	1,	13,	12,	7,	11,	4,	2,	8,
		13,	7,	0,	9,	3,	4,	6,	10,	2,	8,	5,	14,	12,	11,	15,	1,
		13,	6,	4,	9,	8,	15,	3,	0,	11,	1,	2,	12,	5,	10,	14,	7,
		1,	10,	13,	0,	6,	9,	8,	7,	4,	15,	14,	3,	11,	5,	2,	12,
	},
	{
		7,	13,	14,	3,	0,	6,	9,	10,	1,	2,	8,	5,	11,	12,	4,	15,
		13,	8,	11,	5,	6,	15,	0,	3,	4,	7,	2,	12,	1,	10,	14,	9,
		10,	6,	9,	0,	12,	11,	7,	13,	15,	1,	3,	14,	5,	2,	8,	4,
		3,	15,	0,	6,	10,	1,	13,	8,	9,	4,	5,	11,	12,	7,	2,	14,
	},
	{
		2,	12,	4,	1,	7,	10,	11,	6,	8,	5,	3,	15,	13,	0,	14,	9,
		14,	11,	2,	12,	4,	7,	13,	1,	5,	0,	15,	10,	3,	9,	8,	6,
		4,	2,	1,	11,	10,	13,	7,	8,	15,	9,	12,	5,	6,	3,	0,	14,
		11,	8,	12,	7,	1,	14,	2,	13,	6,	15,	0,	9,	10,	4,	5,	3,
	},
	{
		12,	1,	10,	15,	9,	2,	6,	8,	0,	13,	3,	4,	14,	7,	5,	11,
		10,	15,	4,	2,	7,	12,	9,	5,	6,	1,	13,	14,	0,	11,	3,	8,
		9,	14,	15,	5,	2,	8,	12,	3,	7,	0,	4,	10,	1,	13,	11,	6,
		4,	3,	2,	12,	9,	5,	15,	10,	11,	14,	1,	7,	6,	0,	8,	13,
	},
	{
		4,	11,	2,	14,	15,	0,	8,	13,	3,	12,	9,	7,	5,	10,	6,	1,
		13,	0,	11,	7,	4,	9,	1,	10,	14,	3,	5,	12,	2,	15,	8,	6,
		1,	4,	11,	13,	12,	3,	7,	14,	10,	15,	6,	8,	0,	5,	9,	2,
		6,	11,	13,	8,	1,	4,	10,	7,	9,	5,	0,	15,	14,	2,	3,	12,
	},
	{
		13,	2,	8,	4,	6,	15,	11,	1,	10,	9,	3,	14,	5,	0,	12,	7,
		1,	15,	13,	8,	10,	3,	7,	4,	12,	5,	6,	11,	0,	14,	9,	2,
		7,	11,	4,	1,	9,	12,	14,	2,	0,	6,	10,	13,	15,	3,	5,	8,
		2,	1,	14,	7,	4,	10,	8,	13,	15,	12,	9,	0,	3,	5,	6,	11,
	}
	};

	unsigned char pbox[32] = {
		15, 6,	19, 20, 28, 11, 27, 16,
		0,	14, 22, 25, 4,	17, 30, 9,
		1,	7,	23, 13, 31, 26, 2,	8,
		18, 12, 29, 5,	21,	10, 3,	24,
	};

	std::bitset<32> Compress(std::bitset<48> input)
	{
		std::bitset<32> out;

		for (size_t i = 0; i < 8; i++)
		{
			size_t pos = i * 6;

			size_t row = (2 * input[pos]) + (1 * input[pos + 5]);
			size_t column = (8 * input[pos + 1]) + (4 * input[pos + 2]) + (2 * input[pos + 3]) + (1 * input[pos + 4]);

			std::bitset<4> val = sbox[i][row][column];

			out[i * 4] = val[3];
			out[i * 4 + 1] = val[2];
			out[i * 4 + 2] = val[1];
			out[i * 4 + 3] = val[0];
		}

		return out;
	}

	std::bitset<32> Mangle(std::bitset<32> in, std::bitset<48> key)
	{
		std::bitset<48> expand = Permute<48>(in, expansion);

		expand ^= key;

		std::bitset<32> fin = Compress(expand);

		return Permute<32>(fin, pbox);
	}
}

namespace des::v1
{
	DES::DES(const std::string& key) :
		EncryptBase(key)
	{
		if (key.size() != k_min_key_size)
		{
			throw Exception{};
		}

		std::bitset<64> key_bits = *(const uint64_t*)key.data();

		std::bitset<56> both = Permute<56>(key_bits, key_perm);

		for (size_t i = 0; i < 16; i++)
		{
			switch (i)
			{
			case 0:
			case 1:
			case 8:
			case 15:
				RotateLeft1Bit(both, 0, 28);
				RotateLeft1Bit(both, 28, 28);
				break;

			default:
				RotateLeft2Bits(both, 0, 28);
				RotateLeft2Bits(both, 28, 28);
				break;
			}

			m_enc_keys[i].bytes = Append(Permute<24>(both, left_round_perm), Permute<24>(both, right_round_perm));

			m_dec_keys[15 - i].bytes = Append(Permute<24>(both, left_round_perm), Permute<24>(both, right_round_perm));
		}
	}

	DES::~DES()
	{
	}

	std::string DES::Encrypt(const std::string& input) const
	{
		return Crypt(input, m_enc_keys);
	}

	std::string DES::Decrypt(const std::string& input) const
	{
		return Crypt(input, m_dec_keys);
	}

	std::string DES::Crypt(const std::string& input, const DES::RoundKey keys[16]) const
	{
		if (input.size() == 0 || input.size() % k_block_size != 0)
		{
			throw Exception{};
		}

		std::string output(input.size(), '\0');

		const uint64_t* in_ptr = (const uint64_t*)input.data();
		uint64_t* out_ptr = (uint64_t*)output.data();

		for (size_t i = 0; i < input.size() / k_block_size; i++)
		{
			out_ptr[i] = CryptBlock(in_ptr[i], keys).to_ullong();
		}

		return output;
	}

	std::bitset<64> DES::CryptBlock(std::bitset<64> block, const DES::RoundKey keys[16]) const
	{
		block = Permute<64>(block, initial_perm);

		std::bitset<32> left = block.to_ullong() & UINT32_MAX;

		std::bitset<32> right = block.to_ullong() >> 32;

		for (size_t i = 0; i < 16; i++)
		{
			std::bitset<32> fin = Mangle(right, keys[i].bytes);

			fin ^= left;

			if (i < 15)
			{
				left = right;
				right = fin;
			}
			else
			{
				left = fin;
			}
		}

		block = Append(left, right);

		block = Permute<64>(block, final_perm);

		return block;
	}

	DESParallel::DESParallel(const std::string& key, size_t group_size) :
		DES(key)
	{
		if (key.size() != k_min_key_size)
		{
			throw Exception{};
		}

		if (group_size == 0 || group_size > (SIZE_MAX / k_block_size))
		{
			throw Exception{};
		}
	}

	std::string DESParallel::Encrypt(const std::string& input) const
	{
		return Crypt(input, m_enc_keys);
	}

	std::string DESParallel::Decrypt(const std::string& input) const
	{
		return Crypt(input, m_dec_keys);
	}

	std::string DESParallel::Crypt(const std::string& input, const DES::RoundKey keys[16]) const
	{
		if (input.size() == 0 || input.size() % k_block_size != 0)
		{
			throw Exception{};
		}

		std::string output(input.size(), '\0');

		const uint64_t* in_ptr = (const uint64_t*)input.data();
		uint64_t* out_ptr = (uint64_t*)output.data();

		int i;

#pragma omp parallel for num_threads(16)
		for (i = 0; i < input.size() / k_block_size; i++)
		{
			out_ptr[i] = CryptBlock(in_ptr[i], keys).to_ullong();
		}

		return output;
	}
}