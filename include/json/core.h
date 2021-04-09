//
// Created by maxwe on 2021-04-07.
//

#ifndef VALKYRIE_JSON_CORE_H
#define VALKYRIE_JSON_CORE_H



/* =================[ Preprocessor Configuration ]================= */


#if !defined(_MSC_VER)
#define JSON_MAY_ALIAS __attribute__((__may_alias__))
#else
#define JSON_MAY_ALIAS
#endif


#if defined(__cplusplus)
#define JSON_BEGIN_C_NAMESPACE extern "C" {
#define JSON_END_C_NAMESPACE }
#else
#define JSON_BEGIN_C_NAMESPACE
#define JSON_END_C_NAMESPACE
#endif





/* =================[ Constants ]================= */


#define JSON_DONT_CARE          ((json_u64_t)-1)
#define JSON_NULL_HANDLE        0

#define JSON_GENERIC_VALUE_SIZE (8 + sizeof(void*))
#define JSON_INLINE_STRING_MAX_LENGTH (JSON_GENERIC_VALUE_SIZE - 2)

#define JSON_BIG_INTEGER_BIT_COUNT 3328
#define JSON_BIG_INTEGER_CAPACITY (JSON_BIG_INTEGER_BIT_COUNT / sizeof(json_u64_t))
#define JSON_BIG_INTEGER_TYPE_BIT (sizeof(json_u64_t) * 8)

#define JSON_BIG_FLOAT_EXPONENT_SIZE 20
#define JSON_BIG_FLOAT_FRACTION_SIZE 236
#define JSON_BIG_FLOAT_EXPONENT_BIAS 0x3FFFF



JSON_BEGIN_C_NAMESPACE


/* =================[ Primitive Types ]================= */

typedef struct JSON_MAY_ALIAS json_byte { unsigned char : 8; } json_byte_t;
typedef json_byte_t*                                           json_address_t;
typedef const json_byte_t*                                     json_const_address_t;


typedef short              json_i16_t;
typedef unsigned short     json_u16_t;
typedef int                json_i32_t;
typedef unsigned           json_u32_t;
typedef long long          json_i64_t;
typedef unsigned long long json_u64_t;
typedef float              json_f32_t;
typedef double             json_f64_t;
typedef char               json_char_t;

typedef int                json_bool_t;
#define JSON_FALSE         0
#define JSON_TRUE          1

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

  JSON_ERROR_CONTEXT_DESTROYED,
  JSON_ERROR_DISCARDED_RESULT,
  JSON_ERROR_INVALID_ARGUMENT,
  JSON_ERROR_NOT_IMPLEMENTED,


  // IO Errors, signify invalid input. Should always be recoverable.

  JSON_ERROR_INVALID_KEY,
  JSON_ERROR_INVALID_KEY_CLOSE_MATCH,
  JSON_ERROR_INCOMPATIBLE_TYPE,


  // State Errors, signify invalid state. Application should attempt to fix the state before trying again.

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





typedef struct json_buffer{
  json_address_t address;
  json_u64_t     size;
} json_buffer_t;


JSON_END_C_NAMESPACE


#endif//VALKYRIE_JSON_CORE_H
