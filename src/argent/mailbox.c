//
// Created by maxwe on 2021-05-21.
//



#include "mailbox_common.h"


typedef struct AGT_cache_aligned {
  agt_mailbox_vptr_t           vtable;
  agt_mailbox_flags_t          flags;
  agt_u16_t                    maxSenders;
  agt_u16_t                    maxReceivers;
  agt_size_t                   nameLength;
  PFN_mailbox_dtor             pfnDtor;
  void*                        pDtorUserData;
  void*                        address;
  agt_size_t                   messageSize;
  agt_size_t                   messageSlots;
  struct mailbox_private_info  info;
  struct mailbox_sender_info   senderInfo;
  struct mailbox_receiver_info receiverInfo;
  agt_char_t                   name[];
}* agt_mutable_mailbox_t;
typedef struct AGT_cache_aligned {
  agt_dynamic_mailbox_vptr_t   vtable;
  agt_mailbox_flags_t          flags;
  agt_u16_t                    maxSenders;
  agt_u16_t                    maxReceivers;
  agt_size_t                   nameLength;
  PFN_mailbox_dtor             pfnDtor;
  void*                        pDtorUserData;
  void*                        address;
  agt_size_t                   mailboxSize;
  agt_size_t                   maxMessageSize;
  struct mailbox_private_info  info;
  struct mailbox_sender_info   senderInfo;
  struct mailbox_receiver_info receiverInfo;
  agt_char_t                   name[];
}* agt_mutable_dynamic_mailbox_t;




#define AGT_MAX_NAME_LENGTH 256
#define AGT_MAX_SENDERS_LIMIT   ((1 << 16) - 1)
#define AGT_MAX_RECEIVERS_LIMIT ((1 << 16) - 1)

#define ALIGNED_BUF_OFFSET (AGT_MAILBOX_ALIGN - 1)
#define ALIGNED_BUF_MASK (~ALIGNED_BUF_OFFSET)


inline static bool         agt_align_buffer__(agt_size_t req_size, void** buffer, agt_size_t* buffer_size) {
  const agt_u64_t buffer_address = (agt_u64_t)*buffer;
  const agt_u64_t buffer_end_address = buffer_address + *buffer_size;
  const agt_u64_t aligned_address = (buffer_address + ALIGNED_BUF_OFFSET) & ALIGNED_BUF_MASK;
  const agt_u64_t mailbox_end_address = aligned_address + req_size;
  const agt_u64_t offset = aligned_address - buffer_address;

  if ( mailbox_end_address > buffer_end_address )
    return false;

  *buffer = (void*)aligned_address;
  *buffer_size -= offset;
  return true;
}
inline static agt_size_t   agt_mailbox_obj_size__(agt_any_mailbox_t mailbox) {
  return sizeof(struct agt_any_mailbox_common) + mailbox_common_base(mailbox)->name_length;
}
inline static agt_status_t agt_alloc_dynamic__(void** result, agt_size_t mailboxSize, const agt_char_t* narrowName, agt_size_t nameLength) {

  agt_status_t status;
  wchar_t wideNameBuffer[AGT_MAX_NAME_LENGTH];
  const wchar_t* name = NULL;

  HANDLE fileMapping = NULL;
  void* placeholder1 = NULL;
  void* placeholder2 = NULL;
  void* view1        = NULL;
  void* view2        = NULL;

  if ( narrowName ) {
    int convResult = MultiByteToWideChar(CP_UTF8, 0, narrowName, nameLength, wideNameBuffer, AGT_MAX_NAME_LENGTH);
    if ( convResult == 0 ) {
      DWORD nextResult = GetLastError();
      if ( nextResult == ERROR_INSUFFICIENT_BUFFER )
        status = AGT_ERROR_NAME_TOO_LONG;
      else if ( nextResult == ERROR_NO_UNICODE_TRANSLATION )
        status = AGT_ERROR_BAD_ENCODING_IN_NAME;
      else
        status = AGT_ERROR_UNKNOWN;
      goto cleanup;
    }
    name = wideNameBuffer;
  }

  SECURITY_ATTRIBUTES securityAttributes;
  securityAttributes.bInheritHandle = TRUE;
  securityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
  securityAttributes.lpSecurityDescriptor = NULL;

  placeholder1 = VirtualAlloc2(
      GetCurrentProcess(),
      NULL,
      mailboxSize * 2,
      MEM_RESERVE | MEM_RESERVE_PLACEHOLDER,
      PAGE_NOACCESS,
      NULL, 0
  );

  if ( !placeholder1 ) {
    status = AGT_ERROR_BAD_ALLOC;
    goto exit;
  }

  if ( !VirtualFree(placeholder1, mailboxSize, MEM_RELEASE | MEM_PRESERVE_PLACEHOLDER) ) {
    status = AGT_ERROR_UNKNOWN;
    goto cleanup;
  }

  placeholder2 = ((agt_char_t*)placeholder1) + mailboxSize;

  fileMapping = CreateFileMapping2(
      INVALID_HANDLE_VALUE,
      &securityAttributes,
      FILE_MAP_ALL_ACCESS,
      PAGE_READWRITE,
      0,
      mailboxSize,
      name,
      NULL, 0
  );



  view1 = MapViewOfFile3(
      fileMapping,
      GetCurrentProcess(),
      placeholder1,
      0,
      mailboxSize,
      MEM_REPLACE_PLACEHOLDER,
      PAGE_READWRITE,
      NULL, 0
  );

  if ( !view1 ) {
    status = AGT_ERROR_UNKNOWN;
    goto cleanup;
  }

  placeholder1 = NULL;

  view2 = MapViewOfFile3(
      fileMapping,
      GetCurrentProcess(),
      placeholder2,
      0,
      mailboxSize,
      MEM_REPLACE_PLACEHOLDER,
      PAGE_READWRITE,
      NULL, 0
  );

  if ( !view2 ) {
    status = AGT_ERROR_UNKNOWN;
    goto cleanup;
  }

  placeholder2 = NULL;
  view1        = NULL;
  view2        = NULL;
  *result      = view1;
  status       = AGT_SUCCESS;

cleanup:
  if ( fileMapping ) {
    CloseHandle(fileMapping);
  }
  if ( placeholder1 ) {
    VirtualFree(placeholder1, 0, MEM_RELEASE);
  }
  if ( placeholder2 ) {
    VirtualFree(placeholder2, 0, MEM_RELEASE);
  }
  if ( view1 ) {
    UnmapViewOfFileEx(view1, 0);
  }
  if ( view2 ) {
    UnmapViewOfFileEx(view2, 0);
  }

exit:
  return status;
}
inline static void         agt_free_dynamic__(void* address, agt_size_t mailboxSize) {
  void* mapping2 = ((char*)address) + mailboxSize;
  UnmapViewOfFile(address);
  UnmapViewOfFile(mapping2);
}



AGT_api agt_status_t        AGT_stdcall agt_mailbox_create(agt_mailbox_t* mailbox, agt_size_t messageSlots, agt_size_t messageSize, const agt_mailbox_create_info_t* createInfo) {
  agt_mailbox_flags_t   flags;
  void*                 objectBuffer;
  void*                 address;
  agt_size_t            objectBufferSize;
  agt_size_t            nameLength;
  agt_mailbox_vptr_t    vptr;
  agt_mutable_mailbox_t mutableMailbox;

  if ( createInfo->max_senders > AGT_MAX_SENDERS_LIMIT || createInfo->max_receivers > AGT_MAX_RECEIVERS_LIMIT )
    return AGT_ERROR_INVALID_ARGUMENT;

  if ( messageSlots < 2 || messageSize % 8 != 0)
    return AGT_ERROR_BAD_SIZE;

  flags = 0;

  if ( createInfo->max_senders == 1 )
    flags |= AGT_MAILBOX_SINGLE_SENDER;
  if ( createInfo->max_receivers == 1 )
    flags |= AGT_MAILBOX_SINGLE_RECEIVER;
  if ( createInfo->flags & AGT_MAILBOX_CREATE_COHERENT_SEND ) {
    flags |= AGT_MAILBOX_COHERENT_SEND;
    if ( createInfo->max_senders == 1 )
      return AGT_ERROR_INVALID_ARGUMENT;
  }
  if ( createInfo->flags & AGT_MAILBOX_CREATE_COHERENT_RECEIVE ) {
    flags |= AGT_MAILBOX_COHERENT_RECEIVE;
    if ( createInfo->max_receivers == 1 )
      return AGT_ERROR_INVALID_ARGUMENT;
  }
  if ( createInfo->flags & AGT_MAILBOX_CREATE_IPC_CAPABLE )
    flags |= AGT_MAILBOX_INTERPROCESS;


  if ( createInfo->name != NULL )
    nameLength = createInfo->name_length ?: strlen(createInfo->name);
  else
    nameLength = 0;

  const agt_u64_t req_size = sizeof(struct agt_any_mailbox_common) + nameLength;

  if ( createInfo->external_buffer != NULL ) {
    objectBuffer       = createInfo->external_buffer;
    objectBufferSize   = createInfo->external_buffer_size;
    if ( !agt_align_buffer__(req_size, &objectBuffer, &objectBufferSize))
      return AGT_ERROR_INSUFFICIENT_BUFFER_SIZE;
    flags |= AGT_MAILBOX_RESIDES_IN_EXTERNAL_BUFFER;
  }
  else {
    objectBuffer       = _aligned_malloc(req_size, AGT_MAILBOX_ALIGN);
    objectBufferSize   = req_size;
    if (objectBuffer == NULL )
      return AGT_ERROR_BAD_ALLOC;
  }

  address      = _aligned_malloc( messageSize * messageSlots, AGT_CACHE_LINE );

  if ( address == NULL ) {
    if ( !(flags & AGT_MAILBOX_RESIDES_IN_EXTERNAL_BUFFER ) )
      _aligned_free(objectBuffer);
    return AGT_ERROR_BAD_ALLOC;
  }

  mutableMailbox = (agt_mutable_mailbox_t)objectBuffer;

  switch ( flags & AGT_MAILBOX_KIND_BITMASK ) {
    case AGT_MAILBOX_KIND_SSSR:
      mutableMailbox->vtable = &agt_mailbox_vtable_sssr;
      break;
    case AGT_MAILBOX_KIND_SSMR:
      mutableMailbox->vtable = &agt_mailbox_vtable_ssmr;
      break;
    case AGT_MAILBOX_KIND_MSSR:
      mutableMailbox->vtable = &agt_mailbox_vtable_mssr;
      break;
    case AGT_MAILBOX_KIND_MSMR:
      mutableMailbox->vtable = &agt_mailbox_vtable_msmr;
      break;
    case AGT_MAILBOX_KIND_SSCMR:
      mutableMailbox->vtable = &agt_mailbox_vtable_sscmr;
      break;
    case AGT_MAILBOX_KIND_MSCMR:
      mutableMailbox->vtable = &agt_mailbox_vtable_mscmr;
      break;
    case AGT_MAILBOX_KIND_CMSMR:
      mutableMailbox->vtable = &agt_mailbox_vtable_cmsmr;
      break;
    case AGT_MAILBOX_KIND_CMSSR:
      mutableMailbox->vtable = &agt_mailbox_vtable_cmssr;
      break;
    case AGT_MAILBOX_KIND_CMSCMR:
      mutableMailbox->vtable = &agt_mailbox_vtable_cmscmr;
      break;
    AGT_no_default;
  }


  mutableMailbox->flags         = flags;
  mutableMailbox->maxSenders    = createInfo->max_senders;
  mutableMailbox->maxReceivers  = createInfo->max_receivers;
  mutableMailbox->messageSize   = messageSize;
  mutableMailbox->messageSlots  = messageSlots;
  mutableMailbox->address       = address;
  mutableMailbox->pfnDtor       = createInfo->destructor_callback;
  mutableMailbox->pDtorUserData = createInfo->destructor_user_data;
  mutableMailbox->nameLength    = nameLength;

  if ( nameLength != 0 )
    memcpy(mutableMailbox->name, createInfo->name, nameLength);

  agt_status_t init_result = mutableMailbox->vtable->init((agt_mailbox_t)mutableMailbox);
  if ( init_result != AGT_SUCCESS ) {
    _aligned_free(address);
    if ( !(flags & AGT_MAILBOX_RESIDES_IN_EXTERNAL_BUFFER) )
      _aligned_free(objectBuffer);
    return init_result;
  }
  *mailbox = (agt_mailbox_t)mutableMailbox;
  return AGT_SUCCESS;
}
AGT_api agt_status_t        AGT_stdcall agt_dynamic_mailbox_create(agt_dynamic_mailbox_t* mailbox, agt_size_t mailboxSize, agt_size_t maxMessageSize, const agt_mailbox_create_info_t* createInfo) {
  agt_status_t                  status;
  agt_mailbox_flags_t           flags;
  void*                         objectBuffer;
  void*                         address;
  agt_size_t                    objectBufferSize;
  agt_size_t                    nameLength;
  agt_dynamic_mailbox_vptr_t    vptr;
  agt_mutable_dynamic_mailbox_t mutableMailbox;

  if ( createInfo->max_senders > AGT_MAX_SENDERS_LIMIT || createInfo->max_receivers > AGT_MAX_RECEIVERS_LIMIT )
    return AGT_ERROR_INVALID_ARGUMENT;

  if ( maxMessageSize != AGT_DONT_CARE ) {
    if ( maxMessageSize > mailboxSize )
      return AGT_ERROR_INVALID_ARGUMENT;
  }

  if ( mailboxSize & (agt_get_dynamic_mailbox_granularity() - 1) ) // granularity is always be a power of 2.
    return AGT_ERROR_BAD_SIZE;

  flags = AGT_MAILBOX_IS_DYNAMIC;

  if ( createInfo->max_senders == 1 )
    flags |= AGT_MAILBOX_SINGLE_SENDER;
  if ( createInfo->max_receivers == 1 )
    flags |= AGT_MAILBOX_SINGLE_RECEIVER;
  if ( createInfo->flags & AGT_MAILBOX_CREATE_COHERENT_SEND ) {
    flags |= AGT_MAILBOX_COHERENT_SEND;
    if ( createInfo->max_senders == 1 ) {
      status = AGT_ERROR_INVALID_ARGUMENT;
      goto exit;
    }
  }
  if ( createInfo->flags & AGT_MAILBOX_CREATE_COHERENT_RECEIVE ) {
    flags |= AGT_MAILBOX_COHERENT_RECEIVE;
    if ( createInfo->max_receivers == 1 ) {
      status = AGT_ERROR_INVALID_ARGUMENT;
      goto exit;
    }
  }
  if ( createInfo->flags & AGT_MAILBOX_CREATE_IPC_CAPABLE )
    flags |= AGT_MAILBOX_INTERPROCESS;


  if ( createInfo->name != NULL )
    nameLength = createInfo->name_length ?: strlen(createInfo->name);
  else
    nameLength = 0;

  const agt_u64_t req_size = sizeof(struct agt_any_mailbox_common) + nameLength;

  if ( createInfo->external_buffer != NULL ) {
    objectBuffer       = createInfo->external_buffer;
    objectBufferSize   = createInfo->external_buffer_size;
    if ( !agt_align_buffer__(req_size, &objectBuffer, &objectBufferSize)) {
      status = AGT_ERROR_INSUFFICIENT_BUFFER_SIZE;
      goto exit;
    }
    flags |= AGT_MAILBOX_RESIDES_IN_EXTERNAL_BUFFER;
  }
  else {
    objectBuffer       = _aligned_malloc(req_size, AGT_MAILBOX_ALIGN);
    objectBufferSize   = req_size;
    if (objectBuffer == NULL ) {
      status = AGT_ERROR_BAD_ALLOC;
      goto exit;
    }
  }

  mutableMailbox = (agt_mutable_dynamic_mailbox_t)objectBuffer;

  status = agt_alloc_dynamic__(&address, mailboxSize, createInfo->name, nameLength);

  if ( status != AGT_SUCCESS )
    goto cleanup;


  switch ( flags & AGT_MAILBOX_KIND_BITMASK ) {
    case AGT_MAILBOX_KIND_SSSR:
      mutableMailbox->vtable = &agt_dynamic_mailbox_vtable_sssr;
      break;
    case AGT_MAILBOX_KIND_SSMR:
      mutableMailbox->vtable = &agt_dynamic_mailbox_vtable_ssmr;
      break;
    case AGT_MAILBOX_KIND_MSSR:
      mutableMailbox->vtable = &agt_dynamic_mailbox_vtable_mssr;
      break;
    case AGT_MAILBOX_KIND_MSMR:
      mutableMailbox->vtable = &agt_dynamic_mailbox_vtable_msmr;
      break;
    case AGT_MAILBOX_KIND_SSCMR:
      mutableMailbox->vtable = &agt_dynamic_mailbox_vtable_sscmr;
      break;
    case AGT_MAILBOX_KIND_MSCMR:
      mutableMailbox->vtable = &agt_dynamic_mailbox_vtable_mscmr;
      break;
    case AGT_MAILBOX_KIND_CMSMR:
      mutableMailbox->vtable = &agt_dynamic_mailbox_vtable_cmsmr;
      break;
    case AGT_MAILBOX_KIND_CMSSR:
      mutableMailbox->vtable = &agt_dynamic_mailbox_vtable_cmssr;
      break;
    case AGT_MAILBOX_KIND_CMSCMR:
      mutableMailbox->vtable = &agt_dynamic_mailbox_vtable_cmscmr;
      break;
    AGT_no_default;
  }

  mutableMailbox->flags          = flags;
  mutableMailbox->maxSenders     = createInfo->max_senders;
  mutableMailbox->maxReceivers   = createInfo->max_receivers;
  mutableMailbox->mailboxSize    = mailboxSize;
  mutableMailbox->maxMessageSize = maxMessageSize;
  mutableMailbox->address        = address;
  mutableMailbox->pfnDtor        = createInfo->destructor_callback;
  mutableMailbox->pDtorUserData  = createInfo->destructor_user_data;
  mutableMailbox->nameLength     = nameLength;

  if ( nameLength != 0 )
    memcpy(mutableMailbox->name, createInfo->name, nameLength);

  status = mutableMailbox->vtable->init((agt_dynamic_mailbox_t)mutableMailbox);

  if ( status == AGT_SUCCESS ) {
    address = NULL;
    objectBuffer = NULL;
  }

cleanup:

  if ( address != NULL ) {
    agt_free_dynamic__(address, mailboxSize);
  }
  if ( objectBuffer != NULL ) {
    if ( !(flags & AGT_MAILBOX_RESIDES_IN_EXTERNAL_BUFFER ) )
      _aligned_free(objectBuffer);
  }

exit:
  return status;
}
AGT_api void                AGT_stdcall agt_mailbox_destroy(agt_any_mailbox_t mailbox) {
  if ( mailbox_common_base(mailbox) ) {
    if ( mailbox_common_base(mailbox)->flags & AGT_MAILBOX_IS_DYNAMIC )
      any_mailbox_cast(agt_dynamic_mailbox_t, mailbox)->vtable->destroy(any_mailbox_cast(agt_dynamic_mailbox_t, mailbox));
    else
      any_mailbox_cast(agt_mailbox_t, mailbox)->vtable->destroy(any_mailbox_cast(agt_mailbox_t, mailbox));
  }
}

AGT_api agt_size_t          AGT_stdcall agt_mailbox_get_message_size(agt_any_mailbox_t mailbox) {
  if ( agt_mailbox_get_flags(mailbox) & AGT_MAILBOX_IS_DYNAMIC )
    return 0;
  return any_mailbox_cast(agt_mailbox_t, mailbox)->message_size;
}
AGT_api agt_mailbox_flags_t AGT_stdcall agt_mailbox_get_flags(agt_any_mailbox_t mailbox) {
  return mailbox_common_base(mailbox)->flags;
}
AGT_api agt_mailbox_kind_t  AGT_stdcall agt_mailbox_get_kind(agt_any_mailbox_t mailbox) {
  return mailbox_common_base(mailbox)->flags & AGT_MAILBOX_KIND_BITMASK;
}
AGT_api agt_size_t          AGT_stdcall agt_mailbox_get_max_senders(agt_any_mailbox_t mailbox) {
  return mailbox_common_base(mailbox)->max_senders;
}
AGT_api agt_size_t          AGT_stdcall agt_mailbox_get_max_receivers(agt_any_mailbox_t mailbox) {
  return mailbox_common_base(mailbox)->max_receivers;
}

AGT_api bool                AGT_stdcall agt_mailbox_get_name(agt_any_mailbox_t mailbox, const agt_char_t** name, agt_size_t* length) {
  if ((*length = mailbox_common_base(mailbox)->name_length)) {
    *name = mailbox_common_base(mailbox)->name;
    return true;
  }
  return false;
}
AGT_api bool                AGT_stdcall agt_mailbox_get_external_buffer(agt_any_mailbox_t mailbox, void** buffer, agt_size_t* size) {
  if ( agt_mailbox_get_flags(mailbox) & AGT_MAILBOX_RESIDES_IN_EXTERNAL_BUFFER ) {
    *buffer = any_mailbox_cast(agt_mailbox_t, mailbox);
    *size   = agt_mailbox_obj_size__(mailbox);
    return true;
  }
  return false;
}

AGT_api agt_size_t          AGT_stdcall agt_get_dynamic_mailbox_granularity() {
  SYSTEM_INFO info;
  GetSystemInfo(&info);
  return info.dwPageSize;
}