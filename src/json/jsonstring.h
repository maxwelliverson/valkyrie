//
// Created by Maxwell on 2021-02-16.
//

#ifndef VALKYRIE_INTERNAL_JSON_STRING_H
#define VALKYRIE_INTERNAL_JSON_STRING_H

#include <json/string.h>

#include "internal_fwd.h"


JSON_BEGIN_C_NAMESPACE


inline static json_u32_t inline_string_get_length__(const char* inlineString) {
  return JSON_INLINE_STRING_MAX_LENGTH - inlineString[JSON_INLINE_STRING_MAX_LENGTH];
}
inline static void       inline_string_set_length__(char* inlineString, json_u32_t length) {
  json_assert_internal(length <= JSON_INLINE_STRING_MAX_LENGTH);
  inlineString[JSON_INLINE_STRING_MAX_LENGTH] = JSON_INLINE_STRING_MAX_LENGTH - (char)length;
}


json_status_t  symbol_registry_init(json_symbol_registry_t registry, json_u32_t initSize, json_internal_allocator_t allocator);
void           symbol_registry_cleanup(json_symbol_registry_t registry);

json_symbol_t* symbol_registry_find(json_symbol_registry_t registry, const char* buffer, json_u64_t length);
json_symbol_t  symbol_registry_register_symbol(json_symbol_registry_t registry, const char* string, json_u64_t length);
json_symbol_t  symbol_registry_remove(json_symbol_registry_t registry, const char* string, json_u64_t length);
void           symbol_registry_remove_symbol(json_symbol_registry_t registry, json_symbol_t symbol);
json_bool_t    symbol_registry_erase(json_symbol_registry_t registry, const char* string, json_u64_t length);
void           symbol_registry_erase_symbol(json_symbol_registry_t registry, json_symbol_t symbol);
void           symbol_registry_clear(json_symbol_registry_t registry);



JSON_END_C_NAMESPACE


#endif //VALKYRIE_INTERNAL_JSON_STRING_H
