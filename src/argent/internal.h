//
// Created by maxwe on 2021-05-21.
//

#ifndef ARGENT_INTERNAL_H
#define ARGENT_INTERNAL_H

#define AGT_SHARED_LIB_EXPORT

#include <argent/core.h>



#if AGT_compiler_gcc || AGT_compiler_clang
# define AGT_cpu_dispatch(...) __attribute__((cpu_dispatch(__VA_ARGS__)))
# define AGT_cpu_specific(...) __attribute__((cpu_specific(__VA_ARGS__)))
#else
# define AGT_cpu_dispatch(...) __declspec(cpu_dispatch(__VA_ARGS__))
# define AGT_cpu_specific(...) __declspec(cpu_specific(__VA_ARGS__))
#endif





#endif//ARGENT_INTERNAL_H
