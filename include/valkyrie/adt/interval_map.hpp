//
// Created by maxwe on 2021-05-04.
//

#ifndef VALKYRIE_ADT_INTERVAL_MAP_HPP
#define VALKYRIE_ADT_INTERVAL_MAP_HPP

#include "interval.hpp"

#include <valkyrie/memory/default_allocator.hpp>

namespace valkyrie{

  template <typename T, typename MappedType, allocator_c Alloc = default_allocator>
  class interval_map{

    struct mapped_wrapper{
      union{
        byte       storage[sizeof(MappedType)] = {};
        MappedType value;
      };

      mapped_wrapper() = default;
      mapped_wrapper(const mapped_wrapper& other) : value(other.value){}
      mapped_wrapper(mapped_wrapper&& other) noexcept : value(std::move(other)){}

      mapped_wrapper&operator=(const mapped_wrapper& other) {
        value = other.value;
        return *this;
      }
      mapped_wrapper&operator=(mapped_wrapper&& other) noexcept {
        value = std::move(other.value);
        return *this;
      }

      ~mapped_wrapper() {
        this->value.~MappedType();
      }
    };

    using branch_type = interval<T>;
    using leaf_type   = interval<T, impl::interval_mixin<mapped_wrapper>>;

  public:


  };

  template <typename T, typename MappedType, allocator_c Alloc = default_allocator>
  class overlapped_interval_map{

  };


}

#endif  //VALKYRIE_ADT_INTERVAL_MAP_HPP
