#include "DESv3.h"

#include "../Shared/Shared.h"

#include <array>

template<size_t S>
std::array<unsigned char, S> Left1(const unsigned char(&data)[S])
{
	std::array<unsigned char, S> array;

	const unsigned char* from_ptr = data + S - 8;
	unsigned char* to_ptr = array.data();

	for (size_t i = 0; i < S / 8; i++)
	{
		to_ptr[0] = from_ptr[0] / 8;
		to_ptr[1] = from_ptr[1] / 8;
		to_ptr[2] = from_ptr[2] / 8;
		to_ptr[3] = from_ptr[3] / 8;
		to_ptr[4] = from_ptr[4] / 8;
		to_ptr[5] = from_ptr[5] / 8;
		to_ptr[6] = from_ptr[6] / 8;
		to_ptr[7] = from_ptr[7] / 8;

		from_ptr -= 8;
		to_ptr += 8;
	}

	return array;
}

template<size_t S>
std::array<unsigned char, S> Right1(const unsigned char(&data)[S])
{
	std::array<unsigned char, S> array;

	const unsigned char* from_ptr = data + S - 8;
	unsigned char* to_ptr = array.data();

	for (size_t i = 0; i < S / 8; i++)
	{
		to_ptr[0] = 1 << (7 - (from_ptr[0] % 8));
		to_ptr[1] = 1 << (7 - (from_ptr[1] % 8));
		to_ptr[2] = 1 << (7 - (from_ptr[2] % 8));
		to_ptr[3] = 1 << (7 - (from_ptr[3] % 8));
		to_ptr[4] = 1 << (7 - (from_ptr[4] % 8));
		to_ptr[5] = 1 << (7 - (from_ptr[5] % 8));
		to_ptr[6] = 1 << (7 - (from_ptr[6] % 8));
		to_ptr[7] = 1 << (7 - (from_ptr[7] % 8));

		from_ptr -= 8;
		to_ptr += 8;
	}

	return array;
}

template<size_t S>
std::array<unsigned char, S> Left2(const unsigned char(&data)[S])
{
	std::array<unsigned char, S> array;

	const unsigned char* from_ptr = data + S - 8;
	unsigned char* to_ptr = array.data();

	for (size_t i = 0; i < S / 8; i++)
	{
		to_ptr[0] = from_ptr[7] / 8;
		to_ptr[1] = from_ptr[6] / 8;
		to_ptr[2] = from_ptr[5] / 8;
		to_ptr[3] = from_ptr[4] / 8;
		to_ptr[4] = from_ptr[3] / 8;
		to_ptr[5] = from_ptr[2] / 8;
		to_ptr[6] = from_ptr[1] / 8;
		to_ptr[7] = from_ptr[0] / 8;

		from_ptr -= 8;
		to_ptr += 8;
	}

	return array;
}

template<size_t S>
std::array<unsigned char, S> Right2(const unsigned char(&data)[S])
{
	std::array<unsigned char, S> array;

	const unsigned char* from_ptr = data + S - 8;
	unsigned char* to_ptr = array.data();

	for (size_t i = 0; i < S / 8; i++)
	{
		to_ptr[0] = 1 << (from_ptr[7] % 8);
		to_ptr[1] = 1 << (from_ptr[6] % 8);
		to_ptr[2] = 1 << (from_ptr[5] % 8);
		to_ptr[3] = 1 << (from_ptr[4] % 8);
		to_ptr[4] = 1 << (from_ptr[3] % 8);
		to_ptr[5] = 1 << (from_ptr[2] % 8);
		to_ptr[6] = 1 << (from_ptr[1] % 8);
		to_ptr[7] = 1 << (from_ptr[0] % 8);

		from_ptr -= 8;
		to_ptr += 8;
	}

	return array;
}

template<size_t S>
std::array<unsigned char, S> Left3(const unsigned char(&data)[S])
{
	std::array<unsigned char, S> array;

	const unsigned char* from_ptr = data;
	unsigned char* to_ptr = array.data();

	for (size_t i = 0; i < S / 8; i++)
	{
		to_ptr[0] = from_ptr[7] / 8;
		to_ptr[1] = from_ptr[6] / 8;
		to_ptr[2] = from_ptr[5] / 8;
		to_ptr[3] = from_ptr[4] / 8;
		to_ptr[4] = from_ptr[3] / 8;
		to_ptr[5] = from_ptr[2] / 8;
		to_ptr[6] = from_ptr[1] / 8;
		to_ptr[7] = from_ptr[0] / 8;

		from_ptr += 8;
		to_ptr += 8;
	}

	return array;
}

template<size_t S>
std::array<unsigned char, S> Right3(const unsigned char(&data)[S])
{
	std::array<unsigned char, S> array;

	const unsigned char* from_ptr = data;
	unsigned char* to_ptr = array.data();

	for (size_t i = 0; i < S / 8; i++)
	{
		to_ptr[0] = 1 << (7 - (from_ptr[7] % 8));
		to_ptr[1] = 1 << (7 - (from_ptr[6] % 8));
		to_ptr[2] = 1 << (7 - (from_ptr[5] % 8));
		to_ptr[3] = 1 << (7 - (from_ptr[4] % 8));
		to_ptr[4] = 1 << (7 - (from_ptr[3] % 8));
		to_ptr[5] = 1 << (7 - (from_ptr[2] % 8));
		to_ptr[6] = 1 << (7 - (from_ptr[1] % 8));
		to_ptr[7] = 1 << (7 - (from_ptr[0] % 8));

		from_ptr += 8;
		to_ptr += 8;
	}

	return array;
}

namespace des::v3
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

	auto key_perm_l = Left1(key_perm);
	auto key_perm_r = Right1(key_perm);

	unsigned char left_round_perm[24] = {
		13, 16, 10, 23, 0,	4,
		2,	27, 14, 5,	20,	9,
		22, 18, 11, 3,	25, 7,
		15, 6,	26,	19, 12, 1,
	};

	auto left_round_perm_l = Left1(left_round_perm);
	auto left_round_perm_r = Right1(left_round_perm);

	unsigned char right_round_perm[24] = {
		40, 51, 30, 36, 46, 54,
		29, 39, 50, 44, 32, 47,
		43, 48, 38, 55, 33, 52,
		45, 41, 49, 35, 28, 31,
	};

	auto right_round_perm_l = Left1(right_round_perm);
	auto right_round_perm_r = Right1(right_round_perm);

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

	auto initial_perm_l = Left2(initial_perm);
	auto initial_perm_r = Right2(initial_perm);

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

	auto final_perm_l = Left2(final_perm);
	auto final_perm_r = Right2(final_perm);

	unsigned char expansion[48] = {
		31, 0,	1,	2,	3,	4,	3,	4,
		5,	6,	7,	8,	7,	8,	9,	10,
		11, 12, 11, 12, 13, 14, 15, 16,
		15, 16, 17, 18, 19, 20, 19, 20,
		21, 22, 23, 24, 23, 24, 25, 26,
		27, 28, 27, 28, 29, 30, 31, 0,
	};

	auto expansion_l = Left3(expansion);
	auto expansion_r = Right3(expansion);

	unsigned char pbox[32] = {
		15, 6,	19, 20, 28, 11, 27, 16,
		0,	14, 22, 25, 4,	17, 30, 9,
		1,	7,	23, 13, 31, 26, 2,	8,
		18, 12, 29, 5,	21,	10, 3,	24,
	};

	auto pbox_l = Left3(pbox);
	auto pbox_r = Right3(pbox);

	unsigned char sbox[2][4][4][16] = {
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

	void Permute(const unsigned char* set, unsigned char* out, const unsigned char* table_div, const unsigned char* table_modshift, size_t table_size)
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
		unsigned char byte_start = bit_start / 8;

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

	unsigned char V(unsigned char byte, unsigned char from, unsigned char to)
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

	void Compress(const unsigned char input[6], unsigned char output[4])
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

	void CryptBlock(const unsigned char* subkeys, unsigned char* block)
	{
		unsigned char temp[8] = { 0 };

		Permute(block, temp, initial_perm_l.data(), initial_perm_r.data(), 64);

		unsigned char* left = temp;
		unsigned char* right = temp + 4;

		for (size_t key = 0; key < 16; key++)
		{
			const unsigned char* round_key = subkeys + (key * 6);

			unsigned char temp2[6] = { 0 };

			// Mangle
			{
				Permute(right, temp2, expansion_l.data(), expansion_r.data(), 48);

				for (size_t i = 0; i < 6; i++)
				{
					temp2[i] ^= round_key[i];
				}

				unsigned char temp3[4] = { 0 };

				Compress(temp2, temp3);

				Permute(temp3, temp2, pbox_l.data(), pbox_r.data(), 32);
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

		Permute(temp, block, final_perm_l.data(), final_perm_r.data(), 64);
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
		Permute((const unsigned char*)key.data(), temp, key_perm_l.data(), key_perm_r.data(), 56);

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

			Permute(temp, enc_subkeys, left_round_perm_l.data(), left_round_perm_r.data(), 24);

			Permute(temp, enc_subkeys + 3, right_round_perm_l.data(), right_round_perm_r.data(), 24);

			Permute(temp, dec_subkeys, left_round_perm_l.data(), left_round_perm_r.data(), 24);

			Permute(temp, dec_subkeys + 3, right_round_perm_l.data(), right_round_perm_r.data(), 24);

			enc_subkeys += 6;
			dec_subkeys -= 6;
		}
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
		DES(key, group_size)
	{

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

	TripleDES::TripleDES(const std::string& key, size_t group_size) :
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

		for (size_t k = 0; k < 3; k++)
		{
			unsigned char* enc_subkeys = m_enc_subkeys[k];
			unsigned char* dec_subkeys = m_dec_subkeys[k] + (6 * 15);

			unsigned char temp[7] = { 0 };
			Permute((const unsigned char*)key.data() + (k * 8), temp, key_perm_l.data(), key_perm_r.data(), 56);

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

				Permute(temp, enc_subkeys, left_round_perm_l.data(), left_round_perm_r.data(), 24);

				Permute(temp, enc_subkeys + 3, right_round_perm_l.data(), right_round_perm_r.data(), 24);

				Permute(temp, dec_subkeys, left_round_perm_l.data(), left_round_perm_r.data(), 24);

				Permute(temp, dec_subkeys + 3, right_round_perm_l.data(), right_round_perm_r.data(), 24);

				enc_subkeys += 6;
				dec_subkeys -= 6;
			}
		}
	}

	void TripleDES::EncryptInPlace(std::string& input) const
	{
		if (input.size() == 0 || input.size() % k_block_size != 0)
		{
			throw Exception{};
		}

		for (size_t i = 0; i < input.size() / k_block_size; i++)
		{
			CryptBlock(m_enc_subkeys[0], (unsigned char*)input.data() + (i * 8));
			CryptBlock(m_dec_subkeys[1], (unsigned char*)input.data() + (i * 8));
			CryptBlock(m_enc_subkeys[2], (unsigned char*)input.data() + (i * 8));
		}
	}

	void TripleDES::DecryptInPlace(std::string& input) const
	{
		if (input.size() == 0 || input.size() % k_block_size != 0)
		{
			throw Exception{};
		}

		for (size_t i = 0; i < input.size() / k_block_size; i++)
		{
			CryptBlock(m_dec_subkeys[0], (unsigned char*)input.data() + (i * 8));
			CryptBlock(m_enc_subkeys[1], (unsigned char*)input.data() + (i * 8));
			CryptBlock(m_dec_subkeys[2], (unsigned char*)input.data() + (i * 8));
		}
	}

	TripleDESParallel::TripleDESParallel(const std::string& key, size_t group_size) :
		TripleDES(key, group_size)
	{

	}

	void TripleDESParallel::EncryptInPlace(std::string& input) const
	{
		if (input.size() == 0 || input.size() % k_block_size != 0)
		{
			throw Exception{};
		}

		int i;

#pragma omp parallel for num_threads(16)
		for (i = 0; i < input.size() / k_block_size; i++)
		{
			CryptBlock(m_enc_subkeys[0], (unsigned char*)input.data() + (i * 8));
			CryptBlock(m_dec_subkeys[1], (unsigned char*)input.data() + (i * 8));
			CryptBlock(m_enc_subkeys[2], (unsigned char*)input.data() + (i * 8));
		}
	}

	void TripleDESParallel::DecryptInPlace(std::string& input) const
	{
		if (input.size() == 0 || input.size() % k_block_size != 0)
		{
			throw Exception{};
		}

		int i;

#pragma omp parallel for num_threads(16)
		for (i = 0; i < input.size() / k_block_size; i++)
		{
			CryptBlock(m_dec_subkeys[0], (unsigned char*)input.data() + (i * 8));
			CryptBlock(m_enc_subkeys[1], (unsigned char*)input.data() + (i * 8));
			CryptBlock(m_dec_subkeys[2], (unsigned char*)input.data() + (i * 8));
		}
	}
}