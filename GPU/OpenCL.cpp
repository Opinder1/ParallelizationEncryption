#include "OpenCl.h"

#include <CL/opencl.h>

#include <fstream>
#include <vector>

namespace opencl
{
    const char* GetString(int err)
    {
        const char* names[63] =
        {
            "CL_SUCCESS",
            "CL_DEVICE_NOT_FOUND",
            "CL_DEVICE_NOT_AVAILABLE",
            "CL_COMPILER_NOT_AVAILABLE",
            "CL_MEM_OBJECT_ALLOCATION_FAILURE",
            "CL_OUT_OF_RESOURCES",
            "CL_OUT_OF_HOST_MEMORY",
            "CL_PROFILING_INFO_NOT_AVAILABLE",
            "CL_MEM_COPY_OVERLAP",
            "CL_IMAGE_FORMAT_MISMATCH",
            "CL_IMAGE_FORMAT_NOT_SUPPORTED",
            "CL_BUILD_PROGRAM_FAILURE",
            "CL_MAP_FAILURE",
            "CL_MISALIGNED_SUB_BUFFER_OFFSET",
            "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST",
            "CL_COMPILE_PROGRAM_FAILURE",
            "CL_LINKER_NOT_AVAILABLE",
            "CL_LINK_PROGRAM_FAILURE",
            "CL_DEVICE_PARTITION_FAILED",
            "CL_KERNEL_ARG_INFO_NOT_AVAILABLE",
            "CL_INVALID_VALUE",
            "CL_INVALID_DEVICE_TYPE",
            "CL_INVALID_PLATFORM",
            "CL_INVALID_DEVICE",
            "CL_INVALID_CONTEXT",
            "CL_INVALID_QUEUE_PROPERTIES",
            "CL_INVALID_COMMAND_QUEUE",
            "CL_INVALID_HOST_PTR",
            "CL_INVALID_MEM_OBJECT",
            "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR",
            "CL_INVALID_IMAGE_SIZE",
            "CL_INVALID_SAMPLER",
            "CL_INVALID_BINARY",
            "CL_INVALID_BUILD_OPTIONS",
            "CL_INVALID_PROGRAM",
            "CL_INVALID_PROGRAM_EXECUTABLE",
            "CL_INVALID_KERNEL_NAME",
            "CL_INVALID_KERNEL_DEFINITION",
            "CL_INVALID_KERNEL",
            "CL_INVALID_ARG_INDEX",
            "CL_INVALID_ARG_VALUE",
            "CL_INVALID_ARG_SIZE",
            "CL_INVALID_KERNEL_ARGS",
            "CL_INVALID_WORK_DIMENSION",
            "CL_INVALID_WORK_GROUP_SIZE",
            "CL_INVALID_WORK_ITEM_SIZE",
            "CL_INVALID_GLOBAL_OFFSET",
            "CL_INVALID_EVENT_WAIT_LIST",
            "CL_INVALID_EVENT",
            "CL_INVALID_OPERATION",
            "CL_INVALID_GL_OBJECT",
            "CL_INVALID_BUFFER_SIZE",
            "CL_INVALID_MIP_LEVEL",
            "CL_INVALID_GLOBAL_WORK_SIZE",
            "CL_INVALID_PROPERTY",
            "CL_INVALID_IMAGE_DESCRIPTOR",
            "CL_INVALID_COMPILER_OPTIONS",
            "CL_INVALID_LINKER_OPTIONS",
            "CL_INVALID_DEVICE_PARTITION_COUNT",
            "CL_INVALID_PIPE_SIZE",
            "CL_INVALID_DEVICE_QUEUE",
            "CL_INVALID_SPEC_ID",
            "CL_MAX_SIZE_RESTRICTION_EXCEEDED",
        };

        if (err > 62)
        {
            return "UNSPECIFIED_ERROR";
        }

        return names[0 - err];
    }

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

        if (!stream.is_open())
        {
            err = -100;
            return nullptr;
        }

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

    cl_device_id k_device = nullptr;
    cl_context k_context = nullptr;
    cl_command_queue k_queue = nullptr;

    void InitOpenCL()
    {
        int err;

        k_device = CreateDevice(err);

        if (err < 0)
        {
            throw CLException{ GetString(err) };
        }

        k_context = clCreateContext(NULL, 1, &k_device, NULL, NULL, &err);

        if (err < 0)
        {
            throw CLException{ GetString(err) };
        }

        k_queue = clCreateCommandQueue(k_context, k_device, 0, &err);

        if (err < 0)
        {
            throw CLException{ GetString(err) };
        }
    }

    void ShutdownOpenCL()
    {
        int err;

        err = clReleaseCommandQueue(k_queue);

        if (err < 0)
        {
            throw CLException{ GetString(err) };
        }

        err = clReleaseContext(k_context);

        if (err < 0)
        {
            throw CLException{ GetString(err) };
        }

        err = clReleaseDevice(k_device);

        if (err < 0)
        {
            throw CLException{ GetString(err) };
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
            throw CLException{ GetString(err) };
        }
    }
}