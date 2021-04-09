//
// Created by maxwe on 2021-04-07.
//

#ifndef VALKYRIE_JSON_DOCUMENT_H
#define VALKYRIE_JSON_DOCUMENT_H

#include "core.h"
#include "context.h"
#include "file.h"


JSON_BEGIN_C_NAMESPACE



typedef struct json_document*   json_document_t;


typedef enum json_document_flags{
  JSON_DOCUMENT_DEFAULT_FLAGS           = 0x0,
  JSON_DOCUMENT_SHARED_MEMORY           = 0x1
} json_document_flags_t;


enum json_create_document_flag_bits{
  JSON_CREATE_DOCUMENT_OPEN_FILE        = 0x1,
  JSON_CREATE_DOCUMENT_REVERSIBLE       = 0x2,
};

typedef json_flags_t json_create_document_flags_t;



typedef struct json_create_document_params{
  json_create_document_flags_t flags;
  json_ctx_t               context;
  json_file_t                  file;
} json_create_document_params_t;


json_status_t json_create_document(json_document_t* pDocument, json_string_ref_t documentName);
json_status_t json_destroy_document(json_document_t document);







JSON_END_C_NAMESPACE



#endif//VALKYRIE_JSON_DOCUMENT_H
