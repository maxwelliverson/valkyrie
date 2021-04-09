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

typedef struct json_parser* json_parser_t;

enum json_create_parser_flag_bits{
  JSON_CREATE_PARSER_DEFAULT_FLAGS = 0x0
};
typedef json_flags_t json_create_parser_flags_t;

typedef struct json_create_parser_params{
  json_create_parser_flags_t flags;
  json_ctx_t context;
} json_create_parser_params_t;


json_status_t json_create_parser(json_parser_t* pResultParser, const json_create_parser_params_t* pParams);
void          json_destroy_parser(json_parser_t parser);



JSON_END_C_NAMESPACE



#endif//VALKYRIE_JSON_PARSER_H
