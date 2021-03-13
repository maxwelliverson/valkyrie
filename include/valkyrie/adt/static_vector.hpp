//
// Created by Maxwell on 2021-01-12.
//

#ifndef VALKYRIE_ADT_STATIC_VECTOR_HPP
#define VALKYRIE_ADT_STATIC_VECTOR_HPP

#include <valkyrie/memory/default_allocator.hpp>
#include <valkyrie/memory/std_allocator.hpp>

#include <boost/container/static_vector.hpp>

namespace valkyrie{
  template <typename T, size_t N>
  using static_vector = boost::container::static_vector<T, N>;
}

#endif //VALKYRIE_ADT_STATIC_VECTOR_HPP
