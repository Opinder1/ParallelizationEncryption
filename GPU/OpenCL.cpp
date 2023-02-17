#include "OpenCl.h"

#include <CL/opencl.h>

#include <fstream>
#include <vector>

cl_device_id CreateDevice() {

    cl_platform_id platform;
    cl_device_id dev;
    int err;

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

cl_program CreateProgramFromData(cl_context ctx, const std::vector<char>& data)
{
    int err;

    const char* ptr = data.data();
    size_t size = data.size();
    cl_program program = clCreateProgramWithSource(ctx, 1, &ptr, &size, &err);

    if (err < 0)
    {
        return nullptr;
    }

    return program;
}

cl_program CreateProgramFromFile(cl_context ctx, const std::string& path)
{
    std::fstream stream(path);

    bool i = stream.is_open();
    
    printf("%i\n", i);

    return CreateProgramFromData(ctx, ReadWholeStream(stream));
}

cl_program BuildProgram(cl_context ctx, cl_device_id dev, const std::string& path)
{
    cl_program program = CreateProgramFromFile(ctx, path);

    if (clBuildProgram(program, 1, &dev, NULL, NULL, NULL) < 0)
    {
        size_t log_size;

        clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

        std::string log(log_size, '\0');

        clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, log_size + 1, log.data(), NULL);
        printf("%s\n", log.c_str());

        return nullptr;
    }

    return program;
}

namespace
{
    cl_device_id k_device = nullptr;
    cl_context k_context = nullptr;
    cl_command_queue k_queue = nullptr;
}

void InitOpenCL()
{
    int err;

    k_device = CreateDevice();
    k_context = clCreateContext(NULL, 1, &k_device, NULL, NULL, &err);
    k_queue = clCreateCommandQueue(k_context, k_device, 0, &err);
}

void ShutdownOpenCL()
{
    clReleaseCommandQueue(k_queue);

    clReleaseContext(k_context);

    clReleaseDevice(k_device);
}

Arg::Arg()
{

}

Arg::~Arg()
{

}

IntInput::IntInput()
{
    m_data = 0;
}

void IntInput::SetData(size_t data)
{
    m_data = data;
}

void IntInput::Apply(void* kernel, size_t index) const
{
    int err = clSetKernelArg((cl_kernel)kernel, index, sizeof(m_data), &m_data);
}

ArrayInput::ArrayInput()
{
    m_buffer = clCreateBuffer(k_context, CL_MEM_READ_ONLY, 1024 * 1024, NULL, NULL);
}

ArrayInput::~ArrayInput()
{
    if (m_buffer)
    {
        clReleaseMemObject((cl_mem)m_buffer);
    }
}

void ArrayInput::SetData(const char* data, size_t size)
{
    if (data && size > 0)
    {
        int err = clEnqueueWriteBuffer(k_queue, (cl_mem)m_buffer, CL_TRUE, 0, size, data, 0, NULL, NULL);
    }
}

void ArrayInput::Apply(void* kernel, size_t index) const
{
    int err = clSetKernelArg((cl_kernel)kernel, index, sizeof(cl_mem), m_buffer);
}

ArrayIO::ArrayIO()
{
    m_buffer = clCreateBuffer(k_context, CL_MEM_WRITE_ONLY, 1024 * 1024, NULL, NULL);

    m_data = nullptr;
    m_size = 0;
}

ArrayIO::~ArrayIO()
{
    if (m_buffer)
    {
        clReleaseMemObject((cl_mem)m_buffer);
    }
}

void ArrayIO::SetData(char* data, size_t size)
{
    m_data = data;
    m_size = size;

    if (m_data && m_size > 0)
    {
        int err = clEnqueueWriteBuffer(k_queue, (cl_mem)m_buffer, CL_TRUE, 0, m_size, m_data, 0, NULL, NULL);
    }
}

void ArrayIO::Apply(void* kernel, size_t index) const
{
    int err = clSetKernelArg((cl_kernel)kernel, index, sizeof(cl_mem), m_buffer);
}

void ArrayIO::Read() const
{
    if (m_data && m_size > 0)
    {
        clEnqueueReadBuffer(k_queue, (cl_mem)m_buffer, CL_TRUE, 0, m_size, m_data, 0, NULL, NULL);
    }
}

Program::Program(const std::string& file)
{
    m_program = BuildProgram(k_context, k_device, file);
}

Program::~Program()
{
    clReleaseProgram((cl_program)m_program);
}

Program::FunctionID Program::GetFunction(const std::string& name)
{
    int err;

    FunctionID id = m_functions.find(name);

    if (id == m_functions.end())
    {
        auto [new_id, success] = m_functions.emplace(name, Function());

        new_id->second.kernel = clCreateKernel((cl_program)m_program, name.c_str(), &err);

        return new_id;
    }
    
    return id;
}

void Program::AddArg(FunctionID id, Arg&& arg)
{
    Function& function = id->second;

    arg.Apply((cl_kernel)function.kernel, function.args.size());

    function.args.emplace_back(std::make_unique<Arg>(std::move(arg)));
}

Arg& Program::GetArg(FunctionID id, size_t index)
{
    Function& function = id->second;

    return *function.args[index];
}

void Program::RunFunction(FunctionID id)
{
    int err;

    Function& function = id->second;

    size_t global_size, local_size;

    err = clGetKernelWorkGroupInfo((cl_kernel)function.kernel, k_device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(local_size), &local_size, NULL);

    global_size = ceil(1000000 / (float)local_size) * local_size;

    printf("global=%zu, local=%zu\n", global_size, local_size);

    err = clEnqueueNDRangeKernel(k_queue, (cl_kernel)function.kernel, 1, NULL, &global_size, &local_size, 0, NULL, NULL);

    clFinish(k_queue);

    for (std::unique_ptr<Arg>& arg : function.args)
    {
        arg->Read();
    }
}