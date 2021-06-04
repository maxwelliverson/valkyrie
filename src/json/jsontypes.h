//
// Created by Maxwell on 2021-02-18.
//

#ifndef VALKYRIE_INTERNAL_JSON_TYPES_H
#define VALKYRIE_INTERNAL_JSON_TYPES_H


#include <json/core.h>
#include <json/file.h>
#include <json/value.h>
#include <json/parser.h>

#include "jsondefs.h"
#include "internal_fwd.h"

#include <stddef.h>


JSON_BEGIN_C_NAMESPACE

typedef const char* json_cstring_t;

enum json_exact_type{
  JSON_EXACT_TYPE_NULL          = 0x00,
  JSON_EXACT_TYPE_FALSE         = 0x01,
  JSON_EXACT_TYPE_SYMBOL        = 0x02,
  JSON_EXACT_TYPE_INT           = 0x04,
  JSON_EXACT_TYPE_FLOAT         = 0x08,
  JSON_EXACT_TYPE_OBJECT        = 0x10,
  JSON_EXACT_TYPE_ARRAY         = 0x20,
  JSON_EXACT_TYPE_TRUE          = 0x41,
  JSON_EXACT_TYPE_INLINE_STRING = 0x42,  /**< No support yet */
  JSON_EXACT_TYPE_UINT          = 0x44,  /**< No support yet */
  JSON_EXACT_TYPE_BIG_FLOAT     = 0x48,  /**< No support yet */
  JSON_EXACT_TYPE_STRING_REF    = 0x82,  /**< No support yet */
  JSON_EXACT_TYPE_BIG_INT       = 0x84,  /**< No support yet */


  JSON_EXACT_TYPE_BOOLEAN_BIT   = 0x01,
  JSON_EXACT_TYPE_STRING_BIT    = 0x02,
  JSON_EXACT_TYPE_INTEGER_BIT   = 0x04,
  JSON_EXACT_TYPE_FLOAT_BIT     = 0x08,

  JSON_EXACT_TYPE_NUMBER_BITS   = JSON_EXACT_TYPE_INTEGER_BIT | JSON_EXACT_TYPE_FLOAT_BIT
};
typedef json_u8_t json_exact_type_t;


struct json_value{
  union{
    struct {
      JSON_alignas(JSON_GENERIC_VALUE_ALIGNMENT) json_byte_t padding[JSON_GENERIC_VALUE_SIZE - sizeof(json_exact_type_t)];
      json_exact_type_t type;
    };
    json_array_t       array;
    json_object_t      object;
    json_symbol_t      symbol;
    json_char_t        inlineString[JSON_GENERIC_VALUE_SIZE - sizeof(json_exact_type_t)];
    json_string_ref_t  stringRef;
    json_u64_t         u64;
    json_i64_t         i64;
    json_f64_t         f64;
    json_big_integer_t bigInt;
    json_big_float_t   bigFloat;
  };
};

struct json_object_member{
  struct json_value              value;
  json_u32_t                     length;
  json_char_t                    data[];
};
struct json_object{
  json_u16_t                     unused;
  json_bool_t                    isOrdered;
  json_u8_t                      alsoUnused;
  json_u32_t                     numItems;
  json_u32_t                     numBuckets;
  json_u32_t                     numTombstones;
  struct json_object_member**    memberTable;
};

/*_Static_assert(offsetof(struct json_object_member, data) == 20, "");
_Static_assert(sizeof(struct json_value) == JSON_GENERIC_VALUE_SIZE, "");
_Static_assert(sizeof(struct json_object) == 24, "");*/

struct json_ordered_object_member_node{
  struct json_ordered_object_member_node* prev;
  struct json_ordered_object_member_node* next;
};
struct json_ordered_object_member{
  struct json_ordered_object_member_node node;
  struct json_object_member              member;
};
struct json_ordered_object{
  struct json_ordered_object_member_node head;
  struct json_object object;
};

struct json_symbol{
  json_u32_t         length;
  json_u32_t         refCount;
  json_char_t        data[];
};
struct json_symbol_registry{
  struct json_symbol**      symbolTable;
  json_internal_allocator_t allocator;
  json_u32_t                numBuckets;
  json_u32_t                numItems;
  json_u32_t                numTombstones;
};

struct json_fixed_size_chunk {
  unsigned char* data;
  unsigned char  firstAvailableBlock;
  unsigned char  blocksAvailable;
};


struct json_pool_chunk{
  unsigned char* data;
  struct {
    json_u32_t offsetInBlock       : 16;
    json_u32_t firstAvailableBlock : 8;
    json_u32_t blocksAvailable     : 8;
  } info[2];
};
struct json_pool_node{
  struct json_pool_chunk* chunk;
  struct json_pool_node*  prev;
  struct json_pool_node*  next;
  json_u32_t              chunkTotalSizeLog2;
  json_u16_t              blockSize;
  json_bool_t             isPrimary;
  json_u8_t               blockSizeLog2;
};
struct json_pool_bin{
  struct json_pool_node  nodes;
  struct json_pool_node* allocNode;
  struct json_pool_node* deallocNode;
  struct json_pool_node* emptyNode;
  json_bool_t            initialized;
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

struct json_object_pool{
  json_fixed_size_allocator_t fixedAllocator;

};




struct vpage {
  json_byte_t data[];
};

enum vmem_node_state{
  JSON_VMEM_NODE_EMPTY,
  JSON_VMEM_NODE_IN_USE_FREE_LIST,
  JSON_VMEM_NODE_SPLIT,
  JSON_VMEM_NODE_SPLIT_LEFT_IN_USE,
  JSON_VMEM_NODE_SPLIT_RIGHT_IN_USE,
  JSON_VMEM_NODE_SPLIT_FULL
};

struct vmem_node {
  //json_u32_t               liveAllocationCount;
  json_u32_t         order   : 6;
  json_u32_t         state   : 4;
  vmem_placeholder_t address;
  struct vmem_node*  parent;
  struct vmem_node_pair* children;
};
struct vmem_node_pair{
  struct vmem_node node[2];
};

struct vmem_node_pair_chunk{
  json_u32_t* chunkMemory;
  json_u32_t  nextAvailableBlock;
  json_u32_t  blocksAvailable;
};
struct vmem_node_pair_chunk_stack{
  struct vmem_node_pair_chunk* startAddr;
  struct vmem_node_pair_chunk* topAddr;
  struct vmem_node_pair_chunk* endAddr;
};

struct vmem_node_pair_node_pool{
  struct vmem_node_pair_chunk_stack chunkStack;
  struct vmem_node_pair_chunk*      allocChunk;
  struct vmem_node_pair_chunk*      deallocChunk;
  struct vmem_node_pair_chunk*      emptyChunk;
};

struct vmem_manager {

  struct vmem_node                 root;
  struct json_fixed_size_allocator splitNodePool;
  json_u32_t                       maxOrder;

  struct vmem_node*                allocRegion;
  struct vmem_node*                allocSegment;
  struct vmem_node*                deallocRegion;
  struct vmem_node*                deallocSegment;


  json_u32_t                       pageSize;
  json_u32_t                       regionSize;
  json_u32_t                       segmentSize;

  /*vmem_node_t allocPage;
  vmem_node_t deallocPage;
  vmem_node_t emptyPage;*/
};

struct json_free_list{

};


struct json_internal_allocator{
  struct vmem_manager     vmemManager;
  struct json_free_list** freeLists;
  struct json_pool_bin    fixedSizePoolBins[JSON_FIXED_SIZE_ALLOCATOR_COUNT];
  json_address_t          initialAddress;
  json_u64_t              addressSpaceSize;
};


struct json_document{
  json_ctx_t         context;
  json_symbol_t      name;
  struct json_object rootObject;
};








struct json_stream{

  json_stream_flags_t flags;


  json_stream_interface_t callbacks;
};


struct json_writer{
  json_u32_t     currentIndentationLevel;
  json_symbol_t  indentString;
};

struct json_parser{
  json_ctx_t          context;
  json_parse_status_t status;
  json_u64_t          errorOffset;

  json_callbacks_t    callbacks;
};



struct json_file{
  json_ctx_t         context;
  json_file_native_t nativeHandle;
  json_u64_t         maxSize;
};


typedef struct json_standard_ctx {

  struct json_internal_allocator allocator;
  struct json_symbol_registry    symbolRegistry;

  json_object_pool      objectPool;



} json_standard_ctx;
typedef struct json_async_ctx {

} json_async_ctx;






typedef struct json_ctx_vtable_record{
  json_status_t(* const setValueSetValue)(void*, json_value_t, const void*, json_exact_type_t);
} json_ctx_vtable_record;
typedef const  json_ctx_vtable_record*  json_ctx_vtable_t;

extern const json_ctx_vtable_record json_standard_ctx_vtable;
extern const json_ctx_vtable_record json_async_ctx_vtable;


typedef struct json_ctx_backend{
  json_ctx_vtable_t vtable;
  json_ctx_flags_t  flags;
  union{
    json_async_ctx    asyncCtx;
    json_standard_ctx standardCtx;
  };
} json_ctx_backend_t;


typedef struct json_ctx{
  json_ctx_backend_t* backend;
  json_
} json_ctx;




inline static json_bool_t symbol_registry_is_empty(const struct json_symbol_registry* registry) {
  return registry->numItems == 0;
}



JSON_END_C_NAMESPACE

#endif //VALKYRIE_INTERNAL_JSON_TYPES_H
