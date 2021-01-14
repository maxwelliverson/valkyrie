//
// Created by Maxwell on 2021-01-12.
//

#ifndef VALKYRIE_CORE_ADT_COLONY_HPP
#define VALKYRIE_CORE_ADT_COLONY_HPP

#include <valkyrie/Core/Types.hpp>

#include <plf_colony.h>

namespace valkyrie::Core{
  template <typename T, typename Alloc = std::allocator<T>, typename FieldSkipType = u16>
  using Colony = plf::colony<T, Alloc, FieldSkipType>;
}

#endif //VALKYRIE_CORE_ADT_COLONY_HPP
