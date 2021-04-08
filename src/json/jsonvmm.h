//
// Created by maxwe on 2021-04-07.
//

#ifndef VALKYRIE_INTERNAL_JSONVMM_H
#define VALKYRIE_INTERNAL_JSONVMM_H


#include "jsonmm_types.h"



JSON_BEGIN_C_NAMESPACE


json_status_t json_vmem_manager_init(struct json_virtual_page_manager* manager);
json_status_t json_vmem_manager_cleanup(struct json_virtual_page_manager* manager);

json_status_t json_vmem_manager_create(json_virtual_page_manager_t* pManager, json_u64_t virtualAddressSpaceSize);
json_status_t json_vmem_manager_destroy(json_virtual_page_manager_t manager);

json_u32_t    json_vmem_get_page_size(json_virtual_page_manager_t manager);
json_status_t json_vmem_get_base_page(json_virtual_page_t* pResultPage, json_address_t address);

json_status_t json_vmem_reserve(json_virtual_page_t* pResultPage, json_virtual_page_manager_t manager, json_u64_t size);
json_status_t json_vmem_commit(json_virtual_page_t pageRange, json_u64_t size, json_u64_t offset);
json_status_t json_vmem_decommit(json_virtual_page_t pageRange);
json_status_t json_vmem_release(json_virtual_page_t pageRange);



JSON_END_C_NAMESPACE


#endif//VALKYRIE_INTERNAL_JSONVMM_H
