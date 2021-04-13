//
// Created by maxwe on 2021-04-12.
//

#ifndef VALKYRIE_INTERNAL_COMMON_H
#define VALKYRIE_INTERNAL_COMMON_H


#include <valkyrie/traits.hpp>


#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace valkyrie{
    inline static u64 thread_id() noexcept {
      // Windows: works on Intel and ARM in both 32- and 64-bit
      return (uintptr_t)NtCurrentTeb();
    }
}



#elif defined(__GNUC__) && \
      (defined(__x86_64__) || defined(__i386__) || defined(__arm__) || defined(__aarch64__))

// TLS register on x86 is in the FS or GS register, see: https://akkadia.org/drepper/tls.pdf
static inline void* mi_tls_slot(size_t slot) mi_attr_noexcept {
  void* res;
  const size_t ofs = (slot*sizeof(void*));
#if defined(__i386__)
  __asm__("movl %%gs:%1, %0" : "=r" (res) : "m" (*((void**)ofs)) : );  // 32-bit always uses GS
#elif defined(__MACH__) && defined(__x86_64__)
  __asm__("movq %%gs:%1, %0" : "=r" (res) : "m" (*((void**)ofs)) : );  // x86_64 macOSX uses GS
#elif defined(__x86_64__) && (MI_INTPTR_SIZE==4)
  __asm__("movl %%fs:%1, %0" : "=r" (res) : "m" (*((void**)ofs)) : );  // x32 ABI
#elif defined(__x86_64__)
  __asm__("movq %%fs:%1, %0" : "=r" (res) : "m" (*((void**)ofs)) : );  // x86_64 Linux, BSD uses FS
#elif defined(__arm__)
  void** tcb; UNUSED(ofs);
  __asm__ volatile ("mrc p15, 0, %0, c13, c0, 3\nbic %0, %0, #3" : "=r" (tcb));
  res = tcb[slot];
#elif defined(__aarch64__)
  void** tcb; UNUSED(ofs);
#if defined(__APPLE__) // M1, issue #343
  __asm__ volatile ("mrs %0, tpidrro_el0" : "=r" (tcb));
#else
  __asm__ volatile ("mrs %0, tpidr_el0" : "=r" (tcb));
#endif
  res = tcb[slot];
#endif
  return res;
}

// setting is only used on macOSX for now
static inline void mi_tls_slot_set(size_t slot, void* value) mi_attr_noexcept {
  const size_t ofs = (slot*sizeof(void*));
#if defined(__i386__)
  __asm__("movl %1,%%gs:%0" : "=m" (*((void**)ofs)) : "rn" (value) : );  // 32-bit always uses GS
#elif defined(__MACH__) && defined(__x86_64__)
  __asm__("movq %1,%%gs:%0" : "=m" (*((void**)ofs)) : "rn" (value) : );  // x86_64 macOSX uses GS
#elif defined(__x86_64__) && (MI_INTPTR_SIZE==4)
  __asm__("movl %1,%%fs:%1" : "=m" (*((void**)ofs)) : "rn" (value) : );  // x32 ABI
#elif defined(__x86_64__)
  __asm__("movq %1,%%fs:%1" : "=m" (*((void**)ofs)) : "rn" (value) : );  // x86_64 Linux, BSD uses FS
#elif defined(__arm__)
  void** tcb; UNUSED(ofs);
  __asm__ volatile ("mrc p15, 0, %0, c13, c0, 3\nbic %0, %0, #3" : "=r" (tcb));
  tcb[slot] = value;
#elif defined(__aarch64__)
  void** tcb; UNUSED(ofs);
#if defined(__APPLE__) // M1, issue #343
  __asm__ volatile ("mrs %0, tpidrro_el0" : "=r" (tcb));
#else
  __asm__ volatile ("mrs %0, tpidr_el0" : "=r" (tcb));
#endif
  tcb[slot] = value;
#endif
}

static inline uintptr_t _mi_thread_id(void) mi_attr_noexcept {
#if defined(__aarch64__) && defined(__APPLE__)  // M1
  // on macOS on the M1, slot 0 does not seem to work, so we fall back to portable C for now. See issue #354
  return (uintptr_t)&_mi_heap_default;
#else
  // in all our other targets, slot 0 is the pointer to the thread control block
  return (uintptr_t)mi_tls_slot(0);
#endif
}
#else
// otherwise use standard C
static inline uintptr_t _mi_thread_id(void) mi_attr_noexcept {
  return (uintptr_t)&_mi_heap_default;
}
#endif



#if defined(__GNUC__)





#define JSON_HAVE_FAST_BITSCAN
static inline size_t json_clz(uintptr_t x) {
  if (x==0) return JSON_INTPTR_BITS;
#if (INTPTR_MAX == LONG_MAX)
  return __builtin_clzl(x);
#else
  return __builtin_clzll(x);
#endif
}
static inline size_t json_ctz(uintptr_t x) {
  if (x==0) return JSON_INTPTR_BITS;
#if (INTPTR_MAX == LONG_MAX)
  return __builtin_ctzl(x);
#else
  return __builtin_ctzll(x);
#endif
}

#elif defined(_MSC_VER)

#define JSON_HAVE_FAST_BITSCAN 1
static inline size_t json_clz(uintptr_t x) {
  if (x==0) return JSON_INTPTR_BITS;
  unsigned long idx;
#if (INTPTR_MAX == LONG_MAX)
  _BitScanReverse(&idx, x);
#else
  _BitScanReverse64(&idx, x);
#endif
  return ((JSON_INTPTR_BITS - 1) - idx);
}
static inline size_t json_ctz(uintptr_t x) {
  if (x==0) return JSON_INTPTR_BITS;
  unsigned long idx;
#if (INTPTR_MAX == LONG_MAX)
  _BitScanForward(&idx, x);
#else
  _BitScanForward64(&idx, x);
#endif
  return idx;
}

#else
static inline size_t json_ctz32(uint32_t x) {
  // de Bruijn multiplication, see <http://supertech.csail.mit.edu/papers/debruijn.pdf>
  static const unsigned char debruijn[32] = {
    0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
    31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
  };
  if (x==0) return 32;
  return debruijn[((x & -(int32_t)x) * 0x077CB531UL) >> 27];
}
static inline size_t json_clz32(uint32_t x) {
  // de Bruijn multiplication, see <http://supertech.csail.mit.edu/papers/debruijn.pdf>
  static const uint8_t debruijn[32] = {
    31, 22, 30, 21, 18, 10, 29, 2, 20, 17, 15, 13, 9, 6, 28, 1,
    23, 19, 11, 3, 16, 14, 7, 24, 12, 4, 8, 25, 5, 26, 27, 0
  };
  if (x==0) return 32;
  x |= x >> 1;
  x |= x >> 2;
  x |= x >> 4;
  x |= x >> 8;
  x |= x >> 16;
  return debruijn[(uint32_t)(x * 0x07C4ACDDUL) >> 27];
}

static inline size_t json_clz(uintptr_t x) {
  if (x==0) return JSON_INTPTR_BITS;
#if (JSON_INTPTR_BITS <= 32)
  return json_clz32((uint32_t)x);
#else
  size_t count = json_clz32((uint32_t)(x >> 32));
  if (count < 32) return count;
  return (32 + json_clz32((uint32_t)x));
#endif
}
static inline size_t json_ctz(uintptr_t x) {
  if (x==0) return JSON_INTPTR_BITS;
#if (JSON_INTPTR_BITS <= 32)
  return json_ctz32((uint32_t)x);
#else
  size_t count = json_ctz32((uint32_t)x);
  if (count < 32) return count;
  return (32 + json_ctz32((uint32_t)(x>>32)));
#endif
}

#endif

// "bit scan reverse": Return index of the highest bit (or JSON_INTPTR_BITS if `x` is zero)
static inline size_t json_bsr(uintptr_t x) {
  return (x==0 ? JSON_INTPTR_BITS : JSON_INTPTR_BITS - 1 - json_clz(x));
}


JSON_END_C_NAMESPACE



// ---------------------------------------------------------------------------------
// Provide our own `_mi_memcpy` for potential performance optimizations.
//
// For now, only on Windows with msvc/clang-cl we optimize to `rep movsb` if
// we happen to run on x86/x64 cpu's that have "fast short rep movsb" (FSRM) support
// (AMD Zen3+ (~2020) or Intel Ice Lake+ (~2017). See also issue #201 and pr #253.
// ---------------------------------------------------------------------------------

#if defined(_WIN32) && (defined(_M_IX86) || defined(_M_X64))
#include <intrin.h>
#include <string.h>
extern json_bool_t _mi_cpu_has_fsrm;
static inline void _mi_memcpy(void* dst, const void* src, size_t n) {
  if (_mi_cpu_has_fsrm) {
    __movsb((unsigned char*)dst, (const unsigned char*)src, n);
  }
  else {
    memcpy(dst, src, n); // todo: use noinline?
  }
}
#else
#include <string.h>
static inline void _mi_memcpy(void* dst, const void* src, size_t n) {
  memcpy(dst, src, n);
}
#endif


// -------------------------------------------------------------------------------
// The `_mi_memcpy_aligned` can be used if the pointers are machine-word aligned
// This is used for example in `json_realloc`.
// -------------------------------------------------------------------------------

#if (__GNUC__ >= 4) || defined(__clang__)
// On GCC/CLang we provide a hint that the pointers are word aligned.
#include <string.h>
static inline void _mi_memcpy_aligned(void* dst, const void* src, size_t n) {
  json_assert_internal(((uintptr_t)dst % JSON_INTPTR_SIZE == 0) && ((uintptr_t)src % JSON_INTPTR_SIZE == 0));
  void* adst = __builtin_assume_aligned(dst, JSON_INTPTR_SIZE);
  const void* asrc = __builtin_assume_aligned(src, JSON_INTPTR_SIZE);
  memcpy(adst, asrc, n);
}
#else
// Default fallback on `_mi_memcpy`
static inline void _mi_memcpy_aligned(void* dst, const void* src, size_t n) {
  json_assert_internal(((uintptr_t)dst % JSON_INTPTR_SIZE == 0) && ((uintptr_t)src % JSON_INTPTR_SIZE == 0));
  _mi_memcpy(dst, src, n);
}
#endif



#endif//VALKYRIE_INTERNAL_COMMON_H
