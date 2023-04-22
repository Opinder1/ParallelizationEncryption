#include "ErrorTest.h"

void TestEncrypt(const EncryptBase& base, size_t block_size)
{
	EXPECT_ANY_THROW(base.Encrypt(""));

	EXPECT_ANY_THROW(base.Encrypt("A"));

	EXPECT_ANY_THROW(base.Encrypt(std::string(block_size + 1, 'a')));

	EXPECT_NO_THROW(base.Encrypt(std::string(block_size, 'a')));

	EXPECT_NO_THROW(base.Encrypt(std::string(block_size * 3, 'a')));
}

void TestDecrypt(const EncryptBase& base, size_t block_size)
{
	EXPECT_ANY_THROW(base.Decrypt(""));

	EXPECT_ANY_THROW(base.Decrypt("A"));

	EXPECT_ANY_THROW(base.Decrypt(std::string(block_size + 1, 'a')));

	EXPECT_NO_THROW(base.Decrypt(std::string(block_size, 'a')));

	EXPECT_NO_THROW(base.Decrypt(std::string(block_size * 3, 'a')));
}