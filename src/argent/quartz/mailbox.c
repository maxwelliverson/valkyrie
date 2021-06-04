//
// Created by maxwe on 2021-05-31.
//

#include "internal.h"





typedef qtz_message_action_t(AGT_stdcall* PFN_global_message_proc)(qtz_mailbox_t mailbox, void* messagePayload);

static const PFN_global_message_proc global_dispatch_table__[];



#define AGT_INVALID_GLOBAL_MESSAGE 0xFFFFFFFFu




inline static qtz_request_t qtz_get_next_free_message__(qtz_mailbox_t mailbox) {
  agt_u32_t thisSlot = atomic_load(&mailbox->nextFreeSlot, acquire);
  agt_u32_t nextSlot;
  qtz_request_t message;
  do {
    message = mailbox->messageSlots + thisSlot;
    nextSlot = message->nextSlot;
    // assert(atomic_load(&message->isFree, relaxed));
  } while ( !atomic_cas(&mailbox->nextFreeSlot, &thisSlot, nextSlot, acquire) );

  return mailbox->messageSlots + thisSlot;
}
inline static qtz_request_t qtz_acquire_next_free_message__(qtz_mailbox_t mailbox) {
  acquire_semaphore(&mailbox->slotSemaphore);
  return qtz_get_next_free_message__(mailbox);
}
inline static qtz_request_t qtz_try_acquire_next_free_message__(qtz_mailbox_t mailbox) {
  if ( !try_acquire_semaphore(&mailbox->slotSemaphore) )
    return NULL;
  return qtz_get_next_free_message__(mailbox);
}
inline static void          qtz_release_message_slot__(qtz_mailbox_t mailbox, qtz_request_t message) {
  const agt_u32_t thisSlot = message - mailbox->messageSlots;
  agt_u32_t newNextSlot = atomic_load(&mailbox->nextFreeSlot, acquire);
  do {
    message->nextSlot = newNextSlot;
  } while( !atomic_cas(&mailbox->nextFreeSlot, &newNextSlot, thisSlot, release) );
  release_semaphore(&mailbox->slotSemaphore);
}
inline static void          qtz_enqueue_message__(qtz_mailbox_t mailbox, qtz_request_t message) {
  const agt_u32_t slot = message - mailbox->messageSlots;
  agt_u32_t lastQueuedSlot = atomic_load(&mailbox->lastQueuedSlot, acquire);
  do {
    message->nextSlot = lastQueuedSlot;
  } while ( !atomic_cas(&mailbox->lastQueuedSlot, &lastQueuedSlot, slot, release) );
  atomic_increment(&mailbox->queuedSinceLastCheck, relaxed);
  atomic_wake_one(&mailbox->queuedSinceLastCheck);
}
inline static qtz_request_t qtz_wait_for_next_queued_message__(qtz_mailbox_t mailbox, qtz_request_t prev) {
  atomic_wait(&mailbox->queuedSinceLastCheck, 0);
  mailbox->minQueuedMessages = atomic_exchange(&mailbox->queuedSinceLastCheck, 0, release);
  return mailbox->messageSlots + prev->nextSlot;
}
inline static qtz_request_t qtz_acquire_first_queued_message__(qtz_mailbox_t mailbox) {
  atomic_wait(&mailbox->queuedSinceLastCheck, 0);
  mailbox->minQueuedMessages = atomic_exchange(&mailbox->queuedSinceLastCheck, 0, release);
  --mailbox->minQueuedMessages;
  return mailbox->messageSlots;
}
inline static qtz_request_t qtz_acquire_next_queued_message__(qtz_mailbox_t mailbox, qtz_request_t prev) {
  qtz_request_t message;
  if ( mailbox->minQueuedMessages == 0 ) {
    message = qtz_wait_for_next_queued_message__(mailbox, prev);
  }
  else {
    message = mailbox->messageSlots + prev->nextSlot;
  }
  --mailbox->minQueuedMessages;
  return message;
}
inline static void          qtz_discard_message__(qtz_mailbox_t mailbox, qtz_request_t message) {
  if ( atomic_flag_test_and_set(&message->isDiscarded) ) {
    qtz_release_message_slot__(mailbox, message);
  }
}
inline static void          qtz_notify_message_listener__(qtz_request_t message) {
  atomic_flag_set(&message->isReady);
  atomic_wake_all(&message->isReady);
}



AGT_stdcall qtz_exit_code_t qtz_mailbox_main_thread_proc(void* params) {
  (void)params;

  qtz_request_t previousMsg = NULL;

  const qtz_mailbox_t mailbox = globalMailbox;


  qtz_request_t message = qtz_acquire_first_queued_message__(mailbox);

  for ( ;; ) {
    switch ( (global_dispatch_table__[message->kind])(mailbox, message->payload) ) {
      case QTZ_DISCARD:
        atomic_flag_test_and_set(&message->isDiscarded);
        break;
      case QTZ_NOTIFY_LISTENER:
        qtz_notify_message_listener__(message);
        break;
      case QTZ_DEFERRED:
        break;
      AGT_no_default;
    }
    qtz_discard_message__(mailbox, previousMsg);
    previousMsg = message;
    if ( atomic_flag_test(&mailbox->shouldCloseMailbox) ) {
      return NO_ERROR;
    }
    message = qtz_acquire_next_queued_message__(mailbox, previousMsg);
  }
}



static void cleanup_global_mailbox() {

}




const static PFN_global_message_proc global_dispatch_table__[] = {
    qtz_alloc_pages_proc__,
    qtz_free_pages_proc__,
    qtz_alloc_static_mailbox_proc__,
    qtz_free_static_mailbox_proc__,
    qtz_alloc_dynamic_mailbox_proc__,
    qtz_free_dynamic_mailbox_proc__,
    qtz_alloc_static_mailbox_shared_proc__,
    qtz_free_static_mailbox_shared_proc__,
    qtz_alloc_dynamic_mailbox_shared_proc__,
    qtz_free_dynamic_mailbox_shared_proc__,
    qtz_open_ipc_link_proc__,
    qtz_close_ipc_link_proc__,
    qtz_send_ipc_message_proc__,
    qtz_open_thread_proc__,
    qtz_close_thread_proc__,
    qtz_attach_thread_proc__,
    qtz_detach_thread_proc__,
    qtz_register_agent_proc__,
    qtz_unregister_agent_proc__
};