//
// Created by maxwe on 2021-05-23.
//

#include "../mailbox_common.h"

#include <argent/mailbox/sscmr.h>


struct AGT_cache_aligned agt_mailbox_sscmr{

};


static agt_status_t agt_init_mailbox_sscmr(agt_mailbox_t mailbox) { }
static agt_status_t agt_attach_sender_sscmr(agt_mailbox_t mailbox, agt_sender_t* pSender, agt_deadline_t deadline) { }
static void         agt_detach_sender_sscmr(agt_mailbox_t mailbox, agt_sender_t sender) { }
static agt_status_t agt_attach_receiver_sscmr(agt_mailbox_t mailbox, agt_receiver_t* pReceiver, agt_deadline_t deadline) { }
static void         agt_detach_receiver_sscmr(agt_mailbox_t mailbox, agt_receiver_t receiver) { }
static void         agt_destroy_mailbox_sscmr(agt_mailbox_t mailbox) { }

static agt_status_t agt_init_dynamic_mailbox_sscmr(agt_dynamic_mailbox_t mailbox) { }
static agt_status_t agt_attach_dynamic_sender_sscmr(agt_dynamic_mailbox_t mailbox, agt_dynamic_sender_t* pSender, agt_deadline_t deadline) { }
static void         agt_detach_dynamic_sender_sscmr(agt_dynamic_mailbox_t mailbox, agt_dynamic_sender_t sender) { }
static agt_status_t agt_attach_dynamic_receiver_sscmr(agt_dynamic_mailbox_t mailbox, agt_dynamic_receiver_t* pReceiver, agt_deadline_t deadline) { }
static void         agt_detach_dynamic_receiver_sscmr(agt_dynamic_mailbox_t mailbox, agt_dynamic_receiver_t receiver) { }
static void         agt_destroy_dynamic_mailbox_sscmr(agt_dynamic_mailbox_t mailbox) { }




const agt_mailbox_vtable_t agt_mailbox_vtable_sscmr = {
    .attachSender   = agt_attach_sender_sscmr,
    .beginSend      = agt_begin_send_sscmr,
    .finishSend     = agt_finish_send_sscmr,
    .detachSender   = agt_detach_sender_sscmr,
    .attachReceiver = agt_attach_receiver_sscmr,
    .beginReceive   = agt_begin_receive_sscmr,
    .finishReceive  = agt_finish_receive_sscmr,
    .detachReceiver = agt_detach_receiver_sscmr,
    .init           = agt_init_mailbox_sscmr,
    .destroy        = agt_destroy_mailbox_sscmr
};
const agt_dynamic_mailbox_vtable_t agt_dynamic_mailbox_vtable_sscmr = {
    .attachSender   = agt_attach_dynamic_sender_sscmr,
    .beginSend      = agt_begin_dynamic_send_sscmr,
    .finishSend     = agt_finish_dynamic_send_sscmr,
    .detachSender   = agt_detach_dynamic_sender_sscmr,
    .attachReceiver = agt_attach_dynamic_receiver_sscmr,
    .beginReceive   = agt_begin_dynamic_receive_sscmr,
    .finishReceive  = agt_finish_dynamic_receive_sscmr,
    .detachReceiver = agt_detach_dynamic_receiver_sscmr,
    .init           = agt_init_dynamic_mailbox_sscmr,
    .destroy        = agt_destroy_dynamic_mailbox_sscmr
};