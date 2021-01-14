//
// Created by Maxwell on 2021-01-12.
//

#ifndef VALKYRIE_CORE_ADT_STATIC_ARRAY_HPP
#define VALKYRIE_CORE_ADT_STATIC_ARRAY_HPP

#include <boost/container/static_vector.hpp>

namespace valkyrie::Core{
  template <typename T, size_t N>
  using StaticArray = boost::container::static_vector<T, N>;
}

#endif //VALKYRIE_CORE_ADT_STATIC_ARRAY_HPP
