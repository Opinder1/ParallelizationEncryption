#include "TimeTest.h"

#include "../Shared/Shared.h"

#include <typeinfo>
#include <fstream>

void TimeEncryptImpl(const EncryptBase& alg, const std::string& in, std::string& out)
{
	out = alg.Encrypt(in);
};

double TimeEncrypt(size_t num, const EncryptBase& alg, const std::string& in, std::string& out)
{
	return TimeFunc(num, TimeEncryptImpl, alg, in, out);
}

void TimeAndGraph(const char* file_name, const char* name, const std::vector<EncryptBase*> algorithms, TimeTest&& test)
{
	printf(Format("TimeAndGraph %s\n", name).c_str());

	std::string command;
	command.append("from matplotlib import pyplot\n");
	//command.append("from scipy.signal import savgol_filter\n");

	for (auto algorithm : algorithms)
	{
		printf("%s\n", algorithm->GetName().c_str());

		std::vector<double> x;
		std::vector<double> y;

		test.ApplyAlgorithm(*algorithm, x, y);

		command.append("x = [");
		for (double value : x)
		{
			command.append(std::to_string(value));
			command.append(",");
		}
		command.append("]\n");

		command.append("y = [");
		for (double value : y)
		{
			command.append(std::to_string(value));
			command.append(",");
		}
		command.append("]\n");

		//command.append("z = savgol_filter(y, 50, 2)\n");

		command.append("pyplot.plot(x, y, label = '");
		command.append(algorithm->GetName());
		command.append("')\n");
	}

	command.append("pyplot.legend()\n");
	//command.append(Format("pyplot.title('%s')\n", name));

	// Second command to generate graph without logs
	std::string command2 = command;

	command.append(Format("pyplot.xlabel('%s %s')\n", test.GetXAxisName(), test.ShouldMakeXLog() ? "(log)" : ""));
	command.append(Format("pyplot.ylabel('%s')\n", test.GetYAxisName()));

	if (test.ShouldMakeXLog()) // Only use log for the x axis
	{
		command.append("pyplot.xscale('log')\n");
	}

	if (file_name)
	{
		command.append(Format("pyplot.savefig('%s')\n", file_name));
	}
	else
	{
		command.append("pyplot.show()\n");
	}

	{
		std::ofstream graph_file("graph.py");
		graph_file << command;
	}

	// Easy way of using matplotlib with c++ for testing purposes since matplotlib is very easy to use unlike C++ libraries
	system("python graph.py");

	// Make a second graph with log axis
	if (test.ShouldMakeLogGraph())
	{
		command2.append(Format("pyplot.xlabel('%s (log)')\n", test.GetXAxisName()));
		command2.append(Format("pyplot.ylabel('%s (log)')\n", test.GetYAxisName()));

		// Only apply log axis to one graph
		command2.append("pyplot.xscale('log')\n");
		command2.append("pyplot.yscale('log')\n");

		if (file_name)
		{
			command2.append(Format("pyplot.savefig('%s_LOG')\n", file_name));
		}
		else
		{
			command2.append("pyplot.show()\n");
		}

		{
			std::ofstream graph_file("graph.py");
			graph_file << command2;
		}

		system("python graph.py");
	}
}