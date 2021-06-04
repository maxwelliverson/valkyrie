//
// Created by Maxwell on 2021-02-14.
//

#ifndef VALKYRIE_JSONMM_H
#define VALKYRIE_JSONMM_H

#include <json/core.h>
#include "internal.h"
#include "jsondefs.h"


/**
 * Small Object sizes:
 *
 *
 * Bin Count: 59
 *
 *
 *    Bin #  | sizeA | sizeB || blockSize | chunkSize (KiB)
 *  ---------|-------|-------||-----------|------------
 *        0  |     1 |     0 || 1         | 0.25
 *        1  |     2 |     0 || 2         | 0.5
 *        2  |     4 |     0 || 4         | 1
 *        3  |     8 |     0 || 8         | 2
 *        4  |    16 |     0 || 16        | 4
 *        5  |    24 |     8 || 32        | 8
 *        6  |    32 |     0 || 32        | 8
 *        7  |    40 |    24 || 64        | 16
 *        8  |    48 |    16 || 64        | 16
 *        9  |    56 |     8 || 64        | 16
 *       10  |    64 |     0 || 64        | 16
 *       11  |    72 |    56 || 128       | 32
 *       12  |    80 |    48 || 128       | 32
 *       13  |    88 |    40 || 128       | 32
 *       14  |    96 |    32 || 128       | 32
 *       15  |   104 |    24 || 128       | 32
 *       16  |   112 |    16 || 128       | 32
 *       17  |   120 |     8 || 128       | 32
 *       18  |   128 |     0 || 128       | 32
 *       19  |   144 |   112 || 256       | 64
 *       20  |   160 |    96 || 256       | 64
 *       21  |   176 |    80 || 256       | 64
 *       22  |   192 |    64 || 256       | 64
 *       23  |   208 |    48 || 256       | 64
 *       24  |   224 |    32 || 256       | 64
 *       25  |   240 |    16 || 256       | 64
 *       26  |   256 |     0 || 256       | 64
 *       27  |   288 |   224 || 512       | 128    // 128
 *       28  |   320 |   192 || 512       | 128
 *       29  |   352 |   160 || 512       | 128
 *       30  |   384 |   128 || 512       | 128
 *       31  |   416 |    96 || 512       | 128
 *       32  |   448 |    64 || 512       | 128
 *       33  |   480 |    32 || 512       | 128
 *       34  |   512 |     0 || 512       | 128
 *       35  |   576 |   448 || 1024      | 256    // 64
 *       36  |   640 |   384 || 1024      | 256
 *       37  |   704 |   320 || 1024      | 256
 *       38  |   768 |   256 || 1024      | 256
 *       39  |   832 |   192 || 1024      | 256
 *       40  |   896 |   128 || 1024      | 256
 *       41  |   960 |    64 || 1024      | 256
 *       42  |  1024 |     0 || 1024      | 256
 *       43  |  1152 |   896 || 2048      | 512    // 32
 *       44  |  1280 |   768 || 2048      | 512
 *       45  |  1408 |   640 || 2048      | 512
 *       46  |  1536 |   512 || 2048      | 512
 *       47  |  1664 |   384 || 2048      | 512
 *       48  |  1792 |   256 || 2048      | 512
 *       49  |  1920 |   128 || 2048      | 512
 *       50  |  2048 |     0 || 2048      | 512
 *       51  |  2304 |  1792 || 4096      | 1024   // 16
 *       52  |  2560 |  1536 || 4096      | 1024
 *       53  |  2816 |  1280 || 4096      | 1024
 *       54  |  3072 |  1024 || 4096      | 1024
 *       55  |  3328 |   768 || 4096      | 1024
 *       56  |  3584 |   512 || 4096      | 1024
 *       57  |  3840 |   256 || 4096      | 1024
 *       58  |  4096 |     0 || 4096      | 1024
 *
 * Other sizes?
 * 3, 5, 6, 7, 9, 10, 11, 12, 13, 14, 15, 18, 20, 22, 26, 28, 30, 36, 44, 52, 60
 *
 *
 *
 *
 *  |_00_|_01_|_02_|_03_|_ ... ... _|_62_|_63_|
 *
 *
 *
 *
 * */



JSON_BEGIN_C_NAMESPACE



typedef struct json_fixed_size_allocator*     json_fixed_size_allocator_t;



/*json_status_t  json_internal_allocator_init(json_internal_allocator_t allocator, json_size_t minSize, json_size_t maxSize, json_size_t blocksPerChunk);
json_status_t  json_internal_allocator_cleanup(json_internal_allocator_t allocator);*/



void*          json_alloc__(json_u64_t size, json_ctx_t context);
void           json_free__(void* address, json_u64_t size, json_ctx_t context);


void*          json_alloc_array__(json_u64_t count, json_u64_t sizePerObject, json_ctx_t context);
void*          json_realloc_array__(void* address, json_u64_t newCount, json_u64_t oldCount, json_u64_t size, json_ctx_t context);
void           json_free_array__(void* address, json_u64_t count, json_u64_t sizePerObject, json_ctx_t context);


json_status_t  json_get_fixed_allocator__(json_fixed_size_allocator_t* pAllocator, json_u64_t size, json_ctx_t context);
void*          json_fixed_alloc__(json_fixed_size_allocator_t allocator);
void           json_fixed_free__(void* p, json_fixed_size_allocator_t allocator);





/*json_status_t  json_fixed_allocator_init(json_fixed_size_allocator_t fixed, json_internal_allocator_t allocator, json_u32_t blockSize);
void           json_fixed_allocator_cleanup(json_fixed_size_allocator_t fixed);

void*          json_fixed_size_alloc(json_fixed_size_allocator_t allocator);
void           json_fixed_size_dealloc(void* address, json_fixed_size_allocator_t allocator);*/




// json_stack_t
/*typedef struct json_stack *          json_stack_t;
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

json_status_t json_create_stack(json_stack_t * pStack, json_internal_allocator_t alloc, const json_create_stack_params_t* pParams);
void          json_destroy_stack(json_stack_t stack, json_internal_allocator_t allocator);
json_status_t json_stack_push(json_stack_t stack, json_u32_t elementSize, json_u32_t elementCount);
json_status_t json_stack_pop(json_address_t* pResult, json_stack_t stack, json_u32_t elementSize, json_u32_t elementCount);
json_status_t json_stack_top(json_address_t* pResult, json_stack_const_t stack, json_u32_t elementSize);*/


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
