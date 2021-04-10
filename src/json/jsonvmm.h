//
// Created by maxwe on 2021-04-07.
//

#ifndef VALKYRIE_INTERNAL_JSONVMM_H
#define VALKYRIE_INTERNAL_JSONVMM_H


#include <json/core.h>

/**
 * Default size of the virtual memory space used by the primary document allocator.
 *
 * Equal to 4GB
 *
 * */
#define JSON_ALLOCATOR_VIRTUAL_MEMORY_SPACE_SIZE (0x1ULL << 31)

/**
 * Default size of the virtual pages used by string pools.
 *
 * Equal to 64KB
 *
 * */
#define JSON_VIRTUAL_PAGE_SIZE (0x1ULL << 15)




JSON_BEGIN_C_NAMESPACE



typedef struct vpage *           vpage_t;
typedef struct vmem_manager*     vmem_manager_t;
typedef struct vmem_placeholder* vmem_placeholder_t;
typedef struct vmem_file*        vmem_file_t;





json_status_t      vmem_manager_init(vmem_manager_t manager, json_u64_t addressSpaceSize);
json_status_t      vmem_manager_cleanup(vmem_manager_t manager);

json_u32_t         vmem_get_page_size(vmem_manager_t manager);
json_status_t      vmem_get_base_page(vpage_t * pResultPage, void* address);

json_status_t      vmem_open_file(vmem_file_t* pFile, json_file_t file, const char* name);
void               vmem_close_file(vmem_file_t file);


void*              vmem_alloc(vmem_manager_t manager, json_u64_t size);
void               vmem_dealloc(vmem_manager_t manager, void* address, json_u64_t size);


json_status_t      vmem_release(vmem_manager_t manager);


void*              vmem_placeholder_commit(vmem_placeholder_t placeholder, json_u64_t size);
vmem_placeholder_t vmem_placeholder_decommit(void* addr, json_u64_t size);

vmem_placeholder_t vmem_placeholder_reserve(json_u64_t size, json_u64_t align);
vmem_placeholder_t vmem_placeholder_split(vmem_placeholder_t placeholder, json_u64_t size);
vmem_placeholder_t vmem_placeholder_coalesce(vmem_placeholder_t placeholder, json_u64_t size);
void               vmem_placeholder_release(vmem_placeholder_t placeholder, json_u64_t size);

void*              vmem_placeholder_map(vmem_placeholder_t placeholder, vmem_file_t file, json_u64_t size, json_u64_t offset);
vmem_placeholder_t vmem_placeholder_unmap(void* addr);



json_status_t json_vmem_map(void** pResult, vmem_manager_t allocator, vmem_file_t file, json_u64_t size, json_u64_t offset);
void          json_vmem_unmap(vmem_manager_t allocator, void* mappedFile, json_u64_t size);








JSON_END_C_NAMESPACE


#endif//VALKYRIE_INTERNAL_JSONVMM_H
