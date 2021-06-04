//
// Created by maxwe on 2021-05-21.
//

#ifndef ARGENT_RECEIVER_H
#define ARGENT_RECEIVER_H

#include "mailbox.h"

AGT_BEGIN_C_NAMESPACE


typedef struct agt_receiver*         agt_receiver_t;
typedef struct agt_dynamic_receiver* agt_dynamic_receiver_t;

/*
 * Add callback hooks to senders and receivers??
 * typedef void(* agt_receiver_callback_t)(void* pUserData, );

typedef struct {

} agt_receiver_hook_t;*/





AGT_api agt_status_t AGT_stdcall agt_open_receiver(agt_receiver_t* receiver, agt_mailbox_t mailbox, agt_u64_t timeout_us);
AGT_api void         AGT_stdcall agt_close_receiver(agt_receiver_t receiver);
AGT_api agt_status_t AGT_stdcall agt_open_dynamic_receiver(agt_dynamic_receiver_t* receiver, agt_dynamic_mailbox_t mailbox, agt_u64_t timeout_us);
AGT_api void         AGT_stdcall agt_close_dynamic_receiver(agt_dynamic_receiver_t receiver);



AGT_api agt_status_t AGT_stdcall agt_begin_receive(agt_receiver_t receiver, void** address, agt_u64_t timeout_us);
AGT_api void         AGT_stdcall agt_finish_receive(agt_receiver_t receiver, void* address, agt_receive_action_t action);

AGT_api agt_status_t AGT_stdcall agt_begin_dynamic_receive(agt_dynamic_receiver_t receiver, void** address, agt_size_t* size, agt_u64_t timeout_us);
AGT_api void         AGT_stdcall agt_finish_dynamic_receive(agt_dynamic_receiver_t receiver, void* address, agt_receive_action_t action);

AGT_END_C_NAMESPACE

#endif  //ARGENT_RECEIVER_H
