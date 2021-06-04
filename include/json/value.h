//
// Created by maxwe on 2021-04-07.
//

#ifndef VALKYRIE_JSON_VALUE_H
#define VALKYRIE_JSON_VALUE_H

#include "core.h"
#include "document.h"
#include "string.h"


JSON_BEGIN_C_NAMESPACE

typedef struct json_value          json_value;
typedef json_value*                json_value_t;
typedef const json_value*          json_const_value_t;
typedef struct json_object*        json_object_t;
typedef struct json_object_member* json_object_member_t;
typedef struct json_symbol*        json_symbol_t;
typedef struct json_big_integer*   json_big_integer_t;
typedef struct json_big_float*     json_big_float_t;


typedef struct json_array{
  json_u32_t  size;
  json_u32_t  capacity;
  json_value* data;
} json_array_t;



typedef enum json_type{
  JSON_TYPE_NULL     = 0x00,
  JSON_TYPE_FALSE    = 0x01,
  JSON_TYPE_TRUE     = 0x41,
  JSON_TYPE_STRING   = 0x02,
  JSON_TYPE_INT      = 0x04,
  JSON_TYPE_FLOAT    = 0x08,
  JSON_TYPE_OBJECT   = 0x10,
  JSON_TYPE_ARRAY    = 0x20
} json_type_t;






typedef struct json_array_ref{
  json_u64_t        size;
  const json_value* data;
} json_array_ref_t;

typedef struct json_value_ref{
  json_value_t   value;
  json_ctx_t     ctx;
}  json_value_ref_t;
typedef struct json_object_ref{
  json_object_t  object;
  json_ctx_t     ctx;
} json_object_ref_t;




json_type_t       json_value_get_type(json_value_t value);
json_bool_t       json_value_is_type(json_value_t value, json_type_t type);

void              json_value_set_value(json_value_ref_t value,   json_value_t otherValue);
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


json_status_t     json_value_get_object(json_object_t* pObject,    json_value_ref_t value);
json_status_t     json_value_get_array(json_array_ref_t* arrayRef, json_value_ref_t value);
json_status_t     json_value_get_string(json_string_ref_t* string, json_value_ref_t value);
json_status_t     json_value_get_uint(json_u64_t* u64,             json_value_ref_t value);
json_status_t     json_value_get_int(json_i64_t* i64,              json_value_ref_t value);
json_status_t     json_value_get_float(json_f64_t* f64,            json_value_ref_t value);
json_status_t     json_value_get_boolean(json_bool_t* boolValue,   json_value_ref_t value);





json_status_t     json_set_value_to_value(json_document_t document, json_object_t object,   json_string_ref_t key, json_value_t value);

json_status_t     json_set_value_to_object(json_document_t document, json_object_t object, json_string_ref_t key, const json_object_t* pNewValue);
json_status_t     json_set_value_to_array(json_document_t document, json_object_t object, json_string_ref_t key, json_array_ref_t arrayRef);




json_status_t     json_get_value(json_value_t*     value,
                                 json_document_t   document,
                                 json_object_t     object,
                                 json_string_ref_t key);

/*
json_status_t     json_get_member(json_member_t* pMember,    json_document_t document, json_object_t* pObject, json_string_ref_t key);

json_status_t     json_member_set_key  (json_document_t document, json_member_t* pMember, json_string_ref_t memberName);
json_status_t     json_member_set_value(json_document_t document, json_member_t* pMember, json_value_t value);

*/





JSON_END_C_NAMESPACE


/*
#if defined(__cplusplus)
#define ASSERT_TYPE_SIZE(type, size) static_assert(sizeof(type) == (size), #type " is not " #size " bytes")

#else
#define ASSERT_TYPE_SIZE(type, size) _Static_assert(sizeof(type) == (size), #type " is not " #size " bytes")
#endif

ASSERT_TYPE_SIZE(json_byte_t, 1);
ASSERT_TYPE_SIZE(json_utf8_t, 1);
ASSERT_TYPE_SIZE(json_string_ref_t, 16);
*/




#endif//VALKYRIE_JSON_VALUE_H
