//
// Created by Maxwell on 2021-02-14.
//

#ifndef VALKYRIE_JSONMM_H
#define VALKYRIE_JSONMM_H

#include "jsonmm_types.h"

#include <limits.h>


#define JSON_FIXED_SIZE_ALLOCATOR_COUNT 17
#define JSON_SMALL_OBJECT_MAX_SIZE      (UCHAR_MAX + 1)


JSON_BEGIN_C_NAMESPACE


typedef struct json_internal_allocator*     json_internal_allocator_t;
typedef struct json_fixed_size_allocator*   json_fixed_size_allocator_t;

typedef struct json_stack *          json_stack_t;
typedef const struct json_stack *    json_stack_const_t;


enum json_stack_flag_bits{
  JSON_STACK_DEFAULT_FLAGS   = 0x0,
  JSON_STACK_FIXED_CAPACITY  = 0x1,
  JSON_STACK_BORROWED_MEMORY = 0x2
};
typedef json_flags_t json_stack_flags_t;
typedef struct json_create_stack_params{

  json_stack_flags_t          flags;

  union{
    json_internal_allocator_t allocator;
    json_address_t            address;
  };
  json_u64_t                  initialCapacity;
  json_u64_t                  maxSize;
} json_create_stack_params_t;




typedef struct json_stack {
  json_internal_allocator_t allocator;
  json_address_t            ownedMemory;
  json_address_t            startAddr;
  json_address_t            topAddr;
  json_address_t            endAddr;
  json_bool_t               isFixedSize;
}                 json_stack;

typedef struct json_fixed_size_chunk {
  unsigned char* data;
  unsigned char  firstAvailableBlock;
  unsigned char  blocksAvailable;
}*     json_chunk_t;
typedef struct json_chunk_stack{
  struct json_fixed_size_chunk* startAddr;
  struct json_fixed_size_chunk* topAddr;
  struct json_fixed_size_chunk* endAddr;
} json_chunk_stack_t;
typedef struct json_fixed_size_allocator{
  json_u32_t                 blockSize;
  json_u32_t                 blocks;
  json_u32_t                 blockAtomSizeLog2;
  json_u32_t                 chunkTotalSize;  // exact size of each chunk, cached to slightly speed up allocations
  json_chunk_stack_t         chunkStack;
  json_internal_allocator_t  allocator;
  json_chunk_t               allocChunk;
  json_chunk_t               deallocChunk;
  json_chunk_t               emptyChunk;
  json_bool_t                initialized;
}   json_fixed_size_allocator;
typedef struct json_small_object_allocator{
  json_fixed_size_allocator        fixedSizeAllocators[JSON_FIXED_SIZE_ALLOCATOR_COUNT];
  // {  1,   2,   4,   8,  16,
  //   24,  32,  40,  48,  64,
  //   80,  96, 112, 128, 144,
  //  160, 176, 192, 208, 224,
  //  240, 256 }
}    json_small_object_allocator_t;



typedef struct json_internal_allocator{
  json_small_object_allocator_t     smallAllocator;
  struct json_virtual_page_manager* virtualPageManager;
}     json_internal_allocator;




json_status_t json_internal_allocator_init(json_internal_allocator_t allocator, json_size_t minSize, json_size_t maxSize, json_size_t blocksPerChunk);
json_status_t json_internal_allocator_cleanup(json_internal_allocator_t allocator);


void*         json_internal_alloc(json_status_t* pStatus, json_u64_t size, json_internal_allocator_t allocator);
void*         json_internal_realloc(json_status_t* pStatus, void* addr, /*json_bool_t* pMoved,*/ json_u64_t newSize, json_u64_t oldSize, json_internal_allocator_t allocator);
void          json_internal_free(void* address, json_u64_t size, json_internal_allocator_t allocator);

json_status_t  json_fixed_allocator_init(json_fixed_size_allocator_t fixed, json_internal_allocator_t allocator, json_u32_t blockSize);
void           json_fixed_allocator_cleanup(json_fixed_size_allocator_t fixed);
void*          json_fixed_alloc(json_fixed_size_allocator_t allocator);
void           json_fixed_dealloc(void* address, json_fixed_size_allocator_t allocator);


/**
 * Creates a new stack with the given parameters.
 *
 * @param [out] pStack Pointer to the address in which the stack handle will be returned
 * @param [in] pParams Optional parameters. Can be null.
 * */
json_status_t json_create_stack(json_stack_t * pStack, json_internal_allocator_t alloc, const json_create_stack_params_t* pParams);
void          json_destroy_stack(json_stack_t stack, json_internal_allocator_t allocator);
json_status_t json_stack_push(json_stack_t stack, json_u32_t elementSize, json_u32_t elementCount);
json_status_t json_stack_pop(json_address_t* pResult, json_stack_t stack, json_u32_t elementSize, json_u32_t elementCount);
json_status_t json_stack_top(json_address_t* pResult, json_stack_const_t stack, json_u32_t elementSize);


// json_generic_array_t
/*typedef struct json_generic_array*          json_generic_array_t;
typedef const struct json_generic_array*    json_generic_array_const_t;
typedef enum json_array_flags{
  JSON_ARRAY_DEFAULT_FLAGS   = 0x0,
  JSON_ARRAY_FIXED_CAPACITY  = 0x1,
  JSON_ARRAY_BORROWED_MEMORY = 0x2
} json_array_flags_t;
typedef struct json_create_array_params{

  json_flags_t flags;

  union{
    json_internal_allocator_t allocator;
    json_address_t            address;
  };
  json_u32_t                  elementSize;
  json_u64_t                  initialCapacity;
  json_u64_t                  maxSize;
} json_create_array_params_t;
json_status_t json_create_array(json_generic_array_t* pArray, const json_create_array_params_t* pParams);
void          json_destroy_array(json_generic_array_t array);*/

// json_generic_list_t
/*typedef struct json_generic_list*           json_generic_list_t;
typedef const struct json_generic_list*     json_generic_list_const_t;
typedef enum json_list_flags {
  JSON_LIST_DEFAULT_FLAGS   = 0x0,
  JSON_LIST_SINGLY_LINKED   = 0x1,
  JSON_LIST_HETEROGENEOUS   = 0x2
} json_list_flags_t;
typedef struct json_create_list_params{

  json_flags_t         flags;
  json_internal_allocator_t allocator;
  json_u32_t                elementSize;

  json_u64_t                maxLength;

}  json_create_list_params_t;
json_status_t json_create_list(json_generic_list_t* pList,    const json_create_list_params_t* pParams);
void          json_destroy_list(json_generic_list_t list);
json_status_t json_list_insert(void** pNewNode, json_generic_list_t list, void* afterNode, json_size_t objSize);
json_status_t json_list_move(json_generic_list_t list, void* element, void* afterElement, json_u32_t elementsToMove);
json_status_t json_list_splice(json_generic_list_t fromList, json_generic_list_t toList, void* fromElement, void* afterElement, json_u32_t elementsToSplice);
json_status_t json_list_copy(json_generic_list_t* pNewList, json_generic_list_const_t oldList, void* fromElement, json_u32_t elementsToMove);
json_status_t json_list_remove(json_generic_list_t list, void* fromElement, json_u32_t elementsToMove);
json_status_t json_list_merge(json_generic_list_t dstList, json_generic_list_t srcList, PFN_genericCompare pfnCompare);
json_status_t json_list_sort(json_generic_list_t list, PFN_genericCompare pfnCompare);

void*         json_list_front(json_generic_list_const_t list);
void*         json_list_back(json_generic_list_const_t list);
json_u32_t    json_list_length(json_generic_list_const_t list);*/

// json_interval_tree_t
/*
 typedef json_bool_t(*PFN_genericCompare)(const void*, const void*);
 typedef struct json_interval_tree*          json_interval_tree_t;
 typedef struct json_address_interval{
  void* min;
  void* max;
}              json_address_interval;
typedef struct json_interval_tree_node*                json_interval_tree_node_t;
typedef enum   json_interval_intersection_kind{
  JSON_INTERVAL_STRICTLY_GREATER_THAN  = 0x0,
  JSON_INTERVAL_PARTIALLY_GREATER_THAN = 0x4,
  JSON_INTERVAL_SUB_INTERVAL           = 0x5,
  JSON_INTERVAL_SUPER_INTERVAL         = 0xC,
  JSON_INTERVAL_PARTIALLY_LESS_THAN    = 0xD,
  JSON_INTERVAL_STRICTLY_LESS_THAN     = 0xF,

  JSON_INTERVAL_AY_BY_FLAG = 0x1,
  JSON_INTERVAL_AY_BX_FLAG = 0x2,
  JSON_INTERVAL_AX_BY_FLAG = 0x4,
  JSON_INTERVAL_AX_BX_FLAG = 0x8
}    json_interval_intersection_kind;
typedef enum   json_interval_tree_node_color{
  JSON_NODE_RED,
  JSON_NODE_BLACK
}      json_interval_tree_node_color;
typedef enum   json_interval_tree_node_position{
  JSON_NODE_LEFT,
  JSON_NODE_RIGHT
}   json_interval_tree_node_position;
typedef enum   json_interval_tree_node_id{
  JSON_INTERVAL_NODE_RED    = 0x0,
  JSON_INTERVAL_NODE_BLACK  = 0x1,
  JSON_INTERVAL_NODE_LEFT   = 0x0,
  JSON_INTERVAL_NODE_RIGHT  = 0x2,
  JSON_INTERVAL_NODE_LEAF   = 0x0,
  JSON_INTERVAL_NODE_BRANCH = 0x4
}         json_interval_tree_node_id;

struct json_interval_tree_node{
  json_address_interval      interval;
  json_i32_t                 leftNode;
  json_i32_t                 rightNode;
  json_interval_tree_node_id id;
};
typedef struct json_interval_tree{
  json_interval_tree_node_t root;
  json_fixed_size_allocator nodeAllocator;
  json_u32_t                blackDepth;
} json_interval_tree;
typedef json_interval_tree*            json_interval_tree_t;*/


JSON_END_C_NAMESPACE




#endif//VALKYRIE_JSONMM_H
