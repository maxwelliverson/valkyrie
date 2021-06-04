//
// Created by maxwe on 2021-05-24.
//


#ifndef ARGENT_MAILBOX_MSSR_H
#define ARGENT_MAILBOX_MSSR_H

#include "../mailbox.h"
#include "../receiver.h"

AGT_BEGIN_C_NAMESPACE

AGT_api agt_status_t AGT_stdcall agt_begin_send_mssr(agt_mailbox_t mailbox, void** address, agt_u64_t timeout);
AGT_api void         AGT_stdcall agt_finish_send_mssr(agt_mailbox_t mailbox, void* address);

AGT_api agt_status_t AGT_stdcall agt_begin_dynamic_send_mssr(agt_dynamic_mailbox_t mailbox, void** address, agt_size_t size, agt_u64_t timeout_milliseconds);
AGT_api void         AGT_stdcall agt_finish_dynamic_send_mssr(agt_dynamic_mailbox_t mailbox, void* address);


AGT_api agt_status_t AGT_stdcall agt_begin_receive_mssr(agt_mailbox_t mailbox, void** address, agt_u64_t timeout_us);
AGT_api void         AGT_stdcall agt_finish_receive_mssr(agt_mailbox_t mailbox, void* address, agt_receive_action_t action);

AGT_api agt_status_t AGT_stdcall agt_begin_dynamic_receive_mssr(agt_dynamic_mailbox_t mailbox, void** address, agt_size_t* size, agt_u64_t timeout_us);
AGT_api void         AGT_stdcall agt_finish_dynamic_receive_mssr(agt_dynamic_mailbox_t mailbox, void* address, agt_receive_action_t action);

AGT_END_C_NAMESPACE

#endif  //ARGENT_MAILBOX_MSSR_H