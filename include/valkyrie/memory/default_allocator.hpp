//
// Created by maxwe on 2021-03-05.
//

#ifndef VALKYRIE_MEMORY_DEFAULT_ALLOCATOR_HPP
#define VALKYRIE_MEMORY_DEFAULT_ALLOCATOR_HPP

#include "valkyrie/memory/allocators/heap_allocator.hpp"
#include "valkyrie/memory/allocators/malloc_allocator.hpp"
#include "valkyrie/memory/allocators/new_allocator.hpp"
#include "valkyrie/memory/allocators/static_allocator.hpp"
#include "virtual_memory.hpp"

namespace valkyrie{
  using default_allocator = VK_concat(VALKYRIE_DEFAULT_ALLOCATOR, _allocator);

  static_assert(allocator_c<default_allocator>);
}

#endif//VALKYRIE_MEMORY_DEFAULT_ALLOCATOR_HPP
