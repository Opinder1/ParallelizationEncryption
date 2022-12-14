#pragma once

#include <string>
#include <chrono>
#include <bitset>

struct Array
{
	const char* data;
	size_t size;
};

inline std::string StrToHex(std::string_view input)
{
	const std::string strs = "0123456789ABCDEF";

	std::string out;

	for (size_t i = 0; i < input.size(); i++)
	{
		unsigned char a = (unsigned char)input[i] / 16;
		unsigned char b = (unsigned char)input[i] % 16;

		out.append(1, strs[a]);
		out.append(1, strs[b]);
	}

	return out;
}

inline std::string HexToBin(std::string_view input)
{
	const std::string strs = "0123456789ABCDEF";

	std::string output;

	for (size_t i = 0; i < input.size(); i += 2)
	{
		unsigned char a = (unsigned char)strs.find_first_of(input[i]);
		unsigned char b = (unsigned char)strs.find_first_of(input[i + 1]);

		unsigned char val = (a * 16) + b;

		output.append(1, (char)val);
	}

	return output;
}

inline std::string StrToBinNew(std::string_view input, size_t bit_count)
{
	std::string out;

	size_t bits = bit_count ? bit_count : (input.size() * 8);

	for (size_t i = 0; i < bits; i++)
	{
		const unsigned char* d = (const unsigned char*)input.data() + i;

		out += '0' + (*d >> (i % 8) & 0x01);
	}

	return out;
}

inline std::string BinToStr(std::string_view input)
{
	std::string out((input.size() + 7) / 8, 0);

	for (size_t i = 0; i < input.size(); i++)
	{
		if (input[i] == '1')
		{
			out[i / 8] += 1 << (i % 8);
		}
	}

	return out;
}

inline std::string StrToBin(std::string_view input, size_t bit_count = 0)
{
	size_t bits = bit_count ? bit_count : (input.size() * 8);

	std::string out(bits, 0);

	for (size_t i = 0; i < bits; i++)
	{
		const unsigned char* d = (const unsigned char*)input.data() + (i / 8);

		out[i] = '0' + ((*d >> (i % 8)) & 0x01);
	}

	return out;
}

template<size_t S>
void RotateLeft1Bit(std::bitset<S>& set, size_t start, size_t size)
{
	bool first = set.test(start);

	size_t end = start + size;

	for (size_t i = start; i < end - 1; i++)
	{
		set[i] = set[i + 1];
	}

	set.set(end - 1, first);
}

template<size_t S>
void RotateLeft2Bits(std::bitset<S>& set, size_t start, size_t size)
{
	bool first = set.test(start);
	bool second = set.test(start + 1);

	size_t end = start + size;

	for (size_t i = start; i < end - 2; i++)
	{
		set[i] = set[i + 2];
	}

	set.set(end - 2, first);
	set.set(end - 1, second);
}

template<size_t A, size_t B>
std::bitset<A> Permute(std::bitset<B> set, int table[A])
{
	std::bitset<A> out;

	for (size_t i = 0; i < A; i++) {
		out[i] = set[table[i] - 1];
	}
	return out;
}

template<size_t A, size_t B>
std::bitset<A + B> Append(std::bitset<A> a, std::bitset<B> b)
{
	std::bitset<A + B> out;

	for (size_t i = 0; i < A; i++)
	{
		out[i] = a[i];
	}

	for (size_t i = 0; i < B; i++)
	{
		out[A + i] = b[i];
	}

	return out;
}

inline void PrintUChars(const char* msg, std::string_view input)
{
	printf("%s:\n", msg);
	for (size_t i = 0; i < input.size(); i++)
	{
		printf("%3.u ", (unsigned char)input[i]);
	}
	printf("\n");
}

inline void PrintHex(const char* msg, std::string_view input)
{
	printf("%s: %s\n", msg, StrToHex(input).c_str());
}

template<class T>
void PrintHex(const char* msg, const T* ptr)
{
	std::string_view input((const char*)ptr, sizeof(T));

	printf("%s: %s\n", msg, StrToHex(input).c_str());
}

inline void PrintBin(const char* msg, std::string_view input)
{
	printf("%s: %s\n", msg, StrToBin(input, 0).c_str());
}

template<class T>
void PrintBin(const char* msg, const T* ptr, size_t bit_count = 0)
{
	std::string_view input((const char*)ptr, sizeof(T));

	printf("%s: %s\n", msg, StrToBin(input, bit_count).c_str());
}

template<class F, class... Args>
inline float TimeFunc(size_t num, const F& function, Args&... args)
{
	auto t1 = std::chrono::steady_clock::now();

	for (size_t i = 0; i < num; i++)
	{
		function(args...);
	}

	auto t2 = std::chrono::steady_clock::now();

	auto time_nano = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1);
	auto time_sec = time_nano.count() / 1000000000.0f;

	return time_sec;
}