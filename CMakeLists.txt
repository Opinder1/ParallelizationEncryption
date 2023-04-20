cmake_minimum_required (VERSION 3.8)

# Set the python executable
set(Python3_EXECUTABLE "C:/users/opind/AppData/Local/Python38/python.exe")

# Enable project folders when generating visual studio solution
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Make sure gtest uses shared runtime libraries
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Needed to make cmake work
set(GEN_FILES ON CACHE BOOL "" FORCE)
set(ENABLE_TESTING ON CACHE BOOL "" FORCE)

# Setup project to use C++ and CUDA
project("ParallelizationEncryption" LANGUAGES C CXX CUDA)

# Add libraries
add_subdirectory("googletest")
add_subdirectory("mbedtls")

# Add projects
add_subdirectory("CPU")
add_subdirectory("GPU")
add_subdirectory("Test")
