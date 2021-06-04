//
// Created by maxwe on 2021-05-22.
//

#include "../mailbox_common.h"

#include <argent/mailbox/ssmr.h>


struct AGT_cache_aligned agt_mailbox_ssmr{

};


static agt_status_t agt_init_mailbox_ssmr(agt_mailbox_t mailbox) { }
static agt_status_t agt_attach_sender_ssmr(agt_mailbox_t mailbox, agt_sender_t* pSender, agt_deadline_t deadline) { }
static void         agt_detach_sender_ssmr(agt_mailbox_t mailbox, agt_sender_t sender) { }
static agt_status_t agt_attach_receiver_ssmr(agt_mailbox_t mailbox, agt_receiver_t* pReceiver, agt_deadline_t deadline) { }
static void         agt_detach_receiver_ssmr(agt_mailbox_t mailbox, agt_receiver_t receiver) { }
static void         agt_destroy_mailbox_ssmr(agt_mailbox_t mailbox) { }

static agt_status_t agt_init_dynamic_mailbox_ssmr(agt_dynamic_mailbox_t mailbox) { }
static agt_status_t agt_attach_dynamic_sender_ssmr(agt_dynamic_mailbox_t mailbox, agt_dynamic_sender_t* pSender, agt_deadline_t deadline) { }
static void         agt_detach_dynamic_sender_ssmr(agt_dynamic_mailbox_t mailbox, agt_dynamic_sender_t sender) { }
static agt_status_t agt_attach_dynamic_receiver_ssmr(agt_dynamic_mailbox_t mailbox, agt_dynamic_receiver_t* pReceiver, agt_deadline_t deadline) { }
static void         agt_detach_dynamic_receiver_ssmr(agt_dynamic_mailbox_t mailbox, agt_dynamic_receiver_t receiver) { }
static void         agt_destroy_dynamic_mailbox_ssmr(agt_dynamic_mailbox_t mailbox) { }




const agt_mailbox_vtable_t agt_mailbox_vtable_ssmr = {
    .attachSender   = agt_attach_sender_ssmr,
    .beginSend      = agt_begin_send_ssmr,
    .finishSend     = agt_finish_send_ssmr,
    .detachSender   = agt_detach_sender_ssmr,
    .attachReceiver = agt_attach_receiver_ssmr,
    .beginReceive   = agt_begin_receive_ssmr,
    .finishReceive  = agt_finish_receive_ssmr,
    .detachReceiver = agt_detach_receiver_ssmr,
    .init           = agt_init_mailbox_ssmr,
    .destroy        = agt_destroy_mailbox_ssmr
};
const agt_dynamic_mailbox_vtable_t agt_dynamic_mailbox_vtable_ssmr = {
    .attachSender   = agt_attach_dynamic_sender_ssmr,
    .beginSend      = agt_begin_dynamic_send_ssmr,
    .finishSend     = agt_finish_dynamic_send_ssmr,
    .detachSender   = agt_detach_dynamic_sender_ssmr,
    .attachReceiver = agt_attach_dynamic_receiver_ssmr,
    .beginReceive   = agt_begin_dynamic_receive_ssmr,
    .finishReceive  = agt_finish_dynamic_receive_ssmr,
    .detachReceiver = agt_detach_dynamic_receiver_ssmr,
    .init           = agt_init_dynamic_mailbox_ssmr,
    .destroy        = agt_destroy_dynamic_mailbox_ssmr
};