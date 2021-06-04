//
// Created by maxwe on 2021-05-31.
//

#ifndef QUARTZ_INTERNAL_H
#define QUARTZ_INTERNAL_H

#include "../internal.h"
#include "../semaphore.h"

#include <argent/quartz.h>

typedef agt_u64_t agt_bitmap_field_t;


#define AGT_BITS_PER_BITMAP_FIELD (sizeof(agt_bitmap_field_t) * 8)
#define AGT_VIRTUAL_REGION_SIZE (AGT_VIRTUAL_PAGE_SIZE * AGT_BITS_PER_BITMAP_FIELD)
#define AGT_VIRTUAL_SEGMENT_SIZE (AGT_VIRTUAL_REGION_SIZE * AGT_BITS_PER_BITMAP_FIELD)
#define AGT_GLOBAL_VIRTUAL_MEMORY_MAX (AGT_VIRTUAL_SEGMENT_SIZE * AGT_BITS_PER_BITMAP_FIELD) // 16GB




/*typedef struct agt_global_message{
  agt_u32_t nextIndex;

} agt_global_message, * agt_global_message_t;*/

#if AGT_system_windows
typedef DWORD qtz_exit_code_t;
#else
typedef int   qtz_exit_code_t;
#endif


typedef enum {
  GLOBAL_MESSAGE_KIND_ALLOC_PAGES,
  GLOBAL_MESSAGE_KIND_FREE_PAGES,
  GLOBAL_MESSAGE_KIND_ALLOCATE_STATIC_MAILBOX,
  GLOBAL_MESSAGE_KIND_DEALLOCATE_STATIC_MAILBOX,
  GLOBAL_MESSAGE_KIND_ALLOCATE_DYNAMIC_MAILBOX,
  GLOBAL_MESSAGE_KIND_DEALLOCATE_DYNAMIC_MAILBOX,
  GLOBAL_MESSAGE_KIND_ALLOCATE_STATIC_MAILBOX_SHARED,
  GLOBAL_MESSAGE_KIND_DEALLOCATE_STATIC_MAILBOX_SHARED,
  GLOBAL_MESSAGE_KIND_ALLOCATE_DYNAMIC_MAILBOX_SHARED,
  GLOBAL_MESSAGE_KIND_DEALLOCATE_DYNAMIC_MAILBOX_SHARED,
  GLOBAL_MESSAGE_KIND_OPEN_IPC_LINK,
  GLOBAL_MESSAGE_KIND_CLOSE_IPC_LINK,
  GLOBAL_MESSAGE_KIND_SEND_IPC_MESSAGE,
  GLOBAL_MESSAGE_KIND_OPEN_THREAD,
  GLOBAL_MESSAGE_KIND_CLOSE_THREAD,
  GLOBAL_MESSAGE_KIND_ATTACH_THREAD,
  GLOBAL_MESSAGE_KIND_DETACH_THREAD,
  GLOBAL_MESSAGE_KIND_REGISTER_AGENT,
  GLOBAL_MESSAGE_KIND_UNREGISTER_AGENT
} qtz_message_kind_t;
typedef enum {
  QTZ_DISCARD,
  QTZ_DEFERRED,
  QTZ_NOTIFY_LISTENER
} qtz_message_action_t;
typedef struct {
  const char*            name;
  qtz_shared_mailbox_t*  pHandleAddr;
} qtz_shared_mailbox_params_t;

struct AGT_cache_aligned qtz_request {
  agt_u32_t                 nextSlot;
  atomic_flag_t             isReady;
  atomic_flag_t             isDiscarded;
  bool                      isForeign;
  agt_u8_t                  kind;
  char                      payload[];
};
struct qtz_mailbox {

  // Cacheline 0 - accessed only by writers
  AGT_cache_aligned

  struct agt_semaphore slotSemaphore;


  // Cacheline 1 - accessed by writers
  AGT_cache_aligned

  atomic_u32_t nextFreeSlot;


  // Cacheline 2 - accessed by writers
  AGT_cache_aligned

  atomic_u32_t lastQueuedSlot;


  // Cacheline 3 - accessed by writers and reader
  AGT_cache_aligned

  atomic_u32_t queuedSinceLastCheck;


  // Cacheline 4 - accessed only by reader
  AGT_cache_aligned

  agt_u32_t       minQueuedMessages;
  agt_u32_t       maxCurrentDeferred;
  agt_u32_t       releasedSinceLastCheck;
  atomic_flag_t   shouldCloseMailbox;
  qtz_exit_code_t exitCode;


  //

  // Message Slots
  struct qtz_request messageSlots[];
};

static atomic_flag_t isInitialized = false;

static agt_size_t    sharedMemorySize;
static void*         sharedMemoryInitialAddress;

static HANDLE        globalMailboxThreadHandle;
static int           globalMailboxThreadId;

static qtz_mailbox_t globalMailbox;

#define AGT_GLOBAL_MESSAGE_SIZE  64
#define AGT_GLOBAL_MESSAGE_SLOTS ((AGT_VIRTUAL_PAGE_SIZE - sizeof(struct qtz_mailbox)) / AGT_GLOBAL_MESSAGE_SIZE)
#define AGT_GLOBAL_MAILBOX_SIZE  (AGT_GLOBAL_MESSAGE_SIZE * AGT_GLOBAL_MESSAGE_SLOTS)
#define AGT_GLOBAL_MAILBOX_ALIGN (AGT_GLOBAL_MAILBOX_SIZE * 2)


AGT_stdcall DWORD qtz_mailbox_main_thread_proc(void* params);


AGT_stdcall qtz_request_t qtz_alloc_static_mailbox(void** pResultAddr, agt_u32_t messageSlots, agt_u32_t messageSize, const qtz_shared_mailbox_params_t* pSharedParams);
AGT_stdcall void          qtz_free_static_mailbox(void* mailboxAddr);
AGT_stdcall qtz_request_t qtz_alloc_dynamic_mailbox(void** pResultAddr, agt_u32_t minMailboxSize, agt_u32_t maxMessageSize, const qtz_shared_mailbox_params_t* pSharedParams);
AGT_stdcall void          qtz_free_dynamic_mailbox(void* mailboxAddr);


AGT_stdcall qtz_message_action_t qtz_alloc_pages_proc__(qtz_mailbox_t mailbox, void* messagePayload);
AGT_stdcall qtz_message_action_t qtz_free_pages_proc__(qtz_mailbox_t mailbox, void* messagePayload);
AGT_stdcall qtz_message_action_t qtz_alloc_static_mailbox_proc__(qtz_mailbox_t mailbox, void* messagePayload);
AGT_stdcall qtz_message_action_t qtz_free_static_mailbox_proc__(qtz_mailbox_t mailbox, void* messagePayload);
AGT_stdcall qtz_message_action_t qtz_alloc_dynamic_mailbox_proc__(qtz_mailbox_t mailbox, void* messagePayload);
AGT_stdcall qtz_message_action_t qtz_free_dynamic_mailbox_proc__(qtz_mailbox_t mailbox, void* messagePayload);
AGT_stdcall qtz_message_action_t qtz_alloc_static_mailbox_shared_proc__(qtz_mailbox_t mailbox, void* messagePayload);
AGT_stdcall qtz_message_action_t qtz_free_static_mailbox_shared_proc__(qtz_mailbox_t mailbox, void* messagePayload);
AGT_stdcall qtz_message_action_t qtz_alloc_dynamic_mailbox_shared_proc__(qtz_mailbox_t mailbox, void* messagePayload);
AGT_stdcall qtz_message_action_t qtz_free_dynamic_mailbox_shared_proc__(qtz_mailbox_t mailbox, void* messagePayload);
AGT_stdcall qtz_message_action_t qtz_open_ipc_link_proc__(qtz_mailbox_t mailbox, void* messagePayload);
AGT_stdcall qtz_message_action_t qtz_close_ipc_link_proc__(qtz_mailbox_t mailbox, void* messagePayload);
AGT_stdcall qtz_message_action_t qtz_send_ipc_message_proc__(qtz_mailbox_t mailbox, void* messagePayload);
AGT_stdcall qtz_message_action_t qtz_open_thread_proc__(qtz_mailbox_t mailbox, void* messagePayload);
AGT_stdcall qtz_message_action_t qtz_close_thread_proc__(qtz_mailbox_t mailbox, void* messagePayload);
AGT_stdcall qtz_message_action_t qtz_attach_thread_proc__(qtz_mailbox_t mailbox, void* messagePayload);
AGT_stdcall qtz_message_action_t qtz_detach_thread_proc__(qtz_mailbox_t mailbox, void* messagePayload);
AGT_stdcall qtz_message_action_t qtz_register_agent_proc__(qtz_mailbox_t mailbox, void* messagePayload);
AGT_stdcall qtz_message_action_t qtz_unregister_agent_proc__(qtz_mailbox_t mailbox, void* messagePayload);





#endif  //QUARTZ_INTERNAL_H
