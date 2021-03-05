//
// Created by Maxwell on 2021-01-12.
//

#ifndef VALKYRIE_ADT_STATIC_ARRAY_HPP
#define VALKYRIE_ADT_STATIC_ARRAY_HPP

#include <boost/container/static_vector.hpp>

namespace valkyrie{
  template <typename T, size_t N>
  using static_array = boost::container::static_vector<T, N>;
}

#endif //VALKYRIE_ADT_STATIC_ARRAY_HPP
