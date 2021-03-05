//
// Created by Maxwell on 2020-12-22.
//

#ifndef VALKYRIE_UTILITY_RANGE_REF_HPP
#define VALKYRIE_UTILITY_RANGE_REF_HPP

#include <valkyrie/utility/iterator.hpp>

namespace valkyrie{
  namespace detail{
    template <typename T, typename IteratorTag>
    class RangeRefIterator;

    template <typename T>
    class RangeRefIterator<T, OutputIterator>{};
    template <typename T>
    class RangeRefIterator<T, InputIterator>{};
    template <typename T>
    class RangeRefIterator<T, ForwardIterator>{

    };
    template <typename T>
    class RangeRefIterator<T, BidirectionalIterator>{

    };
    template <typename T>
    class RangeRefIterator<T, RandomAccessIterator>{

    };
    template <typename T>
    class RangeRefIterator<T, ContiguousIterator>{

    };





    template <typename T, typename IteratorTag>
    class RangeRefSentinel;
  }
}

#endif//VALKYRIE_UTILITY_RANGE_REF_HPP
