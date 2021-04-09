//
// Created by maxwe on 2021-04-07.
//

#ifndef VALKYRIE_JSON_CONTEXT_H
#define VALKYRIE_JSON_CONTEXT_H

#include "core.h"
#include "stream.h"
#include "task.h"

JSON_BEGIN_C_NAMESPACE



/* =============[ Type Declarations ]============= */


enum json_ctx_flag_bits {
  JSON_CONTEXT_ASYNCHRONOUS  = 0x1 /**< Context is internally synchronized, and all library calls made within this context are thread safe. When a synchronous library function is called, its asynchronous counterpart is called internally and subsequently waited on.    */,
  JSON_CONTEXT_LOGGED        = 0x2,
  JSON_CONTEXT_TRACED_MEMORY = 0x4
};
enum json_create_ctx_flag_bits{
  JSON_CREATE_CONTEXT_DEFAULT_FLAGS = 0x0,
  JSON_CREATE_CONTEXT_ASYNCHRONOUS  = 0x1 /**< Indicates that the created context will be asynchronous. */,
  JSON_CREATE_CONTEXT_DEBUG_MEMORY  = 0x2 /**< Enabled guard buffers, leak detection, and double free detection on internal allocations. */
};


typedef json_flags_t json_ctx_flags_t;
typedef json_flags_t json_create_ctx_flags_t;






typedef struct json_ctx_allocation_callbacks{
  void(* pfnOnAlloc)(void* pUserData, void* returnAddr, json_u64_t size);
  void(* pfnOnRealloc)(void* pUserData, void* returnAddr, void* originalAddr, json_u64_t newSize, json_u64_t originalSize);
  void(* pfnOnFree)(void* pUserData, void* addr, json_u64_t size);
  void*                         pUserData;
} json_ctx_allocation_callbacks_t;

typedef struct json_ctx_setup_params{
  json_create_ctx_flags_t                flags;
  const json_ctx_allocation_callbacks_t* allocationCallbacks;
  json_stream_t                          opLog;
  json_stream_t                          errorLog;
  json_u64_t                             virtualAddressSpaceSize;
} json_ctx_setup_params_t;





/**
 *
 * Creates a new Context object.
 * Call json_ctx_release when done using the context.
 *
 * \returns On success, returns JSON_SUCCESS.
 * \pre  pContext must not point to a valid Context object.
 * \post pContext will point to a valid Context object if and only if JSON_SUCCESS is returned.
 * */
json_status_t json_create_ctx(json_ctx_t* pContext                   /**< [out] Return address of the new Context object */,
                                  const json_ctx_setup_params_t* pParams /**< [in]  Optional pointer to parameters to control context creation. If null, default parameters are used. */);

/**
 * Retains a Context handle.
 * Contexts are reference counted internally, and retaining the handle increases the
 * reference count. Each call to json_ctx_retain must have a matching call to
 * json_ctx_release.
 */
json_status_t json_ctx_retain(json_ctx_t context);
/**
 * Releases a Context handle.
 * Contexts are reference counted internally, so the underlying object will be destroyed
 * when the last retained handle is released. If context is a JSON_NULL_HANDLE, this
 * function does nothing.
 */
void          json_ctx_release(json_ctx_t context);


json_status_t json_ctx_next_task(json_task_t* pOperation, json_ctx_t ctx);

json_status_t json_ctx_tidy_symbols(json_ctx_t ctx);






JSON_END_C_NAMESPACE



#endif//VALKYRIE_JSON_CONTEXT_H
