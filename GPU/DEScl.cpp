#include "DEScl.h"

namespace opencl::des
{
	const unsigned char key_perm_l[56] = {
		3, 2, 1, 0, 3, 2, 1, 0,
		3, 2, 1, 0, 7, 6, 5, 4,
		3, 2, 1, 0, 7, 6, 5, 4,
		7, 6, 5, 4, 7, 6, 5, 4,
		7, 6, 5, 4, 3, 2, 1, 0,
		7, 6, 5, 4, 3, 2, 1, 0,
		7, 6, 5, 4, 3, 2, 1, 0,
	};

	const unsigned char key_perm_r[56] = {
		8, 8, 8, 8, 16, 16, 16, 16,
		4, 4, 4, 4, 8, 8, 8, 8,
		2, 2, 2, 2, 4, 4, 4, 4,
		16, 16, 16, 16, 2, 2, 2, 2,
		32, 32, 32, 32, 32, 32, 32, 32,
		64, 64, 64, 64, 64, 64, 64, 64,
		128, 128, 128, 128, 128, 128, 128, 128,
	};

	const unsigned char left_round_perm_l[24] = {
		3, 0, 1, 0, 3, 2, 1, 0,
		1, 0, 2, 1, 2, 2, 1, 0,
		1, 2, 1, 2, 0, 0, 0, 3,
	};

	const unsigned char left_round_perm_r[24] = {
		64, 1, 1, 2, 32, 16, 8, 64,
		2, 4, 8, 64, 2, 32, 16, 16,
		4, 128, 32, 1, 128, 8, 32, 16,
	};

	const unsigned char right_round_perm_l[24] = {
		4, 6, 5, 5, 6, 4, 3, 3,
		6, 5, 4, 5, 5, 6, 4, 6,
		5, 6, 3, 4, 5, 6, 3, 4,
	};

	const unsigned char right_round_perm_r[24] = {
		64, 8, 4, 64, 64, 16, 8, 1,
		32, 8, 128, 1, 16, 128, 2, 1,
		128, 16, 2, 8, 2, 2, 4, 1,
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

	DES::DES(const std::string& key, size_t group_size) :
		EncryptBase(key),
		m_program("des.cl"),
		m_enc_id(m_program.MakeFunction("crypt")),
		m_dec_id(m_program.MakeFunction("crypt"))
	{
		if (group_size == 0 || group_size > (SIZE_MAX / k_block_size))
		{
			throw Exception{};
		}

		if (key.size() != k_min_key_size)
		{
			throw Exception{};
		}

		unsigned char subkeys[192] = { 0 };

		unsigned char* enc_subkeys = subkeys;
		unsigned char* dec_subkeys = subkeys + (6 * 16) + (6 * 15);

		unsigned char temp[7] = { 0 };
		Permute((const unsigned char*)key.data(), temp, key_perm_l, key_perm_r, 56);

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

		m_program.AddArg<ArrayInput>(m_enc_id);
		m_program.AddArg<IntInput>(m_enc_id);
		m_program.AddArg<ArrayIO>(m_enc_id);

		m_program.UpdateArg<ArrayInput>(m_enc_id, 0, [&](auto& arg) {arg.SetData(subkeys, 96); });

		m_program.AddArg<ArrayInput>(m_dec_id);
		m_program.AddArg<IntInput>(m_dec_id);
		m_program.AddArg<ArrayIO>(m_dec_id);

		m_program.UpdateArg<ArrayInput>(m_dec_id, 0, [&](auto& arg) {arg.SetData(subkeys + 96, 96); });
	}

	DES::~DES()
	{

	}

	void DES::EncryptInPlace(std::string& input) const
	{
		if (input.size() == 0 || input.size() % k_block_size != 0)
		{
			throw Exception{};
		}

		m_program.UpdateArg<ArrayIO>(m_enc_id, 2, [&](auto& arg) { arg.SetData(input.data(), input.size()); });

		unsigned int num = (unsigned int)input.size() / k_block_size;

		m_program.UpdateArg<IntInput>(m_enc_id, 1, [&](auto& arg) { arg.SetData(m_rounds); });

		m_program.RunFunction(m_enc_id, num);
	}

	void DES::DecryptInPlace(std::string& input) const
	{
		if (input.size() == 0 || input.size() % k_block_size != 0)
		{
			throw Exception{};
		}

		m_program.UpdateArg<ArrayIO>(m_dec_id, 2, [&](auto& arg) { arg.SetData(input.data(), input.size()); });

		unsigned int num = (unsigned int)input.size() / k_block_size;

		m_program.UpdateArg<IntInput>(m_dec_id, 1, [&](auto& arg) { arg.SetData(m_rounds); });

		m_program.RunFunction(m_dec_id, num);
	}

	TripleDES::TripleDES(const std::string& key, size_t group_size) :
		EncryptBase(key),
		m_program("des.cl"),
		m_enc_id(m_program.MakeFunction("triplecrypt")),
		m_dec_id(m_program.MakeFunction("triplecrypt"))
	{
		if (group_size == 0 || group_size > (SIZE_MAX / k_block_size))
		{
			throw Exception{};
		}

		if (key.size() != k_min_key_size)
		{
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

		m_program.AddArg<ArrayInput>(m_enc_id);
		m_program.AddArg<IntInput>(m_enc_id);
		m_program.AddArg<ArrayIO>(m_enc_id);

		m_program.UpdateArg<ArrayInput>(m_enc_id, 0, [&](auto& arg) {arg.SetData(subkeys, 96 * 3); });

		m_program.AddArg<ArrayInput>(m_dec_id);
		m_program.AddArg<IntInput>(m_dec_id);
		m_program.AddArg<ArrayIO>(m_dec_id);

		m_program.UpdateArg<ArrayInput>(m_dec_id, 0, [&](auto& arg) {arg.SetData(subkeys[3], 96 * 3); });
	}

	TripleDES::~TripleDES()
	{

	}

	void TripleDES::EncryptInPlace(std::string& input) const
	{
		if (input.size() == 0 || input.size() % k_block_size != 0)
		{
			throw Exception{};
		}

		m_program.UpdateArg<ArrayIO>(m_enc_id, 2, [&](auto& arg) { arg.SetData(input.data(), input.size()); });

		unsigned int num = (unsigned int)input.size() / k_block_size;

		m_program.UpdateArg<IntInput>(m_enc_id, 1, [&](auto& arg) { arg.SetData(m_rounds); });

		m_program.RunFunction(m_enc_id, num);
	}

	void TripleDES::DecryptInPlace(std::string& input) const
	{
		if (input.size() == 0 || input.size() % k_block_size != 0)
		{
			throw Exception{};
		}

		m_program.UpdateArg<ArrayIO>(m_dec_id, 2, [&](auto& arg) { arg.SetData(input.data(), input.size()); });

		unsigned int num = (unsigned int)input.size() / k_block_size;

		m_program.UpdateArg<IntInput>(m_dec_id, 1, [&](auto& arg) { arg.SetData(m_rounds); });

		m_program.RunFunction(m_dec_id, num);
	}
}