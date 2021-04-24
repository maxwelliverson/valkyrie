//
// Created by maxwe on 2021-03-05.
//

#ifndef VALKYRIE_MEMORY_DEFAULT_ALLOCATOR_HPP
#define VALKYRIE_MEMORY_DEFAULT_ALLOCATOR_HPP

#include "heap_allocator.hpp"
#include "new_allocator.hpp"
#include "static_allocator.hpp"
#include "virtual_memory.hpp"
#include "malloc_allocator.hpp"

namespace valkyrie{
  using default_allocator = VK_concat(VALKYRIE_DEFAULT_ALLOCATOR, _allocator);
}

#endif//VALKYRIE_MEMORY_DEFAULT_ALLOCATOR_HPP
