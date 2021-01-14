//
// Created by Maxwell on 2021-01-12.
//

#ifndef VALKYRIE_CORE_ADT_STACK_HPP
#define VALKYRIE_CORE_ADT_STACK_HPP

#include <plf_stack.h>

namespace valkyrie::Core{
  template <typename T, typename Alloc = std::allocator<T>>
  using Stack = plf::stack<T, Alloc>;
}

#endif //VALKYRIE_CORE_ADT_STACK_HPP
