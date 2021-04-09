//
// Created by Maxwell on 2021-02-16.
//

#ifndef VALKYRIE_INTERNAL_JSON_STRING_H
#define VALKYRIE_INTERNAL_JSON_STRING_H

#include <json/string.h>


JSON_BEGIN_C_NAMESPACE

typedef struct json_symbol{
  json_u32_t         refCount;
  json_u32_t         length;
  json_u64_t         hash;
  json_char_t        data[];
} json_symbol;

typedef struct json_symbol_registry{
  json_symbol_t* symbols;
  json_symbol_t* deadSymbols;
} json_symbol_registry;

typedef json_symbol_registry* json_symbol_registry_t;




JSON_END_C_NAMESPACE


#endif //VALKYRIE_INTERNAL_JSON_STRING_H
