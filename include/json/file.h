//
// Created by maxwe on 2021-04-07.
//

#ifndef VALKYRIE_JSON_FILE_H
#define VALKYRIE_JSON_FILE_H

#include "core.h"


JSON_BEGIN_C_NAMESPACE

typedef struct json_file_manager* json_file_manager_t;
typedef struct json_file*         json_file_t;


#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
typedef void* json_file_native_t;
#else
typedef int   json_file_native_t;
#endif


enum json_file_status_flag_bits{
  JSON_FILE_OPEN      = 0x1,
  JSON_FILE_MAPPED    = 0x2,
  JSON_FILE_ANONYMOUS = 0x4
};
enum json_open_file_flag_bits{
  JSON_CREATE_FILE_DEFAULT_FLAGS  = 0x0,
  JSON_CREATE_FILE_OVERWRITE      = 0x1,
  JSON_CREATE_FILE_RESERVE_MEMORY = 0x2,
  JSON_CREATE_FILE_CLONE          = 0x4
};

typedef json_flags_t json_file_status_flags_t;
typedef json_flags_t json_open_file_flags_t;


/**
 *  Struct for holding the parameters of json_open_file
 *
 *  \msc
 *
 *  \endmsc
 *
 * */
typedef struct json_open_file_params{
  json_open_file_flags_t flags;
  const char*            name;          // If name is null, an anonymous, memory backed file is created. This can later be written to memory by cloning
  json_u64_t             reservedSize;  // Ignored if flag JSON_CREATE_FILE_RESERVE_MEMORY is not set, uses default if
  json_file_t            fileToClone;   // Ignored if flag JSON_CREATE_FILE_CLONE is not set

} json_open_file_params_t;



json_file_native_t       json_file_get_native(json_file_t file);
json_file_status_flags_t json_file_get_flags(json_file_t file);


json_status_t            json_open_file(json_file_t* pResult);






JSON_END_C_NAMESPACE



#endif//VALKYRIE_JSON_FILE_H
