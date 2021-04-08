//
// Created by Maxwell on 2021-02-13.
//

#ifndef VALKYRIE_JSON_H
#define VALKYRIE_JSON_H


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


#define JSON_GENERIC_VALUE_SIZE (8 + sizeof(void*))

#define JSON_BIG_INTEGER_BIT_COUNT 3328
#define JSON_BIG_INTEGER_CAPACITY (JSON_BIG_INTEGER_BIT_COUNT / sizeof(json_u64_t))
#define JSON_BIG_INTEGER_TYPE_BIT (sizeof(json_u64_t) * 8)

#define JSON_BIG_FLOAT_EXPONENT_SIZE 20
#define JSON_BIG_FLOAT_FRACTION_SIZE 236
#define JSON_BIG_FLOAT_EXPONENT_BIAS 0x3FFFF

JSON_BEGIN_C_NAMESPACE







// Implementation Types

typedef struct json_symbol*            json_symbol_t;

typedef struct json_value_pool*        json_value_pool_t;
typedef struct json_object_pool*       json_object_pool_t;
typedef struct json_string_pool*       json_string_pool_t;
typedef struct json_string_pool_entry* json_string_pool_entry_t;




//struct json_pooled_string;
struct json_string_ref;
struct json_inline_string;

struct json_big_integer;
struct json_big_float;
struct json_integer;
struct json_float;




// api Types

struct json_document;





/*typedef struct json_document {
  json_object_pool_t object_pool;
  json_object_t*     p_object;
} json_document_t;*/











typedef json_u32_t json_flags_t;


/**
 * @enum json_allocator_flags
 *
 *
 * @var JSON_ALLOCATOR_DEFAULT = 0x0
 * @var JSON_ALLOCATOR_CUSTOM = 0x1
 *
 * */

















/*
 *
 * api Declarations
 *
 * */




























JSON_END_C_NAMESPACE

#endif//VALKYRIE_JSON_H
