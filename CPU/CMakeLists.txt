# CMakeList.txt : CMake project for CPUGCC, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (CPU_source "CPU.cpp" "MbedTest.cpp" "DESv1.cpp" "AESv1.cpp" "AESv2.cpp" "AESv3.cpp")

# Add source to this project's executable.
add_executable (CPU ${CPU_source})

set_target_properties(CPU PROPERTIES
            CXX_STANDARD 17
            CXX_EXTENSIONS OFF
)

target_link_libraries(CPU gtest)
target_link_libraries(CPU mbedcrypto)

# TODO: Add tests and install targets if needed.
