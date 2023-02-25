#pragma once

#include "../Shared/Base.h"

#include <string>

template<class T>
class Triple : EncryptBase
{
public:
	Triple(const std::string& key) :
		EncryptBase(key),
		a(key.substr(0, key.size() / 3)),
		a(key.substr(0, key.size() / 3)),
		a(key.substr(0, key.size() / 3))
	{

	}

	~Triple()
	{

	}

	std::string Encrypt(const std::string& input) const override
	{
		a.Encrypt(input);
	}

	std::string Decrypt(const std::string& input) const override
	{

	}

private:
	T a;
	T b;
	T c;
};