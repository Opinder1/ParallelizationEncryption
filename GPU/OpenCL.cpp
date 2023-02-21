#include "OpenCl.h"

#include <CL/opencl.h>

#include <fstream>
#include <vector>

cl_device_id CreateDevice(int& err) {

    cl_platform_id platform;
    cl_device_id dev;

    err = clGetPlatformIDs(1, &platform, NULL);

    if (err < 0)
    {
        return nullptr;
    }

    // Access a device
    // GPU
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &dev, NULL);

    if (err == CL_DEVICE_NOT_FOUND)
    {
        // CPU
        err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, NULL);
    }

    if (err < 0)
    {
        return nullptr;
    }

    return dev;
}

std::vector<char> ReadWholeStream(std::istream& stream)
{
    stream.seekg(0, stream.end);

    size_t s = stream.tellg();

    stream.seekg(0, stream.beg);

    std::vector<char> buffer;

    buffer.resize(s);

    stream.read(buffer.data(), buffer.size());

    return buffer;
}

cl_program CreateProgramFromData(cl_context ctx, const std::vector<char>& data, int& err)
{
    const char* ptr = data.data();
    size_t size = data.size();
    cl_program program = clCreateProgramWithSource(ctx, 1, &ptr, &size, &err);

    if (err < 0)
    {
        return nullptr;
    }

    return program;
}

cl_program CreateProgramFromFile(cl_context ctx, const std::string& path, int& err)
{
    std::fstream stream(path);

    bool i = stream.is_open();

    return CreateProgramFromData(ctx, ReadWholeStream(stream), err);
}

cl_program BuildProgram(cl_context ctx, cl_device_id dev, const std::string& path, int& err)
{
    cl_program program = CreateProgramFromFile(ctx, path, err);

    if (err < 0)
    {
        return nullptr;
    }

    err = clBuildProgram(program, 1, &dev, NULL, NULL, NULL);

    if (err < 0)
    {
        size_t log_size;

        clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

        std::string log(log_size, '\0');

        clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, log_size + 1, log.data(), NULL);
        printf("CL ERROR %s\n", log.c_str());

        return nullptr;
    }

    return program;
}

namespace opencl
{
    cl_device_id k_device = nullptr;
    cl_context k_context = nullptr;
    cl_command_queue k_queue = nullptr;

    void InitOpenCL()
    {
        int err;

        k_device = CreateDevice(err);

        if (err < 0)
        {
            throw;
        }

        k_context = clCreateContext(NULL, 1, &k_device, NULL, NULL, &err);

        if (err < 0)
        {
            throw;
        }

        k_queue = clCreateCommandQueue(k_context, k_device, 0, &err);

        if (err < 0)
        {
            throw;
        }
    }

    void ShutdownOpenCL()
    {
        if (clReleaseCommandQueue(k_queue) < 0)
        {
            throw;
        }

        if (clReleaseContext(k_context) < 0)
        {
            throw;
        }

        if (clReleaseDevice(k_device) < 0)
        {
            throw;
        }
    }

    Arg::Arg(const Program& program) :
        m_program(program)
    {

    }

    Arg::~Arg()
    {

    }

    void Arg::HandleError(int err) const
    {
        m_program._HandleError(err);
    }

    IntInput::IntInput(const Program& program) :
        Arg(program)
    {
        m_data = 0;
    }

    void IntInput::SetData(size_t data)
    {
        m_data = data;
    }

    void IntInput::Apply(void* kernel, size_t index) const
    {
        HandleError(clSetKernelArg((cl_kernel)kernel, index, sizeof(m_data), &m_data));
    }

    ArrayInput::ArrayInput(const Program& program) :
        Arg(program)
    {
        m_buffer = nullptr;
        m_size = 0;
    }

    ArrayInput::~ArrayInput()
    {
        Dealloc();
    }

    void ArrayInput::SetData(const void* data, size_t size)
    {
        if (data && size > 0)
        {
            if (size > m_size)
            {
                Realloc(size);
            }

            HandleError(clEnqueueWriteBuffer(k_queue, (cl_mem)m_buffer, CL_TRUE, 0, size, data, 0, NULL, NULL));

            m_size = size;
        }
    }

    void ArrayInput::Apply(void* kernel, size_t index) const
    {
        if (m_buffer)
        {
            HandleError(clSetKernelArg((cl_kernel)kernel, index, sizeof(m_buffer), &m_buffer));
        }
    }

    void ArrayInput::Dealloc()
    {
        if (m_buffer)
        {
            HandleError(clReleaseMemObject((cl_mem)m_buffer));
            m_buffer = nullptr;
        }
    }

    void ArrayInput::Realloc(size_t size)
    {
        Dealloc();

        int err;
        m_buffer = clCreateBuffer(k_context, CL_MEM_READ_ONLY, size, NULL, &err);
        HandleError(err);
    }

    ArrayIO::ArrayIO(const Program& program) :
        Arg(program)
    {
        m_buffer = nullptr;
        m_data = nullptr;
        m_size = 0;
    }

    ArrayIO::~ArrayIO()
    {
        Dealloc();
    }

    void ArrayIO::SetData(void* data, size_t size)
    {
        if (data && size > 0)
        {
            if (size > m_size)
            {
                Realloc(size);
            }

            HandleError(clEnqueueWriteBuffer(k_queue, (cl_mem)m_buffer, CL_TRUE, 0, size, data, 0, NULL, NULL));

            m_data = data;
            m_size = size;
        }
    }

    void ArrayIO::Apply(void* kernel, size_t index) const
    {
        if (m_buffer)
        {
            HandleError(clSetKernelArg((cl_kernel)kernel, index, sizeof(m_buffer), &m_buffer));
        }
    }

    void ArrayIO::Read() const
    {
        if (m_data && m_size > 0)
        {
            HandleError(clEnqueueReadBuffer(k_queue, (cl_mem)m_buffer, CL_TRUE, 0, m_size, m_data, 0, NULL, NULL));
        }
    }

    void ArrayIO::Dealloc()
    {
        if (m_buffer)
        {
            HandleError(clReleaseMemObject((cl_mem)m_buffer));
            m_buffer = nullptr;
        }
    }

    void ArrayIO::Realloc(size_t size)
    {
        Dealloc();

        int err;
        m_buffer = clCreateBuffer(k_context, CL_MEM_READ_WRITE, size, NULL, &err);
        HandleError(err);
    }

    Program::Program(const std::string& file)
    {
        int err;

        m_program = BuildProgram(k_context, k_device, file, err);

        _HandleError(err);
    }

    Program::~Program()
    {
        _HandleError(clReleaseProgram((cl_program)m_program));
    }

    Program::FunctionID Program::MakeFunction(const std::string& name)
    {
        Function& function = m_functions.emplace_back(Function());

        int err;

        function.kernel = clCreateKernel((cl_program)m_program, name.c_str(), &err);

        _HandleError(err);

        return m_functions.size() - 1;
    }

    Arg& Program::_GetArg(FunctionID id, size_t index, void*& kernel) const
    {
        const Function& function = m_functions[id];

        kernel = function.kernel;

        return *function.args[index];
    }

    void Program::RunFunction(FunctionID id, size_t elements) const
    {
        _HandleError(clFinish(k_queue));

        const Function& function = m_functions[id];

        _HandleError(clEnqueueNDRangeKernel(k_queue, (cl_kernel)function.kernel, 1, NULL, &elements, NULL, 0, NULL, NULL));

        _HandleError(clFinish(k_queue));

        for (const std::unique_ptr<Arg>& arg : function.args)
        {
            arg->Read();
        }

        _HandleError(clFinish(k_queue));
    }

    void Program::_HandleError(int err) const
    {
        m_err = err;

        if (err < 0)
        {
            throw;
        }
    }
}