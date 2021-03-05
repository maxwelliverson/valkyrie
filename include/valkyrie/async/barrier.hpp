//
// Created by Maxwell on 2020-11-18.
//

#ifndef VALKYRIE_ASYNC_BARRIER_HPP
#define VALKYRIE_ASYNC_BARRIER_HPP

#include <valkyrie/async/semaphore.hpp>

#include <barrier>

namespace valkyrie{
  template <typename Fn>
  using Barrier = std::barrier<Fn>;
}

#endif//VALKYRIE_ASYNC_BARRIER_HPP
