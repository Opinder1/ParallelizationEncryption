# CMakeList.txt : CMake project for CPUGCC, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (Test_source "CPU.cpp")

# Add source to this project's executable.
add_executable(Test ${Test_source})

set_target_properties(Test PROPERTIES
    CXX_STANDARD 17
    CXX_EXTENSIONS OFF
    FOLDER "Project"
)

target_link_libraries(Test CPU GPU gtest mbedcrypto opencl)