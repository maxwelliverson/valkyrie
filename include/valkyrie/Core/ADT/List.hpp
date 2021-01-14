//
// Created by Maxwell on 2021-01-12.
//

#ifndef VALKYRIE_CORE_ADT_LIST_HPP
#define VALKYRIE_CORE_ADT_LIST_HPP

#include <plf_list.h>

namespace valkyrie::Core{

  template <typename T, typename Alloc = std::allocator<T>>
  using List = plf::list<T, Alloc>;
}

#endif //VALKYRIE_CORE_ADT_LIST_HPP
