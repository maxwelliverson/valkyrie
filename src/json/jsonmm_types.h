//
// Created by Maxwell on 2021-02-18.
//

#ifndef VALKYRIE_JSONMM_TYPES_H
#define VALKYRIE_JSONMM_TYPES_H


#include <json/core.h>

/**
 * Default size of the virtual memory space used by the primary document allocator.
 *
 * Equal to 4GB
 *
 * */
#define JSON_ALLOCATOR_VIRTUAL_MEMORY_SPACE_SIZE (0x1ULL << 31)

/**
 * Default size of the virtual pages used by string pools.
 *
 * Equal to 64KB
 *
 * */
#define JSON_VIRTUAL_PAGE_SIZE (0x1ULL << 15)







JSON_BEGIN_C_NAMESPACE


















JSON_END_C_NAMESPACE

#endif//VALKYRIE_JSONMM_TYPES_H
