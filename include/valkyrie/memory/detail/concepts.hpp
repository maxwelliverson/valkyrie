//
// Created by maxwe on 2021-03-04.
//

#ifndef VALKYRIE_MEMORY_DETAIL_CONCEPTS_HPP
#define VALKYRIE_MEMORY_DETAIL_CONCEPTS_HPP

#include <valkyrie/traits.hpp>

namespace valkyrie::memory{

  class block;
  class arena;

  inline namespace concepts{
    template <typename T>
    concept node = std::convert;
    template <typename T>
    concept node_array = ;
    template <typename T>
    concept raw_allocator = ;
    template <typename T>
    concept composable_raw_allocator = ;
    template <typename T>
    concept block_allocator = ;
    template <typename T>
    concept storage_policy = ;
    template <typename T>
    concept segregatable = ;
    template <typename T>
    concept tracker      = ;
  }

}

#endif//VALKYRIE_MEMORY_DETAIL_CONCEPTS_HPP
