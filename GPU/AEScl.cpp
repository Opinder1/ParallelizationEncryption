#include "AEScl.h"

#include "OpenCL.h"

void TestAESCL()
{
	InitOpenCL();

	Program program("aes.cl");

	auto i = program.GetFunction("encrypt");

	program.AddArg(i, ArrayInput());
	program.AddArg(i, IntInput());
	program.AddArg(i, ArrayIO());

	ShutdownOpenCL();
}