//
// Created by maxwe on 2021-05-31.
//

#ifndef AGATE_QUARTZ_H
#define AGATE_QUARTZ_H

#include "core.h"

AGT_BEGIN_C_NAMESPACE

typedef agt_u32_t qtz_local_id_t;
typedef agt_u64_t qtz_global_id_t;

typedef struct qtz_mailbox*        qtz_mailbox_t;
typedef struct qtz_request*        qtz_request_t;
typedef struct qtz_deputy*         qtz_deputy_t;
typedef struct qtz_process*        qtz_process_t;
typedef struct qtz_shared_mailbox* qtz_shared_mailbox_t;


typedef struct {} qtz_init_params_t;


agt_status_t qtz_init(const qtz_init_params_t* params);

agt_status_t qtz_task_status(qtz_request_t message);
void         qtz_task_wait(qtz_request_t message);
void         qtz_task_discard(qtz_request_t message);




extern const agt_u32_t qtz_page_size;

typedef int(AGT_stdcall*qtz_deputy_proc_t)(void*);

AGT_stdcall qtz_request_t qtz_alloc_pages(agt_u32_t requestCount, void** ppPages, const agt_u32_t* pPageCounts);
AGT_stdcall void          qtz_free_pages(agt_u32_t requestCount, void* const * ppPages, const agt_u32_t* pPageCounts);



AGT_stdcall qtz_request_t qtz_open_ipc_link();
AGT_stdcall void          qtz_close_ipc_link();
AGT_stdcall qtz_request_t qtz_send_ipc_message(const void* buffer, agt_u32_t messageSize);

AGT_stdcall qtz_request_t qtz_open_thread(qtz_deputy_t* pDeputy, qtz_deputy_proc_t proc, void* userData);
AGT_stdcall void          qtz_close_thread(qtz_deputy_t deputy, bool sendKillSignal);

AGT_stdcall qtz_request_t qtz_attach_thread();
AGT_stdcall void          qtz_detach_thread();
AGT_stdcall qtz_request_t qtz_register_agent();
AGT_stdcall void          qtz_unregister_agent();




AGT_END_C_NAMESPACE

#endif  //AGATE_QUARTZ_H
