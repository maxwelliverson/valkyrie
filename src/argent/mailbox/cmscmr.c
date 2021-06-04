//
// Created by maxwe on 2021-05-23.
//

#include "../mailbox_common.h"

#include <argent/mailbox/cmscmr.h>


struct AGT_cache_aligned agt_mailbox_cmscmr{

};


static agt_status_t agt_init_mailbox_cmscmr(agt_mailbox_t mailbox) { }
static agt_status_t agt_attach_sender_cmscmr(agt_mailbox_t mailbox, agt_sender_t* pSender, agt_deadline_t deadline) { }
static void         agt_detach_sender_cmscmr(agt_mailbox_t mailbox, agt_sender_t sender) { }
static agt_status_t agt_attach_receiver_cmscmr(agt_mailbox_t mailbox, agt_receiver_t* pReceiver, agt_deadline_t deadline) { }
static void         agt_detach_receiver_cmscmr(agt_mailbox_t mailbox, agt_receiver_t receiver) { }
static void         agt_destroy_mailbox_cmscmr(agt_mailbox_t mailbox) { }

static agt_status_t agt_init_dynamic_mailbox_cmscmr(agt_dynamic_mailbox_t mailbox) { }
static agt_status_t agt_attach_dynamic_sender_cmscmr(agt_dynamic_mailbox_t mailbox, agt_dynamic_sender_t* pSender, agt_deadline_t deadline) { }
static void         agt_detach_dynamic_sender_cmscmr(agt_dynamic_mailbox_t mailbox, agt_dynamic_sender_t sender) { }
static agt_status_t agt_attach_dynamic_receiver_cmscmr(agt_dynamic_mailbox_t mailbox, agt_dynamic_receiver_t* pReceiver, agt_deadline_t deadline) { }
static void         agt_detach_dynamic_receiver_cmscmr(agt_dynamic_mailbox_t mailbox, agt_dynamic_receiver_t receiver) { }
static void         agt_destroy_dynamic_mailbox_cmscmr(agt_dynamic_mailbox_t mailbox) { }




const agt_mailbox_vtable_t agt_mailbox_vtable_cmscmr = {
    .attachSender   = agt_attach_sender_cmscmr,
    .beginSend      = agt_begin_send_cmscmr,
    .finishSend     = agt_finish_send_cmscmr,
    .detachSender   = agt_detach_sender_cmscmr,
    .attachReceiver = agt_attach_receiver_cmscmr,
    .beginReceive   = agt_begin_receive_cmscmr,
    .finishReceive  = agt_finish_receive_cmscmr,
    .detachReceiver = agt_detach_receiver_cmscmr,
    .init           = agt_init_mailbox_cmscmr,
    .destroy        = agt_destroy_mailbox_cmscmr
};
const agt_dynamic_mailbox_vtable_t agt_dynamic_mailbox_vtable_cmscmr = {
    .attachSender   = agt_attach_dynamic_sender_cmscmr,
    .beginSend      = agt_begin_dynamic_send_cmscmr,
    .finishSend     = agt_finish_dynamic_send_cmscmr,
    .detachSender   = agt_detach_dynamic_sender_cmscmr,
    .attachReceiver = agt_attach_dynamic_receiver_cmscmr,
    .beginReceive   = agt_begin_dynamic_receive_cmscmr,
    .finishReceive  = agt_finish_dynamic_receive_cmscmr,
    .detachReceiver = agt_detach_dynamic_receiver_cmscmr,
    .init           = agt_init_dynamic_mailbox_cmscmr,
    .destroy        = agt_destroy_dynamic_mailbox_cmscmr
};