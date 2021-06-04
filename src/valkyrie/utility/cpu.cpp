//
// Created by maxwe on 2021-05-20.
//

#include <valkyrie/utility/cpu.hpp>

#include <valkyrie/meta.hpp>


#include <intrin.h>

using namespace valkyrie;

namespace {

  inline void cpuid(u32* info) {
#if VK_compiler_msvc
    __cpuidex(reinterpret_cast<int*>(info), info[0], info[2]);
    /**eax = cpu_info[0];
    *ebx = cpu_info[1];
    *ecx = cpu_info[2];
    *edx = cpu_info[3];*/
#elif defined(HAVE_GCC_GET_CPUID) && defined(USE_GCC_GET_CPUID)
    __get_cpuid(info[0], info, info + 1, info + 2, info + 3);
#else
    u32 a = info[0], b, c = info[2], d;
    asm volatile("cpuid\n\t" : "+a"(a), "=b"(b), "+c"(c), "=d"(d));
    info[0] = a;
    info[1] = b;
    info[2] = c;
    info[3] = d;
#endif
  }

#define cpu_lookup(eax_, ecx_) do { eax = eax_; ecx = ecx_; cpuid(regs); } while(false);

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

  static_assert(sizeof(pseudo_cpu) == sizeof(impl::cpu_info));


  inline impl::cpu_info get_cpu_info() noexcept {
    pseudo_cpu cpu;
    u32        regs[4];

          u32& eax = regs[0];
    const u32& ebx = regs[1];
          u32& ecx = regs[2];
    const u32& edx = regs[3];



    cpu_lookup(0x1, 0x0) {
      cpu.a1_c0_ecx = ecx;
      cpu.a1_c0_edx = edx;
    }

    cpu_lookup(0x7, 0x0) {
      cpu.a7_c0_ebx = ebx;
      cpu.a7_c0_ecx = ecx;
      cpu.a7_c0_edx = edx;
    }

    cpu_lookup(0x7, 0x1) {
      cpu.a7_c1_eax = eax;
    }

    cpu_lookup(0x80000001, 0x0) {
      cpu.a80000001_c0_ecx = ecx;
      cpu.a80000001_c0_edx = edx;
    }

    cpu_lookup(0x5, 0x0) {
      cpu.a5_c0_eax = eax;
      cpu.a5_c0_ebx = ebx;
      cpu.a5_c0_ecx = ecx;
      cpu.a5_c0_edx = edx;
    }

    cpu_lookup(0x80000002, 0x0) {
      cpu.a80000002_c0_eax = eax;
      cpu.a80000002_c0_ebx = ebx;
      cpu.a80000002_c0_ecx = ecx;
      cpu.a80000002_c0_edx = edx;
    }
    cpu_lookup(0x80000003, 0x0) {
      cpu.a80000003_c0_eax = eax;
      cpu.a80000003_c0_ebx = ebx;
      cpu.a80000003_c0_ecx = ecx;
      cpu.a80000003_c0_edx = edx;
    }
    cpu_lookup(0x80000004, 0x0) {
      cpu.a80000004_c0_eax = eax;
      cpu.a80000004_c0_ebx = ebx;
      cpu.a80000004_c0_ecx = ecx;
      cpu.a80000004_c0_edx = edx;
    }

    cpu_lookup(0x80000006, 0x0) {
      cpu.a80000006_c0_ecx = ecx;
    }

    cpu_lookup(0x17, 0x1) {
      cpu.a17_c1_eax = eax;
      cpu.a17_c1_ebx = ebx;
      cpu.a17_c1_ecx = ecx;
      cpu.a17_c1_edx = edx;
    }
    cpu_lookup(0x17, 0x2) {
      cpu.a17_c2_eax = eax;
      cpu.a17_c2_ebx = ebx;
      cpu.a17_c2_ecx = ecx;
      cpu.a17_c2_edx = edx;
    }
    cpu_lookup(0x17, 0x1) {
      cpu.a17_c2_eax = eax;
      cpu.a17_c2_ebx = ebx;
      cpu.a17_c2_ecx = ecx;
      cpu.a17_c2_edx = edx;
    }

    cpu_lookup(0x80000008, 0x0) {
      cpu.a80000008_c0_eax = eax;
    }




    return *reinterpret_cast<const impl::cpu_info*>(static_cast<const void*>(&cpu));
  }
}

const impl::cpu_info valkyrie::this_cpu alignas(std::hardware_destructive_interference_size) = get_cpu_info();