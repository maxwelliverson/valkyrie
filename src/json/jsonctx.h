//
// Created by Maxwell on 2021-02-16.
//

#ifndef VALKYRIE_INTERNAL_JSON_CTX_H
#define VALKYRIE_INTERNAL_JSON_CTX_H

#include <json/context.h>
#include "jsonvalue.h"
#include "jsonmm.h"
#include "jsonvmm.h"
#include "jsonstring.h"


JSON_BEGIN_C_NAMESPACE

//json_status_t json_internal_lock();


typedef const struct json_ctx* json_const_ctx_t;


typedef struct json_standard_ctx {

  json_internal_allocator allocator;

  json_object_pool      objectPool;
  json_value_pool       valuePool;
  json_symbol_registry  symbolRegistry;



} json_standard_ctx;
typedef struct json_async_ctx {

} json_async_ctx;






typedef struct json_ctx_vtable_record{
  json_status_t(* const setValueSetValue)(void*, json_value_t, const void*, json_exact_type_t);
} json_ctx_vtable_record;
typedef const  json_ctx_vtable_record*  json_ctx_vtable_t;

extern const json_ctx_vtable_record json_standard_ctx_vtable;
extern const json_ctx_vtable_record json_async_ctx_vtable;


typedef struct json_ctx_backend{
  json_ctx_vtable_t vtable;
  json_ctx_flags_t  flags;
  union{
    json_async_ctx    asyncCtx;
    json_standard_ctx standardCtx;
  };
} json_ctx_backend_t;


typedef struct json_ctx{
  json_ctx_backend_t* backend;
  json_
} json_ctx;


JSON_END_C_NAMESPACE

#endif//VALKYRIE_INTERNAL_JSON_CTX_H
