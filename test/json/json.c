//
// Created by maxwe on 2021-05-18.
//

#include <json/stream.h>
#include <json/context.h>

#include <stdio.h>
#include <string.h>

#define zero_init(var) memset(&(var), 0, sizeof(var))

#define scan_into_buffer(from, format) (sscanf_s(from, format, bufferCursor, remainingBuffer), (lastBufferEntry = bufferCursor, (update_cursor(), lastBufferEntry)))
#define update_cursor() (lastScannedLength = strlen(bufferCursor), (bufferCursor += lastScannedLength, remainingBuffer -= lastScannedLength))
/*#define update_cursor() \
  do { \
    json_size_t lastScannedLength = strlen(bufferCursor) + 1; \
    bufferCursor += lastScannedLength; \
    remainingBuffer -= lastScannedLength; \
  } while(0);*/

#define check_status() \
  do { \
    if ( status != JSON_SUCCESS ) { \
      fprintf(stderr, "\n\n\nError at %s:%d\n\n\t=> %s\n\n", __FILE__, __LINE__, json_status_description(status));                      \
      goto exit;                \
    }                               \
  } while(0)

#define INLINE_BUFFER_SIZE (1024 * 4)

int main(int argc, char** argv){

  /*json_ctx_log_handlers_t     ctxLogHandlers;
  json_ctx_memory_callbacks_t ctxMemoryCallbacks;
  json_ctx_allocator_info_t   ctxAllocatorInfo;*/



  json_status_t   status    = JSON_SUCCESS;

  json_ctx_t      ctx       = JSON_NULL_HANDLE;
  json_file_t     fromFile  = JSON_NULL_HANDLE;
  json_stream_t   inStream  = JSON_NULL_HANDLE;
  json_stream_t   outStream = JSON_NULL_HANDLE;
  json_document_t document  = JSON_NULL_HANDLE;

  json_ctx_create_info_t    ctxCreateInfo;
  json_file_create_info_t   fileCreateInfo;
  json_stream_create_info_t inStreamCreateInfo;
  json_stream_create_info_t outStreamCreateInfo;


  zero_init(ctxCreateInfo);
  zero_init(fileCreateInfo);
  zero_init(inStreamCreateInfo);
  zero_init(outStreamCreateInfo);

  json_char_t               inlineBuffer[INLINE_BUFFER_SIZE];
  json_char_t*              bufferCursor = inlineBuffer;
  json_size_t               remainingBuffer = INLINE_BUFFER_SIZE;

  json_size_t               lastScannedLength;
  const json_char_t*        lastBufferEntry;

  zero_init(inlineBuffer);




  ctxCreateInfo.flags = JSON_CREATE_CTX_DEFAULT_FLAGS;

  status = json_ctx_create(&ctx, &ctxCreateInfo);
  check_status();

  fileCreateInfo.flags   = JSON_CREATE_FILE_DEFAULT_FLAGS;
  fileCreateInfo.name    = scan_into_buffer(argv[argc - 1], "%s.json");
  fileCreateInfo.context = ctx;

  status = json_file_open(&fromFile, &fileCreateInfo);
  check_status();

  inStreamCreateInfo.context = ctx;
  inStreamCreateInfo.flags   = JSON_CREATE_STREAM_FROM_FILE | JSON_CREATE_STREAM_READ;
  inStreamCreateInfo.file    = fromFile;

  status = json_stream_create(&inStream, &inStreamCreateInfo);
  check_status();

  outStreamCreateInfo.context = ctx;
  outStreamCreateInfo.flags   = JSON_CREATE_STREAM_FROM_STDIO | JSON_CREATE_STREAM_WRITE;

  status = json_stream_create(&outStream, &outStreamCreateInfo);
  check_status();







exit:

  if ( outStream )
    json_stream_destroy(outStream);

  if ( inStream )
    json_stream_destroy(inStream);

  if ( fromFile )
    json_file_close(fromFile);

  if ( ctx )
    json_ctx_release(ctx);

  return status;
}