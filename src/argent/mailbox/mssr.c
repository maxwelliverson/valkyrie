//
// Created by maxwe on 2021-05-22.
//

#include "../mailbox_common.h"

#include <argent/mailbox/mssr.h>


struct AGT_cache_aligned agt_mailbox_mssr{

};


static agt_status_t agt_init_mailbox_mssr(agt_mailbox_t mailbox) { }
static agt_status_t agt_attach_sender_mssr(agt_mailbox_t mailbox, agt_sender_t* pSender, agt_deadline_t deadline) { }
static void         agt_detach_sender_mssr(agt_mailbox_t mailbox, agt_sender_t sender) { }
static agt_status_t agt_attach_receiver_mssr(agt_mailbox_t mailbox, agt_receiver_t* pReceiver, agt_deadline_t deadline) { }
static void         agt_detach_receiver_mssr(agt_mailbox_t mailbox, agt_receiver_t receiver) { }
static void         agt_destroy_mailbox_mssr(agt_mailbox_t mailbox) { }

static agt_status_t agt_init_dynamic_mailbox_mssr(agt_dynamic_mailbox_t mailbox) { }
static agt_status_t agt_attach_dynamic_sender_mssr(agt_dynamic_mailbox_t mailbox, agt_dynamic_sender_t* pSender, agt_deadline_t deadline) { }
static void         agt_detach_dynamic_sender_mssr(agt_dynamic_mailbox_t mailbox, agt_dynamic_sender_t sender) { }
static agt_status_t agt_attach_dynamic_receiver_mssr(agt_dynamic_mailbox_t mailbox, agt_dynamic_receiver_t* pReceiver, agt_deadline_t deadline) { }
static void         agt_detach_dynamic_receiver_mssr(agt_dynamic_mailbox_t mailbox, agt_dynamic_receiver_t receiver) { }
static void         agt_destroy_dynamic_mailbox_mssr(agt_dynamic_mailbox_t mailbox) { }




const agt_mailbox_vtable_t agt_mailbox_vtable_mssr = {
    .attachSender   = agt_attach_sender_mssr,
    .beginSend      = agt_begin_send_mssr,
    .finishSend     = agt_finish_send_mssr,
    .detachSender   = agt_detach_sender_mssr,
    .attachReceiver = agt_attach_receiver_mssr,
    .beginReceive   = agt_begin_receive_mssr,
    .finishReceive  = agt_finish_receive_mssr,
    .detachReceiver = agt_detach_receiver_mssr,
    .init           = agt_init_mailbox_mssr,
    .destroy        = agt_destroy_mailbox_mssr
};
const agt_dynamic_mailbox_vtable_t agt_dynamic_mailbox_vtable_mssr = {
    .attachSender   = agt_attach_dynamic_sender_mssr,
    .beginSend      = agt_begin_dynamic_send_mssr,
    .finishSend     = agt_finish_dynamic_send_mssr,
    .detachSender   = agt_detach_dynamic_sender_mssr,
    .attachReceiver = agt_attach_dynamic_receiver_mssr,
    .beginReceive   = agt_begin_dynamic_receive_mssr,
    .finishReceive  = agt_finish_dynamic_receive_mssr,
    .detachReceiver = agt_detach_dynamic_receiver_mssr,
    .init           = agt_init_dynamic_mailbox_mssr,
    .destroy        = agt_destroy_dynamic_mailbox_mssr
};