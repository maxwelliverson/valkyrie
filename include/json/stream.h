//
// Created by maxwe on 2021-04-07.
//

#ifndef VALKYRIE_JSON_STREAM_H
#define VALKYRIE_JSON_STREAM_H

#include "core.h"
#include "file.h"
#include "string.h"


JSON_BEGIN_C_NAMESPACE



/**
 * When creating a read-only stream, the following members may be null:
 *  - putBegin
 *  - put
 *  - flush
 *  - putEnd
 *
 * When creating a write-only stream, the following members may be null:
 *  - peek
 *  - take
 *  - tell
 *  - putBegin
 *  - putEnd
 *
 * When creating a read-write stream, no members may be null.
 * */
typedef struct json_stream_interface{
  // Look at the next character in the stream
  json_char_t  (* peek)(const void* pUserData);

  // Take the next character in the stream, and then increment the current position
  json_char_t  (* take)(void* pUserData);

  // Return the number of characters read thus far
  json_size_t  (* tell)(void* pUserData);

  // Begin a series of write operations at the current position
  json_char_t* (* putBegin)(void* pUserData);

  // Writes a single character
  void         (* put)(void* pUserData, json_char_t c);

  // Writes many characters
  void         (* puts)(void* pUserData, const json_char_t* string, json_u64_t length);

  // Flushes the buffer
  void         (* flush)(void* pUserData);

  // Ends the series of write operations started at pBegin.
  // pBegin must have been obtained from a prior call to putBegin
  // Returns the total number of characters written since the
  // corresponding call to putBegin.
  json_size_t  (* putEnd)(void* pUserData, json_char_t* pBegin);
} json_stream_interface_t;



enum json_stream_flag_bits{
  JSON_STREAM_INPUT  = 0x1,
  JSON_STREAM_OUTPUT = 0x2
};
enum json_create_stream_flag_bits{
  JSON_CREATE_STREAM_FROM_FILE   = 0x1,
  JSON_CREATE_STREAM_FROM_BUFFER = 0x2,
  JSON_CREATE_STREAM_FROM_STDIO  = 0x4,
  JSON_CREATE_STREAM_WRITE       = 0x8,
  JSON_CREATE_STREAM_READ        = 0x10
};

typedef json_flags_t json_stream_flags_t;
typedef json_flags_t json_create_stream_flags_t;


typedef struct json_stream_buffering_info{
  json_size_t bufferSize;         /**<            Size in bytes of buffers to be used. If ppExternalBuffers is nonnull, each address in the array must refer to a location in memory to which this many bytes can be written. */
  json_size_t bufferCount;        /**<            Number of buffers to be used. If ppExternalBuffers is nonnull, this is the length of that array. */
  void**      ppExternalBuffers;  /**< [optional] Array of bufferCount buffers, each bufferSize bytes long. If null, internal buffers will be allocated by the context for use. Otherwise, the external buffers must remain valid for the entire lifetime of the stream. */
} json_stream_buffering_info_t;


typedef struct json_stream_create_info{
  json_ctx_t                          context;
  json_create_stream_flags_t          flags;
  const json_stream_buffering_info_t* pBuffering;
  const json_stream_interface_t*      pInterface;
  union{
    json_file_t  file;
    struct {
      void*      address;
      json_u64_t size;
    } buffer;
  };
} json_stream_create_info_t;






json_status_t       json_stream_create(json_stream_t* pResultStream, const json_stream_create_info_t* pParams);
void                json_stream_destroy(json_stream_t stream);


json_stream_flags_t json_stream_get_flags(json_stream_t stream);





JSON_END_C_NAMESPACE


#endif//VALKYRIE_JSON_STREAM_H
