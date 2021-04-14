//
// Created by maxwe on 2021-04-13.
//

#ifndef VALKYRIE_INTERNAL_JSON_FWD_H
#define VALKYRIE_INTERNAL_JSON_FWD_H

#include <json/core.h>


JSON_BEGIN_C_NAMESPACE


typedef struct json_symbol_registry*          json_symbol_registry_t;

typedef struct json_internal_allocator*       json_internal_allocator_t;
typedef struct json_fixed_size_allocator*     json_fixed_size_allocator_t;
typedef struct json_fixed_size_chunk*         json_chunk_t;
typedef struct json_chunk_stack*              json_chunk_stack_t;

typedef struct vpage *                        vpage_t;
typedef struct vmem_placeholder *             vmem_placeholder_t;
typedef struct vmem_node *                    vmem_node_t;
typedef struct vmem_split *                   vmem_split_t;
typedef struct vmem_manager *                 vmem_manager_t;




JSON_END_C_NAMESPACE

#endif//VALKYRIE_INTERNAL_JSON_FWD_H
