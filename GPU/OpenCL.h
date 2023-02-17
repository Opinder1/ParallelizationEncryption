#pragma once

#include <string>
#include <memory>
#include <unordered_map>

void InitOpenCL();

void ShutdownOpenCL();

class Arg
{
	friend class Program;

public:
	Arg(const Arg&) = delete;
	Arg& operator=(const Arg&) = delete;

	Arg(Arg&&) = default;
	Arg& operator=(Arg&&) = default;

	Arg();
	~Arg();

private:
	virtual void Apply(void* kernel, size_t index) const {};

	virtual void Read() const {};
};

class IntInput : public Arg
{
public:
	IntInput();

	void SetData(size_t data);

private:
	void Apply(void* kernel, size_t index) const override;

	void Read() const override {};

private:
	size_t m_data;
};

class ArrayInput : public Arg
{
public:
	ArrayInput();

	~ArrayInput();

	void SetData(const char* data, size_t size);

private:
	void Apply(void* kernel, size_t index) const override;

	void Read() const override {};

private:
	void* m_buffer;
};

class ArrayIO : public Arg
{
public:
	ArrayIO();

	~ArrayIO();

	void SetData(char* data, size_t size);

private:
	void Apply(void* kernel, size_t index) const override;

	void Read() const override;

private:
	void* m_buffer;

	char* m_data;
	size_t m_size;
};

class Program
{
	struct Function
	{
		void* kernel;

		std::vector<std::unique_ptr<Arg>> args;
	};

	using Map = std::unordered_map<std::string, Function>;

	using FunctionID = Map::iterator;

public:
	Program(const std::string& file);

	~Program();

	FunctionID GetFunction(const std::string& name);

	void AddArg(FunctionID, Arg&& arg);

	Arg& GetArg(FunctionID, size_t index);

	void RunFunction(FunctionID);

private:
	void* m_program = nullptr;

	Map m_functions;
};