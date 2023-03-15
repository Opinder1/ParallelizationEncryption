#pragma once

#include "../Shared/Base.h"

#include <string>

template<class T>
class Triple : public EncryptBase
{
public:
	static constexpr size_t k_block_size = T::k_block_size;
	static constexpr size_t k_min_key_size = T::k_min_key_size * 3;

	Triple(const std::string& key) :
		EncryptBase(key),
		a(key.substr(0, key.size() / 3)),
		b(key.substr(key.size() / 3, key.size() / 3)),
		c(key.substr((key.size() * 2) / 3, key.size() / 3))
	{

	}

	~Triple()
	{

	}

	void EncryptInPlace(std::string& input) const override
	{
		a.EncryptInPlace(input);
		b.DecryptInPlace(input);
		c.EncryptInPlace(input);
	}

	void DecryptInPlace(std::string& input) const override
	{
		a.DecryptInPlace(input);
		b.EncryptInPlace(input);
		c.DecryptInPlace(input);
	}

private:
	T a;
	T b;
	T c;
};