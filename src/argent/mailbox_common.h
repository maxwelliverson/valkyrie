//
// Created by maxwe on 2021-05-22.
//

#ifndef ARGENT_INTERNAL_MAILBOX_COMMON_H
#define ARGENT_INTERNAL_MAILBOX_COMMON_H

#include <argent/mailbox.h>
#include <argent/sender.h>
#include <argent/receiver.h>

#include "atomic.h"


typedef agt_status_t (AGT_stdcall* PFN_init_mailbox)(agt_mailbox_t mailbox);
typedef agt_status_t (AGT_stdcall* PFN_attach_sender)(agt_mailbox_t mailbox, agt_sender_t* pSender, agt_deadline_t deadline);
typedef agt_status_t (AGT_stdcall* PFN_begin_send)(agt_mailbox_t mailbox, void** address, agt_deadline_t deadline);
typedef void         (AGT_stdcall* PFN_finish_send)(agt_mailbox_t mailbox, void* address);
typedef void         (AGT_stdcall* PFN_detach_sender)(agt_mailbox_t mailbox, agt_sender_t sender);
typedef agt_status_t (AGT_stdcall* PFN_attach_receiver)(agt_mailbox_t mailbox, agt_receiver_t* pReceiver, agt_deadline_t deadline);
typedef agt_status_t (AGT_stdcall* PFN_begin_receive)(agt_mailbox_t mailbox, void** address, agt_deadline_t deadline);
typedef void         (AGT_stdcall* PFN_finish_receive)(agt_mailbox_t mailbox, void* address, agt_receive_action_t action);
typedef void         (AGT_stdcall* PFN_detach_receiver)(agt_mailbox_t mailbox, agt_receiver_t receiver);
typedef void         (AGT_stdcall* PFN_destroy_mailbox)(agt_mailbox_t mailbox);

typedef agt_status_t (AGT_stdcall* PFN_init_dynamic_mailbox)(agt_dynamic_mailbox_t mailbox);
typedef agt_status_t (AGT_stdcall* PFN_attach_dynamic_sender)(agt_dynamic_mailbox_t mailbox, agt_dynamic_sender_t* pSender, agt_deadline_t deadline);
typedef agt_status_t (AGT_stdcall* PFN_begin_dynamic_send)(agt_dynamic_mailbox_t mailbox, void** address, agt_size_t size, agt_deadline_t deadline);
typedef void         (AGT_stdcall* PFN_finish_dynamic_send)(agt_dynamic_mailbox_t mailbox, void* address);
typedef void         (AGT_stdcall* PFN_detach_dynamic_sender)(agt_dynamic_mailbox_t mailbox, agt_dynamic_sender_t sender);
typedef agt_status_t (AGT_stdcall* PFN_attach_dynamic_receiver)(agt_dynamic_mailbox_t mailbox, agt_dynamic_receiver_t* pReceiver, agt_deadline_t deadline);
typedef agt_status_t (AGT_stdcall* PFN_begin_dynamic_receive)(agt_dynamic_mailbox_t mailbox, void** address, agt_size_t* size, agt_deadline_t deadline);
typedef void         (AGT_stdcall* PFN_finish_dynamic_receive)(agt_dynamic_mailbox_t mailbox, void* address, agt_receive_action_t action);
typedef void         (AGT_stdcall* PFN_detach_dynamic_receiver)(agt_dynamic_mailbox_t mailbox, agt_dynamic_receiver_t receiver);
typedef void         (AGT_stdcall* PFN_destroy_dynamic_mailbox)(agt_dynamic_mailbox_t mailbox);


typedef struct AGT_alignas(32) {
  PFN_attach_sender   attachSender;
  PFN_begin_send      beginSend;
  PFN_finish_send     finishSend;
  PFN_detach_sender   detachSender;
  PFN_attach_receiver attachReceiver;
  PFN_begin_receive   beginReceive;
  PFN_finish_receive  finishReceive;
  PFN_detach_receiver detachReceiver;

  PFN_init_mailbox    init;
  PFN_destroy_mailbox destroy;

} agt_mailbox_vtable_t;
typedef struct AGT_alignas(32) {
  PFN_attach_dynamic_sender   attachSender;
  PFN_begin_dynamic_send      beginSend;
  PFN_finish_dynamic_send     finishSend;
  PFN_detach_dynamic_sender   detachSender;
  PFN_attach_dynamic_receiver attachReceiver;
  PFN_begin_dynamic_receive   beginReceive;
  PFN_finish_dynamic_receive  finishReceive;
  PFN_detach_dynamic_receiver detachReceiver;

  PFN_init_dynamic_mailbox    init;
  PFN_destroy_dynamic_mailbox destroy;
} agt_dynamic_mailbox_vtable_t;


/*
typedef struct agt_mailbox_msmr*       agt_mailbox_msmr_t;
typedef struct agt_mailbox_ssmr*       agt_mailbox_ssmr_t;
typedef struct agt_mailbox_mssr*       agt_mailbox_mssr_t;
typedef struct agt_mailbox_sssr*       agt_mailbox_sssr_t;
typedef struct agt_mailbox_cmsmr*      agt_mailbox_cmsmr_t;
typedef struct agt_mailbox_sscmr*      agt_mailbox_sscmr_t;
typedef struct agt_mailbox_cmssr*      agt_mailbox_cmssr_t;
typedef struct agt_mailbox_mscmr*      agt_mailbox_mscmr_t;
typedef struct agt_mailbox_cmscmr*     agt_mailbox_cmscmr_t;

typedef struct agt_dyn_mailbox_msmr*   agt_dyn_mailbox_msmr_t;
typedef struct agt_dyn_mailbox_ssmr*   agt_dyn_mailbox_ssmr_t;
typedef struct agt_dyn_mailbox_mssr*   agt_dyn_mailbox_mssr_t;
typedef struct agt_dyn_mailbox_sssr*   agt_dyn_mailbox_sssr_t;
typedef struct agt_dyn_mailbox_cmsmr*  agt_dyn_mailbox_cmsmr_t;
typedef struct agt_dyn_mailbox_sscmr*  agt_dyn_mailbox_sscmr_t;
typedef struct agt_dyn_mailbox_cmssr*  agt_dyn_mailbox_cmssr_t;
typedef struct agt_dyn_mailbox_mscmr*  agt_dyn_mailbox_mscmr_t;
typedef struct agt_dyn_mailbox_cmscmr* agt_dyn_mailbox_cmscmr_t;
*/


extern const agt_mailbox_vtable_t agt_mailbox_vtable_sssr;
extern const agt_mailbox_vtable_t agt_mailbox_vtable_ssmr;
extern const agt_mailbox_vtable_t agt_mailbox_vtable_mssr;
extern const agt_mailbox_vtable_t agt_mailbox_vtable_msmr;
extern const agt_mailbox_vtable_t agt_mailbox_vtable_sscmr;
extern const agt_mailbox_vtable_t agt_mailbox_vtable_cmssr;
extern const agt_mailbox_vtable_t agt_mailbox_vtable_cmsmr;
extern const agt_mailbox_vtable_t agt_mailbox_vtable_mscmr;
extern const agt_mailbox_vtable_t agt_mailbox_vtable_cmscmr;

extern const agt_dynamic_mailbox_vtable_t agt_dynamic_mailbox_vtable_sssr;
extern const agt_dynamic_mailbox_vtable_t agt_dynamic_mailbox_vtable_ssmr;
extern const agt_dynamic_mailbox_vtable_t agt_dynamic_mailbox_vtable_mssr;
extern const agt_dynamic_mailbox_vtable_t agt_dynamic_mailbox_vtable_msmr;
extern const agt_dynamic_mailbox_vtable_t agt_dynamic_mailbox_vtable_sscmr;
extern const agt_dynamic_mailbox_vtable_t agt_dynamic_mailbox_vtable_cmssr;
extern const agt_dynamic_mailbox_vtable_t agt_dynamic_mailbox_vtable_cmsmr;
extern const agt_dynamic_mailbox_vtable_t agt_dynamic_mailbox_vtable_mscmr;
extern const agt_dynamic_mailbox_vtable_t agt_dynamic_mailbox_vtable_cmscmr;

typedef const agt_mailbox_vtable_t*         agt_mailbox_vptr_t;
typedef const agt_dynamic_mailbox_vtable_t* agt_dynamic_mailbox_vptr_t;

typedef PFN_agt_mailbox_destructor PFN_mailbox_dtor;


struct AGT_cache_aligned agt_mailbox_common{
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
};
struct AGT_cache_aligned agt_dynamic_mailbox_common{
  const agt_dynamic_mailbox_vptr_t vtable;
  const agt_mailbox_flags_t        flags;
  const agt_u16_t                  maxSenders;
  const agt_u16_t                  maxReceivers;
  const agt_size_t                 nameLength;
  const PFN_mailbox_dtor           pfnDtor;
  void* const                      pDtorUserData;
  void* const                      address;
  const agt_size_t                 mailboxSize;
  const agt_size_t                 maxMessageSize;
};
struct AGT_cache_aligned mailbox_private_info{};
struct AGT_cache_aligned mailbox_sender_info{
  atomic_u64_t write_offset;
};
struct AGT_cache_aligned mailbox_receiver_info{
  atomic_u64_t read_offset;
};

struct AGT_cache_aligned agt_any_mailbox_common{
  const void* const            vtable;
  const agt_mailbox_flags_t    flags;
  const agt_u16_t              max_senders;
  const agt_u16_t              max_receivers;
  const agt_size_t             name_length;
  const PFN_mailbox_dtor       pfn_dtor;
  void* const                  dtor_user_data;
  void* const                  address;
  struct mailbox_private_info  info;
  struct mailbox_sender_info   senders;
  struct mailbox_receiver_info receivers;
  const agt_char_t             name[];
};
struct AGT_cache_aligned agt_mailbox{
  const agt_mailbox_vptr_t     vtable;
  const agt_mailbox_flags_t    flags;
  const agt_u16_t              max_senders;
  const agt_u16_t              max_receivers;
  const agt_size_t             name_length;
  const PFN_mailbox_dtor       pfn_dtor;
  void* const                  dtor_user_data;
  void* const                  address;
  const agt_size_t             message_size;
  const agt_size_t             message_slots;
  struct mailbox_private_info  info;
  struct mailbox_sender_info   sender_info;
  struct mailbox_receiver_info receiver_info;
  const agt_char_t             name[];
};
struct AGT_cache_aligned agt_dynamic_mailbox{
  const agt_dynamic_mailbox_vptr_t vtable;
  const agt_mailbox_flags_t        flags;
  const agt_u16_t                  max_senders;
  const agt_u16_t                  max_receivers;
  const agt_size_t                 name_length;
  const PFN_mailbox_dtor           pfn_dtor;
  void* const                      dtor_user_data;
  void* const                      address;
  const agt_size_t                 mailbox_size;
  const agt_size_t                 max_message_size;
  struct mailbox_private_info      info;
  struct mailbox_sender_info       sender_info;
  struct mailbox_receiver_info     receiver_info;
  const agt_char_t                 name[];
};


#if AGT_has_attribute(transparent_union)
#define any_mailbox_cast(type, mailbox) (mailbox.m_##type)
#define mailbox_common_base(mailbox) ((struct agt_any_mailbox_common*)mailbox.m_agt_mailbox_t)
#else
#define any_mailbox_cast(type, mailbox) ((type)mailbox)
#define mailbox_common_base(mailbox) ((struct agt_any_mailbox_common*)mailbox)
#endif



_Static_assert(sizeof(struct agt_any_mailbox_common)   == AGT_MAILBOX_SIZE, "");
_Static_assert(_Alignof(struct agt_any_mailbox_common) == AGT_MAILBOX_ALIGN, "");

_Static_assert(sizeof(struct agt_mailbox)   == AGT_MAILBOX_SIZE, "");
_Static_assert(_Alignof(struct agt_mailbox) == AGT_MAILBOX_ALIGN, "");

_Static_assert(sizeof(struct agt_dynamic_mailbox)   == AGT_MAILBOX_SIZE, "");
_Static_assert(_Alignof(struct agt_dynamic_mailbox) == AGT_MAILBOX_ALIGN, "");




#endif  //ARGENT_INTERNAL_MAILBOX_COMMON_H
