#include "TimeTest.h"

#include "../Shared/Shared.h"

#include <typeinfo>

void TimeEncryptImpl(const EncryptBase& alg, const std::string& in, std::string& out)
{
	out = alg.Encrypt(in);
	out = alg.Decrypt(out);
};

double TimeEncrypt(size_t num, const EncryptBase& alg, const std::string& in, std::string& out)
{
	return TimeFunc(num, TimeEncryptImpl, alg, in, out);
}

void TimeAndGraph(const char* name, const std::vector<EncryptBase*> algorithms, TimeTest&& test)
{
	printf(Format("TimeAndGraph %s\n", name).c_str());

	std::string command;
	command.append("python -c \"");
	command.append("from matplotlib import pyplot;");

	for (auto algorithm : algorithms)
	{
		printf("%s\n", algorithm->GetName().c_str());

		std::string x = "[";
		std::string y = "[";

		test.ApplyAlgorithm(*algorithm, x, y);

		x.append("],");
		y.append("],");

		command.append("pyplot.plot(");
		command.append(x);
		command.append(y);
		command.append("label='");
		command.append(algorithm->GetName());
		command.append("');");
	}

	command.append("pyplot.legend();");
	//command.append(Format("pyplot.title('%s algorithm speed based on data size input');", name));
	command.append(Format("pyplot.xlabel('%s');", test.GetXAxisName()));
	command.append(Format("pyplot.ylabel('%s');", test.GetYAxisName()));

	// Second command to generate graph without logs
	std::string command2 = command;

	if (test.ShouldMakeXLog()) // Only use log for the x axis
	{
		command.append("pyplot.xscale('log');");
	}

	command.append("pyplot.show();\"");

	// Easy way of using matplotlib with c++ for testing purposes
	system(command.c_str());

	// Make a second graph with log axis
	if (test.ShouldMakeLogGraph())
	{
		command2.append("pyplot.show();\"");

		// Only apply log axis to one graph
		command2.append("pyplot.xscale('log');");
		command2.append("pyplot.yscale('log');");

		system(command2.c_str());
	}
}