//
// Created by Maxwell on 2020-11-16.
//

#ifndef VALKYRIE_STACK_HPP
#define VALKYRIE_STACK_HPP

#include <plf_stack.h>

namespace valkyrie::Core{
  template <typename T, typename Alloc = std::allocator<T>>
  using Stack = plf::stack<T, Alloc>;
}

#endif//VALKYRIE_STACK_HPP
