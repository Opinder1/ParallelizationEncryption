#include "DESv2.h"

#include "../Shared/Shared.h"

namespace des::v2
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

	void Permute(const unsigned char* set, unsigned char* out, const unsigned char* table, size_t table_size)
	{
		table += table_size - 8;

		for (size_t i = 0; i < table_size / 8; i++)
		{
			out[i] =	(!!(set[table[0] / 8] & (1 << (7 - (table[0] % 8)))) << 0) |
						(!!(set[table[1] / 8] & (1 << (7 - (table[1] % 8)))) << 1) |
						(!!(set[table[2] / 8] & (1 << (7 - (table[2] % 8)))) << 2) |
						(!!(set[table[3] / 8] & (1 << (7 - (table[3] % 8)))) << 3) |
						(!!(set[table[4] / 8] & (1 << (7 - (table[4] % 8)))) << 4) |
						(!!(set[table[5] / 8] & (1 << (7 - (table[5] % 8)))) << 5) |
						(!!(set[table[6] / 8] & (1 << (7 - (table[6] % 8)))) << 6) |
						(!!(set[table[7] / 8] & (1 << (7 - (table[7] % 8)))) << 7);

			table -= 8;
		}
	}

	void Permute2(const unsigned char* set, unsigned char* out, const unsigned char* table, size_t table_size)
	{
		table += table_size - 8;

		for (size_t i = 0; i < table_size / 8; i++)
		{
			out[i] = (!!(set[table[0] / 8] & (1 << (table[0] % 8))) << 7) |
				(!!(set[table[1] / 8] & (1 << (table[1] % 8))) << 6) |
				(!!(set[table[2] / 8] & (1 << (table[2] % 8))) << 5) |
				(!!(set[table[3] / 8] & (1 << (table[3] % 8))) << 4) |
				(!!(set[table[4] / 8] & (1 << (table[4] % 8))) << 3) |
				(!!(set[table[5] / 8] & (1 << (table[5] % 8))) << 2) |
				(!!(set[table[6] / 8] & (1 << (table[6] % 8))) << 1) |
				(!!(set[table[7] / 8] & (1 << (table[7] % 8))) << 0);

			table -= 8;
		}
	}

	void Permute3(const unsigned char* set, unsigned char* out, const unsigned char* table, size_t table_size)
	{
		for (size_t i = 0; i < table_size / 8; i++)
		{
			out[i] = (!!(set[table[0] / 8] & (1 << (7 - (table[0] % 8)))) << 7) |
				(!!(set[table[1] / 8] & (1 << (7 - (table[1] % 8)))) << 6) |
				(!!(set[table[2] / 8] & (1 << (7 - (table[2] % 8)))) << 5) |
				(!!(set[table[3] / 8] & (1 << (7 - (table[3] % 8)))) << 4) |
				(!!(set[table[4] / 8] & (1 << (7 - (table[4] % 8)))) << 3) |
				(!!(set[table[5] / 8] & (1 << (7 - (table[5] % 8)))) << 2) |
				(!!(set[table[6] / 8] & (1 << (7 - (table[6] % 8)))) << 1) |
				(!!(set[table[7] / 8] & (1 << (7 - (table[7] % 8)))) << 0);

			table += 8;
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

	bool GetVal(const unsigned char* in, size_t bit)
	{
		return (in[bit / 8] & (1 << (7 - (bit % 8)))) != 0;
	}

	void Compress(const unsigned char input[6], unsigned char output[4])
	{
		for (size_t i = 0; i < 8; i++)
		{
			size_t pos = i * 6;

			size_t row = (2 * GetVal(input, pos)) + (1 * GetVal(input, pos + 5));
			size_t column = (8 * GetVal(input, pos + 1)) + (4 * GetVal(input, pos + 2)) + (2 * GetVal(input, pos + 3)) + (1 * GetVal(input, pos + 4));

			unsigned char nibble = sbox[i][row][column];

			output[i / 2] |= (i % 2 == 0) ? (nibble << 4) : nibble;
		}
	}

	void CryptBlock(const unsigned char* subkeys, unsigned char* block)
	{
		unsigned char temp[8] = { 0 };

		Permute2(block, temp, initial_perm, 64);

		unsigned char* left = temp;
		unsigned char* right = temp + 4;

		for (size_t key = 0; key < 16; key++)
		{
			const unsigned char* round_key = subkeys + (key * 6);

			unsigned char temp2[6] = { 0 };
			unsigned char temp3[4] = { 0 };

			// Mangle
			{
				Permute3(right, temp2, expansion, 48);

				for (size_t i = 0; i < 6; i++)
				{
					temp2[i] ^= round_key[i];
				}

				Compress(temp2, temp3);

				Permute3(temp3, temp2, pbox, 32);
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

		Permute2(temp, block, final_perm, 64);
	}

	DES::DES(const std::string& key, size_t group_size) :
		EncryptBase(key)
	{
		if (group_size == 0 || group_size > (SIZE_MAX / k_block_size))
		{
			throw Exception{};
		}

		if (key.size() != k_min_key_size)
		{
			throw Exception{};
		}

		unsigned char* enc_subkeys = m_enc_subkeys;
		unsigned char* dec_subkeys = m_dec_subkeys + (6 * 15);

		unsigned char temp[7] = { 0 };
		Permute((const unsigned char*)key.data(), temp, key_perm, 56);

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

			Permute(temp, enc_subkeys, left_round_perm, 24);

			Permute(temp, enc_subkeys + 3, right_round_perm, 24);

			Permute(temp, dec_subkeys, left_round_perm, 24);

			Permute(temp, dec_subkeys + 3, right_round_perm, 24);

			enc_subkeys += 6;
			dec_subkeys -= 6;
		}
	}

	DES::~DES()
	{

	}

	std::string DES::GetName() const
	{
		return "DES v2";
	}

	void DES::EncryptInPlace(std::string& input) const
	{
		if (input.size() == 0 || input.size() % k_block_size != 0)
		{
			throw Exception{};
		}

		for (size_t i = 0; i < input.size() / k_block_size; i++)
		{
			CryptBlock(m_enc_subkeys, (unsigned char*)input.data() + (i * 8));
		}
	}

	void DES::DecryptInPlace(std::string& input) const
	{
		if (input.size() == 0 || input.size() % k_block_size != 0)
		{
			throw Exception{};
		}

		for (size_t i = 0; i < input.size() / k_block_size; i++)
		{
			CryptBlock(m_dec_subkeys, (unsigned char*)input.data() + (i * 8));
		}
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

	std::string DESParallel::GetName() const
	{
		return "DES v2 16 threads";
	}

	void DESParallel::EncryptInPlace(std::string& input) const
	{
		if (input.size() == 0 || input.size() % k_block_size != 0)
		{
			throw Exception{};
		}

		int i;

#pragma omp parallel for num_threads(16)
		for (i = 0; i < input.size() / k_block_size; i++)
		{
			CryptBlock(m_enc_subkeys, (unsigned char*)input.data() + (i * 8));
		}
	}

	void DESParallel::DecryptInPlace(std::string& input) const
	{
		if (input.size() == 0 || input.size() % k_block_size != 0)
		{
			throw Exception{};
		}

		int i;

#pragma omp parallel for num_threads(16)
		for (i = 0; i < input.size() / k_block_size; i++)
		{
			CryptBlock(m_dec_subkeys, (unsigned char*)input.data() + (i * 8));
		}
	}
}