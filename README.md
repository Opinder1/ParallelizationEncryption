# ParallelizationEncryption

To build on windows:

Perl is required and can be installed for windows at https://learn.perl.org/installing/windows.html

CUDA toolkit is required when using CUDA and can be installed for windows at https://developer.nvidia.com/cuda-toolkit

Python3 is required to build the project and the python libraries "jsonschema" and "jinja2" need to be installed if not already

The following cmake command can be run in the visual studio developer command prompt to build a visual studio solution for windows:

cmake -DGEN_FILES=ON -DENABLE_TESTING=Off .
