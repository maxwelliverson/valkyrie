//
// Created by maxwe on 2021-05-22.
//



#include "../mailbox_common.h"
#include "../semaphore.h"

#include <argent/mailbox/sssr.h>


struct AGT_cache_aligned agt_mailbox_sssr{
  const agt_mailbox_vptr_t     vtable;
  const agt_mailbox_flags_t    flags;
  const agt_u16_t              maxSenders;
  const agt_u16_t              maxReceivers;
  const agt_size_t             nameLength;
  const PFN_mailbox_dtor       pfnDtor;
  void* const                  pDtorUserData;
  void* const                  address;
  const agt_size_t             messageSize;
  const agt_size_t             messageSlots;

  struct agt_binary_semaphore  senderSemaphore;
  struct agt_binary_semaphore  receiverSemaphore;

  struct mailbox_sender_info   sender_info;
  struct mailbox_receiver_info receiver_info;
  const agt_char_t             name[];
};

_Static_assert(sizeof(struct agt_mailbox_sssr) == AGT_MAILBOX_SIZE, "");
_Static_assert(_Alignof(struct agt_mailbox_sssr) == AGT_MAILBOX_ALIGN, "");

static agt_status_t agt_init_mailbox_sssr(agt_mailbox_t mailbox) { }
static agt_status_t agt_attach_sender_sssr(agt_mailbox_t mailbox, agt_sender_t* pSender, agt_deadline_t deadline) { }
static void         agt_detach_sender_sssr(agt_mailbox_t mailbox, agt_sender_t sender) { }
static agt_status_t agt_attach_receiver_sssr(agt_mailbox_t mailbox, agt_receiver_t* pReceiver, agt_deadline_t deadline) { }
static void         agt_detach_receiver_sssr(agt_mailbox_t mailbox, agt_receiver_t receiver) { }
static void         agt_destroy_mailbox_sssr(agt_mailbox_t mailbox) { }

static agt_status_t agt_init_dynamic_mailbox_sssr(agt_dynamic_mailbox_t mailbox) { }
static agt_status_t agt_attach_dynamic_sender_sssr(agt_dynamic_mailbox_t mailbox, agt_dynamic_sender_t* pSender, agt_deadline_t deadline) { }
static void         agt_detach_dynamic_sender_sssr(agt_dynamic_mailbox_t mailbox, agt_dynamic_sender_t sender) { }
static agt_status_t agt_attach_dynamic_receiver_sssr(agt_dynamic_mailbox_t mailbox, agt_dynamic_receiver_t* pReceiver, agt_deadline_t deadline) { }
static void         agt_detach_dynamic_receiver_sssr(agt_dynamic_mailbox_t mailbox, agt_dynamic_receiver_t receiver) { }
static void         agt_destroy_dynamic_mailbox_sssr(agt_dynamic_mailbox_t mailbox) { }




const agt_mailbox_vtable_t agt_mailbox_vtable_sssr = {
    .attachSender   = agt_attach_sender_sssr,
    .beginSend      = agt_begin_send_sssr,
    .finishSend     = agt_finish_send_sssr,
    .detachSender   = agt_detach_sender_sssr,
    .attachReceiver = agt_attach_receiver_sssr,
    .beginReceive   = agt_begin_receive_sssr,
    .finishReceive  = agt_finish_receive_sssr,
    .detachReceiver = agt_detach_receiver_sssr,
    .init           = agt_init_mailbox_sssr,
    .destroy        = agt_destroy_mailbox_sssr
};
const agt_dynamic_mailbox_vtable_t agt_dynamic_mailbox_vtable_sssr = {
    .attachSender   = agt_attach_dynamic_sender_sssr,
    .beginSend      = agt_begin_dynamic_send_sssr,
    .finishSend     = agt_finish_dynamic_send_sssr,
    .detachSender   = agt_detach_dynamic_sender_sssr,
    .attachReceiver = agt_attach_dynamic_receiver_sssr,
    .beginReceive   = agt_begin_dynamic_receive_sssr,
    .finishReceive  = agt_finish_dynamic_receive_sssr,
    .detachReceiver = agt_detach_dynamic_receiver_sssr,
    .init           = agt_init_dynamic_mailbox_sssr,
    .destroy        = agt_destroy_dynamic_mailbox_sssr
};