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
typedef unsigned char json_exact_type_t;


struct json_value{
  union{
    struct {
      json_byte_t       padding[JSON_GENERIC_VALUE_SIZE - sizeof(json_exact_type_t)];
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
struct json_object_member_node{
  struct json_object_member_node* prev;
  struct json_object_member_node* next;
};
struct json_object_member{
  struct json_object_member_node node;
  struct json_value              value;
  json_u32_t                     length;
  json_char_t                    data[];
};
struct json_object{
  json_u32_t                     numItems;
  json_u32_t                     numBuckets;
  json_u32_t                     numTombstones;
  struct json_object_member**    memberTable;
  struct json_object_member_node header;
  json_internal_allocator_t      allocator;
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
  //json_u32_t                itemSize;
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
  /*struct {
    json_u16_t offsetInBlock;
    json_u8_t  firstAvailableBlock;
    json_u8_t  blocksAvailable;
  } info[2];*/
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

typedef struct json_object_pool{
  struct json_fixed_size_allocator fixedAllocator;

} json_object_pool;




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


struct json_document{
  json_object_pool   objectPool;
  struct json_object rootObject;
};








struct json_stream{

  json_stream_flags_t flags;


  json_stream_interface_t callbacks;
};


struct json_writer{
  json_u32_t     currentIndentationLevel;
  json_cstring_t indentString;


};

struct json_parser{

  json_parse_status_t status;
  json_u64_t          errorOffset;

  json_callbacks_t    callbacks;
};



struct json_file{
  json_file_native_t nativeHandle;
  json_u64_t         maxSize;
};







inline static json_bool_t symbol_registry_is_empty(const struct json_symbol_registry* registry) {
  return registry->numItems == 0;
}



JSON_END_C_NAMESPACE

#endif //VALKYRIE_INTERNAL_JSON_TYPES_H
