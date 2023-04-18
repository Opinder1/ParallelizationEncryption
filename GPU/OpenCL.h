#pragma once

#include "Api.h"

#include <string>
#include <memory>
#include <unordered_map>

namespace opencl
{
	struct CLException
	{
		const char* msg;
	};

	GPU_API void InitOpenCL();

	GPU_API void ShutdownOpenCL();

	class Program;

	class Arg
	{
		friend class Program;

	public:
		Arg(const Arg&) = delete;
		Arg& operator=(const Arg&) = delete;

		Arg(Arg&&) = default;
		Arg& operator=(Arg&&) = default;

		Arg(const Program& program);
		~Arg();

	protected:
		void HandleError(int err) const;

	private:
		virtual void Apply(void* kernel, size_t index) const {};

		virtual void Read() const {};

	private:
		const Program& m_program;
	};

	class IntInput : public Arg
	{
	public:
		IntInput(const Program& program);

		void SetData(size_t data);

	private:
		void Apply(void* kernel, size_t index) const override;

		void Read() const override {};

	private:
		unsigned int m_data;
	};

	class ArrayInput : public Arg
	{
	public:
		ArrayInput(const Program& program);

		~ArrayInput();

		void SetData(const void* data, size_t size);

	private:
		void Apply(void* kernel, size_t index) const override;

		void Read() const override {};

		void Dealloc();

		void Realloc(size_t size);

	private:
		void* m_buffer;

		size_t m_size;
	};

	class ArrayIO : public Arg
	{
	public:
		ArrayIO(const Program& program);

		~ArrayIO();

		void SetData(void* data, size_t size);

	private:
		void Apply(void* kernel, size_t index) const override;

		void Read() const override;

		void Dealloc();

		void Realloc(size_t size);

	private:
		void* m_buffer;

		void* m_data;
		size_t m_size;
	};

	class Program
	{
		friend Arg;

		struct Function
		{
			void* kernel;

			std::string name;

			std::vector<std::unique_ptr<Arg>> args;
		};

	public:
		using FunctionID = size_t;

	public:
		Program(const std::string& file);

		~Program();

		FunctionID MakeFunction(const std::string& name);

		template<class T>
		void AddArg(FunctionID id)
		{
			Function& function = m_functions[id];

			Arg& arg = *function.args.emplace_back(std::make_unique<T>(*this));

			arg.Apply(function.kernel, function.args.size() - 1);
		}

		template<class T, class Callable>
		void UpdateArg(FunctionID id, size_t index, Callable callable) const
		{
			void* kernel;

			Arg& arg = _GetArg(id, index, kernel);

			callable(dynamic_cast<T&>(arg));

			arg.Apply(kernel, index);
		}

		void RunFunction(FunctionID id, size_t elements) const;

	private:
		Arg& _GetArg(FunctionID id, size_t index, void*& kernel) const;

		void _HandleError(int err) const;

	private:
		void* m_program = nullptr;

		mutable int m_err = 0;

		std::vector<Function> m_functions;
	};
}