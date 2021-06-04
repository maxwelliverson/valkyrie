//
// Created by maxwe on 2021-05-21.
//

#ifndef ARGENT_SENDER_H
#define ARGENT_SENDER_H

#include "mailbox.h"


AGT_BEGIN_C_NAMESPACE

typedef struct agt_sender*         agt_sender_t;
typedef struct agt_dynamic_sender* agt_dynamic_sender_t;

AGT_api agt_status_t AGT_stdcall agt_open_sender(_In_ agt_mailbox_t mailbox, _Outptr_result_nullonfailure_ agt_sender_t* pSender, agt_u64_t timeout);
AGT_api void         AGT_stdcall agt_close_sender(_In_ agt_sender_t sender);
AGT_api agt_status_t AGT_stdcall agt_open_dynamic_sender(_In_ agt_dynamic_mailbox_t mailbox, _Outptr_result_nullonfailure_ agt_dynamic_sender_t* pSender, agt_u64_t timeout);
AGT_api void         AGT_stdcall agt_close_dynamic_sender(_In_ agt_dynamic_sender_t sender);

AGT_api agt_status_t AGT_stdcall agt_begin_send(agt_sender_t sender, void** address, agt_u64_t timeout_us);
AGT_api void         AGT_stdcall agt_finish_send(agt_sender_t sender, void* address);

AGT_api agt_status_t AGT_stdcall agt_begin_dynamic_send(agt_dynamic_sender_t sender, void** address, agt_size_t size, agt_u64_t timeout_milliseconds);
AGT_api void         AGT_stdcall agt_finish_dynamic_send(agt_dynamic_sender_t sender, void* address);


AGT_END_C_NAMESPACE

#endif  //ARGENT_SENDER_H
