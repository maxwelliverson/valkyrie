//
// Created by maxwe on 2021-04-07.
//

#ifndef VALKYRIE_INTERNAL_JSONVMM_H
#define VALKYRIE_INTERNAL_JSONVMM_H


#include <json/core.h>

/**
 * Default size of the virtual memory space used by the context allocator.
 *
 * Equal to 64GB
 *
 * */
#define JSON_ALLOCATOR_VIRTUAL_MEMORY_SPACE_SIZE (0x1ULL << 36)

/**
 * Default size of the virtual memory pages
 *
 * Equal to 64KB
 *
 * */
#define JSON_VIRTUAL_PAGE_SIZE (0x1ULL << 16)






JSON_BEGIN_C_NAMESPACE



typedef struct vpage *           vpage_t;
typedef struct vmem_manager*     vmem_manager_t;
typedef struct vmem_placeholder* vmem_placeholder_t;
typedef struct vmem_file*        vmem_file_t;





/*json_status_t      vmem_manager_init(vmem_manager_t manager, json_u64_t addressSpaceSize);
json_status_t      vmem_manager_cleanup(vmem_manager_t manager);*/

json_u32_t         json_get_virtual_page_size__(vmem_manager_t manager);
json_status_t      json_get_virtual_base_page__(vpage_t* pResultPage, void* address);

json_status_t      json_open_virtual_file__(vmem_file_t* pFile, json_file_t file, const char* name);
void               json_close_virtual_file__(vmem_file_t file);


void*              json_virtual_alloc__(vmem_manager_t manager, json_u64_t size);
void               json_virtual_free__(vmem_manager_t manager, void* address, json_u64_t size);


/*json_status_t      vmem_release(vmem_manager_t manager);*/


void*              json_placeholder_commit__(vmem_placeholder_t placeholder, json_u64_t size);
vmem_placeholder_t json_placeholder_decommit__(void* addr, json_u64_t size);

vmem_placeholder_t json_placeholder_reserve__(json_u64_t size, json_u64_t align);
vmem_placeholder_t json_placeholder_split__(vmem_placeholder_t placeholder, json_u64_t size);
vmem_placeholder_t json_placeholder_coalesce__(vmem_placeholder_t placeholder, json_u64_t size);
void               json_placeholder_release__(vmem_placeholder_t placeholder, json_u64_t size);

void*              json_placeholder_map__(vmem_placeholder_t placeholder, vmem_file_t file, json_u64_t size, json_u64_t offset);
vmem_placeholder_t json_placeholder_unmap__(void* addr);



/*json_status_t json_vmem_map(void** pResult, vmem_manager_t allocator, vmem_file_t file, json_u64_t size, json_u64_t offset);
void          json_vmem_unmap(vmem_manager_t allocator, void* mappedFile, json_u64_t size);*/








JSON_END_C_NAMESPACE


#endif//VALKYRIE_INTERNAL_JSONVMM_H
