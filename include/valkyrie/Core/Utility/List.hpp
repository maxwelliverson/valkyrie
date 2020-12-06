//
// Created by Maxwell on 2020-11-09.
//

#ifndef VALKYRIE_LIST_HPP
#define VALKYRIE_LIST_HPP

#include <plf_list.h>

namespace valkyrie::Core{

  template <typename T, typename Alloc = std::allocator<T>>
  using List = plf::list<T, Alloc>;
}

#endif//VALKYRIE_LIST_HPP
