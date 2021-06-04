//
// Created by maxwe on 2021-05-06.
//

#include <valkyrie/utility/simd.hpp>

using namespace valkyrie;

namespace {

  struct pseudo_cpu{
    /*u32 processors;
    u32 numa_nodes;

    u32 l1d_cache;
    u32 l1i_cache;
    u64 l2_cache;
    u64 l3_cache;*/

    u32 a1_c0_ecx;
    u32 a1_c0_edx;

    u32 a7_c0_ebx;
    u32 a7_c0_ecx;
    u32 a7_c0_edx;
    u32 a7_c1_eax;

    u32 a80000001_c0_ecx;
    u32 a80000001_c0_edx;

    u32 a5_c0_eax;
    u32 a5_c0_ebx;
    u32 a5_c0_ecx;
    u32 a5_c0_edx;

    // 0x80000002, 0x80000003, 0x80000004

    u32 a80000002_c0_eax;
    u32 a80000002_c0_ebx;
    u32 a80000002_c0_ecx;
    u32 a80000002_c0_edx;
    u32 a80000003_c0_eax;
    u32 a80000003_c0_ebx;
    u32 a80000003_c0_ecx;
    u32 a80000003_c0_edx;
    u32 a80000004_c0_eax;
    u32 a80000004_c0_ebx;
    u32 a80000004_c0_ecx;
    u32 a80000004_c0_edx;

    u32 a80000006_c0_ecx;

    u32 a16_c0_eax;
    u32 a16_c0_ebx;
    u32 a16_c0_ecx;

    // eax: 0x17, ecx: 0x1, 0x2, 0x3

    u32 a17_c1_eax;
    u32 a17_c1_ebx;
    u32 a17_c1_ecx;
    u32 a17_c1_edx;
    u32 a17_c2_eax;
    u32 a17_c2_ebx;
    u32 a17_c2_ecx;
    u32 a17_c2_edx;
    u32 a17_c3_eax;
    u32 a17_c3_ebx;
    u32 a17_c3_ecx;
    u32 a17_c3_edx;

    u32 a80000008_c0_eax;

  };

  static_assert(sizeof(pseudo_cpu) == 164);

  static inline void cpuid(u32 *eax, u32 *ebx, u32 *ecx, u32 *edx) {
#if VK_compiler_msvc
    int cpu_info[4];
    __cpuid(cpu_info, *eax);
    *eax = cpu_info[0];
    *ebx = cpu_info[1];
    *ecx = cpu_info[2];
    *edx = cpu_info[3];
#elif defined(HAVE_GCC_GET_CPUID) && defined(USE_GCC_GET_CPUID)
    u32 level = *eax;
    __get_cpuid(level, eax, ebx, ecx, edx);
    #else
    u32 a = *eax, b, c = *ecx, d;
    asm volatile("cpuid\n\t" : "+a"(a), "=b"(b), "+c"(c), "=d"(d));
    *eax = a;
    *ebx = b;
    *ecx = c;
    *edx = d;
#endif
  }
}
