//
// Created by maxwe on 2021-04-10.
//

#ifndef VALKYRIE_INTERNAL_JSON_H
#define VALKYRIE_INTERNAL_JSON_H

#include <limits.h>
#include <json/core.h>

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

#if !defined(JSON_MEM_SECURE)
#define JSON_MEM_SECURE 0
#endif

// Define JSON_DEBUG for debug mode
// #define JSON_DEBUG 1  // basic assertion checks and statistics, check double free, corrupted free list, and invalid pointer free.
// #define JSON_DEBUG 2  // + internal assertion checks
// #define JSON_DEBUG 3  // + extensive internal invariant checking (cmake -DJSON_DEBUG_FULL=ON)
#if !defined(JSON_DEBUG)
#if !defined(NDEBUG) || defined(_DEBUG)
#define JSON_DEBUG 2
#else
#define JSON_DEBUG 0
#endif
#endif

// Reserve extra padding at the end of each block to be more resilient against heap block overflows.
// The padding can detect byte-precise buffer overflow on free.
#if !defined(JSON_MEM_PADDING) && (JSON_DEBUG>=1)
#define JSON_MEM_PADDING  1
#endif


// Encoded free lists allow detection of corrupted free lists
// and can detect buffer overflows, modify after free, and double `free`s.
#if (JSON_MEM_SECURE>=3 || JSON_DEBUG>=1 || JSON_MEM_PADDING > 0)
#define JSON_MEM_ENCODE_FREELIST  1
#endif


#if (JSON_DEBUG>0)
#define json_trace_message(...)  json_trace_message__(__VA_ARGS__)
#else
#define json_trace_message(...)
#endif



#define JSON_GENERIC_VALUE_SIZE (8 + sizeof(void*))
#define JSON_INLINE_STRING_MAX_LENGTH (JSON_GENERIC_VALUE_SIZE - 2)

#define JSON_BIG_INTEGER_BIT_COUNT 3328
#define JSON_BIG_INTEGER_CAPACITY (JSON_BIG_INTEGER_BIT_COUNT / sizeof(json_u64_t))
#define JSON_BIG_INTEGER_TYPE_BIT (sizeof(json_u64_t) * 8)

#define JSON_BIG_FLOAT_EXPONENT_SIZE 20
#define JSON_BIG_FLOAT_FRACTION_SIZE 236
#define JSON_BIG_FLOAT_EXPONENT_BIAS 0x3FFFF





#define JSON_INTPTR_SHIFT 3

#define JSON_INTPTR_SIZE  (1<<JSON_INTPTR_SHIFT)
#define JSON_INTPTR_BITS  (JSON_INTPTR_SIZE*8)

#define KiB     ((size_t)1024)
#define MiB     (KiB*KiB)
#define GiB     (MiB*KiB)

#define JSON_SMALL_WSIZE_MAX  (128)
#define JSON_SMALL_SIZE_MAX   (JSON_SMALL_WSIZE_MAX*sizeof(void*))


#define JSON_SMALL_PAGE_SHIFT               (13 + JSON_INTPTR_SHIFT)      // 64kb
#define JSON_MEDIUM_PAGE_SHIFT              ( 3 + JSON_SMALL_PAGE_SHIFT)  // 512kb
#define JSON_LARGE_PAGE_SHIFT               ( 3 + JSON_MEDIUM_PAGE_SHIFT) // 4mb
#define JSON_SEGMENT_SHIFT                  ( JSON_LARGE_PAGE_SHIFT)      // 4mb


// Derived constants
#define JSON_SEGMENT_SIZE                   (1UL<<JSON_SEGMENT_SHIFT)
#define JSON_SEGMENT_MASK                   ((uintptr_t)JSON_SEGMENT_SIZE - 1)

#define JSON_SMALL_PAGE_SIZE                (1UL<<JSON_SMALL_PAGE_SHIFT)
#define JSON_MEDIUM_PAGE_SIZE               (1UL<<JSON_MEDIUM_PAGE_SHIFT)
#define JSON_LARGE_PAGE_SIZE                (1UL<<JSON_LARGE_PAGE_SHIFT)

#define JSON_SMALL_PAGES_PER_SEGMENT        (JSON_SEGMENT_SIZE/JSON_SMALL_PAGE_SIZE)
#define JSON_MEDIUM_PAGES_PER_SEGMENT       (JSON_SEGMENT_SIZE/JSON_MEDIUM_PAGE_SIZE)
#define JSON_LARGE_PAGES_PER_SEGMENT        (JSON_SEGMENT_SIZE/JSON_LARGE_PAGE_SIZE)

// The max object size are checked to not waste more than 12.5% internally over the page sizes.
// (Except for large pages since huge objects are allocated in 4MiB chunks)
#define JSON_SMALL_OBJ_SIZE_MAX             (JSON_SMALL_PAGE_SIZE/4)   // 16kb
#define JSON_MEDIUM_OBJ_SIZE_MAX            (JSON_MEDIUM_PAGE_SIZE/4)  // 128kb
#define JSON_LARGE_OBJ_SIZE_MAX             (JSON_LARGE_PAGE_SIZE/2)   // 2mb
#define JSON_LARGE_OBJ_WSIZE_MAX            (JSON_LARGE_OBJ_SIZE_MAX/JSON_INTPTR_SIZE)
#define JSON_HUGE_OBJ_SIZE_MAX              (2*JSON_INTPTR_SIZE*JSON_SEGMENT_SIZE)        // (must match JSON_REGION_MAX_ALLOC_SIZE in memory.c)

// Maximum number of size classes. (spaced exponentially in 12.5% increments)
#define JSON_BIN_HUGE  (73U)

#define JSON_HUGE_BLOCK_SIZE   ((json_u32_t)JSON_HUGE_OBJ_SIZE_MAX)


#if defined(__GNUC__) || defined(__clang__)
#define JSON_unlikely(x)     __builtin_expect((x),0)
#define JSON_likely(x)       __builtin_expect((x),1)
#else
#define JSON_unlikely(x)     (x)
#define JSON_likely(x)       (x)
#endif

#ifndef __has_builtin
#define __has_builtin(x)  0
#endif


#if (JSON_DEBUG)
// use our own assertion to print without memory allocation
void    json_assert_fail__(const char* assertion, const char* fname, unsigned int line, const char* func );
#define json_assert(expr)     ((expr) ? (void)0 : json_assert_fail__("Failed Assertion: ( " #expr " ) evaluated to false.",__FILE__,__LINE__,__func__))
#define json_assert_msg(expr, msg)     ((expr) ? (void)0 : json_assert_fail__(msg ": (" #expr ") evaluated to false.",__FILE__,__LINE__,__func__))
#else
#define json_assert(x)
#define json_assert_msg(expr, msg)
#endif

#if (JSON_DEBUG>1)
#define json_assert_internal    json_assert
#define json_assert_msg_internal json_assert_msg
#else
#define json_assert_internal(x)
#define json_assert_msg_internal(x, msg)
#endif

#if (JSON_DEBUG>2)
#define json_assert_expensive   json_assert
#define json_assert_expensive_msg json_assert
#else
#define json_assert_expensive(x)
#define json_assert_expensive_msg(x, msg)
#endif


#define UNUSED(x)     (void)(x)
#if (JSON_DEBUG>0)
#define UNUSED_RELEASE(x)
#else
#define UNUSED_RELEASE(x)  UNUSED(x)
#endif


#define JSON_INIT4(x)   x(),x(),x(),x()
#define JSON_INIT8(x)   JSON_INIT4(x),JSON_INIT4(x)
#define JSON_INIT16(x)  JSON_INIT8(x),JSON_INIT8(x)
#define JSON_INIT32(x)  JSON_INIT16(x),JSON_INIT16(x)
#define JSON_INIT64(x)  JSON_INIT32(x),JSON_INIT32(x)
#define JSON_INIT128(x) JSON_INIT64(x),JSON_INIT64(x)
#define JSON_INIT256(x) JSON_INIT128(x),JSON_INIT128(x)






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
