//
// Created by Maxwell on 2021-02-14.
//

#ifndef VALKYRIE_JSONMM_H
#define VALKYRIE_JSONMM_H

#include "jsonmm_types.h"


JSON_BEGIN_C_NAMESPACE




json_status_t json_internal_allocator_init(json_internal_allocator_t allocator, json_size_t minSize, json_size_t maxSize, json_size_t blocksPerChunk);
json_status_t json_internal_allocator_cleanup(json_internal_allocator_t allocator);


json_status_t json_internal_alloc(json_address_t* pResult, json_internal_allocator_t allocator, json_u64_t size);
json_status_t json_internal_realloc(json_address_t* pAddress, json_bool_t* pMoved, json_internal_allocator_t allocator, json_u64_t newSize, json_u64_t oldSize);
json_status_t json_internal_free(json_internal_allocator_t allocator, json_address_t address, json_u64_t size);

json_status_t  json_fixed_allocator_init(json_fixed_size_allocator_t fixed, json_internal_allocator_t chunkAlloc, json_u32_t blockSize, json_u32_t blocks);
void           json_fixed_allocator_cleanup(json_fixed_size_allocator_t fixed);
json_address_t json_fixed_alloc(json_fixed_size_allocator_t allocator);
void           json_fixed_dealloc(json_address_t address, json_fixed_size_allocator_t allocator);


/**
 * Creates a new stack with the given parameters.
 *
 * @param [out] pStack Pointer to the address in which the stack handle will be returned
 * @param [in] pParams Optional parameters. Can be null.
 * */
json_status_t json_create_stack(json_generic_stack_t* pStack, json_internal_allocator_t alloc, const json_create_stack_params_t* pParams);
void          json_destroy_stack(json_generic_stack_t stack, json_internal_allocator_t allocator);
json_status_t json_stack_push(json_generic_stack_t stack, json_u32_t elementSize, json_u32_t elementCount);
json_status_t json_stack_pop(json_address_t* pResult, json_generic_stack_t stack, json_u32_t elementSize, json_u32_t elementCount);
json_status_t json_stack_top(json_address_t* pResult, json_generic_stack_t stack, json_u32_t elementSize);


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

void*         json_list_front(json_generic_list_const_t list);
void*         json_list_back(json_generic_list_const_t list);
json_u32_t    json_list_length(json_generic_list_const_t list);





json_status_t json_create_array(json_generic_array_t* pArray, const json_create_array_params_t* pParams);
void          json_destroy_array(json_generic_array_t array);



JSON_END_C_NAMESPACE




#endif//VALKYRIE_JSONMM_H
