//
// Created by Maxwell on 2021-01-12.
//

#ifndef VALKYRIE_ADT_SMALL_VECTOR_HPP
#define VALKYRIE_ADT_SMALL_VECTOR_HPP

#include <valkyrie/memory/default_allocator.hpp>
#include <valkyrie/memory/std_allocator.hpp>

#include <boost/container/small_vector.hpp>

#include <vector>

namespace valkyrie{
  template <typename T, size_t N = 4, raw_allocator Alloc = default_allocator>
  using small_vector = boost::container::small_vector<T, N, std_allocator<T, Alloc>>;

  template <typename T, raw_allocator Alloc = default_allocator>
  using vector = std::vector<T, std_allocator<T, Alloc>>;
}

#endif//VALKYRIE_ADT_SMALL_VECTOR_HPP
