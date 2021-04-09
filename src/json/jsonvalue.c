//
// Created by maxwe on 2021-04-07.
//

#include "jsonvalue.h"

#include <assert.h>
#include <string.h>





inline static json_bool_t json_type_is_valid__(json_type_t type) {
  switch (type) {
    case JSON_TYPE_NULL:
    case JSON_TYPE_FALSE:
    case JSON_TYPE_TRUE:
    case JSON_TYPE_STRING:
    case JSON_TYPE_INT:
    case JSON_TYPE_FLOAT:
    case JSON_TYPE_OBJECT:
    case JSON_TYPE_ARRAY:
      return JSON_TRUE;
    default:
      return JSON_FALSE;
  }
}
inline static json_bool_t json_exact_type_is_valid__(json_exact_type_t type) {
  switch ((enum json_exact_type)type) {
    case JSON_EXACT_TYPE_NULL:
    case JSON_EXACT_TYPE_FALSE:
    case JSON_EXACT_TYPE_SYMBOL:
    case JSON_EXACT_TYPE_INT:
    case JSON_EXACT_TYPE_FLOAT:
    case JSON_EXACT_TYPE_OBJECT:
    case JSON_EXACT_TYPE_ARRAY:
    case JSON_EXACT_TYPE_TRUE:
    case JSON_EXACT_TYPE_INLINE_STRING:
    case JSON_EXACT_TYPE_UINT:
    case JSON_EXACT_TYPE_BIG_FLOAT:
    case JSON_EXACT_TYPE_STRING_REF:
    case JSON_EXACT_TYPE_BIG_INT:
    case JSON_EXACT_TYPE_NUMBER_BITS:
      return JSON_TRUE;
    default:
      return JSON_FALSE;
  }
}

/**
 * \pre  value is not in a valid state.
 * \post value is in a valid state of type InlineString
 */
inline static void        json_inline_string_init__(json_value_t value, json_string_ref_t str) {
  assert(str.length <= JSON_INLINE_STRING_MAX_LENGTH);
  memset((void*)value->inlineString, 0, JSON_INLINE_STRING_MAX_LENGTH + 1);
  memcpy(value->inlineString, str.data, str.length);
  value->inlineString[JSON_INLINE_STRING_MAX_LENGTH] = JSON_INLINE_STRING_MAX_LENGTH - str.length;
  value->type = JSON_EXACT_TYPE_INLINE_STRING;
}

inline static void        json_string_ref_init__(json_value_t val, json_string_ref_t str) {
  val->stringRef = str;
  val->type      = JSON_EXACT_TYPE_STRING_REF;
}
inline static void        json_symbol_init__(json_value_ref_t val, json_string_ref_t str);





/**
 * \pre  value is in a valid state of any type
 * \post value is in an invalid state
 */
inline static json_status_t json_value_release__(json_value_ref_t val) {

}



json_bool_t       json_value_is_type(json_value_t value, json_type_t type){

  assert(json_type_is_valid__(type));
  assert(json_exact_type_is_valid__(value->type));

  return (value->type & type) == type;
}
json_type_t       json_value_get_type(json_value_t value){
  switch ((enum json_exact_type)value->type) {
    case JSON_EXACT_TYPE_NULL:          return JSON_TYPE_NULL;

    case JSON_EXACT_TYPE_FALSE:         return JSON_TYPE_FALSE;

    case JSON_EXACT_TYPE_TRUE:          return JSON_TYPE_TRUE;

    case JSON_EXACT_TYPE_SYMBOL:
    case JSON_EXACT_TYPE_INLINE_STRING:
    case JSON_EXACT_TYPE_STRING_REF:    return JSON_TYPE_STRING;

    case JSON_EXACT_TYPE_INT:
    case JSON_EXACT_TYPE_UINT:
    case JSON_EXACT_TYPE_BIG_INT:       return JSON_TYPE_INT;

    case JSON_EXACT_TYPE_FLOAT:
    case JSON_EXACT_TYPE_BIG_FLOAT:     return JSON_TYPE_FLOAT;

    case JSON_EXACT_TYPE_OBJECT:        return JSON_TYPE_OBJECT;

    case JSON_EXACT_TYPE_ARRAY:         return JSON_TYPE_ARRAY;

    default:
      __assume(0);
      assert(0);
      return JSON_TYPE_NULL;
  }
}




void              json_value_set_value(json_value_ref_t value,   json_value_t otherValue) {
  value.value->
}
void              json_value_set_object(json_value_ref_t value,  json_object_t object);
void              json_value_set_array(json_value_ref_t value,   json_array_ref_t array);
void              json_value_set_string(json_value_ref_t value,  json_string_ref_t string, json_bool_t copyString);
//void              json_value_set_symbol(json_value_ref_t value,  json_symbol_t symbol);
void              json_value_set_uint(json_value_ref_t value,    json_u64_t u64);
void              json_value_set_int(json_value_ref_t value,     json_i64_t u64);
void              json_value_set_float(json_value_ref_t value,   json_f64_t f64);
void              json_value_set_boolean(json_value_ref_t value, json_bool_t boolValue);
void              json_value_set_true(json_value_ref_t value);
void              json_value_set_false(json_value_ref_t value);
void              json_value_set_null(json_value_ref_t value);