#pragma once

#include <string>

class EncryptBase
{
public:
	EncryptBase(const std::string& key) {}

	std::string GetKey() const { return m_key; }

	virtual std::string Encrypt(const std::string& input) const = 0;

	virtual std::string Decrypt(const std::string& input) const = 0;

private:
	std::string m_key;
};