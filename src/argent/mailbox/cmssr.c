//
// Created by maxwe on 2021-05-23.
//



#include "../mailbox_common.h"

#include <argent/mailbox/cmssr.h>


struct AGT_cache_aligned agt_mailbox_cmssr{

};


static agt_status_t agt_init_mailbox_cmssr(agt_mailbox_t mailbox) { }
static agt_status_t agt_attach_sender_cmssr(agt_mailbox_t mailbox, agt_sender_t* pSender, agt_deadline_t deadline) { }
static void         agt_detach_sender_cmssr(agt_mailbox_t mailbox, agt_sender_t sender) { }
static agt_status_t agt_attach_receiver_cmssr(agt_mailbox_t mailbox, agt_receiver_t* pReceiver, agt_deadline_t deadline) { }
static void         agt_detach_receiver_cmssr(agt_mailbox_t mailbox, agt_receiver_t receiver) { }
static void         agt_destroy_mailbox_cmssr(agt_mailbox_t mailbox) { }

static agt_status_t agt_init_dynamic_mailbox_cmssr(agt_dynamic_mailbox_t mailbox) { }
static agt_status_t agt_attach_dynamic_sender_cmssr(agt_dynamic_mailbox_t mailbox, agt_dynamic_sender_t* pSender, agt_deadline_t deadline) { }
static void         agt_detach_dynamic_sender_cmssr(agt_dynamic_mailbox_t mailbox, agt_dynamic_sender_t sender) { }
static agt_status_t agt_attach_dynamic_receiver_cmssr(agt_dynamic_mailbox_t mailbox, agt_dynamic_receiver_t* pReceiver, agt_deadline_t deadline) { }
static void         agt_detach_dynamic_receiver_cmssr(agt_dynamic_mailbox_t mailbox, agt_dynamic_receiver_t receiver) { }
static void         agt_destroy_dynamic_mailbox_cmssr(agt_dynamic_mailbox_t mailbox) { }




const agt_mailbox_vtable_t agt_mailbox_vtable_cmssr = {
    .attachSender   = agt_attach_sender_cmssr,
    .beginSend      = agt_begin_send_cmssr,
    .finishSend     = agt_finish_send_cmssr,
    .detachSender   = agt_detach_sender_cmssr,
    .attachReceiver = agt_attach_receiver_cmssr,
    .beginReceive   = agt_begin_receive_cmssr,
    .finishReceive  = agt_finish_receive_cmssr,
    .detachReceiver = agt_detach_receiver_cmssr,
    .init           = agt_init_mailbox_cmssr,
    .destroy        = agt_destroy_mailbox_cmssr
};
const agt_dynamic_mailbox_vtable_t agt_dynamic_mailbox_vtable_cmssr = {
    .attachSender   = agt_attach_dynamic_sender_cmssr,
    .beginSend      = agt_begin_dynamic_send_cmssr,
    .finishSend     = agt_finish_dynamic_send_cmssr,
    .detachSender   = agt_detach_dynamic_sender_cmssr,
    .attachReceiver = agt_attach_dynamic_receiver_cmssr,
    .beginReceive   = agt_begin_dynamic_receive_cmssr,
    .finishReceive  = agt_finish_dynamic_receive_cmssr,
    .detachReceiver = agt_detach_dynamic_receiver_cmssr,
    .init           = agt_init_dynamic_mailbox_cmssr,
    .destroy        = agt_destroy_dynamic_mailbox_cmssr
};