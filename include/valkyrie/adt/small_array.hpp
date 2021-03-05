//
// Created by Maxwell on 2021-01-12.
//

#ifndef VALKYRIE_ADT_SMALL_ARRAY_HPP
#define VALKYRIE_ADT_SMALL_ARRAY_HPP

#include <boost/container/small_vector.hpp>

#include <vector>

namespace valkyrie{
  template <typename T, size_t N = 4, typename Alloc = std::allocator<T>>
  using small_array = boost::container::small_vector<T, N, Alloc>;
  template <typename T, typename Alloc = std::allocator<T>>
  using DynamicArray = std::vector<T, Alloc>;
}

#endif //VALKYRIE_ADT_SMALL_ARRAY_HPP
