#pragma once

#ifdef DLL_IMPORT
#define CPU_API __declspec(dllimport)
#else
#define CPU_API __declspec(dllexport)
#endif