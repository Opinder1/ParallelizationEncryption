# ParallelizationEncryption

To build on windows:

Perl is required and can be installed for windows at https://learn.perl.org/installing/windows.html

CUDA toolkit is required when using CUDA and can be installed for windows at https://developer.nvidia.com/cuda-toolkit

The following cmake command can be run in the visual studio developer command prompt to build a visual studio solution for windows:

cmake -DGEN_FILES=ON -DENABLE_TESTING=Off .
