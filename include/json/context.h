//
// Created by maxwe on 2021-04-07.
//

#ifndef VALKYRIE_JSON_CONTEXT_H
#define VALKYRIE_JSON_CONTEXT_H

#include "core.h"
#include "async.h"

JSON_BEGIN_C_NAMESPACE



/* =============[ Type Declarations ]============= */


/**
 * \brief Contains the "global state" used internally by the whole library.
 *
 *
 *
 * */
typedef struct json_context* json_context_t;





typedef void(*PFN_json_alloc_notification)(void* /* __restrict */ pUserData /**< Pointer to user data. Passed value will be  */,
                                           void* /* __restrict */ returnAddr,
                                           json_u64_t size);

typedef void(*PFN_json_realloc_notification)(void* /* __restrict */ pUserData,
                                             void* returnAddr,
                                             void* originalAddr,
                                             json_u64_t newSize,
                                             json_u64_t originalSize);

typedef void(*PFN_json_free_notification)(void* /* __restrict */ pUserData,
                                          void* /* __restrict */ addr,
                                          json_u64_t size);

enum json_context_flag_bits {
  JSON_CONTEXT_DEFAULT_FLAGS = 0x0 /**< Default allocator flag */,
  JSON_CONTEXT_ASYNCHRONOUS  = 0x1 /**< Context is internally synchronized, and all library calls made within this context are thread safe. When a synchronous library function is called, its asynchronous counterpart is called internally and subsequently waited on.    */

};
enum json_create_context_flag_bits{
  JSON_CREATE_CONTEXT_DEFAULT_FLAGS = 0x0,
  JSON_CREATE_CONTEXT_ASYNCHRONOUS  = 0x1 /**< Indicates that the created context will be asynchronous. */,
  JSON_CREATE_CONTEXT_DEBUG_MEMORY  = 0x2,
  JSON_CREATE_CONTEXT_RETAIN_HANDLE = 0x4
};


typedef json_flags_t json_context_flags_t;
typedef json_flags_t json_create_context_flags_t;






typedef struct json_allocator_callbacks{
  PFN_json_alloc_notification   pfnOnAlloc;
  PFN_json_realloc_notification pfnOnRealloc;
  PFN_json_free_notification    pfnOnFree;
  void*                         pUserData;
} json_allocator_callbacks_t;





typedef struct json_context_setup_params{
  json_create_context_flags_t       flags;
  const json_allocator_callbacks_t* memoryNotifications;
  json_context_t                    otherContext;
} json_context_setup_params_t;





/**
 *
 * Creates a new Context object.
 *
 * \returns On success, returns JSON_SUCCESS.
 * \pre  pContext must not point to a valid Context object.
 * \post pContext will point to a valid Context object if and only if JSON_SUCCESS is returned.
 * */
json_status_t json_create_context(json_context_t* pContext                   /**< [out] Return address of the new Context object */,
                                  const json_context_setup_params_t* pParams /**< [in]  Optional pointer to parameters to control context creation. If null, default parameters are used. */);

/**
 * Destroys a Context object.
 * Contexts are reference counted internally, so the underlying object won't actually be
 * destroyed until every retained handle is destroyed. If context is a JSON_NULL_HANDLE,
 * this function is a noop.
 */
void json_destroy_context(json_context_t context /**< [in] Handle to the object to be destroyed */);






json_status_t json_ctx_next_async_operation(json_async_operation_t* pOperation, json_context_t ctx);






JSON_END_C_NAMESPACE



#endif//VALKYRIE_JSON_CONTEXT_H
