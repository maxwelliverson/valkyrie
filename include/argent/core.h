//
// Created by maxwe on 2021-05-21.
//

#ifndef ARGENT_CORE_H
#define ARGENT_CORE_H



#include <assert.h>



/* ====================[ Portability Config ]================== */


#if defined(__cplusplus)
#define AGT_BEGIN_C_NAMESPACE extern "C" {
#define AGT_END_C_NAMESPACE }

#include <cassert>

#else
#define AGT_BEGIN_C_NAMESPACE
#define AGT_END_C_NAMESPACE

#include <assert.h>
#include <stdbool.h>
#endif

#if defined(NDEBUG)
#define AGT_debug_build false
#else
#define AGT_debug_build true
#endif

#if defined(_WIN64) || defined(_WIN32)
#define AGT_system_windows true
#define AGT_system_linux false
#define AGT_system_macos false
#if defined(_WIN64)
#define AGT_64bit true
#define AGT_32bit false
#else
#define AGT_64bit false
#define AGT_32bit true
#endif
#else
#define AGT_system_windows false
#if defined(__LP64__)
#define AGT_64bit true
#define AGT_32bit false
#else
#define AGT_64bit false
#define AGT_32bit true
#endif
#if defined(__linux__)

#define AGT_system_linux true
#define AGT_system_macos false
#elif defined(__OSX__)
#define AGT_system_linux false
#define AGT_system_macos true
#else
#define AGT_system_linux false
#define AGT_system_macos false
#endif
#endif


#if defined(_MSC_VER)
#define AGT_compiler_msvc true
#else
#define AGT_compiler_msvc false
#endif


#if defined(__clang__)
#define AGT_compiler_clang true
#else
#define AGT_compiler_clang false
#endif
#if defined(__GNUC__)
#define AGT_compiler_gcc true
#else
#define AGT_compiler_gcc false
#endif




/* =================[ Portable Attributes ]================= */



#if defined(__cplusplus)
# if (__cplusplus >= 201103L) || (_MSC_VER > 1900)  // C++11
#  define AGT_noexcept   noexcept
# else
#  define AGT_noexcept   throw()
# endif
# if (__cplusplus >= 201703)
#  define AGT_nodiscard [[nodiscard]]
# endif
#else
# define AGT_noexcept
# if (__GNUC__ >= 4) || defined(__clang__)
#  define AGT_nodiscard    __attribute__((warn_unused_result))
# elif (_MSC_VER >= 1700)
#  define AGT_nodiscard    _Check_return_
# else
#  define AGT_nodiscard
# endif
#endif


# if defined(__has_attribute)
#  define AGT_has_attribute(...) __has_attribute(__VA_ARGS__)
# else
#  define AGT_has_attribute(...) false
# endif


#if defined(_MSC_VER) || defined(__MINGW32__)
# if !defined(AGT_SHARED_LIB)
#  define AGT_api
# elif defined(AGT_SHARED_LIB_EXPORT)
#  define AGT_api __declspec(dllexport)
# else
#  define AGT_api __declspec(dllimport)
# endif
#
# if defined(__MINGW32__)
#  define AGT_restricted
# else
#  define AGT_restricted __declspec(restrict)
# endif
# pragma warning(disable:4127)   // suppress constant conditional warning
# define AGT_noinline          __declspec(noinline)
# define AGT_forceinline       __forceinline
# define AGT_noalias           __declspec(noalias)
# define AGT_thread_local      __declspec(thread)
# define AGT_alignas(n)        __declspec(align(n))
# define AGT_restrict          __restrict
#
# define AGT_cdecl __cdecl
# define AGT_stdcall __stdcall
# define AGT_vectorcall __vectorcall
# define AGT_may_alias
# define AGT_alloc_size(...)
# define AGT_alloc_align(p)
#
# define AGT_assume(...) __assume(__VA_ARGS__)
# define AGT_assert(expr) assert(expr)
# define AGT_unreachable AGT_assert(false); AGT_assume(false)
#
#elif defined(__GNUC__)                 // includes clang and icc
#
# if defined(__cplusplus)
# define AGT_restrict __restrict
# else
# define AGT_restrict restrict
# endif
# define AGT_cdecl                      // leads to warnings... __attribute__((cdecl))
# define AGT_stdcall
# define AGT_vectorcall
# define AGT_may_alias    __attribute__((may_alias))
# define AGT_api          __attribute__((visibility("default")))
# define AGT_restricted
# define AGT_malloc       __attribute__((malloc))
# define AGT_noinline     __attribute__((noinline))
# define AGT_noalias
# define AGT_thread_local __thread
# define AGT_alignas(n)   __attribute__((aligned(n)))
# define AGT_assume(...)
# define AGT_forceinline  __attribute__((always_inline))
# define AGT_unreachable  __builtin_unreachable()
#
# if (defined(__clang_major__) && (__clang_major__ < 4)) || (__GNUC__ < 5)
#  define AGT_alloc_size(...)
#  define AGT_alloc_align(p)
# elif defined(__INTEL_COMPILER)
#  define AGT_alloc_size(...)       __attribute__((alloc_size(__VA_ARGS__)))
#  define AGT_alloc_align(p)
# else
#  define AGT_alloc_size(...)       __attribute__((alloc_size(__VA_ARGS__)))
#  define AGT_alloc_align(p)      __attribute__((alloc_align(p)))
# endif
#else
# define AGT_restrict
# define AGT_cdecl
# define AGT_stdcall
# define AGT_vectorcall
# define AGT_api
# define AGT_may_alias
# define AGT_restricted
# define AGT_malloc
# define AGT_alloc_size(...)
# define AGT_alloc_align(p)
# define AGT_noinline
# define AGT_noalias
# define AGT_forceinline
# define AGT_thread_local            __thread        // hope for the best :-)
# define AGT_alignas(n)
# define AGT_assume(...)
# define AGT_unreachable AGT_assume(false)
#endif

#if AGT_has_attribute(transparent_union)
#define AGT_transparent_union_2(a, b) union __attribute__((transparent_union)) { a m_##a; b m_##b; }
#define AGT_transparent_union_3(a, b, c) union __attribute__((transparent_union)) { a m_##a; b m_##b; c m_##c; }
#define AGT_transparent_union_4(a, b, c, d) union __attribute__((transparent_union)) { a m_##a; b m_##b; c m_##c; d m_##d; }
#define AGT_transparent_union_5(a, b, c, d, e) union __attribute__((transparent_union)) { a m_##a; b m_##b; c m_##c; d m_##d; e m_##e; }
#define AGT_transparent_union_6(a, b, c, d, e, f) union __attribute__((transparent_union)) { a m_##a; b m_##b; c m_##c; d m_##d; e m_##e; f m_##f; }
#define AGT_transparent_union_7(a, b, c, d, e, f, g) union __attribute__((transparent_union)) { a m_##a; b m_##b; c m_##c; d m_##d; e m_##e; f m_##f; g m_##g; }
#define AGT_transparent_union_8(a, b, c, d, e, f, g, h) union __attribute__((transparent_union)) { a m_##a; b m_##b; c m_##c; d m_##d; e m_##e; f m_##f; g m_##g; h m_##h; }
#define AGT_transparent_union_9(a, b, c, d, e, f, g, h, i) union __attribute__((transparent_union)) { a m_##a; b m_##b; c m_##c; d m_##d; e m_##e; f m_##f; g m_##g; h m_##h; i m_##i; }
#define AGT_transparent_union_10(a, b, c, d, e, f, g, h, i, j) union __attribute__((transparent_union)) { a m_##a; b m_##b; c m_##c; d m_##d; e m_##e; f m_##f; g m_##g; h m_##h; i m_##i; j m_##j;  }
#define AGT_transparent_union_18(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r) union __attribute__((transparent_union)) { a m_##a; b m_##b; c m_##c; d m_##d; e m_##e; f m_##f; g m_##g; h m_##h; i m_##i; j m_##j; k m_##k; l m_##l; m m_##m; n m_##n; o m_##o; p m_##p; q m_##q; r m_##r; }
#else
#define AGT_transparent_union_2(a, b) void*
#define AGT_transparent_union_3(a, b, c) void*
#define AGT_transparent_union_4(a, b, c, d) void*
#define AGT_transparent_union_5(a, b, c, d, e) void*
#define AGT_transparent_union_6(a, b, c, d, e, f) void*
#define AGT_transparent_union_7(a, b, c, d, e, f, g) void*
#define AGT_transparent_union_8(a, b, c, d, e, f, g, h) void*
#define AGT_transparent_union_9(a, b, c, d, e, f, g, h, i) void*
#define AGT_transparent_union_10(a, b, c, d, e, f, g, h, i, j) void*
#define AGT_transparent_union_18(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r) void*
#endif



#define AGT_no_default default: AGT_unreachable

#define AGT_cache_aligned     AGT_alignas(AGT_CACHE_LINE)
#define AGT_page_aligned      AGT_alignas(AGT_PHYSICAL_PAGE_SIZE)





/* =================[ Macro Functions ]================= */


#if AGT_compiler_gcc || AGT_compiler_clang
# define AGT_assume_aligned(p, align) __builtin_assume_aligned(p, align)
#elif AGT_compiler_msvc
# define AGT_assume_aligned(p, align) (AGT_assume(((agt_u64_t)p) % align == 0), p)
#else
# define AGT_assume_aligned(p, align) p
#endif




/* =================[ Constants ]================= */


#define AGT_DONT_CARE          ((agt_u64_t)-1)
#define AGT_NULL_HANDLE        ((void*)0)

#define AGT_WAIT               ((agt_u64_t)-1)
#define AGT_DO_NOT_WAIT        ((agt_u64_t)0)

#define AGT_CACHE_LINE         ((agt_size_t)1 << 6)
#define AGT_PHYSICAL_PAGE_SIZE ((agt_size_t)1 << 12)
#define AGT_VIRTUAL_PAGE_SIZE  ((agt_size_t)(1 << 16))



/* =================[ Types ]================= */


typedef unsigned char      agt_u8_t;
typedef   signed char      agt_i8_t;
typedef          char      agt_char_t;
typedef unsigned short     agt_u16_t;
typedef   signed short     agt_i16_t;
typedef unsigned int       agt_u32_t;
typedef   signed int       agt_i32_t;
typedef unsigned long long agt_u64_t;
typedef   signed long long agt_i64_t;


typedef agt_u64_t          agt_address_t;
typedef agt_u64_t          agt_size_t;


typedef agt_u32_t          agt_flags32_t;
typedef agt_u64_t          agt_flags64_t;





typedef enum {
  AGT_SUCCESS,
  AGT_ERROR_INTERNAL,
  AGT_ERROR_UNKNOWN,
  AGT_ERROR_BAD_SIZE,
  AGT_ERROR_INVALID_ARGUMENT,
  AGT_ERROR_BAD_ENCODING_IN_NAME,
  AGT_ERROR_NAME_TOO_LONG,
  AGT_ERROR_INSUFFICIENT_BUFFER_SIZE,
  AGT_ERROR_BAD_ALLOC,
  AGT_ERROR_TOO_MANY_SENDERS,
  AGT_ERROR_TOO_MANY_RECEIVERS
} agt_status_t;


#endif  //ARGENT_CORE_H
