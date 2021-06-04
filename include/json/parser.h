//
// Created by maxwe on 2021-04-07.
//

#ifndef VALKYRIE_JSON_PARSER_H
#define VALKYRIE_JSON_PARSER_H

#include "core.h"
#include "context.h"


JSON_BEGIN_C_NAMESPACE

typedef enum json_parse_status{
  JSON_PARSE_SUCCESS,
  JSON_PARSE_IN_PROGRESS,

  JSON_PARSE_ERROR_DOCUMENT_EMPTY,
  JSON_PARSE_ERROR_DOCUMENT_ROOT_NOT_SINGULAR,
  JSON_PARSE_ERROR_INVALID_VALUE,

  JSON_PARSE_ERROR_OBJ_MISSING_NAME,
  JSON_PARSE_ERROR_OBJ_MISSING_COLON,
  JSON_PARSE_ERROR_OBJ_MISSING_COMMA_OR_CURLY_BRACKET,

  JSON_PARSE_ERROR_ARRAY_MISSING_COMMA_OR_SQUARE_BRACKET,

  JSON_PARSE_ERROR_STRING_UNICODE_ESCAPE_INVALID_HEX,
  JSON_PARSE_ERROR_STRING_UNICODE_INVALID_SURROGATE,
  JSON_PARSE_ERROR_STRING_INVALID_ESCAPE_SEQUENCE,
  JSON_PARSE_ERROR_STRING_MISSING_QUOTATION_MARK,
  JSON_PARSE_ERROR_STRING_INVALID_ENCODING,

  JSON_PARSE_ERROR_NUMBER_MISSING_FRACTION,
  JSON_PARSE_ERROR_NUMBER_MISSING_EXPONENT,

  JSON_PARSE_ERROR_TERMINATED,
  JSON_PARSE_ERROR_UNKNOWN_SYNTAX_ERROR
} json_parse_status_t;

typedef struct json_parser*               json_parser_t;


enum json_create_parser_flag_bits{
  JSON_CREATE_PARSER_DEFAULT_FLAGS      = 0x0,
  JSON_CREATE_PARSER_IN_PLACE           = 0x1,
  JSON_CREATE_PARSER_VALIDATE_ENCODING  = 0x2,
  JSON_CREATE_PARSER_ITERATIVE          = 0x4,
  JSON_CREATE_PARSER_STOP_WHEN_DONE     = 0x8,
  JSON_CREATE_PARSER_FULL_PRECISION     = 0x10,
  JSON_CREATE_PARSER_COMMENTS           = 0x20,
  JSON_CREATE_PARSER_RAW_NUMBERS        = 0x40,
  JSON_CREATE_PARSER_TRAILING_COMMAS    = 0x80,
  JSON_CREATE_PARSER_NAN_AND_INF        = 0x100,
  JSON_CREATE_PARSER_ESCAPED_APOSTROPHE = 0x200
};
typedef json_flags_t json_create_parser_flags_t;


/*typedef json_u32_t json_automata_state_t;
typedef json_u8_t  json_automata_input_t;

typedef struct json_automata_state_info{
  json_automata_state_t(* pfnNextStatus)(void* pUserData, const json_automata_input_t* input, json_size_t inputLength);
  json_parse_status_t status;
} json_automata_state_info_t;

typedef struct json_automata{
  json_automata_state_info_t* pStates;
  json_size_t                 stateCount;
  void*                       pUserData;
} json_automata_t;*/


typedef struct json_create_parser_params{
  json_create_parser_flags_t flags;
  json_ctx_t                 context;
  const json_callbacks_t*    customInterface;
  void*                      customUserData;
} json_create_parser_params_t;


json_status_t json_parser_create(json_parser_t* pResultParser, const json_create_parser_params_t* pParams);
void          json_parser_destroy(json_parser_t parser);


json_status_t json_parser_reset(json_parser_t parser);

json_parse_status_t json_parse(json_parser_t parser);
json_parse_status_t json_parse_stream(json_parser_t parser, json_stream_t stream);



JSON_END_C_NAMESPACE



#endif//VALKYRIE_JSON_PARSER_H
