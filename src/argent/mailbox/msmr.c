//
// Created by maxwe on 2021-05-22.
//

#include "../mailbox_common.h"

#include <argent/mailbox/msmr.h>


struct AGT_cache_aligned agt_mailbox_msmr{

};


static agt_status_t agt_init_mailbox_msmr(agt_mailbox_t mailbox) { }
static agt_status_t agt_attach_sender_msmr(agt_mailbox_t mailbox, agt_sender_t* pSender, agt_deadline_t deadline) { }
static void         agt_detach_sender_msmr(agt_mailbox_t mailbox, agt_sender_t sender) { }
static agt_status_t agt_attach_receiver_msmr(agt_mailbox_t mailbox, agt_receiver_t* pReceiver, agt_deadline_t deadline) { }
static void         agt_detach_receiver_msmr(agt_mailbox_t mailbox, agt_receiver_t receiver) { }
static void         agt_destroy_mailbox_msmr(agt_mailbox_t mailbox) { }

static agt_status_t agt_init_dynamic_mailbox_msmr(agt_dynamic_mailbox_t mailbox) { }
static agt_status_t agt_attach_dynamic_sender_msmr(agt_dynamic_mailbox_t mailbox, agt_dynamic_sender_t* pSender, agt_deadline_t deadline) { }
static void         agt_detach_dynamic_sender_msmr(agt_dynamic_mailbox_t mailbox, agt_dynamic_sender_t sender) { }
static agt_status_t agt_attach_dynamic_receiver_msmr(agt_dynamic_mailbox_t mailbox, agt_dynamic_receiver_t* pReceiver, agt_deadline_t deadline) { }
static void         agt_detach_dynamic_receiver_msmr(agt_dynamic_mailbox_t mailbox, agt_dynamic_receiver_t receiver) { }
static void         agt_destroy_dynamic_mailbox_msmr(agt_dynamic_mailbox_t mailbox) { }




const agt_mailbox_vtable_t agt_mailbox_vtable_msmr = {
    .attachSender   = agt_attach_sender_msmr,
    .beginSend      = agt_begin_send_msmr,
    .finishSend     = agt_finish_send_msmr,
    .detachSender   = agt_detach_sender_msmr,
    .attachReceiver = agt_attach_receiver_msmr,
    .beginReceive   = agt_begin_receive_msmr,
    .finishReceive  = agt_finish_receive_msmr,
    .detachReceiver = agt_detach_receiver_msmr,
    .init           = agt_init_mailbox_msmr,
    .destroy        = agt_destroy_mailbox_msmr
};
const agt_dynamic_mailbox_vtable_t agt_dynamic_mailbox_vtable_msmr = {
    .attachSender   = agt_attach_dynamic_sender_msmr,
    .beginSend      = agt_begin_dynamic_send_msmr,
    .finishSend     = agt_finish_dynamic_send_msmr,
    .detachSender   = agt_detach_dynamic_sender_msmr,
    .attachReceiver = agt_attach_dynamic_receiver_msmr,
    .beginReceive   = agt_begin_dynamic_receive_msmr,
    .finishReceive  = agt_finish_dynamic_receive_msmr,
    .detachReceiver = agt_detach_dynamic_receiver_msmr,
    .init           = agt_init_dynamic_mailbox_msmr,
    .destroy        = agt_destroy_dynamic_mailbox_msmr
};