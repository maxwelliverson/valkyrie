//
// Created by maxwe on 2021-05-20.
//

#ifndef VALKYRIE_UTILITY_CPU_HPP
#define VALKYRIE_UTILITY_CPU_HPP


#include <valkyrie/preprocessor.hpp>
#include <valkyrie/primitives.hpp>

#include <new>

namespace valkyrie{

  namespace impl{

    enum class l2_associativity : u8{
      disabled = 0x00,
      direct   = 0x10,
      x2       = 0x20,
      x4       = 0x40,
      x8       = 0x60,
      x16      = 0x80,
      x32      = 0xA0,
      x48      = 0xB0,
      x64      = 0xC0,
      x96      = 0xD0,
      x128     = 0xE0,
      full     = 0xF0,
      variable = 0x70
    };

    struct cpu_info{
      // (eax: 0x1) => ecx

      u32 has_sse3 : 1;
      u32 has_pclmulqdq : 1;
      u32 has_dtes64 : 1;
      u32 has_monitor : 1;
      u32 has_ds_cpl : 1;
      u32 has_vmx : 1;
      u32 has_smx : 1;
      u32 has_est : 1;
      u32 has_tm2 : 1;
      u32 has_ssse3 : 1;
      u32 has_cnxt_id : 1;
      u32 has_sdbg : 1;
      u32 has_fma : 1;
      u32 has_cmpxchg16b : 1;
      u32 has_xtpr_control : 1;
      u32 has_pdcm : 1;
      u32 : 1;
      u32 has_pcid : 1;
      u32 has_dca : 1;
      u32 has_sse4_1 : 1;
      u32 has_sse4_2 : 1;
      u32 has_x2apic : 1;
      u32 has_movbe : 1;
      u32 has_popcnt : 1;
      u32 has_tsc_deadline : 1;
      u32 has_aes : 1;
      u32 has_xsave : 1;
      u32 has_osxsave : 1;
      u32 has_avx : 1;
      u32 has_f16c : 1;
      u32 has_rdrand : 1;
      u32 : 0;


      // (eax: 0x1) => edx

      u32 has_x87 : 1;
      u32 has_vme : 1;
      u32 has_de  : 1;
      u32 has_pse : 1;
      u32 has_tsc : 1;
      u32 has_msr : 1;
      u32 has_pae : 1;
      u32 has_mce : 1;
      u32 has_cmpxchg8 : 1;
      u32 has_apic : 1;
      u32 : 1;
      u32 has_sysenter : 1;
      u32 has_mtrr : 1;
      u32 has_pge : 1;
      u32 has_mca : 1;
      u32 has_cmov : 1;
      u32 has_pat : 1;
      u32 has_pse36 : 1;
      u32 has_psn : 1;
      u32 has_clflush : 1;
      u32 : 1;
      u32 has_ds : 1;
      u32 has_acpi : 1;
      u32 has_mmx : 1;
      u32 has_fxsr : 1;
      u32 has_sse : 1;
      u32 has_sse2 : 1;
      u32 has_ss : 1;
      u32 has_htt : 1;
      u32 has_tm : 1;
      u32 : 1;
      u32 has_pbe : 1;




      // (eax: 0x7, ecx: 0x0) => ebx

      u32 has_fsgsbase : 1;
      u32 has_ia32_tsc_adjust : 1;
      u32 has_sgx : 1;
      u32 has_bmi1 : 1;
      u32 has_hle : 1;
      u32 has_avx2 : 1;
      u32 fdp_excptn_only : 1;
      u32 has_smep : 1;
      u32 has_bmi2 : 1;
      u32 has_fast_rep_movsb : 1;
      u32 has_invpcid : 1;
      u32 has_rtm : 1;
      u32 has_rtd_m : 1;
      u32 deprecated_fpu_cs_ds : 1;
      u32 has_memprot_ext : 1;
      u32 has_rtd_a : 1;
      u32 has_avx512f : 1;
      u32 has_avx512dq : 1;
      u32 has_rdseed : 1;
      u32 has_adx : 1;
      u32 has_smap : 1;
      u32 has_avx512_ifma : 1;
      u32 : 1;
      u32 has_clflushopt : 1;
      u32 has_clwb : 1;
      u32 has_ipt : 1;
      u32 has_avx512pf : 1;
      u32 has_avx512er : 1;
      u32 has_avx512cd : 1;
      u32 has_sha : 1;
      u32 has_avx512bw : 1;
      u32 has_avx512v : 1;

      // (eax: 0x7, ecx: 0x0) => ecx

      u32 has_prefetchwt1 : 1;
      u32 has_avx512_vbmi : 1;
      u32 has_umip : 1;
      u32 has_pku : 1;
      u32 has_ospke : 1;
      u32 has_waitpkg : 1;
      u32 has_avx512_vbmi2 : 1;
      u32 has_cet_ss : 1;
      u32 has_gfni : 1;
      u32 has_vaes : 1;
      u32 has_vpclmulqdq : 1;
      u32 has_avx512_vnni : 1;
      u32 has_avx512_bitalg : 1;
      u32 has_tme_en : 1;
      u32 has_avx512_vpopcntdq : 1;
      u32 : 1;
      u32 has_la57 : 1;
      u32 mawau_bits : 5;
      u32 has_rdpid : 1;
      u32 has_key_locker : 1;
      u32 : 1;
      u32 has_cldemote : 1;
      u32 : 1;
      u32 has_movdiri : 1;
      u32 has_movdir64b : 1;
      u32 has_enqcmd : 1;
      u32 has_sgx_lc : 1;
      u32 supports_pks : 1;


      // (eax: 0x7, ecx: 0x0) => edx

      u32 : 2;
      u32 has_avx512_4vnniw : 1;
      u32 has_avx512_4fmaps : 1;
      u32 has_fs_rep_mov    : 1;
      u32 has_uintr         : 1;
      u32 : 2;
      u32 has_avx512_vp2intersect : 1;
      u32 : 1;
      u32 has_md_clear : 1;
      u32 : 3;
      u32 has_serialize : 1;
      u32 is_hybrid : 1;
      u32 has_tsxldtrk : 1;
      u32 : 1;
      u32 has_pconfig : 1;
      u32 : 1;
      u32 has_cet_ibt : 1;
      u32 : 1;
      u32 has_amx_bf16 : 1;
      u32 has_avx512_fp16 : 1;
      u32 has_amx_tile : 1;
      u32 has_amx_int8 : 1;
      u32 has_ibrs : 1;
      u32 has_stibp : 1;
      u32 has_l1d_flush : 1;
      u32 has_ia32_arch_capabilities : 1;
      u32 has_ia32_core_capabilities : 1;
      u32 has_ssbd : 1;

      // (eax: 0x7, ecx: 0x1) => eax

      u32 : 4;
      u32 has_avx_vnni : 1;
      u32 has_avx512_bf16 : 1;
      u32 : 4;
      u32 has_zl_movsb : 1;
      u32 has_fs_stosb : 1;
      u32 has_fs_cmpsb_scasb : 1;
      u32 : 9;
      u32 has_hreset : 1;
      u32 : 3;
      u32 has_lam : 1;
      u32 : 0;


      // (eax: 0x80000001) => ecx

      u32 has_xahf_x64 : 1;
      u32 : 3;
      u32 has_lzcnt : 1;
      u32 : 2;
      u32 has_prefetchw : 1;
      u32 : 0;

      // (eax: 0x80000001) => edx

      u32 : 11;
      u32 has_syscall_x64 : 1;
      u32 : 8;
      u32 has_execute_disable_bit : 1;
      u32 : 5;
      u32 has_1gb_pages : 1;
      u32 has_rdtscp : 1;
      u32 : 1;
      u32 is_x64 : 1;
      u32 : 0;


      // (eax: 0x5) => eax
      u32 min_monitor_line_size;

      // (eax: 0x5) => ebx
      u32 max_monitor_line_size;

      // (eax: 0x5) => ecx
      u32 : 1;
      u32 break_on_interrupt_mwait : 1;
      u32 : 30;

      // (eax: 0x5) => edx

      u32 mwait_c0_states : 4;
      u32 mwait_c1_states : 4;
      u32 mwait_c2_states : 4;
      u32 mwait_c3_states : 4;
      u32 mwait_c4_states : 4;
      u32 mwait_c5_states : 4;
      u32 mwait_c6_states : 4;
      u32 mwait_c7_states : 4;



      // (eax: 0x80000002, 0x80000003, 0x80000004) => (eax, ebx, ecx, edx) * 3

      utf8 brand_name[48];

      // (eax: 0x80000006) => ecx

      u8  cache_line_size;
      l2_associativity l2_associativity;
      u16 cache_size_kb;

      // (eax: 0x16) => eax, ebx, ecx
      u32 base_frequency;
      u32 max_frequency;
      u32 bus_frequency;

      // (eax: 0x17, ecx: 0x1, 0x2, 0x3) => (eax, ebx, ecx, edx) * 3

      utf8 soc_vendor_name[48];


      // (eax: 0x80000008) => eax

      u8  physical_address_bits;
      u8  virtual_address_bits;
      u16 : 16;
    };
  }


  extern const impl::cpu_info this_cpu alignas(std::hardware_destructive_interference_size);

}

#endif  //VALKYRIE_UTILITY_CPU_HPP
