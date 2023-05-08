# CMakeList.txt : CMake project for CPUGCC, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.9)

set (GPU_source "AES.cu" "AEScl.cpp" "DES.cu" "DEScl.cpp" "OpenCL.cpp")

# Add source to this project's executable.
add_library(GPU ${GPU_source})

set_target_properties(GPU PROPERTIES
    CXX_STANDARD 17
    CXX_EXTENSIONS OFF
    FOLDER "Project"
)

target_link_libraries(GPU gtest mbedcrypto)