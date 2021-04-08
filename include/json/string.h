//
// Created by maxwe on 2021-04-07.
//

#ifndef VALKYRIE_JSON_STRING_H
#define VALKYRIE_JSON_STRING_H

#include "core.h"


JSON_BEGIN_C_NAMESPACE



typedef struct json_string_ref   {
  const json_char_t* data;
  json_size_t        length;
} json_string_ref_t;


#define JSON_STRING_LITERAL(...) ((json_string_ref_t){ __VA_ARGS__, sizeof(__VA_ARGS__) - 1 })


json_string_ref_t json_cstring_to_string_ref(const json_char_t* pString);



JSON_END_C_NAMESPACE



#endif//VALKYRIE_JSON_STRING_H
