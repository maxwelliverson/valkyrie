//
// Created by maxwe on 2021-04-08.
//

#ifndef VALKYRIE_INTERNAL_JSON_ASYNC_H
#define VALKYRIE_INTERNAL_JSON_ASYNC_H

#include <json/context.h>
#include <json/task.h>

JSON_BEGIN_C_NAMESPACE


typedef enum json_message_type{
  JSON_MSG_RELEASE_CTX,
  JSON_MSG_RETAIN_CTX,

} json_message_type_t;


typedef struct json_message{
  json_u32_t          nextMsgOffset;
  json_message_type_t msgType;
  json_byte_t         msgData[];
} json_message_t;




typedef struct json_task{
  json_ctx_t               ctx;
  json_task_status_t          asyncStatus;
  json_status_t                opStatus;
  struct json_task* next;
} json_task;




JSON_END_C_NAMESPACE

#endif//VALKYRIE_INTERNAL_JSON_ASYNC_H
