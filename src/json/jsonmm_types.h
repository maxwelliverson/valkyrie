//
// Created by Maxwell on 2021-02-18.
//

#ifndef VALKYRIE_JSONMM_TYPES_H
#define VALKYRIE_JSONMM_TYPES_H

#include <json/json.h>

JSON_BEGIN_C_NAMESPACE

typedef json_bool_t(*PFN_genericCompare)(const void*, const void*);

typedef struct json_internal_allocator*     json_internal_allocator_t;
typedef struct json_fixed_size_allocator*   json_fixed_size_allocator_t;
typedef struct json_small_object_allocator* json_small_object_allocator_t;

typedef struct json_generic_stack*          json_generic_stack_t;
typedef struct json_generic_list*           json_generic_list_t;
typedef struct json_generic_array*          json_generic_array_t;
typedef struct json_interval_tree*          json_interval_tree_t;

typedef const struct json_generic_stack*    json_generic_stack_const_t;
typedef const struct json_generic_list*     json_generic_list_const_t;
typedef const struct json_generic_array*    json_generic_array_const_t;


typedef struct json_virtual_page*           json_virtual_page_t;
typedef struct json_virtual_page_manager*   json_virtual_page_manager_t;
typedef struct json_virtual_page_desc*      json_virtual_page_desc_t;



typedef enum json_stack_flags{
  JSON_STACK_DEFAULT_FLAGS   = 0x0,
  JSON_STACK_FIXED_CAPACITY  = 0x1,
  JSON_STACK_BORROWED_MEMORY = 0x2
} json_stack_flags_t;
typedef enum json_list_flags {
  JSON_LIST_DEFAULT_FLAGS   = 0x0,
  JSON_LIST_SINGLY_LINKED   = 0x1,
  JSON_LIST_HETEROGENEOUS   = 0x2
} json_list_flags_t;
typedef enum json_array_flags{
  JSON_ARRAY_DEFAULT_FLAGS   = 0x0,
  JSON_ARRAY_FIXED_CAPACITY  = 0x1,
  JSON_ARRAY_BORROWED_MEMORY = 0x2
} json_array_flags_t;

typedef struct json_create_stack_params{

  json_flags_t flags;

  union{
    json_internal_allocator_t allocator;
    json_address_t            address;
  };
  json_u64_t                  initialCapacity;
  json_u64_t                  maxSize;
} json_create_stack_params_t;
typedef struct json_create_list_params{

  json_flags_t         flags;
  json_internal_allocator_t allocator;
  json_u32_t                elementSize;

  json_u64_t                maxLength;

}  json_create_list_params_t;
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


typedef struct json_generic_stack{
  json_internal_allocator_t allocator;
  json_address_t            ownedMemory;
  json_address_t            startAddr;
  json_address_t            topAddr;
  json_address_t            endAddr;
  json_bool_t               isFixedSize;
}  json_generic_stack;

typedef struct json_fixed_chunk__{
  unsigned char* data;
  unsigned char  firstAvailableBlock;
  unsigned char  blocksAvailable;
}*         json_fixed_chunk_t;
typedef struct json_fixed_size_allocator{
  json_u32_t                 blockSizeLog2;
  json_u32_t                 blocks;
  json_generic_stack         chunks;
  json_internal_allocator_t  chunkAllocator;
  json_fixed_chunk_t         allocChunk;
  json_fixed_chunk_t         deallocChunk;
  json_fixed_chunk_t         emptyChunk;
}   json_fixed_size_allocator;
typedef struct json_internal_allocator{
  json_fixed_size_allocator fixedSizeAllocators[9]; // 1, 2, 4, 8, 16, 32, 64, 128, 256
} json_internal_allocator;


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
typedef json_interval_tree*            json_interval_tree_t;


JSON_END_C_NAMESPACE

#endif//VALKYRIE_JSONMM_TYPES_H
