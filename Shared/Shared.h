#pragma once

#include <array>
#include <string>
#include <string_view>
#include <bitset>
#include <chrono>
#include <algorithm>

template<class F, class... Args>
inline double TimeFunc(size_t num, const F& function, Args&... args)
{
	// Error checking for number of iterations for median
	if (num == 0) num = 1;
	if (num > 16) num = 16;

	std::array<double, 16> times = { 0 };

	for (size_t i = 0; i < num; i++)
	{
		auto t1 = std::chrono::steady_clock::now();

		function(args...);

		auto t2 = std::chrono::steady_clock::now();

		auto time_nano = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1);
		double time_sec = time_nano.count() / 1000000000.0;

		times[i] = time_sec;
	}

	// Sort for median using only the first num elements that were written to
	std::sort(times.begin(), times.begin() + num);
	
	return times[num / 2];
}

inline char ToLower(char input)
{
	if (input >= 'A' && input <= 'Z')
	{
		return input + ('a' - 'A');
	}
	else
	{
		return input;
	}
}

inline std::string BinToStr(std::string_view input)
{
	std::string output((input.size() + 7) / 8, 0);

	for (size_t i = 0; i < input.size(); i++)
	{
		if (input[i] == '1')
		{
			output[i / 8] += 1 << (i % 8);
		}
	}

	return output;
}

inline std::string StrToBinOld(std::string_view input, size_t bit_count = 0)
{
	size_t bits = bit_count ? bit_count : (input.size() * 8);

	std::string output(bits, 0);

	for (size_t i = 0; i < bits; i++)
	{
		const unsigned char* d = (const unsigned char*)input.data() + (i / 8);

		output[i] = '0' + ((*d >> (i % 8)) & 0x01);
	}

	return output;
}

inline std::string StrToBin(std::string_view input, size_t bit_count = 0)
{
	size_t bits = bit_count ? bit_count : (input.size() * 8);

	bits = (bits * 9) / 8;

	std::string output(bits, 0);

	size_t bit = 0;
	size_t i = 0;
	for (size_t bit = 0; bit < bits; bit++)
	{
		if (bit % 9 == 0)
		{
			output[bit] = ' ';
		}
		else
		{
			const unsigned char* d = (const unsigned char*)input.data() + (i / 8);

			output[bit] = '0' + ((*d >> (7 - (i % 8))) & 0x01);

			i++;
		}
	}

	return output;
}

inline std::string StrToHex(std::string_view input)
{
	const std::string strs = "0123456789abcdef";

	std::string output;

	for (size_t i = 0; i < input.size(); i++)
	{
		unsigned char a = (unsigned char)input[i] / 16;
		unsigned char b = (unsigned char)input[i] % 16;

		output.append(1, strs[a]);
		output.append(1, strs[b]);
	}

	return output;
}

inline std::string HexToStr(std::string_view input)
{
	const std::string strs = "0123456789abcdef";

	std::string output;

	for (size_t i = 0; i < input.size(); i += 2)
	{
		unsigned char a = (unsigned char)strs.find_first_of(ToLower(input[i]));
		unsigned char b = (unsigned char)strs.find_first_of(ToLower(input[i + 1]));

		unsigned char val = (a * 16) + b;

		output.append(1, (char)val);
	}

	return output;
}

inline std::string BinToHex(std::string_view input)
{
	return StrToHex(BinToStr(input));
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
std::bitset<A> Permute(std::bitset<B> set, unsigned char table[A])
{
	std::bitset<A> out;

	for (size_t i = 0; i < A; i++)
	{
		out.set(i, set.test(B - table[i] - 1));
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

inline void PrintHexStr(const char* msg, std::string_view input)
{
	printf("%s: %s\n", msg, StrToHex(input).c_str());
}

template<class T>
void PrintHex(const char* msg, const T* ptr)
{
	std::string_view input((const char*)ptr, sizeof(T));

	printf("%s: %s\n", msg, StrToHex(input).c_str());
}

inline void PrintBinStr(const char* msg, std::string_view input)
{
	printf("%s: %s\n", msg, StrToBin(input, 0).c_str());
}

template<class T>
void PrintBin(const char* msg, const T* ptr, size_t bit_count = 0)
{
	std::string_view input((const char*)ptr, sizeof(T));

	printf("%s: %s\n", msg, StrToBin(input, bit_count).c_str());
}

inline size_t CeilSize(size_t val, size_t size)
{
	return (((val - 1) / size) + 1) * size;
}

template<class... Args>
std::string Format(const char* format, Args&&... args)
{
	char buffer[256];
	sprintf_s(buffer, format, std::forward<Args>(args)...);
	return std::string(buffer);
}

inline int SizeToInt(size_t size)
{
	if (size > size_t(INT_MAX))
	{
		return int(INT_MAX);
	}
	else
	{
		return int(size);
	}
}