//
// Created by maxwe on 2021-04-08.
//

#include "jsonctx.h"

inline static json_status_t json_create_standard_ctx_(json_standard_ctx* pResult, const json_ctx_setup_params_t* pParams);
inline static json_status_t json_create_async_ctx_(json_async_ctx* pResult, const json_ctx_setup_params_t* pParams);


json_status_t json_create_ctx(json_ctx_t* pContext, const json_ctx_setup_params_t* pParams) {
  if (!pContext || !pParams)
    return JSON_ERROR_INVALID_ARGUMENT;



  if (pParams->flags & JSON_CREATE_CONTEXT_ASYNCHRONOUS)
    return json_cre


  if (pParams && (pParams-> || pParams->pfnReallocNotification || pParams->pfnFreeNotification)) {
    // make json_traced_context object
  } else {
    // only make json_ctx object
  }





  return JSON_SUCCESS;
}
json_status_t json_destroy_context(json_ctx_t context) {}