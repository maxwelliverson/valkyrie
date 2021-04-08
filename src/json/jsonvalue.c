//
// Created by maxwe on 2021-04-07.
//

#include "jsonvalue.h"





json_bool_t       json_value_is_type(json_value_t value, json_type_t type){
  switch (type) {
    case JSON_TYPE_NULL:   return value->type & JSON_EXACT_TYPE_NULL;
    case JSON_TYPE_FALSE:  return value->type & JSON_EXACT_TYPE_FALSE;
    case JSON_TYPE_TRUE:   return value->type & JSON_EXACT_TYPE_TRUE;
    case JSON_TYPE_STRING: return value->type & JSON_EXACT_TYPE_STRING_BIT;
    case JSON_TYPE_NUMBER: return value->type & JSON_EXACT_TYPE_NUMBER_BITS;
    case JSON_TYPE_OBJECT: return value->type & JSON_EXACT_TYPE_OBJECT;
    case JSON_TYPE_ARRAY:  return value->type & JSON_EXACT_TYPE_ARRAY;
    default:
      return JSON_FALSE;
  }
}
json_type_t       json_value_get_type(json_value_t value){

}