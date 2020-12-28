//
// Created by Maxwell on 2020-12-22.
//

#ifndef VALKYRIE_CORE_RANGE_REF_HPP
#define VALKYRIE_CORE_RANGE_REF_HPP

#include <valkyrie/Core/Utility/Iterator.hpp>

namespace valkyrie::Core{
  namespace Detail{
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

#endif//VALKYRIE_CORE_RANGE_REF_HPP
