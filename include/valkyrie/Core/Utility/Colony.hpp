//
// Created by Maxwell on 2020-11-09.
//

#ifndef VALKYRIE_COLONY_HPP
#define VALKYRIE_COLONY_HPP

#include <plf_colony.h>

namespace valkyrie::Core{
  template <typename T, typename Alloc = std::allocator<T>, typename FieldSkipType = u16>
  using Colony = plf::colony<T, Alloc, FieldSkipType>;
}

#endif//VALKYRIE_COLONY_HPP
