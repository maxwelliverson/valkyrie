//
// Created by Maxwell on 2021-01-12.
//

#ifndef VALKYRIE_ADT_STACK_HPP
#define VALKYRIE_ADT_STACK_HPP

#include <plf_stack.h>

namespace valkyrie{
  template <typename T, typename Alloc = std::allocator<T>>
  using Stack = plf::stack<T, Alloc>;
}

#endif //VALKYRIE_ADT_STACK_HPP
