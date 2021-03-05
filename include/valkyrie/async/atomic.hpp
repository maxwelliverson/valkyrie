//
// Created by Maxwell on 2020-11-22.
//

#ifndef VALKYRIE_ASYNC_ATOMIC_HPP
#define VALKYRIE_ASYNC_ATOMIC_HPP

#include <valkyrie/preprocessor.hpp>
#include <valkyrie/primitives.hpp>

#include <atomic>

namespace valkyrie{
  namespace detail{
    template <typename T>
    struct GetAtomicType{
      using type = std::atomic_ref<T>;
    };
    template <typename T> requires(requires{ typename std::atomic<T>; { sizeof(std::atomic<T>) }; })
    struct GetAtomicType<T>{
      using type = std::atomic<T>;
    };
  }

  template <typename T>
  using Atomic = typename detail::GetAtomicType<T>::type;
}

#endif//VALKYRIE_ASYNC_ATOMIC_HPP
