//
// Created by maxwe on 2021-04-10.
//

#ifndef VALKYRIE_INTERNAL_JSON_H
#define VALKYRIE_INTERNAL_JSON_H

#include <limits.h>
#include <json/core.h>

#include "jsondefs.h"

#include <intrin.h>
#include <immintrin.h>


#ifdef _MSC_VER
#pragma intrinsic(__lzcnt, __lzcnt64)
#pragma warning(disable:4214) // bitfield is not int
#endif



// Minimal alignment necessary. On most platforms 16 bytes are needed
// due to SSE registers for example. This must be at least `JSON_INTPTR_SIZE`
#ifndef JSON_MAX_ALIGN_SIZE
#define JSON_MAX_ALIGN_SIZE  16   // sizeof(max_align_t)
#endif



// Define JSON_COLLECT_MEM_STATS as 1 to maintain statistics; set it to 2 to have detailed statistics (but costs some performance).
// #define JSON_COLLECT_MEM_STATS 1

// Define JSON_MEM_SECURE to enable security mitigations
// #define JSON_MEM_SECURE 1  // guard page around metadata
// #define JSON_MEM_SECURE 2  // guard page around each mimalloc page
// #define JSON_MEM_SECURE 3  // encode free lists (detect corrupted free list (buffer overflow), and invalid pointer free)
// #define JSON_MEM_SECURE 4  // checks for double free. (may be more expensive)

typedef enum json_mem_opt_stats{
  JSON_MEM_STATS_DISABLED,
  JSON_MEM_STATS_ENABLED,
  JSON_MEM_STATS_DETAILED
}     json_mem_opt_stats_t;
typedef enum json_mem_opt_security {
  JSON_MEM_SECURITY_NONE,
  JSON_MEM_SECURITY_GUARD_METADATA,
  JSON_MEM_SECURITY_GUARD_PAGES,
  JSON_MEM_SECURITY_ENCODE_FREELISTS,
  JSON_MEM_SECURITY_ALL
} json_mem_opt_security_t;
typedef enum json_mem_opt_debug{
  JSON_DEBUG_DISABLED,
  JSON_DEBUG_BASIC,
  JSON_DEBUG_INTERNAL,
  JSON_DEBUG_FULL
}     json_mem_opt_debug_t;





/**
 *
 * Inline functions!
 *
 * */






JSON_BEGIN_C_NAMESPACE


typedef struct json_cpu_features_s{
  json_u32_t  processorCount;
  json_u32_t  pageSize;
  json_bool_t hasFSRM;
  json_bool_t hasCNTLZ;
  json_bool_t hasCNTTZ;
} json_cpu_features_t;
extern const json_cpu_features_t* const json_cpu;





// Is `x` a power of two? (0 is considered a power of two)
static inline json_bool_t json_is_power_of_two(uintptr_t x) {
  return ((x & (x - 1)) == 0);
}

// Align upwards
static inline json_u64_t json_align_log2(json_u64_t sz, json_u64_t alignLog2) {
  json_assert_internal(alignLog2 < JSON_INTPTR_BITS);
  return ((sz - 1) | ~(((json_u64_t)-1) << alignLog2)) + 1;
}
static inline json_u64_t json_align_pow2(json_u64_t sz, json_u64_t alignment) {
  json_assert_internal(alignment != 0);
  json_assert_internal(json_is_power_of_two(alignment));
  /*const uintptr_t mask = alignment - 1;
  return ((sz + mask) & ~mask);*/
  return ((sz - 1) | (alignment - 1)) + 1;
}
static inline json_u64_t json_align_up(json_u64_t sz,   json_u64_t alignment) {
  json_assert_internal(alignment != 0);
  uintptr_t mask = alignment - 1;
  if (json_is_power_of_two(alignment)) {
    return json_align_pow2(sz, alignment);
  }
  else {
    return (((sz + mask)/alignment)*alignment);
  }
}

// Divide upwards: `s <= json_divide_up(s,d)*d < s+d`.
static inline uintptr_t json_divide_up(uintptr_t size, size_t divider) {
  json_assert_internal(divider != 0);
  return (divider == 0 ? size : ((size + divider - 1) / divider));
}

// Is memory zero initialized?
static inline json_bool_t json_mem_is_zero(void* p, size_t size) {
  for (size_t i = 0; i < size; i++) {
    if (((json_u8_t*)p)[i] != 0)
      return false;
  }
  return true;
}

// Align a byte size to a size in _machine words_,
// i.e. byte size == `wsize*sizeof(void*)`.
static inline size_t json_wsize_from_size(size_t size) {
  json_assert_internal(size <= SIZE_MAX - sizeof(uintptr_t));
  return (size + sizeof(uintptr_t) - 1) >> JSON_INTPTR_SHIFT;
}

// Does malloc satisfy the alignment constraints already?
static inline json_bool_t json_malloc_satisfies_alignment(size_t alignment, size_t size) {
  return (alignment == sizeof(void*) || (alignment == JSON_MAX_ALIGN_SIZE && size > (JSON_MAX_ALIGN_SIZE/2)));
}

// Overflow detecting multiply
#if __has_builtin(__builtin_umul_overflow) || __GNUC__ >= 5
#if defined(_CLOCK_T)    // for Illumos
#undef _CLOCK_T
#endif
static inline json_bool_t json_mul_overflow(size_t count, size_t size, size_t* total) {
  #if (SIZE_MAX == UINT_MAX)
    return __builtin_umul_overflow(count, size, total);
  #elif (SIZE_MAX == ULONG_MAX)
    return __builtin_umull_overflow(count, size, total);
  #else
    return __builtin_umulll_overflow(count, size, total);
  #endif
}
#else /* __builtin_umul_overflow is unavailable */
static inline json_bool_t json_mul_overflow(size_t count, size_t size, size_t* total) {
#define JSON_MUL_NO_OVERFLOW ((size_t)1 << (4*sizeof(size_t)))  // sqrt(SIZE_MAX)
  *total = count * size;
  return ((size >= JSON_MUL_NO_OVERFLOW || count >= JSON_MUL_NO_OVERFLOW)
          && size > 0 && (SIZE_MAX / size) < count);
}
#endif

// Safe multiply `count*size` into `total`; return `true` on overflow.
static inline json_bool_t json_count_size_overflow(size_t count, size_t size, size_t* total) {
  if (count==1) {  // quick check for the case where count is one (common for C++ allocators)
    *total = size;
    return false;
  }
  else if (JSON_unlikely(json_mul_overflow(count, size, total))) {
    json_error_message(EOVERFLOW, "allocation request is too large (%zu * %zu bytes)\n", count, size);
    *total = SIZE_MAX;
    return true;
  }
  else
    return false;
}







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
/*static inline size_t json_clz(uintptr_t x) {
  if (x==0)
    return JSON_INTPTR_BITS;
  unsigned long idx;
  _BitScanReverse64(&idx, x);
  return ((JSON_INTPTR_BITS - 1) - idx);
}*/
static inline json_u32_t json_clz32(json_u32_t x) {
  return __lzcnt(x);
}
static inline size_t json_clz(uintptr_t x) {
  return __lzcnt64(x);
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
static inline json_u32_t json_bsr(uintptr_t x) {
#if defined(_MSC_VER)
  if (JSON_unlikely(x==0))
    return JSON_INTPTR_BITS;
  unsigned long idx;
  _BitScanReverse64(&idx, x);
  return idx;
#else
  return (x==0 ? JSON_INTPTR_BITS : JSON_INTPTR_BITS - 1 - json_clz(x));
#endif
}


static inline json_u64_t json_bit_ceil(json_u64_t x) {
  return 0x1 << (json_bsr(x - 1) + 1);
}


JSON_END_C_NAMESPACE



// ---------------------------------------------------------------------------------
// Provide our own `json_memcpy` for potential performance optimizations.
//
// For now, only on Windows with msvc/clang-cl we optimize to `rep movsb` if 
// we happen to run on x86/x64 cpu's that have "fast short rep movsb" (FSRM) support 
// (AMD Zen3+ (~2020) or Intel Ice Lake+ (~2017). See also issue #201 and pr #253. 
// ---------------------------------------------------------------------------------

#if defined(_WIN32) && (defined(_M_IX86) || defined(_M_X64))
#include <intrin.h>
#include <string.h>
static inline void json_memcpy(void* dst, const void* src, size_t n) {
  if (json_cpu->hasFSRM) {
    __movsb((unsigned char*)dst, (const unsigned char*)src, n);
  }
  else {
    memcpy(dst, src, n); // todo: use noinline?
  }
}
#else
#include <string.h>
static inline void json_memcpy(void* dst, const void* src, size_t n) {
  memcpy(dst, src, n);
}
#endif


// -------------------------------------------------------------------------------
// The `json_memcpy_aligned` can be used if the pointers are machine-word aligned 
// This is used for example in `json_realloc`.
// -------------------------------------------------------------------------------

#if (__GNUC__ >= 4) || defined(__clang__)
// On GCC/CLang we provide a hint that the pointers are word aligned.
#include <string.h>
static inline void json_memcpy_aligned(void* dst, const void* src, size_t n) {
  json_assert_internal(((uintptr_t)dst % JSON_INTPTR_SIZE == 0) && ((uintptr_t)src % JSON_INTPTR_SIZE == 0));
  void* adst = __builtin_assume_aligned(dst, JSON_INTPTR_SIZE);
  const void* asrc = __builtin_assume_aligned(src, JSON_INTPTR_SIZE);
  memcpy(adst, asrc, n);
}
#else
static inline void json_memcpy_aligned(void* dst, const void* src, size_t n) {
  json_assert_internal(((uintptr_t)dst % JSON_INTPTR_SIZE == 0) && ((uintptr_t)src % JSON_INTPTR_SIZE == 0));
  json_memcpy(dst, src, n);
}
#endif






#endif//VALKYRIE_INTERNAL_JSON_H
