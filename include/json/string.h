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


/**
 * Null characters within pString are ignored, unless stringLength is
 * JSON_DONT_CARE, in which case pString will be assumed to be null
 * terminated. If stringLength is JSON_DONT_CARE, and pString is NOT
 * null terminated, here there be dragons. Don't do it.
 * If stringLength is JSON_DONT_CARE, pString will be treated as a null terminated.
 */
json_status_t json_symbol_lookup(json_symbol_t* pResultSymbol, const json_char_t* pString, json_size_t stringLength);


void json_symbol_retain(json_symbol_t symbol);
void json_symbol_release(json_symbol_t symbol);




JSON_END_C_NAMESPACE



#endif//VALKYRIE_JSON_STRING_H
