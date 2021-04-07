//
// Created by Maxwell on 2021-02-14.
//

#include <json/json.h>

#include "jsonmm.h"


#include <assert.h>
#include <string.h>
#include <intrin.h>
#include <stdint.h>












struct json_hetero_slist_entry__{
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
};




typedef struct json_symbol{
  json_u32_t         refCount;
  json_u32_t         length;
  json_address_t     this_page;
  const json_utf8_t* pEntry;
} json_symbol;

typedef struct json_symbol_registry{

} json_symbol_registry;

typedef json_symbol_registry* json_symbol_registry_t;

typedef struct json_value_pool {

} json_value_pool;
typedef struct json_object_pool{

} json_object_pool;
typedef struct json_string_pool{

} json_string_pool;





typedef void*(* PFN_doAlloc)  (json_context_t alloc,            json_u64_t size);
typedef void*(* PFN_doRealloc)(json_context_t alloc, void* ptr, json_u64_t oldSize, json_u64_t newSize);
typedef void (* PFN_doFree)   (json_context_t alloc, void* ptr, json_u64_t size);

typedef json_status_t (* PFN_doListInsert)(void** pNewNode, json_generic_list_t list, void* afterNode, json_size_t objSize);
typedef json_status_t (* PFN_doListMove)(json_generic_list_t list, void* element, void* afterElement, json_u32_t elementsToMove);
typedef json_status_t (* PFN_doListSplice)(json_generic_list_t fromList, json_generic_list_t toList, void* fromElement, void* afterElement, json_u32_t elementsToSplice);
typedef json_status_t (* PFN_doListCopy)(json_generic_list_t* pNewList, json_generic_list_const_t oldList, void* fromElement, json_u32_t elementsToMove);
typedef json_status_t (* PFN_doListRemove)(json_generic_list_t list, void* fromElement, json_u32_t elementsToMove);
typedef json_status_t (* PFN_doListMerge)(json_generic_list_t dstList, json_generic_list_t srcList, PFN_genericCompare pfnCompare);
typedef json_status_t (* PFN_doListSort)(json_generic_list_t list, PFN_genericCompare pfnCompare);

typedef void*         (* PFN_doListFront)(json_generic_list_const_t list);
typedef void*         (* PFN_doListBack)(json_generic_list_const_t list);
typedef json_u32_t    (* PFN_doListLength)(json_generic_list_const_t list);


typedef struct json_allocator_vtable{
  PFN_doAlloc   pfnDoAlloc;
  PFN_doRealloc pfnDoRealloc;
  PFN_doFree    pfnDoFree;
} json_allocator_vtable;
typedef  const json_allocator_vtable*     json_allocator_vtable_t;
typedef struct json_generic_list_vtable{
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
typedef const json_generic_list_vtable* json_generic_list_vtable_t;

typedef struct json_context{

  json_allocator_vtable vtable;

  json_address_t        initial_virtual_address;
  json_u64_t            virtual_address_space_size;

  json_object_pool      object_pool;
  json_value_pool       value_pool;
  json_string_pool      string_pool;

} json_context;
typedef struct json_traced_context{
  json_context                  base;

  PFN_json_alloc_notification   pfnAllocNotif;
  PFN_json_realloc_notification pfnReallocNotif;
  PFN_json_free_notification    pfnFreeNotif;

  void*                         pUserData;
} json_traced_context;


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

typedef struct json_internal_allocator json_internal_allocator;



#define IS_IN_CHUNK(p, chunk, blockSizeLog2, blocks) ((typeof(p))(chunk)->data < (p) && (p) < (typeof(p))((chunk)->data + ((blocks) << (blockSizeLog2))))



inline static json_u32_t         json_ceil_log2__(json_u32_t value) {
  unsigned long result;
  if (!(value & (value - 1)))
    return value;
  if (!_BitScanReverse(&result, value))
    return 1;
  return 0x2 << result;
}
inline static json_u32_t         json_fixed_alloc_index__(json_u32_t size, json_u32_t minSize, json_u32_t maxSize) {
  _cr
}


inline static json_status_t      json_chunk_init__(json_fixed_chunk_t chunk, json_internal_allocator_t alloc, json_u32_t blockSizeLog2, json_u32_t blocks) {

  json_status_t result;

  assert((blocks << blockSizeLog2) <= JSON_VIRTUAL_PAGE_SIZE);
  assert(blocks <= 256);

  result = json_internal_alloc((json_address_t*)&chunk->data, alloc, blocks << blockSizeLog2);
  if (result != JSON_SUCCESS)
    return result;

  chunk->firstAvailableBlock = 0;
  chunk->blocksAvailable     = blocks;


  const json_u32_t blockSize = 0x1 << blockSizeLog2;

  unsigned char i = 0;
  unsigned char* p = chunk->data;
  for (; i < blocks; p += blockSize)
    (*(json_u32_t*)p) = ++i;

  return result;
}
inline static json_address_t     json_chunk_alloc__(json_fixed_chunk_t chunk, json_u32_t blockSizeLog2) {
  if (!chunk->blocksAvailable)
    return NULL;

  unsigned char* pData = chunk->data + (chunk->firstAvailableBlock << blockSizeLog2);
  chunk->firstAvailableBlock = *pData;
  --chunk->blocksAvailable;
  return (json_address_t)pData;
}
inline static void               json_chunk_dealloc__(json_fixed_chunk_t chunk, void* p, json_u32_t blockSizeLog2) {
  assert(p >= (void*)chunk->data);
  unsigned char* pRelease = (unsigned char*)p;
  assert(((pRelease - chunk->data) & (blockSizeLog2 - 1)) == 0);
  *pRelease = chunk->firstAvailableBlock;
  chunk->firstAvailableBlock = (pRelease - chunk->data) >> blockSizeLog2;
  ++chunk->blocksAvailable;
}
inline static void               json_chunk_destroy__(json_fixed_chunk_t chunk, json_internal_allocator_t alloc, json_u32_t blockSizeLog2, json_u32_t blocks) {
  assert(blocks == chunk->blocksAvailable);
  json_internal_free(alloc, (json_address_t)chunk->data, blocks << blockSizeLog2);
}
inline static void               json_chunk_swap__(json_fixed_chunk_t chunkA, json_fixed_chunk_t chunkB) {
  struct json_fixed_chunk__ tmp;
  memcpy(&tmp, chunkA, sizeof(struct json_fixed_chunk__));
  memmove(chunkA, chunkB, sizeof(struct json_fixed_chunk__));
  memcpy(chunkB, &tmp, sizeof(struct json_fixed_chunk__));
}

inline static void*              default_do_alloc__  (json_context_t alloc,            json_u64_t size) { }
inline static void*              default_do_realloc__(json_context_t alloc, void* ptr, json_u64_t oldSize, json_u64_t newSize) { }
inline static void               default_do_free__   (json_context_t alloc, void* ptr, json_u64_t size) { }

inline static void*              custom_do_alloc__  (json_context_t alloc,            json_u64_t size) {
  void* result = default_do_alloc__(alloc, size);
  assert(result != NULL);
  struct json_traced_context* custom_alloc = (struct json_traced_context*)alloc;
  custom_alloc->pfnAllocNotif(custom_alloc->pUserData, result, size);
  return result;
}
inline static void*              custom_do_realloc__(json_context_t alloc, void* ptr, json_u64_t oldSize, json_u64_t newSize) {
  void* result = default_do_realloc__(alloc, ptr, oldSize, newSize);
  assert(result != NULL);
  struct json_traced_context* custom_alloc = (struct json_traced_context*)alloc;
  custom_alloc->pfnReallocNotif(custom_alloc->pUserData, result, ptr, newSize, oldSize);
  return result;
}
inline static void               custom_do_free__   (json_context_t alloc, void* ptr, json_u64_t size) {
  default_do_free__(alloc, ptr, size);
  struct json_traced_context* custom_alloc = (struct json_traced_context*)alloc;
  custom_alloc->pfnFreeNotif(custom_alloc->pUserData, ptr, size);
}


inline static json_status_t      json_stack_init__(json_generic_stack_t stack, const json_create_stack_params_t* pParams) {
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
inline static void               json_stack_destroy__(json_generic_stack_t stack) {
  assert(stack->topAddr == stack->startAddr); // There are some legitimate circumstances where you might want to destroy a stack that still has items in it but better safe than sorry here...
  json_internal_free(stack->allocator, stack->startAddr, stack->endAddr - stack->startAddr);
}

inline static json_status_t      json_fixed_push_chunk__(json_fixed_size_allocator_t allocator) {
  struct json_fixed_chunk__* currentChunk;
  json_status_t pushResult = json_stack_push(&allocator->chunks, sizeof(struct json_fixed_chunk__), 1);
  if (pushResult != JSON_SUCCESS)
    return pushResult;
  currentChunk = ((struct json_fixed_chunk__*)allocator->chunks.topAddr) - 1;
  json_status_t initResult = json_chunk_init__(currentChunk, allocator->chunkAllocator, allocator->blockSizeLog2, allocator->blocks);
  if (initResult == JSON_SUCCESS) {
    allocator->allocChunk = currentChunk;
    allocator->deallocChunk = (struct json_fixed_chunk__*)allocator->chunks.startAddr;
  }
  return initResult;
}
inline static json_fixed_chunk_t json_find_dealloc_chunk__(json_address_t addr, json_fixed_size_allocator_t allocator) {
  json_fixed_chunk_t lo, hi;
  const struct json_fixed_chunk__* loBound, *hiBound;
  lo = allocator->deallocChunk;
  hi = allocator->deallocChunk + 1;
  loBound = (const struct json_fixed_chunk__*)allocator->chunks.startAddr;
  hiBound = (const struct json_fixed_chunk__*)allocator->chunks.topAddr;

  if (hi == hiBound)
    hi = NULL;
  for (;;) {

    if (lo) {
      if (IS_IN_CHUNK(addr, lo, allocator->blockSizeLog2, allocator->blocks))
        return lo;
      if (lo == loBound) {
        lo = NULL;
        if (!hi)
          break;
      } else
        --lo;
    }
    if (hi) {
      if (IS_IN_CHUNK(addr, hi, allocator->blockSizeLog2, allocator->blocks))
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



json_status_t      json_fixed_allocator_init(json_fixed_size_allocator_t fixed, json_internal_allocator_t chunkAlloc, json_u32_t blockSize, json_u32_t blocks) {
  assert(chunkAlloc);
  assert(fixed);
  assert(!(blockSize & (blockSize - 1)));
  assert(blocks < 256);

  struct json_create_stack_params stackParams = {
      .flags           = JSON_STACK_DEFAULT_FLAGS,
      .allocator       = chunkAlloc,
      .initialCapacity = 256,
      .maxSize         = JSON_DONT_CARE
  };
  json_status_t result = json_stack_init__(&fixed->chunks, &stackParams);
  if (result != JSON_SUCCESS)
    return result;

  fixed->blockSizeLog2 = json_ceil_log2__(blockSize);
  fixed->blocks        = blocks;

  result = json_fixed_push_chunk__(fixed);
  if (result != JSON_SUCCESS)
    return result;

  fixed->emptyChunk = fixed->allocChunk;
  return JSON_SUCCESS;
}
void               json_fixed_allocator_cleanup(json_fixed_size_allocator_t fixed) {
  /*assert(fixed->emptyChunk     == (json_fixed_chunk_t)fixed->chunks.startAddr);
  assert(fixed->chunks.topAddr == (fixed->chunks.startAddr + sizeof(struct json_fixed_chunk__)));*/

  json_fixed_chunk_t lastChunk;
  json_status_t popResult;

  popResult = json_stack_pop((json_address_t*)&lastChunk, &fixed->chunks, sizeof(struct json_fixed_chunk__), 1);

  while (popResult == JSON_SUCCESS) {
    json_chunk_destroy__(lastChunk, fixed->chunkAllocator, fixed->blockSizeLog2, fixed->blocks);
    popResult = json_stack_pop((json_address_t*)&lastChunk, &fixed->chunks, sizeof(struct json_fixed_chunk__), 1);
  }

  assert(popResult == JSON_ERROR_COLLECTION_IS_EMPTY);

  json_stack_destroy__(&fixed->chunks);
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




json_status_t json_internal_alloc(json_address_t* pResult, json_internal_allocator_t allocator, json_u64_t size) {
  //TODO: Replace temporary implementation

  if (!pResult || size == 0)
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
  return JSON_ERROR_UNKNOWN;
}
json_status_t json_internal_realloc(json_address_t* pAddress, json_bool_t* pMoved, json_internal_allocator_t allocator, json_u64_t newSize, json_u64_t oldSize) {

  //TODO: Replace temporary implementation

  if (!pAddress)
    return JSON_ERROR_INVALID_ARGUMENT;

  json_address_t oldAddr = *pAddress;

  *pAddress = realloc(oldAddr, newSize);

  if (!*pAddress)
    return JSON_ERROR_UNKNOWN;

  if (pMoved)
    *pMoved = (*pAddress != oldAddr);

  return JSON_SUCCESS;
}
json_status_t json_internal_free(json_internal_allocator_t allocator, json_address_t address, json_u64_t size) {
  //TODO: Replace temporary implementation
  free(address);
  return JSON_SUCCESS;
}


json_address_t json_fixed_alloc(json_fixed_size_allocator_t allocator) {
  if (allocator->allocChunk == NULL || allocator->allocChunk->blocksAvailable == 0) {
    struct json_fixed_chunk__* currentChunk = (struct json_fixed_chunk__*)allocator->chunks.startAddr;
    for (;; ++currentChunk) {
      if (currentChunk == (struct json_fixed_chunk__*)allocator->chunks.topAddr) {
        json_status_t pushResult = json_fixed_push_chunk__(allocator);
        assert(pushResult == JSON_SUCCESS);
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
  return json_chunk_alloc__(allocator->allocChunk, allocator->blockSizeLog2);
}
void           json_fixed_dealloc(json_address_t address, json_fixed_size_allocator_t allocator) {
  allocator->deallocChunk = json_find_dealloc_chunk__(address, allocator);
  assert(IS_IN_CHUNK(address, allocator->deallocChunk, allocator->blockSizeLog2, allocator->blocks));
  json_chunk_dealloc__(allocator->deallocChunk, address, allocator->blockSizeLog2);

  if (allocator->deallocChunk->blocksAvailable == allocator->blocks) {
    if (allocator->emptyChunk) {
      assert(allocator->chunks.startAddr != allocator->chunks.topAddr);

      struct json_fixed_chunk__* lastChunk;
      json_status_t popResult = json_stack_pop((json_address_t*)&lastChunk, &allocator->chunks, sizeof(struct json_fixed_chunk__), 1);
      assert(popResult == JSON_SUCCESS);

      if (lastChunk == allocator->deallocChunk)
        allocator->deallocChunk = allocator->emptyChunk;
      else if (lastChunk != allocator->emptyChunk)
        json_chunk_swap__(allocator->emptyChunk, lastChunk);
      json_chunk_destroy__(lastChunk, allocator->chunkAllocator, allocator->blockSizeLog2, allocator->blocks);
      if (allocator->allocChunk == lastChunk || allocator->emptyChunk->blocksAvailable == 0)
        allocator->allocChunk = allocator->deallocChunk;
    }
    allocator->emptyChunk = allocator->deallocChunk;
  }
}


json_status_t json_create_stack(json_generic_stack_t* pStack, json_internal_allocator_t allocator, const json_create_stack_params_t* pParams) {
  json_generic_stack_t stack;
  json_status_t result;
  result = json_internal_alloc((json_address_t*)&stack, allocator, sizeof(json_generic_stack));
  if (!result)
    return result;
  return json_stack_init__(stack, pParams);
}
void          json_destroy_stack(json_generic_stack_t stack, json_internal_allocator_t allocator) {
  json_stack_destroy__(stack);
  json_internal_free(allocator, (json_address_t)stack, sizeof(json_generic_stack));
}
json_status_t json_stack_push(json_generic_stack_t stack, json_u32_t elementSize, json_u32_t elementCount) {
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
json_status_t json_stack_pop(json_address_t* pResult, json_generic_stack_t stack, json_u32_t elementSize, json_u32_t elementCount) {
  const json_u32_t poppedSize = elementSize * elementCount;
  if (stack->topAddr - stack->startAddr < poppedSize)
    return JSON_ERROR_INVALID_ARGUMENT;
  stack->topAddr -= poppedSize;
  *pResult = stack->topAddr;
  return JSON_SUCCESS;
}
json_status_t json_stack_top(json_address_t* pResult, json_generic_stack_t stack, json_u32_t elementSize) {
  if (stack->topAddr - stack->startAddr < elementSize)
    return JSON_ERROR_INVALID_ARGUMENT;
  *pResult = (stack->topAddr - elementSize);
  return JSON_SUCCESS;
}




/**
 * Creates a new list with the given parameters.
 *
 * \param [out] pList Address where a handle to the new list will be returned
 * \param [in]  pParams
 *
 * \details If pParams is null,
 *
 * */
json_status_t json_create_list(json_generic_list_t* pList,    const json_create_list_params_t* pParams);
void          json_destroy_list(json_generic_list_t list);
/**
 * \param [in] afterNode Pointer to the value after which to insert the new node. If null, the new node is inserted at the front of the list.
 *
 * \invariant For an empty json_generic_list_t list, and any n less than or equal to the value of json_create_list_params::maxLength
 * specified during the creation of list, the following assertions hold: \code
 * void* pNewNode;
 * json_status_t insertResult;
 *
 * // inserts n objects in arbitrary positions
 *
 * insertResult = json_list_insert(&pNewNode, list, json_list_back(list), 4);
 * assert(!JSON_SUCCESS || json_list_back(list) == pNewNode);
 * insertResult = json_list_insert(&pNewNode, list, NULL, 4);
 * assert(!JSON_SUCCESS || json_list_front(list) == pNewNode);
 * \endcode
 * */
json_status_t json_list_insert(void** pNewNode, json_generic_list_t list, void* afterNode, json_size_t objSize);
json_status_t json_list_move(json_generic_list_t list, void* element, void* afterElement, json_u32_t elementsToMove);
json_status_t json_list_splice(json_generic_list_t fromList, json_generic_list_t toList, void* fromElement, void* afterElement, json_u32_t elementsToSplice);
json_status_t json_list_copy(json_generic_list_t* pNewList, json_generic_list_const_t oldList, void* fromElement, json_u32_t elementsToMove);
json_status_t json_list_remove(json_generic_list_t list, void* fromElement, json_u32_t elementsToMove);
json_status_t json_list_merge(json_generic_list_t dstList, json_generic_list_t srcList, PFN_genericCompare pfnCompare);
json_status_t json_list_sort(json_generic_list_t list, PFN_genericCompare pfnCompare);

void*         json_list_front(json_generic_list_const_t list) {

}
void*         json_list_back(json_generic_list_const_t list);
json_u32_t    json_list_length(json_generic_list_const_t list);





json_status_t json_create_array(json_generic_array_t* pArray, const json_create_array_params_t* pParams);
void          json_destroy_array(json_generic_array_t array);















json_status_t json_create_context(json_context_t* pContext, const json_context_setup_params_t* pParams) {

  if (pParams && (pParams->pfnAllocNotification || pParams->pfnReallocNotification || pParams->pfnFreeNotification)) {
    // make json_traced_context object
  } else {
    // only make json_context object
  }





  return JSON_SUCCESS;
}
json_status_t json_destroy_context(json_context_t context) {}





