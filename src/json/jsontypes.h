//
// Created by Maxwell on 2021-02-18.
//

#ifndef VALKYRIE_INTERNAL_JSON_TYPES_H
#define VALKYRIE_INTERNAL_JSON_TYPES_H


#include <json/core.h>
#include <json/file.h>
#include "jsondefs.h"


JSON_BEGIN_C_NAMESPACE

typedef struct json_internal_allocator*       json_internal_allocator_t;
typedef struct json_fixed_size_allocator*     json_fixed_size_allocator_t;
typedef struct json_fixed_size_chunk*         json_chunk_t;
typedef struct json_chunk_stack*              json_chunk_stack_t;

typedef struct vpage *                        vpage_t;
typedef struct vmem_placeholder *             vmem_placeholder_t;
typedef struct vmem_node *                    vmem_node_t;
typedef struct vmem_split *                   vmem_split_t;
typedef struct vmem_manager *                 vmem_manager_t;

struct json_fixed_size_chunk {
  unsigned char* data;
  unsigned char  firstAvailableBlock;
  unsigned char  blocksAvailable;
};
struct json_chunk_stack{
  struct json_fixed_size_chunk* startAddr;
  struct json_fixed_size_chunk* topAddr;
  struct json_fixed_size_chunk* endAddr;
};
struct json_fixed_size_allocator{
  json_u32_t                 blockSize;
  json_u32_t                 blocks;
  json_u32_t                 blockAtomSizeLog2;
  json_u32_t                 chunkTotalSize;  // exact size of each chunk, cached to slightly speed up allocations
  struct json_chunk_stack    chunkStack;
  json_internal_allocator_t  allocator;
  json_chunk_t               allocChunk;
  json_chunk_t               deallocChunk;
  json_chunk_t               emptyChunk;
  json_bool_t                initialized;
};

struct vpage {
  json_byte_t data[];
};

struct vmem_node {
  //json_u32_t               liveAllocationCount;
  json_u32_t         order   : 5;
  json_u32_t         state   : 3;
  vmem_placeholder_t address;
  vmem_node_t        parent;
  vmem_split_t       children;
};
struct vmem_split{
  struct vmem_node node[2];
};
struct vmem_node_pool{

};
struct json_virtual_page_node_pool{

};

struct json_virtual_page_list_entry{
  vmem_node_t node;

};

struct vmem_manager {

  struct vmem_node                 root;
  struct json_fixed_size_allocator splitNodePool;

  /*vmem_node_t allocPage;
  vmem_node_t deallocPage;
  vmem_node_t emptyPage;*/
};


struct json_internal_allocator{
  struct json_fixed_size_allocator fixedSizeAllocators[JSON_FIXED_SIZE_ALLOCATOR_COUNT];
  struct vmem_manager              vmemManager;
  json_address_t                   initialAddress;
  json_u64_t                       addressSpaceSize;
};


struct json_file{
  json_file_native_t nativeHandle;
  json_u64_t         maxSize;
};




JSON_END_C_NAMESPACE

#endif //VALKYRIE_INTERNAL_JSON_TYPES_H
