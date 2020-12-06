//
// Created by Maxwell on 2020-11-22.
//

#ifndef VALKYRIE_ATOMIC_HPP
#define VALKYRIE_ATOMIC_HPP

#include <valkyrie/Core/Config.hpp>
#include <valkyrie/Core/Types.hpp>

#include <atomic>

namespace valkyrie::Core{
  namespace Detail{
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
  using Atomic = typename Detail::GetAtomicType<T>::type;
}

#endif//VALKYRIE_ATOMIC_HPP
