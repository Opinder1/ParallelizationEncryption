# CMakeList.txt : CMake project for CPUGCC, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.9)

set (CPU_source "MbedTest.cpp" "DESv2.cpp" "DESv3.cpp" "AESv1.cpp" "AESv2.cpp" "AESv3.cpp" "MBedTest.cpp")

# Add source to this project's executable.
add_library(CPU ${CPU_source})

find_package(OpenMP REQUIRED)

set_target_properties(CPU PROPERTIES
    CXX_STANDARD 17
    CXX_EXTENSIONS OFF
    FOLDER "Project"
)

target_link_libraries(CPU gtest mbedcrypto OpenMP::OpenMP_CXX)