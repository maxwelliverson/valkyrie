//
// Created by maxwe on 2021-05-23.
//

#include "../mailbox_common.h"

#include <argent/mailbox/cmsmr.h>


struct AGT_cache_aligned agt_mailbox_cmsmr{

};


static agt_status_t agt_init_mailbox_cmsmr(agt_mailbox_t mailbox) { }
static agt_status_t agt_attach_sender_cmsmr(agt_mailbox_t mailbox, agt_sender_t* pSender, agt_deadline_t deadline) { }
static void         agt_detach_sender_cmsmr(agt_mailbox_t mailbox, agt_sender_t sender) { }
static agt_status_t agt_attach_receiver_cmsmr(agt_mailbox_t mailbox, agt_receiver_t* pReceiver, agt_deadline_t deadline) { }
static void         agt_detach_receiver_cmsmr(agt_mailbox_t mailbox, agt_receiver_t receiver) { }
static void         agt_destroy_mailbox_cmsmr(agt_mailbox_t mailbox) { }

static agt_status_t agt_init_dynamic_mailbox_cmsmr(agt_dynamic_mailbox_t mailbox) { }
static agt_status_t agt_attach_dynamic_sender_cmsmr(agt_dynamic_mailbox_t mailbox, agt_dynamic_sender_t* pSender, agt_deadline_t deadline) { }
static void         agt_detach_dynamic_sender_cmsmr(agt_dynamic_mailbox_t mailbox, agt_dynamic_sender_t sender) { }
static agt_status_t agt_attach_dynamic_receiver_cmsmr(agt_dynamic_mailbox_t mailbox, agt_dynamic_receiver_t* pReceiver, agt_deadline_t deadline) { }
static void         agt_detach_dynamic_receiver_cmsmr(agt_dynamic_mailbox_t mailbox, agt_dynamic_receiver_t receiver) { }
static void         agt_destroy_dynamic_mailbox_cmsmr(agt_dynamic_mailbox_t mailbox) { }




const agt_mailbox_vtable_t agt_mailbox_vtable_cmsmr = {
    .attachSender   = agt_attach_sender_cmsmr,
    .beginSend      = agt_begin_send_cmsmr,
    .finishSend     = agt_finish_send_cmsmr,
    .detachSender   = agt_detach_sender_cmsmr,
    .attachReceiver = agt_attach_receiver_cmsmr,
    .beginReceive   = agt_begin_receive_cmsmr,
    .finishReceive  = agt_finish_receive_cmsmr,
    .detachReceiver = agt_detach_receiver_cmsmr,
    .init           = agt_init_mailbox_cmsmr,
    .destroy        = agt_destroy_mailbox_cmsmr
};
const agt_dynamic_mailbox_vtable_t agt_dynamic_mailbox_vtable_cmsmr = {
    .attachSender   = agt_attach_dynamic_sender_cmsmr,
    .beginSend      = agt_begin_dynamic_send_cmsmr,
    .finishSend     = agt_finish_dynamic_send_cmsmr,
    .detachSender   = agt_detach_dynamic_sender_cmsmr,
    .attachReceiver = agt_attach_dynamic_receiver_cmsmr,
    .beginReceive   = agt_begin_dynamic_receive_cmsmr,
    .finishReceive  = agt_finish_dynamic_receive_cmsmr,
    .detachReceiver = agt_detach_dynamic_receiver_cmsmr,
    .init           = agt_init_dynamic_mailbox_cmsmr,
    .destroy        = agt_destroy_dynamic_mailbox_cmsmr
};