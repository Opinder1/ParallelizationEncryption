# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

#set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
set(Python3_EXECUTABLE "C:/users/opind/AppData/Local/Python38/python.exe")

project ("ParallelizationEncryption" LANGUAGES C CXX)

include_directories("C:/VisualStudio/lib/include") # Location of all include folders for libraries
link_directories("lib") # Localtion of gtest library .a
link_directories("mbedtls/library") # Location of mbedtls library .a

# Directorys for cmakes for libraries
add_subdirectory ("C:/VisualStudio/libraries/googletest-release-1.12.1" "gtest")
add_subdirectory ("C:/VisualStudio/libraries/mbedtls-2.26.0" "mbedtls")

# Include sub-projects.
add_subdirectory ("CPUGCC")
add_subdirectory ("CPU")
