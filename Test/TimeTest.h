#pragma once

#include "../Shared/Base.h"

#include <chrono>
#include <string>
#include <vector>

struct TimeTest
{
	virtual void ApplyAlgorithm(const EncryptBase& algorithm, std::string& x, std::string& y) = 0;

	virtual const char* GetXAxisName() = 0;

	virtual const char* GetYAxisName() = 0;

	virtual bool ShouldMakeLogGraph() = 0;

	virtual bool ShouldMakeXLog() = 0;
};

double TimeEncrypt(size_t num, const EncryptBase& alg, const std::string& in, std::string& out);

void TimeAndGraph(const char* name, const std::vector<EncryptBase*> algorithms, TimeTest&& test);