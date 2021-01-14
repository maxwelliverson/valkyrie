//
// Created by Maxwell on 2021-01-12.
//

#ifndef VALKYRIE_CORE_ADT_SMALL_ARRAY_HPP
#define VALKYRIE_CORE_ADT_SMALL_ARRAY_HPP

#include <boost/container/small_vector.hpp>

#include <vector>

namespace valkyrie::Core{
  template <typename T, size_t N = 4, typename Alloc = std::allocator<T>>
  using SmallArray = boost::container::small_vector<T, N, Alloc>;
  template <typename T, typename Alloc = std::allocator<T>>
  using DynamicArray = std::vector<T, Alloc>;
}

#endif //VALKYRIE_CORE_ADT_SMALL_ARRAY_HPP
