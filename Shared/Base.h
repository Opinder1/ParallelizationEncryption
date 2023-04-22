#pragma once

#include <string>

class EncryptBase
{
public:
	EncryptBase(const std::string& key) {}

	std::string GetKey() const { return m_key; }

	virtual std::string GetName() const = 0;

	virtual void EncryptInPlace(std::string& input) const = 0;

	virtual void DecryptInPlace(std::string& input) const = 0;

	inline std::string Encrypt(const std::string& input) const
	{
		std::string output = input;

		EncryptInPlace(output);

		return output;
	}

	virtual std::string Decrypt(const std::string& input) const
	{
		std::string output = input;

		DecryptInPlace(output);

		return output;
	}

private:
	std::string m_key;
};