//
// Created by Maxwell on 2021-01-12.
//

#ifndef VALKYRIE_ADT_STACK_HPP
#define VALKYRIE_ADT_STACK_HPP

#include <valkyrie/memory/default_allocator.hpp>
#include <valkyrie/memory/std_allocator.hpp>

#include <plf_stack.h>

namespace valkyrie{
  template <typename T, allocator_c Alloc = default_allocator>
  using stack = plf::stack<T, std_allocator<T, Alloc>>;
}

#endif //VALKYRIE_ADT_STACK_HPP
