//
// Created by maxwe on 2021-04-07.
//

#ifndef VALKYRIE_JSON_ASYNC_H
#define VALKYRIE_JSON_ASYNC_H

#include "core.h"


JSON_BEGIN_C_NAMESPACE

typedef struct json_async_operation*  json_async_operation_t;

typedef enum json_async_status{
  JSON_OPERATION_COMPLETE,
  JSON_OPERATION_NOT_READY
} json_async_status_t;


/**
 *
 * \return JSON_SUCCESS: operation is now complete, and the result can be retrieved with json_operation_result.
 * */
json_status_t       json_operation_wait(json_async_operation_t operation, json_u64_t timeout);
json_async_status_t json_operation_get_status(json_async_operation_t operation);


/**
 * If operation is not complete
 *   return JSON_NOT_READY
 *
 * If pReturnValue is NULL
 *   If returnValueBufferSize is NULL
 *     If operation's return value is not void and is nodiscard
 *       return JSON_ERROR_DISCARDED_RESULT
 *     return JSON_SUCCESS
 *   *returnValueBufferSize = requiredBufferSize
 *   return JSON_SUCCESS
 * If returnValueBufferSize if NULL
 *   return JSON_ERROR_INVALID_ARGUMENT
 * If *returnValueBufferSize <
 *
 * */
json_status_t       json_operation_result(json_address_t* pReturnValue, json_u64_t* returnValueBufferSize, json_async_operation_t operation);

json_async_status_t json_operation_fulfill(json_async_operation_t operation);



JSON_END_C_NAMESPACE


#endif//VALKYRIE_JSON_ASYNC_H
