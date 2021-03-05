//
// Created by Maxwell on 2021-01-12.
//

#ifndef VALKYRIE_ADT_COLONY_HPP
#define VALKYRIE_ADT_COLONY_HPP

#include <valkyrie/primitives.hpp>

#include <plf_colony.h>

namespace valkyrie{
  template <typename T, typename Alloc = std::allocator<T>, typename FieldSkipType = u16>
  using Colony = plf::colony<T, Alloc, FieldSkipType>;
}

#endif //VALKYRIE_ADT_COLONY_HPP
