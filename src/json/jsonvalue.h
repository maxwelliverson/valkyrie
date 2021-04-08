//
// Created by maxwe on 2021-04-07.
//

#ifndef VALKYRIE_JSON_INTERNAL_VALUE_H
#define VALKYRIE_JSON_INTERNAL_VALUE_H

#include <json/value.h>

JSON_BEGIN_C_NAMESPACE

/**
 *    0x00  =>  00 | 0 0 0 0 0 0  =  null
 *
 *    0x01  =>  00 | 0 0 0 0 0 1  =  false
 *    0x41  =>  01 | 0 0 0 0 0 1  =  true
 *
 *    0x02  =>  00 | 0 0 0 0 1 0  =  symbol
 *    0x42  =>  01 | 0 0 0 0 1 0  =  inline string
 *    0x82  =>  10 | 0 0 0 0 1 0  =  string ref
 *
 *
 *    0x04  =>  00 | 0 0 0 1 0 0  =  int64
 *    0x44  =>  01 | 0 0 0 1 0 0  =  uint64
 *    0x84  =>  10 | 0 0 0 1 0 0  =  big int
 *
 *    0x08  =>  00 | 0 0 1 0 0 0  =  float64
 *    0x48  =>  01 | 0 0 1 0 0 0  =  big float
 *
 *    0x10  =>  00 | 0 1 0 0 0 0  =  object
 *
 *    0x20  =>  00 | 1 0 0 0 0 0  =  array
 *
 * */

enum json_exact_type{
  JSON_EXACT_TYPE_NULL          = 0x00,
  JSON_EXACT_TYPE_FALSE         = 0x01,
  JSON_EXACT_TYPE_SYMBOL        = 0x02,
  JSON_EXACT_TYPE_INT           = 0x04,
  JSON_EXACT_TYPE_FLOAT         = 0x08,
  JSON_EXACT_TYPE_OBJECT        = 0x10,
  JSON_EXACT_TYPE_ARRAY         = 0x20,
  JSON_EXACT_TYPE_TRUE          = 0x41,
  JSON_EXACT_TYPE_INLINE_STRING = 0x42,  /**< No support yet */
  JSON_EXACT_TYPE_UINT          = 0x44,  /**< No support yet */
  JSON_EXACT_TYPE_BIG_FLOAT     = 0x48,  /**< No support yet */
  JSON_EXACT_TYPE_STRING_REF    = 0x82,  /**< No support yet */
  JSON_EXACT_TYPE_BIG_INT       = 0x84,  /**< No support yet */


  JSON_EXACT_TYPE_BOOLEAN_BIT   = 0x01,
  JSON_EXACT_TYPE_STRING_BIT    = 0x02,
  JSON_EXACT_TYPE_INTEGER_BIT   = 0x04,
  JSON_EXACT_TYPE_FLOAT_BIT     = 0x08,

  JSON_EXACT_TYPE_NUMBER_BITS   = JSON_EXACT_TYPE_INTEGER_BIT | JSON_EXACT_TYPE_FLOAT_BIT
};
typedef unsigned char json_exact_type_t;


struct json_value{
  union{
    struct {
      json_byte_t       padding[JSON_GENERIC_VALUE_SIZE - sizeof(json_exact_type_t)];
      json_exact_type_t type;
    };
    json_array_t       array;
    json_object_t      object;
    json_symbol_t      symbol;
    json_char_t        inlineString[JSON_GENERIC_VALUE_SIZE - sizeof(json_exact_type_t)];
    json_string_ref_t  stringRef;
    json_u64_t         u64;
    json_i64_t         i64;
    json_f64_t         f64;
    json_big_integer_t bigInt;
    json_big_float_t   bigFloat;
  };
};

struct json_object_member{
  json_value_t key;
  json_value_t value;
};

struct json_object{

};


//ASSERT_TYPE_SIZE(json_value_t, JSON_GENERIC_VALUE_SIZE);



JSON_END_C_NAMESPACE

#endif//VALKYRIE_JSON_INTERNAL_VALUE_H
