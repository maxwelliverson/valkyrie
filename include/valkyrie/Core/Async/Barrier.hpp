//
// Created by Maxwell on 2020-11-18.
//

#ifndef VALKYRIE_CORE_ASYNC_BARRIER_HPP
#define VALKYRIE_CORE_ASYNC_BARRIER_HPP

#include <valkyrie/Core/Async/Semaphore.hpp>

#include <barrier>

namespace valkyrie::Core{
  template <typename Fn>
  using Barrier = std::barrier<Fn>;
}

#endif//VALKYRIE_CORE_ASYNC_BARRIER_HPP
