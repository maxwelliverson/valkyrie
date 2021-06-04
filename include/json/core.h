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
#  define JSON_restricted
#  define JSON_malloc __attribute__((malloc))
# else
#  if (_MSC_VER >= 1900) && !defined(__EDG__)
#   define JSON_restricted __declspec(allocator) __declspec(restrict)
#  else
#   define JSON_restricted __declspec(restrict)
#  endif
#
#  define JSON_malloc
# endif
# pragma warning(disable:4127)   // suppress constant conditional warning (due to JSON_MEM_SECURE paths)
# define JSON_noinline          __declspec(noinline)
# define JSON_forceinline       __forceinline
# define JSON_noalias           __declspec(noalias)
# define JSON_thread_local      __declspec(thread)
# define JSON_alignas(n)        __declspec(align(n))
# define JSON_restrict          __restrict
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
# if defined(__cplusplus)
# define JSON_restrict __restrict
# else
# define JSON_restrict restrict
# endif
# define JSON_cdecl                      // leads to warnings... __attribute__((cdecl))
# define JSON_may_alias    __attribute__((may_alias))
# define JSON_api          __attribute__((visibility("default")))
# define JSON_restricted
# define JSON_malloc       __attribute__((malloc))
# define JSON_noinline     __attribute__((noinline))
# define JSON_noalias
# define JSON_thread_local __thread
# define JSON_alignas(n)   __attribute__((aligned(n)))
# define JSON_assume(...)
# define JSON_forceinline  __attribute__((always_inline))
# define JSON_unreachable  __builtin_unreachable()
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
# define JSON_restrict
# define JSON_cdecl
# define JSON_api
# define JSON_may_alias
# define JSON_restricted
# define JSON_malloc
# define JSON_alloc_size(...)
# define JSON_alloc_align(p)
# define JSON_noinline
# define JSON_noalias
# define JSON_forceinline
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
typedef json_u64_t         json_flags64_t;



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
}     json_status_t;
typedef enum json_command_id{

  /* [ Context API ] */

  JSON_API_CTX_CREATE,
  JSON_API_CTX_RETAIN,
  JSON_API_CTX_RELEASE,
  JSON_API_CTX_TIDY_SYMBOLS,
  JSON_API_CTX_NEXT_TASK,


  /* [ Stream API ] */

  JSON_API_STREAM_CREATE,
  JSON_API_STREAM_DESTROY,
  JSON_API_STREAM_GET_FLAGS,


  /* [ Document API ] */

  JSON_API_DOCUMENT_CREATE,
  JSON_API_DOCUMENT_DESTROY,


  /* [ File API ] */

  JSON_API_FILE_OPEN,
  JSON_API_FILE_CLOSE,
  JSON_API_FILE_GET_FLAGS,


  /* [ Task API ] */

  JSON_API_TASK_WAIT,
  JSON_API_TASK_IS_COMPLETE,
  JSON_API_TASK_FULFILL,
  JSON_API_TASK_DISCARD,


  /* [ Parser API ] */

  JSON_API_PARSER_CREATE,
  JSON_API_PARSER_DESTROY,
  JSON_API_PARSER_RESET,
  JSON_API_PARSE,


  /* [ Value API ] */

  JSON_API_VALUE_IS_TYPE,
  JSON_API_VALUE_SET_VALUE,
  JSON_API_VALUE_SET_OBJECT,
  JSON_API_VALUE_SET_ARRAY,
  JSON_API_VALUE_SET_INT,
  JSON_API_VALUE_SET_UINT,
  JSON_API_VALUE_SET_FLOAT,
  JSON_API_VALUE_SET_STRING,
  JSON_API_VALUE_SET_BOOLEAN,
  JSON_API_VALUE_SET_TRUE,
  JSON_API_VALUE_SET_FALSE,
  JSON_API_VALUE_SET_NULL,
  JSON_API_VALUE_GET_OBJECT,
  JSON_API_VALUE_GET_ARRAY,
  JSON_API_VALUE_GET_STRING,
  JSON_API_VALUE_GET_INT,
  JSON_API_VALUE_GET_UINT,
  JSON_API_VALUE_GET_FLOAT,
  JSON_API_VALUE_GET_BOOLEAN,


  /* [ PRIVATE APIS ] */

  /* [ Memory API ] */


  JSON_PRIVATE_API_ALLOCATE,
  JSON_PRIVATE_API_REALLOCATE,
  JSON_PRIVATE_API_DEALLOCATE,


  /* [ Symbol API ] */

  JSON_PRIVATE_API_SYMBOL_LOOKUP,
  JSON_PRIVATE_API_SYMBOL_RETAIN,
  JSON_PRIVATE_API_SYMBOL_RELEASE,



} json_command_id_t;
typedef enum json_signal_id{
  JSON_CMD_NULL,
  JSON_CMD_BOOL,
  JSON_CMD_INT,
  JSON_CMD_UINT,
  JSON_CMD_DOUBLE,
  JSON_CMD_RAW_NUMBER,
  JSON_CMD_STRING,
  JSON_CMD_OBJECT_START,
  JSON_CMD_KEY,
  JSON_CMD_OBJECT_END,
  JSON_CMD_ARRAY_START,
  JSON_CMD_ARRAY_END
}  json_signal_id_t;





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



typedef struct json_callbacks{
  json_bool_t (* onNull)(void*);
  json_bool_t (* onBool)(void*, json_bool_t b);
  json_bool_t (* onInt)(void*, json_i64_t i);
  json_bool_t (* onUInt)(void*, json_u64_t u);
  json_bool_t (* onDouble)(void*, double d);
  json_bool_t (* onRawNumber)(void*, const json_char_t* string, json_size_t length, json_bool_t copy);
  json_bool_t (* onString)(void*, const json_char_t* string, json_size_t length, json_bool_t copy);
  json_bool_t (* onObjectStart)(void*);
  json_bool_t (* onKey)(void*, const json_char_t* string, json_size_t length, json_bool_t copy);
  json_bool_t (* onObjectEnd)(void*, json_size_t memberCount);
  json_bool_t (* onArrayStart)(void*);
  json_bool_t (* onArrayEnd)(void*, json_size_t elementCount);

  void*          pUserData;
} json_callbacks_t;


/* =================[ Core Function Declarations ]================= */

const json_char_t* json_status_description(json_status_t status);
const json_char_t* json_command_name(json_command_id_t commandId);




JSON_END_C_NAMESPACE


#endif//VALKYRIE_JSON_CORE_H
