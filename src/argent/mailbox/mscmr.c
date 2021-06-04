//
// Created by maxwe on 2021-05-23.
//

#include "../mailbox_common.h"

#include <argent/mailbox/mscmr.h>


struct AGT_cache_aligned agt_mailbox_mscmr{

};


static agt_status_t agt_init_mailbox_mscmr(agt_mailbox_t mailbox) { }
static agt_status_t agt_attach_sender_mscmr(agt_mailbox_t mailbox, agt_sender_t* pSender, agt_deadline_t deadline) { }
static void         agt_detach_sender_mscmr(agt_mailbox_t mailbox, agt_sender_t sender) { }
static agt_status_t agt_attach_receiver_mscmr(agt_mailbox_t mailbox, agt_receiver_t* pReceiver, agt_deadline_t deadline) { }
static void         agt_detach_receiver_mscmr(agt_mailbox_t mailbox, agt_receiver_t receiver) { }
static void         agt_destroy_mailbox_mscmr(agt_mailbox_t mailbox) { }

static agt_status_t agt_init_dynamic_mailbox_mscmr(agt_dynamic_mailbox_t mailbox) { }
static agt_status_t agt_attach_dynamic_sender_mscmr(agt_dynamic_mailbox_t mailbox, agt_dynamic_sender_t* pSender, agt_deadline_t deadline) { }
static void         agt_detach_dynamic_sender_mscmr(agt_dynamic_mailbox_t mailbox, agt_dynamic_sender_t sender) { }
static agt_status_t agt_attach_dynamic_receiver_mscmr(agt_dynamic_mailbox_t mailbox, agt_dynamic_receiver_t* pReceiver, agt_deadline_t deadline) { }
static void         agt_detach_dynamic_receiver_mscmr(agt_dynamic_mailbox_t mailbox, agt_dynamic_receiver_t receiver) { }
static void         agt_destroy_dynamic_mailbox_mscmr(agt_dynamic_mailbox_t mailbox) { }




const agt_mailbox_vtable_t agt_mailbox_vtable_mscmr = {
    .attachSender   = agt_attach_sender_mscmr,
    .beginSend      = agt_begin_send_mscmr,
    .finishSend     = agt_finish_send_mscmr,
    .detachSender   = agt_detach_sender_mscmr,
    .attachReceiver = agt_attach_receiver_mscmr,
    .beginReceive   = agt_begin_receive_mscmr,
    .finishReceive  = agt_finish_receive_mscmr,
    .detachReceiver = agt_detach_receiver_mscmr,
    .init           = agt_init_mailbox_mscmr,
    .destroy        = agt_destroy_mailbox_mscmr
};
const agt_dynamic_mailbox_vtable_t agt_dynamic_mailbox_vtable_mscmr = {
    .attachSender   = agt_attach_dynamic_sender_mscmr,
    .beginSend      = agt_begin_dynamic_send_mscmr,
    .finishSend     = agt_finish_dynamic_send_mscmr,
    .detachSender   = agt_detach_dynamic_sender_mscmr,
    .attachReceiver = agt_attach_dynamic_receiver_mscmr,
    .beginReceive   = agt_begin_dynamic_receive_mscmr,
    .finishReceive  = agt_finish_dynamic_receive_mscmr,
    .detachReceiver = agt_detach_dynamic_receiver_mscmr,
    .init           = agt_init_dynamic_mailbox_mscmr,
    .destroy        = agt_destroy_dynamic_mailbox_mscmr
};