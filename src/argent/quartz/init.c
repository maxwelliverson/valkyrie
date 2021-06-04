//
// Created by maxwe on 2021-05-31.
//

#include "internal.h"


static agt_status_t qtz_init_shmem__() {
  static bool successfullyInitialized = false;
  if ( successfullyInitialized )
    return AGT_SUCCESS;

  agt_status_t status = AGT_SUCCESS;

  // TODO: initialize memory subsystem


  void* result = VirtualAlloc2(GetCurrentProcess(),
                               NULL,
                               AGT_GLOBAL_VIRTUAL_MEMORY_MAX,
                               MEM_RESERVE | MEM_RESERVE_PLACEHOLDER,
                               PAGE_NOACCESS,
                               NULL,
                               0);
  if ( !result ) {
    status = AGT_ERROR_BAD_ALLOC;
    goto exit;
  }

  if ( !VirtualFree(result, AGT_VIRTUAL_PAGE_SIZE, MEM_RELEASE | MEM_PRESERVE_PLACEHOLDER) ) {
    status = AGT_ERROR_BAD_ALLOC;
  }



  sharedMemoryInitialAddress = result;
  sharedMemorySize           = AGT_GLOBAL_VIRTUAL_MEMORY_MAX;


  result = NULL;

















  exit:
  if ( status == AGT_SUCCESS ) {
    successfullyInitialized = true;
    atomic_fence();
  }
  else {
    if ( result ) {
      VirtualFree(result, 0, MEM_RELEASE);
    }
  }
  return status;
}
static agt_status_t qtz_init_mailbox__() {
  static bool successfullyInitialized = false;
  if ( successfullyInitialized )
    return AGT_SUCCESS;

  agt_status_t status = AGT_SUCCESS;

  // TODO: initialize global mailbox

  SECURITY_ATTRIBUTES securityAttributes = {
      .nLength = sizeof(SECURITY_ATTRIBUTES),
      .lpSecurityDescriptor = NULL,
      .bInheritHandle = TRUE
  };

  HANDLE fileMapping = CreateFileMapping2(NULL,
                                          &securityAttributes,
                                          FILE_MAP_WRITE,
                                          PAGE_READWRITE,
                                          0,
                                          AGT_VIRTUAL_PAGE_SIZE,
                                          NULL,
                                          NULL,
                                          0);
  void* mappedFile = MapViewOfFile3(fileMapping,
                                    GetCurrentProcess(),
                                    sharedMemoryInitialAddress,
                                    0,
                                    AGT_VIRTUAL_PAGE_SIZE,
                                    MEM_REPLACE_PLACEHOLDER,
                                    PAGE_READWRITE,
                                    NULL,
                                    0);
  qtz_mailbox_t mailbox = (qtz_mailbox_t)mappedFile;

  mailbox->slotSemaphore.counter        = AGT_GLOBAL_MESSAGE_SLOTS;
  mailbox->slotSemaphore.waitingThreads = 0;
  mailbox->queuedSinceLastCheck         = 0;
  mailbox->nextFreeSlot                 = 0;
  mailbox->lastQueuedSlot               = 0;
  mailbox->minQueuedMessages            = 0;
  mailbox->maxCurrentDeferred           = 0;
  mailbox->releasedSinceLastCheck       = 0;
  mailbox->exitCode                     = 0;


  for ( agt_u32_t i = 0; i < AGT_GLOBAL_MESSAGE_SLOTS;) {
    qtz_request_t message = mailbox->messageSlots + i;
    message->nextSlot = ++i;
  }








  exit:
  if ( status == AGT_SUCCESS ) {
    successfullyInitialized = true;
    atomic_fence();
  }
  return status;
}
static agt_status_t qtz_init_main_thread__() {
  static bool successfullyInitialized = false;
  if ( successfullyInitialized )
    return AGT_SUCCESS;

  agt_status_t status = AGT_SUCCESS;

  // TODO: initialize global thread

  SECURITY_ATTRIBUTES securityAttributes = {
      .nLength = sizeof(SECURITY_ATTRIBUTES),
      .lpSecurityDescriptor = NULL,
      .bInheritHandle = TRUE
  };

  DWORD threadId;

  // dwStackSize is rounded up to the allocation granularity of the system (generally 64KB),
  // but setting dwStackSize to 0 yields the default stack size, so setting it to 1 yields
  // the minimum stack size. This is nice for our global mailbox thread, as it essentially
  // runs in a flat loop, and thus is guaranteed to never have much of a stack at all.
  //_beginthreadex();
  HANDLE result = CreateThread(&securityAttributes, 1, qtz_mailbox_main_thread_proc, NULL, 0, &threadId);

  if ( !result ) {
    status = AGT_ERROR_INTERNAL;
    goto exit;
  }

  globalMailboxThreadHandle = result;
  globalMailboxThreadId     = threadId;


  exit:
  if ( status == AGT_SUCCESS ) {
    successfullyInitialized = true;
    atomic_fence();
  }
  return status;
}

agt_status_t qtz_init() {
  agt_status_t status = AGT_SUCCESS;
  if ( !atomic_flag_test(&isInitialized) ) {
    static struct agt_semaphore exclusiveInitMutex = { .counter = 1 };
    if ( !try_acquire_semaphore(&exclusiveInitMutex) ) {
      acquire_semaphore(&exclusiveInitMutex);
      if ( atomic_flag_test(&isInitialized) )
        return status;
    }

    status = qtz_init_shmem__();
    if ( status != AGT_SUCCESS )
      goto exit;

    status = qtz_init_mailbox__();
    if ( status != AGT_SUCCESS )
      goto exit;

    status = qtz_init_main_thread__();

    exit:
    if ( status == AGT_SUCCESS ) {
      atomic_flag_set(&isInitialized);
      release_many_semaphores(&exclusiveInitMutex, 4096);
    }
    else {
      release_semaphore(&exclusiveInitMutex);
    }
  }
  return status;
}