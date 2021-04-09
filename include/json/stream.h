//
// Created by maxwe on 2021-04-07.
//

#ifndef VALKYRIE_JSON_STREAM_H
#define VALKYRIE_JSON_STREAM_H

#include "core.h"
#include "file.h"
#include "string.h"


JSON_BEGIN_C_NAMESPACE






enum json_stream_flag_bits{
  JSON_STREAM_INPUT  = 0x1,
  JSON_STREAM_OUTPUT = 0x2
};
enum json_create_stream_flag_bits{
  JSON_CREATE_STREAM_FROM_FILE   = 0x1,
  JSON_CREATE_STREAM_FROM_BUFFER = 0x2,
  JSON_CREATE_STREAM_WRITE_ONLY  = 0x4,
  JSON_CREATE_STREAM_READ_ONLY   = 0x8
};

typedef json_flags_t json_stream_flags_t;
typedef json_flags_t json_create_stream_flags_t;



typedef struct json_create_stream_params{
  json_create_stream_flags_t flags;
  json_ctx_t                 context;
  json_file_t                file;
  json_buffer_t              buffer;
} json_create_stream_params_t;






json_status_t       json_create_stream(json_stream_t* pResultStream, const json_create_stream_params_t* pParams);
void                json_destroy_stream(json_stream_t stream);


json_stream_flags_t json_stream_get_flags(json_stream_t stream);





JSON_END_C_NAMESPACE


#endif//VALKYRIE_JSON_STREAM_H
