//
// Created by Maxwell on 2021-01-12.
//

#ifndef VALKYRIE_ADT_COLONY_HPP
#define VALKYRIE_ADT_COLONY_HPP

#include <valkyrie/primitives.hpp>
#include <valkyrie/memory/default_allocator.hpp>
#include <valkyrie/memory/std_allocator.hpp>


#include <plf_colony.h>

namespace valkyrie{
  template <typename T, raw_allocator Alloc = default_allocator, typename FieldSkipType = u16>
  using colony = plf::colony<T, std_allocator<T, Alloc>, FieldSkipType>;
}

#endif //VALKYRIE_ADT_COLONY_HPP
