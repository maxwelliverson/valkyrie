//
// Created by Maxwell on 2021-02-14.
//

#include <json/json.h>

#include "jsonmm.h"


#include <assert.h>
#include <string.h>
#include <intrin.h>
#include <stdint.h>

#pragma intrinsic(__lzcnt, __lzcnt64/*, __ll_lshift, __ll_rshift, __ull_rshift*/)

/*struct json_hetero_slist_entry__{
  json_i32_t  nextOffset;
  json_u32_t  dataSize;
  json_byte_t data[];
};
struct json_hetero_dlist_entry__{
  json_i32_t  nextOffset;
  json_i32_t  prevOffset;
  json_u64_t  dataSize;
  json_byte_t data[];
};
struct json_slist_entry__{
  json_i64_t  nextOffset;
  json_byte_t data[];
};
struct json_dlist_entry__{
  json_i32_t  nextOffset;
  json_i32_t  prevOffset;
  json_byte_t data[];
};*/



/*typedef json_status_t (* PFN_doListInsert)(void** pNewNode, json_generic_list_t list, void* afterNode, json_size_t objSize);
typedef json_status_t (* PFN_doListMove)(json_generic_list_t list, void* element, void* afterElement, json_u32_t elementsToMove);
typedef json_status_t (* PFN_doListSplice)(json_generic_list_t fromList, json_generic_list_t toList, void* fromElement, void* afterElement, json_u32_t elementsToSplice);
typedef json_status_t (* PFN_doListCopy)(json_generic_list_t* pNewList, json_generic_list_const_t oldList, void* fromElement, json_u32_t elementsToMove);
typedef json_status_t (* PFN_doListRemove)(json_generic_list_t list, void* fromElement, json_u32_t elementsToMove);
typedef json_status_t (* PFN_doListMerge)(json_generic_list_t dstList, json_generic_list_t srcList, PFN_genericCompare pfnCompare);
typedef json_status_t (* PFN_doListSort)(json_generic_list_t list, PFN_genericCompare pfnCompare);

typedef void*         (* PFN_doListFront)(json_generic_list_const_t list);
typedef void*         (* PFN_doListBack)(json_generic_list_const_t list);
typedef json_u32_t    (* PFN_doListLength)(json_generic_list_const_t list);*/
/*typedef struct json_generic_list_vtable{
  PFN_doListInsert insert;
  PFN_doListMove   move;
  PFN_doListSplice splice;
  PFN_doListCopy   copy;
  PFN_doListRemove remove;
  PFN_doListMerge  merge;
  PFN_doListSort   sort;
  PFN_doListFront  front;
  PFN_doListBack   back;
  PFN_doListLength length;
} json_generic_list_vtable;
typedef const json_generic_list_vtable* json_generic_list_vtable_t;*/

/*typedef void*(* PFN_doAlloc)  (json_ctx_t alloc,            json_u64_t size);
typedef void*(* PFN_doRealloc)(json_ctx_t alloc, void* ptr, json_u64_t oldSize, json_u64_t newSize);
typedef void (* PFN_doFree)   (json_ctx_t alloc, void* ptr, json_u64_t size);
typedef struct json_allocator_vtable{
  PFN_doAlloc   pfnDoAlloc;
  PFN_doRealloc pfnDoRealloc;
  PFN_doFree    pfnDoFree;
} json_allocator_vtable;
typedef  const json_allocator_vtable*     json_allocator_vtable_t;


typedef struct json_ctx{

  json_allocator_vtable vtable;

  json_address_t        initial_virtual_address;
  json_u64_t            virtual_address_space_size;

  json_object_pool      object_pool;
  json_value_pool       value_pool;
  json_string_pool      string_pool;

} json_ctx;
typedef struct json_traced_context{
  json_ctx                  base;

  PFN_json_alloc_notification   pfnAllocNotif;
  PFN_json_realloc_notification pfnReallocNotif;
  PFN_json_free_notification    pfnFreeNotif;

  void*                         pUserData;
} json_traced_context;*/


/*struct json_virtual_page{
  json_byte_t data[];
};

typedef struct json_virtual_page_desc{
  json_virtual_page_t pPage;

} json_virtual_page_desc;

struct json_virtual_page_manager{
  json_generic_stack      pageStack;

  json_virtual_page_desc* pAllocPage;
  json_virtual_page_desc* pDeallocPage;
  json_virtual_page_desc* pEmptyPage;
};*/



#define IS_IN_CHUNK(p, chunk, blockSize, blocks) ((typeof(p))(chunk)->data < (p) && (p) < (typeof(p))((chunk)->data + ((blocks) * (blockSize))))

#define U64_HIGHEST_BIT ~((~0ULL) >> 1)


#define UB UINT32_MAX


typedef struct json_per_chunk_info{
  json_u32_t blockSize;
  json_u32_t atomSizeLog2;
  json_u32_t blockCount;
  json_u32_t totalAllocSize;
} json_per_chunk_info;

static const json_per_chunk_info per_chunk_info_table__[JSON_FIXED_SIZE_ALLOCATOR_COUNT] = {
    { .blockSize = 1,   .atomSizeLog2 = 0, .blockCount = 256, .totalAllocSize = 256   },    // 0
    { .blockSize = 2,   .atomSizeLog2 = 1, .blockCount = 256, .totalAllocSize = 512   },    // 1
    { .blockSize = 4,   .atomSizeLog2 = 2, .blockCount = 256, .totalAllocSize = 1024  },    // 2
    { .blockSize = 8,   .atomSizeLog2 = 3, .blockCount = 256, .totalAllocSize = 2048  },    // 3

    { .blockSize = 16,  .atomSizeLog2 = 4, .blockCount = 256, .totalAllocSize = 4096  },    // 4
    { .blockSize = 24,  .atomSizeLog2 = 3, .blockCount =  85, .totalAllocSize = 2048  },    // 5

    { .blockSize = 32,  .atomSizeLog2 = 5, .blockCount = 256, .totalAllocSize = 8192  },    // 6
    { .blockSize = 40,  .atomSizeLog2 = 3, .blockCount =  51, .totalAllocSize = 2048  },    // 7
    { .blockSize = 48,  .atomSizeLog2 = 4, .blockCount =  85, .totalAllocSize = 4096  },    // 8

    { .blockSize = 64,  .atomSizeLog2 = 6, .blockCount = 256, .totalAllocSize = 16384 },    // 9
    { .blockSize = 80,  .atomSizeLog2 = 4, .blockCount =  51, .totalAllocSize = 4096  },    // 10
    { .blockSize = 96,  .atomSizeLog2 = 5, .blockCount =  85, .totalAllocSize = 8192  },    // 11
  //{ .blockSize = 112, .atomSizeLog2 = 0, .blockCount = 146, .totalAllocSize = 16384 },

    { .blockSize = 128, .atomSizeLog2 = 7, .blockCount = 256, .totalAllocSize = 32768 },    // 12
    //{ .blockSize = 144, .atomSizeLog2 = 0, .blockCount = 227, .totalAllocSize = 32768 },
    { .blockSize = 160, .atomSizeLog2 = 5, .blockCount =  51, .totalAllocSize = 8192  },    // 13
    //{ .blockSize = 176, .atomSizeLog2 = 0, .blockCount = 186, .totalAllocSize = 32768 },
    { .blockSize = 192, .atomSizeLog2 = 6, .blockCount =  85, .totalAllocSize = 16384 },    // 14
    //{ .blockSize = 208, .atomSizeLog2 = 0, .blockCount = 157, .totalAllocSize = 32768 },
    { .blockSize = 224, .atomSizeLog2 = 5, .blockCount =  36, .totalAllocSize = 8192  },    // 15
    //{ .blockSize = 240, .atomSizeLog2 = 0, .blockCount = 136, .totalAllocSize = 32768 },

    { .blockSize = 256, .atomSizeLog2 = 8, .blockCount = 256, .totalAllocSize = 65536 }    // 16
};

static const json_u32_t fixed_size_allocator_index_table__[JSON_SMALL_OBJECT_MAX_SIZE] = {
     0,  1, UB,  2, UB, UB, UB,  3,  4,  4,  4,  4,  4,  4,  4,  4,
     5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,
     7,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  8,
     9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
     11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
     14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
     14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
     15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
     15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
     16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
     16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16/*,
    17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
    18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
    19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
    20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
    21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21*/
};


#undef UB



inline static json_u32_t         json_small_obj_get_index__(json_u64_t sz) {
  assert(sz <= 256);
  return fixed_size_allocator_index_table__[sz - 1];
}


inline static json_bool_t        json_is_in_chunk__(void* addr, const struct json_fixed_size_chunk* chunk, json_u32_t blockSize, json_u32_t blocks){
  return (chunk->data <= (unsigned char*)addr) && ((unsigned char*)addr < (chunk->data + (blockSize * blocks)));
}
inline static json_u64_t         json_align_to__(json_u64_t value, json_u64_t align){
  const json_u64_t alignMask = align - 1;
  assert(!(align & alignMask)); // ensure alignment is a power of 2
  return value & alignMask ? (value | alignMask) + 1 : value;
}

inline static json_u64_t         json_bit_ceil__(json_u64_t value) {



  /*
     json_u64_t tmp = value - 1;

     while (value & tmp) {  // L0
       tmp |= value;        // L1
       tmp |= tmp >> 1;     // L2
       value = tmp + 1;     // L3
     }
     return value;
   *
   *
   *
   *
   * value =     01100010 10100100
   * tmp   =     01100010 10100011
   *
   * L0 while    01100010 10100000
   * L1 tmp      01100010 10100111
   *    tmp>>1   00110001 01010011
   * L2 tmp      01110011 11110111
   * L3 value    01110011 11111000
   * L0 while    01110011 11110000
   * L1 tmp      01110011 11111111
   *    tmp>>1   00111001 11111111
   * L2 tmp      01111011 11111111
   * L3 value    01111100 00000000
   * L0 while    01111000 00000000
   * L1 tmp      01111111 11111111
   *    tmp>>1   00111111 11111111
   * L2 tmp      01111111 11111111
   * L3 value    10000000 00000000
   * */

  if (!(value & (value - 1)))
    return value;
  return __ull_rshift(U64_HIGHEST_BIT, (json_i32_t)(__lzcnt64(value) - 1));
}
inline static json_u64_t         json_pow2_factor__(json_u64_t value) {
  return ((value ^ (value - 1)) + 1) >> 1;
}
inline static json_u32_t         json_ceil_log2__(json_u64_t value) {
  assert(value != 0);
  return 64 - __lzcnt64(value);

  /*
   * 00000010 10101100 00111010 10110100
   * 32 - 6
   * */
}
inline static json_u64_t         json_fixed_alloc_index__(json_u64_t size, json_u64_t minSize, json_u64_t maxSize) {

}

/*
 * 0000 1000 1101 0000 -> 2256 ::  offset
 * 0000 0000 0011 0000 -> 48   ::  blockSize
 * 0000 0000 0010 1111 -> 47   ::  offset / blockSize
 * 0000 0000 1000 1101 -> 141  ::
 *
 *
 *
 * */

inline static json_status_t      json_chunk_init__(json_chunk_t chunk, json_fixed_size_allocator_t alloc) {

  json_status_t result;

  assert((alloc->blocks * alloc->blockSize) <= alloc->chunkTotalSize);
  assert(alloc->blocks <= 256);

  chunk->data = json_internal_alloc(&result, alloc->chunkTotalSize, alloc->allocator);

  if (result != JSON_SUCCESS)
    goto exit;


  chunk->firstAvailableBlock = 0;
  chunk->blocksAvailable     = alloc->blocks;

  unsigned char i = 0;
  unsigned char* p = chunk->data;
  const json_u32_t blockSize      = alloc->blockSize;
  const json_u32_t blocks         = alloc->blocks;
  //const json_u32_t blockAtomSize  = 0x1 << alloc->blockAtomSizeLog2;
  const json_u32_t blockAtomRatio = blockSize >> alloc->blockAtomSizeLog2;

  for (; i < blocks; p += blockSize)
    *p = ++i * blockAtomRatio;


exit:
  return result;
}
inline static void*              json_chunk_alloc__(json_chunk_t chunk, json_u32_t blockAtomSizeLog2) {
  if (!chunk->blocksAvailable)
    return NULL;

  unsigned char* pData = chunk->data + (chunk->firstAvailableBlock << blockAtomSizeLog2);
  chunk->firstAvailableBlock = *pData;
  --chunk->blocksAvailable;
  return pData;
}
inline static void               json_chunk_dealloc__(json_chunk_t chunk, void* p, json_u32_t blockAtomSizeLog2) {
  assert(p >= (void*)chunk->data);
  unsigned char* pRelease = p;
  assert(((pRelease - chunk->data) & ((0x1 << blockAtomSizeLog2) - 1)) == 0);  // test if alignment is proper
  *pRelease = chunk->firstAvailableBlock;
  chunk->firstAvailableBlock = (pRelease - chunk->data) >> blockAtomSizeLog2;
  ++chunk->blocksAvailable;
}
inline static void               json_chunk_destroy__(json_chunk_t chunk, json_fixed_size_allocator_t fixed) {
  assert(fixed->blocks == chunk->blocksAvailable);
  json_internal_free((json_address_t)chunk->data, fixed->chunkTotalSize, fixed->allocator);
}
inline static void               json_chunk_swap__(json_chunk_t chunkA, json_chunk_t chunkB) {
  struct json_fixed_size_chunk tmp;
  memcpy(&tmp, chunkA, sizeof(struct json_fixed_size_chunk));
  memmove(chunkA, chunkB, sizeof(struct json_fixed_size_chunk));
  memcpy(chunkB, &tmp, sizeof(struct json_fixed_size_chunk));
}

inline static json_status_t      json_chunk_stack_init__(json_chunk_stack_t* chunkStack, json_internal_allocator_t alloc, json_u32_t initialCapacity) {
  json_status_t result;
  const json_u32_t reqSize = initialCapacity * sizeof(struct json_fixed_size_chunk);

  if (result == JSON_SUCCESS) {
    chunkStack->startAddr  = json_internal_alloc(&result, reqSize, alloc);
    chunkStack->topAddr    = chunkStack->startAddr;
    chunkStack->endAddr    = chunkStack->startAddr + (initialCapacity + 1);
  }

  return result;
}
inline static void               json_chunk_stack_cleanup__(json_chunk_stack_t* chunkStack, json_internal_allocator_t allocator) {
  assert( chunkStack->startAddr == chunkStack->topAddr );
  const json_u32_t stackSizeBytes = chunkStack->endAddr - chunkStack->startAddr;
  json_internal_free(chunkStack->startAddr, stackSizeBytes, allocator);
}
inline static json_status_t      json_chunk_stack_push__(json_chunk_stack_t* chunkStack, json_internal_allocator_t allocator) {

  const json_u64_t oldCapacity = chunkStack->endAddr - chunkStack->startAddr;
  //const json_u32_t newSize = json_ceil_log2__((json_u32_t)((oldSize + (json_u32_t)pushedSize) * 1.5));

  json_status_t result = JSON_SUCCESS;

  if (chunkStack->endAddr == chunkStack->topAddr) {
    const json_u64_t newCapacity  = ((oldCapacity >> 1) + 1) * 3;
    chunkStack->startAddr = json_internal_realloc(&result,
                                                  chunkStack->startAddr,
                                                  newCapacity * sizeof(struct json_fixed_size_chunk),
                                                  oldCapacity * sizeof(struct json_fixed_size_chunk),
                                                  allocator);
    if ( result != JSON_SUCCESS )
      goto exit;

    chunkStack->topAddr   = chunkStack->startAddr + oldCapacity;
    chunkStack->endAddr   = chunkStack->startAddr + newCapacity;
  }

  ++chunkStack->topAddr;

exit:
  return result;
}
inline static json_chunk_t       json_chunk_stack_pop__(json_chunk_stack_t* chunkStack) {
  assert( chunkStack );
  if ( chunkStack->topAddr == chunkStack->startAddr )
    return JSON_NULL_HANDLE;
  --chunkStack->topAddr;
  return chunkStack->topAddr;
}
inline static json_chunk_t       json_chunk_stack_top__(json_chunk_stack_t* chunkStack) {
  assert( chunkStack );
  if ( chunkStack->topAddr == chunkStack->startAddr )
    return JSON_NULL_HANDLE;
  return chunkStack->topAddr - 1;
}



/*inline static void*              default_do_alloc__  (json_ctx_t alloc,            json_u64_t size) { }
inline static void*              default_do_realloc__(json_ctx_t alloc, void* ptr, json_u64_t oldSize, json_u64_t newSize) { }
inline static void               default_do_free__   (json_ctx_t alloc, void* ptr, json_u64_t size) { }

inline static void*              custom_do_alloc__  (json_ctx_t alloc,            json_u64_t size) {
  void* result = default_do_alloc__(alloc, size);
  assert(result != NULL);
  struct json_traced_context* custom_alloc = (struct json_traced_context*)alloc;
  custom_alloc->pfnAllocNotif(custom_alloc->pUserData, result, size);
  return result;
}
inline static void*              custom_do_realloc__(json_ctx_t alloc, void* ptr, json_u64_t oldSize, json_u64_t newSize) {
  void* result = default_do_realloc__(alloc, ptr, oldSize, newSize);
  assert(result != NULL);
  struct json_traced_context* custom_alloc = (struct json_traced_context*)alloc;
  custom_alloc->pfnReallocNotif(custom_alloc->pUserData, result, ptr, newSize, oldSize);
  return result;
}
inline static void               custom_do_free__   (json_ctx_t alloc, void* ptr, json_u64_t size) {
  default_do_free__(alloc, ptr, size);
  struct json_traced_context* custom_alloc = (struct json_traced_context*)alloc;
  custom_alloc->pfnFreeNotif(custom_alloc->pUserData, ptr, size);
}*/


inline static json_status_t      json_stack_init__(json_stack_t stack, const json_create_stack_params_t* pParams) {
  json_status_t result = JSON_SUCCESS;
  if (pParams->flags & JSON_STACK_BORROWED_MEMORY) {
    assert(pParams->flags & JSON_STACK_FIXED_CAPACITY);
    stack->allocator = NULL;
    stack->startAddr = pParams->address;
    stack->ownedMemory = NULL;
  } else {
    stack->allocator = pParams->allocator;
    result = json_internal_alloc(&stack->startAddr, stack->allocator, pParams->initialCapacity);
    stack->ownedMemory = stack->startAddr;
  }
  stack->topAddr   = stack->startAddr;
  stack->endAddr   = stack->startAddr + pParams->initialCapacity;
  stack->isFixedSize = (json_bool_t)(pParams->flags & JSON_STACK_FIXED_CAPACITY);
  return result;
}
inline static void               json_stack_destroy__(json_stack_t stack) {
  assert(stack->topAddr == stack->startAddr); // There are some legitimate circumstances where you might want to destroy a stack that still has items in it but better safe than sorry here...
  json_internal_free(stack->allocator, stack->startAddr, stack->endAddr - stack->startAddr);
}

inline static json_status_t      json_fixed_push_chunk__(json_fixed_size_allocator_t allocator) {
  struct json_fixed_size_chunk * currentChunk;
  json_status_t                  result;
  result = json_chunk_stack_push__(&allocator->chunkStack, allocator->allocator);
  if ( result != JSON_SUCCESS )
    goto exit;
  //json_status_t pushResult = json_stack_push(&allocator->chunks, sizeof(struct json_fixed_size_chunk), 1);
  currentChunk = json_chunk_stack_top__(&allocator->chunkStack);
  result = json_chunk_init__(currentChunk, allocator);
  if (result != JSON_SUCCESS)
    goto exit;

  allocator->allocChunk   = currentChunk;
  allocator->deallocChunk = allocator->chunkStack.startAddr;

exit:
  return result;
}
inline static json_chunk_t       json_find_dealloc_chunk__(void* addr, json_fixed_size_allocator_t allocator) {
  json_chunk_t lo, hi;
  const struct json_fixed_size_chunk * loBound, *hiBound;
  lo      = allocator->deallocChunk;
  hi      = allocator->deallocChunk + 1;
  loBound = allocator->chunkStack.startAddr;
  hiBound = allocator->chunkStack.topAddr;

  if (hi == hiBound)
    hi = NULL;
  for (;;) {

    if (lo) {
      if (json_is_in_chunk__(addr, lo, allocator->blockSize, allocator->blocks))
        return lo;
      if (lo == loBound) {
        lo = NULL;
        if (!hi)
          break;
      } else
        --lo;
    }
    if (hi) {
      if (json_is_in_chunk__(addr, hi, allocator->blockSize, allocator->blocks))
        return hi;
      if (++hi == hiBound) {
        hi = NULL;
        if (!lo)
          break;
      }
    }
  }

  assert(0 && "Attempted to deallocate a pointer not obtained from this fixed size allocator");
  return NULL;
}

inline static json_status_t      json_fixed_allocator_init_from_info__(json_fixed_size_allocator_t fixed, json_internal_allocator_t allocator, json_u32_t index) {
  assert(allocator);
  assert(fixed);

  json_status_t result;
  const json_per_chunk_info* const chunkInfo = per_chunk_info_table__ + index;

  result = json_chunk_stack_init__(&fixed->chunkStack, allocator, 2);
  if (result != JSON_SUCCESS)
    goto exit;

  fixed->blockSize         = chunkInfo->blockSize;
  fixed->blocks            = chunkInfo->blockCount;
  fixed->chunkTotalSize    = chunkInfo->totalAllocSize;
  fixed->blockAtomSizeLog2 = chunkInfo->atomSizeLog2;

  result = json_fixed_push_chunk__(fixed);
  if (result != JSON_SUCCESS) {
    json_chunk_stack_cleanup__(&fixed->chunkStack, allocator);
    goto exit;
  }

  fixed->allocator   = allocator;
  fixed->emptyChunk  = fixed->allocChunk;
  fixed->initialized = JSON_TRUE;

  exit:
  return result;
}




json_status_t      json_fixed_allocator_init(json_fixed_size_allocator_t fixed, json_internal_allocator_t allocator, json_u32_t blockSize) {
  assert(allocator);
  assert(fixed);



  /*assert(!(blockSize & (blockSize - 1)));
  assert(blocks < 256);*/

  /*struct json_create_stack_params stackParams = {
      .flags           = JSON_STACK_DEFAULT_FLAGS,
      .allocator       = chunkAlloc,
      .initialCapacity = 256,
      .maxSize         = JSON_DONT_CARE
  };*/

  json_status_t result;









  result = json_chunk_stack_init__(&fixed->chunkStack, allocator, 2);
  if (result != JSON_SUCCESS)
    goto exit;

  const json_u64_t pow2AtomSize = json_pow2_factor__(blockSize);
  fixed->chunkTotalSize         = pow2AtomSize * JSON_SMALL_OBJECT_MAX_SIZE;
  fixed->blockAtomSizeLog2      = json_ceil_log2__(pow2AtomSize);
  fixed->blocks                 = JSON_SMALL_OBJECT_MAX_SIZE / (blockSize >> fixed->blockAtomSizeLog2);
  fixed->blockSize              = blockSize;
  fixed->allocator              = allocator;


  result = json_fixed_push_chunk__(fixed);

  if ( result != JSON_SUCCESS ) {
    json_chunk_stack_cleanup__(&fixed->chunkStack, allocator);
    goto exit;
  }

  fixed->emptyChunk = fixed->allocChunk;
  fixed->initialized = JSON_TRUE;

exit:
  return result;
}
void               json_fixed_allocator_cleanup(json_fixed_size_allocator_t fixed) {
  /*assert(fixed->emptyChunk     == (json_fixed_chunk_t)fixed->chunks.startAddr);
  assert(fixed->chunks.topAddr == (fixed->chunks.startAddr + sizeof(struct json_fixed_chunk__)));*/


  json_chunk_t topChunk;
  json_chunk_stack_t* const chunkStack = &fixed->chunkStack;

  topChunk = json_chunk_stack_pop__(chunkStack);

  while ( topChunk != JSON_NULL_HANDLE ) {
    json_chunk_destroy__(topChunk, fixed);
    topChunk = json_chunk_stack_pop__(chunkStack);
  }


  json_chunk_stack_cleanup__(chunkStack, fixed->allocator);
}

/*inline static json_interval_tree_node_t json_interval_tree_new_node__(json_interval_tree_t tree, void* address, json_u32_t size) {
  json_interval_tree_node_t newNode = (json_interval_tree_node_t)json_fixed_alloc(&tree->nodeAllocator);
  newNode->interval.min = address;
  newNode->interval.max = ((json_address_t)address) + size;
  newNode->leftNode = NULL;
  newNode->rightNode = NULL;
  return newNode;
}
inline static void                      json_interval_tree_delete_node__(json_interval_tree_t tree, json_interval_tree_node_t node) {
  if (node->leftNode != NULL) {
    json_interval_tree_delete_node__(tree, node->leftNode);
    json_interval_tree_delete_node__(tree, node->rightNode);
  }
  json_fixed_dealloc((json_address_t)node, &tree->nodeAllocator);
}

inline static json_status_t             json_interval_tree_init__(json_interval_tree_t tree, json_internal_allocator_t allocator) {
  tree->root = NULL;
  return json_fixed_allocator_init__(&tree->nodeAllocator, allocator, sizeof(struct json_interval_tree_node), 256);
}
inline static void                      json_interval_tree_destroy__(json_interval_tree_t tree) {
  if (tree->root != NULL) {
    json_interval_tree_delete_node__(tree, tree->root);
  }
  json_fixed_allocator_cleanup__(&tree->nodeAllocator);
}

inline static json_interval_tree_node_t json_interval_tree_insert_node__(json_interval_tree_t tree, json_interval_tree_node_t node, void* address, json_u32_t size);
inline static json_interval_tree_node_t json_interval_tree_insert_node_left__(json_interval_tree_t tree, json_interval_tree_node_t node, void* address, json_u32_t size);
inline static json_interval_tree_node_t json_interval_tree_insert_node_right__(json_interval_tree_t tree, json_interval_tree_node_t node, void* address, json_u32_t size);

inline static json_interval_tree_node_t json_interval_tree_insert_node__(json_interval_tree_t tree, json_interval_tree_node_t node, void* address, json_u32_t size) {
  if (node->leftNode) {
    if (address <= node->interval.min) {
      if (node->interval.max <= address + size) {
        // do fucky stuff
      } else {

      }
    }
  } else {

  }
}
inline static json_interval_tree_node_t json_interval_tree_insert_node_left__(json_interval_tree_t tree, json_interval_tree_node_t node, void* address, json_u32_t size){}
inline static json_interval_tree_node_t json_interval_tree_insert_node_right__(json_interval_tree_t tree, json_interval_tree_node_t node, void* address, json_u32_t size){}

inline static void                      json_interval_tree_insert__(json_interval_tree_t tree, void* address, json_u32_t size) {
  json_interval_tree_node_t thisNode = tree->root;
  if (!thisNode) {
    tree->root = json_interval_tree_new_node__(tree, address, size);
    return;
  }

  tree->root = json_interval_tree_insert_node__(tree, tree->root, address, size);
}
inline static void                      json_interval_tree_remove__(json_interval_tree_t tree, void* address, json_u32_t size);
inline static json_interval_tree_node_t json_interval_tree_find__(json_interval_tree_t tree, void* address);*/




void* json_internal_alloc(json_status_t* pStatus, json_u64_t size, json_internal_allocator_t allocator) {
  //TODO: Replace temporary implementation

  assert( pStatus );

  if ( size <= JSON_SMALL_OBJECT_MAX_SIZE) {
    const json_u32_t fixedSizeIndex = json_small_obj_get_index__(size);
    assert( fixedSizeIndex < JSON_FIXED_SIZE_ALLOCATOR_COUNT );
    json_fixed_size_allocator_t fixedSizeAlloc = &allocator->smallAllocator.fixedSizeAllocators[fixedSizeIndex];
    if ( !fixedSizeAlloc->initialized ) {
      *pStatus = json_fixed_allocator_init_from_info__(fixedSizeAlloc, allocator, fixedSizeIndex);
      if ( !*pStatus )
        return NULL;
    }
    else
      *pStatus = JSON_SUCCESS;
    return json_fixed_alloc(fixedSizeAlloc);
  }
  else {
    //TODO: Implement
    *pStatus = JSON_ERROR_NOT_IMPLEMENTED;
    return NULL;
  }

  /*if (!pStatus || size == 0)
    return JSON_ERROR_INVALID_ARGUMENT;

  if (size > allocator->maxSize)
    *pResult = malloc(size);
  else {
    const json_u32_t size_lg2 = json_ceil_log2__((json_u32_t)size);
    *pResult = allocator->fixedSizeAllocators[];
  }


  //*pResult = malloc(size);
  if (*pResult)
    return JSON_SUCCESS;
  return JSON_ERROR_UNKNOWN;*/
}
void* json_internal_realloc(json_status_t * pStatus, void* address, json_u64_t newSize, json_u64_t oldSize, json_internal_allocator_t allocator) {

  /*//TODO: Replace temporary implementation

  if (!pAddress)
    return JSON_ERROR_INVALID_ARGUMENT;

  json_address_t oldAddr = *pAddress;

  *pAddress = realloc(oldAddr, newSize);

  if (!*pAddress)
    return JSON_ERROR_UNKNOWN;

  if (pMoved)
    *pMoved = (*pAddress != oldAddr);

  return JSON_SUCCESS;*/
  *pStatus = JSON_ERROR_NOT_IMPLEMENTED;
  return NULL;
}
void  json_internal_free(void* address, json_u64_t size, json_internal_allocator_t allocator) {

  if ( size <= JSON_SMALL_OBJECT_MAX_SIZE ) {
    const json_u32_t fixedSizeIndex = json_small_obj_get_index__(size);
    assert( fixedSizeIndex < JSON_FIXED_SIZE_ALLOCATOR_COUNT );
    json_fixed_size_allocator_t fixedSizeAlloc = &allocator->smallAllocator.fixedSizeAllocators[fixedSizeIndex];
    assert( fixedSizeAlloc->initialized );
    json_fixed_dealloc(address, fixedSizeAlloc);
  } else {
    //TODO: Replace temporary implementation
    //free(address);
    assert( JSON_FALSE );
  }



  //return JSON_SUCCESS;
}


void* json_fixed_alloc(json_fixed_size_allocator_t allocator) {
  if (allocator->allocChunk == NULL || allocator->allocChunk->blocksAvailable == 0) {
    struct json_fixed_size_chunk * currentChunk = allocator->chunkStack.startAddr;
    for (;; ++currentChunk) {
      if (currentChunk == allocator->chunkStack.topAddr) {
        json_status_t pushResult = json_fixed_push_chunk__(allocator);
        assert(pushResult == JSON_SUCCESS);
        allocator->allocChunk = currentChunk;
        break;
      }
      if (currentChunk->blocksAvailable > 0) {
        allocator->allocChunk = currentChunk;
        break;
      }
    }
  }
  assert(allocator->allocChunk != NULL);
  assert(allocator->allocChunk->blocksAvailable > 0);

  if (allocator->allocChunk == allocator->emptyChunk)
    allocator->emptyChunk = NULL;

  return json_chunk_alloc__(allocator->allocChunk, allocator->blockAtomSizeLog2);
}
void  json_fixed_dealloc(void* address, json_fixed_size_allocator_t allocator) {
  allocator->deallocChunk = json_find_dealloc_chunk__(address, allocator);
  assert(json_is_in_chunk__(address, allocator->deallocChunk, allocator->blockSize, allocator->blocks));
  json_chunk_dealloc__(allocator->deallocChunk, address, allocator->blockAtomSizeLog2);

  if (allocator->deallocChunk->blocksAvailable == allocator->blocks) {
    if (allocator->emptyChunk) {
      assert(allocator->chunkStack.startAddr != allocator->chunkStack.topAddr);

      json_chunk_t lastChunk = json_chunk_stack_pop__(&allocator->chunkStack);
      if (lastChunk == allocator->deallocChunk)
        allocator->deallocChunk = allocator->emptyChunk;
      else if (lastChunk != allocator->emptyChunk)
        json_chunk_swap__(allocator->emptyChunk, lastChunk);
      json_chunk_destroy__(lastChunk, allocator);
      if (allocator->allocChunk == lastChunk || allocator->emptyChunk->blocksAvailable == 0)
        allocator->allocChunk = allocator->deallocChunk;
    }
    allocator->emptyChunk = allocator->deallocChunk;
  }
}


json_status_t json_create_stack(json_stack_t * pStack, json_internal_allocator_t allocator, const json_create_stack_params_t* pParams) {
  json_stack_t stack;
  json_status_t result;
  result = json_internal_alloc((json_address_t*)&stack, allocator, sizeof(json_stack));
  if (!result)
    return result;
  return json_stack_init__(stack, pParams);
}
void          json_destroy_stack(json_stack_t stack, json_internal_allocator_t allocator) {
  json_stack_destroy__(stack);
  json_internal_free(stack, sizeof(json_stack), allocator);
}
json_status_t json_stack_push(json_stack_t stack, json_u32_t elementSize, json_u32_t elementCount) {
  const json_u64_t pushedSize = elementCount * elementSize;
  if (pushedSize > UINT32_MAX)
    return JSON_ERROR_INVALID_ARGUMENT;
  const json_u32_t oldSize = stack->endAddr - stack->startAddr;
  const json_u32_t newSize = json_ceil_log2__((json_u32_t)((oldSize + (json_u32_t)pushedSize) * 1.5));

  if (stack->endAddr - stack->topAddr < elementSize * elementCount) {
    if (stack->isFixedSize)
      return JSON_ERROR_COLLECTION_IS_FULL;
    json_address_t oldStart = stack->startAddr;
    json_bool_t    startMoved = JSON_FALSE;
    json_status_t  reallocResult = json_internal_realloc(&stack->startAddr, &startMoved, stack->allocator, newSize, oldSize);
    if (!reallocResult)
      return reallocResult;
    if (startMoved)
      stack->topAddr += (stack->startAddr - oldStart);
    stack->endAddr = stack->startAddr + newSize;
  }
  stack->topAddr += pushedSize;
  return JSON_SUCCESS;
}
json_status_t json_stack_pop(json_address_t* pResult, json_stack_t stack, json_u32_t elementSize, json_u32_t elementCount) {
  const json_u32_t poppedSize = elementSize * elementCount;
  if (stack->topAddr - stack->startAddr < poppedSize)
    return JSON_ERROR_INVALID_ARGUMENT;
  stack->topAddr -= poppedSize;
  *pResult = stack->topAddr;
  return JSON_SUCCESS;
}
json_status_t json_stack_top(json_address_t* pResult, json_stack_const_t stack, json_u32_t elementSize) {
  if (stack->topAddr - stack->startAddr < elementSize)
    return JSON_ERROR_INVALID_ARGUMENT;
  *pResult = (stack->topAddr - elementSize);
  return JSON_SUCCESS;
}


















