
__constant const uchar initial_perm_l[64] = {
	0, 1, 2, 3, 4, 5, 6, 7,
	0, 1, 2, 3, 4, 5, 6, 7,
	0, 1, 2, 3, 4, 5, 6, 7,
	0, 1, 2, 3, 4, 5, 6, 7,
	0, 1, 2, 3, 4, 5, 6, 7,
	0, 1, 2, 3, 4, 5, 6, 7,
	0, 1, 2, 3, 4, 5, 6, 7,
	0, 1, 2, 3, 4, 5, 6, 7,
};

__constant const uchar initial_perm_r[64] = {
	64, 64, 64, 64, 64, 64, 64, 64,
	16, 16, 16, 16, 16, 16, 16, 16,
	4, 4, 4, 4, 4, 4, 4, 4,
	1, 1, 1, 1, 1, 1, 1, 1,
	128, 128, 128, 128, 128, 128, 128, 128,
	32, 32, 32, 32, 32, 32, 32, 32,
	8, 8, 8, 8, 8, 8, 8, 8,
	2, 2, 2, 2, 2, 2, 2, 2,
};

__constant const uchar final_perm_l[64] = {
	3, 7, 2, 6, 1, 5, 0, 4,
	3, 7, 2, 6, 1, 5, 0, 4,
	3, 7, 2, 6, 1, 5, 0, 4,
	3, 7, 2, 6, 1, 5, 0, 4,
	3, 7, 2, 6, 1, 5, 0, 4,
	3, 7, 2, 6, 1, 5, 0, 4,
	3, 7, 2, 6, 1, 5, 0, 4,
	3, 7, 2, 6, 1, 5, 0, 4,
};

__constant const uchar final_perm_r[64] = {
	1, 1, 1, 1, 1, 1, 1, 1,
	2, 2, 2, 2, 2, 2, 2, 2,
	4, 4, 4, 4, 4, 4, 4, 4,
	8, 8, 8, 8, 8, 8, 8, 8,
	16, 16, 16, 16, 16, 16, 16, 16,
	32, 32, 32, 32, 32, 32, 32, 32,
	64, 64, 64, 64, 64, 64, 64, 64,
	128, 128, 128, 128, 128, 128, 128, 128,
};

__constant const uchar expansion_l[48] = {
	0, 0, 0, 0, 0, 0, 0, 3,
	1, 1, 1, 0, 1, 0, 0, 0,
	2, 1, 1, 1, 1, 1, 1, 1,
	2, 2, 2, 2, 2, 2, 2, 1,
	3, 3, 3, 2, 3, 2, 2, 2,
	0, 3, 3, 3, 3, 3, 3, 3,
};

__constant const uchar expansion_r[48] = {
	8, 16, 8, 16, 32, 64, 128, 1,
	32, 64, 128, 1, 128, 1, 2, 4,
	128, 1, 2, 4, 8, 16, 8, 16,
	8, 16, 8, 16, 32, 64, 128, 1,
	32, 64, 128, 1, 128, 1, 2, 4,
	128, 1, 2, 4, 8, 16, 8, 16,
};

__constant const uchar pbox_l[32] = {
	2, 3, 1, 3, 2, 2, 0, 1,
	1, 3, 2, 0, 3, 2, 1, 0,
	1, 0, 3, 3, 1, 2, 0, 0,
	3, 0, 1, 2, 0, 3, 1, 2,
};

__constant const uchar pbox_r[32] = {
	128, 16, 16, 8, 8, 16, 2, 1,
	64, 2, 64, 8, 64, 2, 2, 128,
	128, 32, 32, 1, 4, 1, 1, 64,
	128, 16, 32, 4, 4, 4, 8, 32,
};

__constant const uchar sbox[2][4][4][16] = {
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

void Permute(const uchar* set, uchar* out, __constant const uchar* table_div, __constant const uchar* table_modshift, size_t table_size)
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

inline uchar V(uchar byte, uchar from, uchar to)
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

uchar4 Compress(uchar8 input)
{
	uchar4 output = 0;

	for (size_t i = 0; i < 2; i++)
	{
		const uchar* in = (uchar*)&input + (i * 3);

		uchar* out = (uchar*)&output + (i * 2);

		{
			uchar row = V(in[0], 0, 1) | V(in[0], 5, 0);
			uchar column = V(in[0], 1, 3) | V(in[0], 2, 2) | V(in[0], 3, 1) | V(in[0], 4, 0);

			uchar val = sbox[i][0][row][column];

			out[0] |= val << 4;
		}

		{
			uchar row = V(in[0], 6, 1) | V(in[1], 3, 0);
			uchar column = V(in[0], 7, 3) | V(in[1], 0, 2) | V(in[1], 1, 1) | V(in[1], 2, 0);

			uchar val = sbox[i][1][row][column];

			out[0] |= val;
		}

		{
			uchar row = V(in[1], 4, 1) | V(in[2], 1, 0);
			uchar column = V(in[1], 5, 3) | V(in[1], 6, 2) | V(in[1], 7, 1) | V(in[2], 0, 0);

			uchar val = sbox[i][2][row][column];

			out[1] |= val << 4;
		}

		{
			uchar row = V(in[2], 2, 1) | V(in[2], 7, 0);
			uchar column = V(in[2], 3, 3) | V(in[2], 4, 2) | V(in[2], 5, 1) | V(in[2], 6, 0);

			uchar val = sbox[i][3][row][column];

			out[1] |= val;
		}
	}

	return output;
}

void crypt_block_ecb(__global const uchar* subkeys, uchar8* block)
{
	uchar8 temp = 0;

	Permute((uchar*)block, (uchar*)&temp, initial_perm_l, initial_perm_r, 64);

	uchar* left = (uchar*)&temp;
	uchar* right = (uchar*)&temp + 4;

	for (size_t key = 0; key < 16; key++)
	{
		const uchar* round_key = subkeys + (key * 6);

		uchar8 temp2 = 0;

		// Mangle
		{
			Permute(right, (uchar*)&temp2, expansion_l, expansion_r, 48);

			for (size_t i = 0; i < 6; i++)
			{
				temp2[i] ^= round_key[i];
			}

			uchar4 temp3 = Compress(temp2);

			Permute((uchar*)&temp3, (uchar*)&temp2, pbox_l, pbox_r, 32);
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

	Permute((uchar*)&temp, (uchar*)block, final_perm_l, final_perm_r, 64);
}

__kernel void crypt_ctr(__global const uchar* subkeys, __global uchar* blocks)
{
	size_t index = get_global_id(0);

	uchar8 block = vload8(index, blocks);
    
	uchar8 index_block = 0;
    
    *(size_t*)&index_block = index;

	crypt_block_ecb(subkeys, &index_block);
    
    for (size_t i = 0; i < 8; i++)
    {
        block[i] ^= index_block[i];
    }

	vstore8(block, index, blocks);
}

__kernel void triplecrypt_ctr(__global const uchar* subkeys, __global uchar* blocks)
{
	size_t index = get_global_id(0);

	uchar8 block = vload8(index, blocks);
    
	uchar8 index_block = 0;
    
    *(size_t*)&index_block = index;

	crypt_block_ecb(subkeys + (96 * 0), &index_block);
	crypt_block_ecb(subkeys + (96 * 1), &index_block);
	crypt_block_ecb(subkeys + (96 * 2), &index_block);
    
    for (size_t i = 0; i < 8; i++)
    {
        block[i] = block[i] ^ index_block[i];
    }

	vstore8(block, index, blocks);
}