//
// Created by maxwe on 2021-05-24.
//

#include "mailbox_common.h"

#include <argent/sender.h>


struct agt_sender{
  agt_mailbox_t mailbox;
};
struct agt_dynamic_sender{
  agt_dynamic_mailbox_t mailbox;
};


AGT_api agt_status_t AGT_stdcall agt_open_sender(agt_mailbox_t mailbox, agt_sender_t* pSender, agt_u64_t timeout) {
  return mailbox->vtable->attachSender(mailbox, pSender, agt_deadline_from_timeout(timeout));
}
AGT_api void         AGT_stdcall agt_close_sender(agt_sender_t sender) {
  if ( sender != NULL ) {
    sender->mailbox->vtable->detachSender(sender->mailbox, sender);
  }
}
AGT_api agt_status_t AGT_stdcall agt_open_dynamic_sender(agt_dynamic_mailbox_t mailbox, agt_dynamic_sender_t* pSender, agt_u64_t timeout) {
  return mailbox->vtable->attachSender(mailbox, pSender, agt_deadline_from_timeout(timeout));
}
AGT_api void         AGT_stdcall agt_close_dynamic_sender(agt_dynamic_sender_t sender) {
  sender->mailbox->vtable->detachSender(sender->mailbox, sender);
}

AGT_api agt_status_t AGT_stdcall agt_begin_send(agt_sender_t sender, void** address, agt_u64_t timeout) {
  return sender->mailbox->vtable->beginSend(sender->mailbox, address, agt_deadline_from_timeout(timeout));
}
AGT_api void         AGT_stdcall agt_finish_send(agt_sender_t sender, void* address) {
  sender->mailbox->vtable->finishSend(sender->mailbox, address);
}

AGT_api agt_status_t AGT_stdcall agt_begin_dynamic_send(agt_dynamic_sender_t sender, void** address, agt_size_t size, agt_u64_t timeout) {
  return sender->mailbox->vtable->beginSend(sender->mailbox, address, size, agt_deadline_from_timeout(timeout));
}
AGT_api void         AGT_stdcall agt_finish_dynamic_send(agt_dynamic_sender_t sender, void* address) {
  sender->mailbox->vtable->finishSend(sender->mailbox, address);
}
