//
// Created by maxwe on 2021-04-07.
//

#ifndef VALKYRIE_JSON_TASK_H
#define VALKYRIE_JSON_TASK_H

#include "core.h"


JSON_BEGIN_C_NAMESPACE



typedef enum json_task_status{
  JSON_TASK_COMPLETE,
  JSON_TASK_NOT_READY,
  JSON_TASK_BUSY
} json_task_status_t;


/**
 *
 * \return JSON_SUCCESS: task is now complete, and the result can be retrieved with json_task_result.
 * */
json_status_t      json_task_wait(json_task_t task, json_u64_t timeout);
json_bool_t        json_task_is_complete(json_task_t task);


/**
 * If task is not complete
 *   return JSON_NOT_READY
 *
 * If pReturnValue is NULL
 *   If returnValueBufferSize is NULL
 *     If task's return value is not void and is nodiscard
 *       return JSON_ERROR_DISCARDED_RESULT
 *     return JSON_SUCCESS
 *   *returnValueBufferSize = requiredBufferSize
 *   return JSON_SUCCESS
 * If returnValueBufferSize if NULL
 *   return JSON_ERROR_INVALID_ARGUMENT
 * If *returnValueBufferSize <
 *
 * */
json_status_t      json_task_result(json_address_t* pReturnValue, json_u64_t* returnValueBufferSize, json_task_t task);


json_task_status_t json_task_fulfill(json_task_t task);
json_status_t      json_task_discard(json_task_t task);



JSON_END_C_NAMESPACE


#endif//VALKYRIE_JSON_TASK_H
