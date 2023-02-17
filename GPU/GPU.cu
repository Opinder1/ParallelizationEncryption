#include "AES.h"

#include "../Shared/Shared.h"

#if EXE
int main()
{
	aes::gpu::AES aes(BinToStr("10101010101110110000100100011000001001110011011011001100110111011010101010111011000010010001100000100111001101101100110011011101"));
	
	{
		std::string in(1024 * 1024 * 10, 'A');

		aes.Encrypt(in);

		aes.Decrypt(in);
	}

	{
		std::string in(64, 'A');

		PrintHexStr("in", in);

		aes.Encrypt(in);

		PrintHexStr("enc", in);

		aes.Decrypt(in);

		PrintHexStr("dec", in);
	}
}
#endif