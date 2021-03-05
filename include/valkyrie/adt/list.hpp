//
// Created by Maxwell on 2021-01-12.
//

#ifndef VALKYRIE_ADT_LIST_HPP
#define VALKYRIE_ADT_LIST_HPP

#include <plf_list.h>

namespace valkyrie{

  template <typename T, typename Alloc = std::allocator<T>>
  using List = plf::list<T, Alloc>;
}

#endif //VALKYRIE_ADT_LIST_HPP
