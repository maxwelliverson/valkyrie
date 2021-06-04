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
  JSON_CTX_THREAD_SAFE  = 0x0001,
  JSON_CTX_ASYNCHRONOUS = 0x0002 /**< Context is internally synchronized, and all library calls made within this context are thread safe. When a synchronous library function is called, its asynchronous counterpart is called internally and subsequently waited on.    */,
  JSON_CTX_LOGGED       = 0x0004,
  JSON_CTX_DEBUG_MEMORY = 0x0008 /**< Enabled memory tracing, guard buffers, leak detection, and double free detection on internal allocations. */
};
enum json_create_ctx_flag_bits{
  JSON_CREATE_CTX_DEFAULT_FLAGS = 0x0,
  JSON_CREATE_CTX_THREAD_SAFE   = 0x1,
  JSON_CREATE_CTX_ASYNCHRONOUS  = 0x2 /**< Indicates that the created context will be asynchronous. */
};


typedef json_flags_t json_ctx_flags_t;
typedef json_flags_t json_create_ctx_flags_t;






typedef struct json_ctx_memory_callbacks{
  void(* pfnOnAlloc)(void* pUserData, void* returnAddr, json_u64_t size);
  void(* pfnOnRealloc)(void* pUserData, void* returnAddr, void* originalAddr, json_u64_t newSize, json_u64_t originalSize);
  void(* pfnOnFree)(void* pUserData, void* addr, json_u64_t size);
  void(* pfnOverflowHandler)(void* pUserData, void* allocationAddress, json_u64_t size, json_u64_t guardPageSize, json_u8_t expectedPattern);
  void(* pfnUnderflowHandler)(void* pUserData, void* allocationAddress, json_u64_t size, json_u64_t guardPageSize, json_u8_t expectedPattern);
  void(* pfnLeakHandler)(void* pUserData, void* leakedAddress, json_u64_t allocationSize);
  void(* pfnDoubleFreeHandler)(void* pUserData, void* doubleFreedAddress, json_u64_t allocationSize);
  void*  pUserData;
} json_ctx_memory_callbacks_t;

typedef struct json_ctx_allocator_info{
  const json_ctx_memory_callbacks_t* pCallbacks;
  json_u64_t                         guardPageSize;
  json_u64_t                         virtualAddressSpaceSize;
} json_ctx_allocator_info_t;

typedef struct json_ctx_log_handlers{
  void(* pfnLogTraceMsg)(void* pUserData, json_command_id_t apiCallId, ...);
  void(* pfnLogErrorMsg)(void* pUserData, json_status_t code, json_string_ref_t message, ...);
  void* pUserData;
} json_ctx_log_handlers_t;


typedef struct json_ctx_create_info{
  json_create_ctx_flags_t          flags;
  const json_ctx_allocator_info_t* pAllocatorInfo;
  const json_ctx_log_handlers_t*   pLogHandlers;
} json_ctx_create_info_t;

extern const json_ctx_log_handlers_t* const json_standard_log_handlers;





/**
 *
 * Creates a new Context object.
 * Call json_ctx_release when done using the context.
 *
 * \returns On success, returns JSON_SUCCESS.
 * \pre  pContext must not point to a valid Context object.
 * \post pContext will point to a valid Context object if and only if JSON_SUCCESS is returned.
 * */
json_status_t json_ctx_create(json_ctx_t* pContext                /**< [out] Return address of the new Context object */,
                              const json_ctx_create_info_t* pInfo /**< [in]  Optional pointer to parameters to control context creation. If null, default parameters are used. */);

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
