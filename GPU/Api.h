#pragma once

#ifdef DLL_IMPORT
#define GPU_API __declspec(dllimport)
#else
#define GPU_API __declspec(dllexport)
#endif