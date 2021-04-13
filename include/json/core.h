//
// Created by maxwe on 2021-04-07.
//

#ifndef VALKYRIE_JSON_CORE_H
#define VALKYRIE_JSON_CORE_H



#if defined(__cplusplus)
#define JSON_BEGIN_C_NAMESPACE extern "C" {
#define JSON_END_C_NAMESPACE }
#else
#define JSON_BEGIN_C_NAMESPACE
#define JSON_END_C_NAMESPACE
#endif




/* =================[ Portable Attributes ]================= */


#define JSON_cache_aligned     JSON_alignas(JSON_CACHE_LINE)

#if defined(__cplusplus)
# if (__cplusplus >= 201103L) || (_MSC_VER > 1900)  // C++11
#  define JSON_noexcept   noexcept
# else
#  define JSON_noexcept   throw()
# endif
# if (__cplusplus >= 201703)
#  define JSON_nodiscard [[nodiscard]]
# endif
#else
# define JSON_noexcept
# if (__GNUC__ >= 4) || defined(__clang__)
#  define JSON_nodiscard    __attribute__((warn_unused_result))
# elif (_MSC_VER >= 1700)
#  define JSON_nodiscard    _Check_return_
# else
#  define JSON_nodiscard
# endif
#endif



#if defined(_MSC_VER) || defined(__MINGW32__)
# if !defined(JSON_SHARED_LIB)
#  define JSON_api
# elif defined(JSON_SHARED_LIB_EXPORT)
#  define JSON_api __declspec(dllexport)
# else
#  define JSON_api __declspec(dllimport)
# endif
#
# if defined(__MINGW32__)
#  define JSON_restrict
#  define JSON_malloc __attribute__((malloc))
# else
#  if (_MSC_VER >= 1900) && !defined(__EDG__)
#   define JSON_restrict __declspec(allocator) __declspec(restrict)
#  else
#   define JSON_restrict __declspec(restrict)
#  endif
#
#  define JSON_malloc
# endif
# pragma warning(disable:4127)   // suppress constant conditional warning (due to JSON_MEM_SECURE paths)
# define JSON_noinline          __declspec(noinline)
# define JSON_noalias           __declspec(noalias)
# define JSON_thread_local      __declspec(thread)
# define JSON_alignas(n)        __declspec(align(n))
#
# define JSON_cdecl __cdecl
# define JSON_may_alias
# define JSON_alloc_size(...)
# define JSON_alloc_align(p)
#
# define JSON_assume(...) __assume(__VA_ARGS__)
# define JSON_unreachable JSON_assume(false)
#
#elif defined(__GNUC__)                 // includes clang and icc
#
# define JSON_cdecl                      // leads to warnings... __attribute__((cdecl))
# define JSON_may_alias __attribute__((may_alias))
# define JSON_api                __attribute__((visibility("default")))
# define JSON_restrict
# define JSON_malloc                __attribute__((malloc))
# define JSON_noinline          __attribute__((noinline))
# define JSON_noalias
# define JSON_thread_local      __thread
# define JSON_alignas(n)        __attribute__((aligned(n)))
# define JSON_assume(...)
# define JSON_unreachable __builtin_unreachable()
#
# if (defined(__clang_major__) && (__clang_major__ < 4)) || (__GNUC__ < 5)
#  define JSON_alloc_size(...)
#  define JSON_alloc_align(p)
# elif defined(__INTEL_COMPILER)
#  define JSON_alloc_size(...)       __attribute__((alloc_size(__VA_ARGS__)))
#  define JSON_alloc_align(p)
# else
#  define JSON_alloc_size(...)       __attribute__((alloc_size(__VA_ARGS__)))
#  define JSON_alloc_align(p)      __attribute__((alloc_align(p)))
# endif
#else
# define JSON_cdecl
# define JSON_api
# define JSON_may_alias
# define JSON_restrict
# define JSON_malloc
# define JSON_alloc_size(...)
# define JSON_alloc_align(p)
# define JSON_noinline
# define JSON_noalias
# define JSON_thread_local            __thread        // hope for the best :-)
# define JSON_alignas(n)
# define JSON_assume(...)
# define JSON_unreachable JSON_assume(false)
#endif

#define JSON_no_default default: JSON_unreachable




/* =================[ Constants ]================= */


#define JSON_DONT_CARE          ((json_u64_t)-1)
#define JSON_NULL_HANDLE        0


#define JSON_CACHE_LINE 64






/* ================================================ */



/* =================[ Primitive Types ]================= */

JSON_BEGIN_C_NAMESPACE

typedef struct JSON_may_alias json_byte { unsigned char : 8; } json_byte_t;
typedef json_byte_t*                                           json_address_t;
typedef const json_byte_t*                                     json_const_address_t;


typedef signed char        json_i8_t;
typedef unsigned char      json_u8_t;
typedef signed short       json_i16_t;
typedef unsigned short     json_u16_t;
typedef int                json_i32_t;
typedef unsigned           json_u32_t;
typedef long long          json_i64_t;
typedef unsigned long long json_u64_t;
typedef float              json_f32_t;
typedef double             json_f64_t;
typedef char               json_char_t;

#if defined(__cplusplus)
typedef bool               json_bool_t;
#else
typedef _Bool              json_bool_t;
#define false         0
#define true          1
#endif


typedef json_u32_t         json_size_t;
typedef json_u32_t         json_flags_t;

typedef struct json_utf8 {
  union {
    struct {
      unsigned char                 : 3;
      unsigned char invalid_4_bytes : 1;
      unsigned char not_3_bytes     : 1;
      unsigned char not_2_bytes     : 1;
      unsigned char is_initial_byte : 1;
      unsigned char is_multibyte    : 1;
    }  meta;
    json_byte_t   byte;
  };
} json_utf8_t;



/* =================[ Core Enumerations ]================= */

typedef enum json_status{

  // Status, does not (necessarily) signify an error, simply a result.

  JSON_SUCCESS,
  JSON_NOT_READY,
  JSON_TIMED_OUT,

  // API Errors, signify improper use of the library. Should be fatal...

  JSON_ERROR_UNKNOWN,

  JSON_ERROR_INTERNAL,
  JSON_ERROR_CONTEXT_DESTROYED,
  JSON_ERROR_DISCARDED_RESULT,
  JSON_ERROR_INVALID_ARGUMENT,
  JSON_ERROR_NOT_IMPLEMENTED,


  // IO Errors, signify invalid input. Should always be recoverable.

  JSON_ERROR_INVALID_KEY,
  JSON_ERROR_INVALID_KEY_CLOSE_MATCH,
  JSON_ERROR_INCOMPATIBLE_TYPE,


  // State Errors, signify invalid state. Application should attempt to fix the state before trying again.

  JSON_ERROR_SYSTEM_MEMORY,
  JSON_ERROR_POOL_OUT_OF_MEMORY,
  JSON_ERROR_POOL_CORRUPTED,
  JSON_ERROR_POOL_TOO_FRAGMENTED,
  JSON_ERROR_COLLECTION_IS_EMPTY,
  JSON_ERROR_COLLECTION_IS_FULL
} json_status_t;




/* =================[ Core Type Declarations ]================= */

/**
 * \brief Contains the "global state" used internally by the whole library.
 *
 *
 *
 * */
typedef struct json_ctx*      json_ctx_t;
typedef struct json_stream*   json_stream_t;
typedef struct json_task*     json_task_t;


typedef struct json_file*     json_file_t;
typedef struct json_document* json_document_t;

typedef struct json_symbol*   json_symbol_t;

typedef struct json_value*    json_value_t;



/*typedef struct json_buffer{
  json_address_t address;
  json_u64_t     size;
} json_buffer_t;*/

JSON_END_C_NAMESPACE


#endif//VALKYRIE_JSON_CORE_H
