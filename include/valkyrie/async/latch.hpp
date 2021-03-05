//
// Created by Maxwell on 2020-11-18.
//

#ifndef VALKYRIE_ASYNC_LATCH_HPP
#define VALKYRIE_ASYNC_LATCH_HPP

#include <valkyrie/async/semaphore.hpp>

#include <latch>

namespace valkyrie{
  using Latch = std::latch;
}

#endif//VALKYRIE_ASYNC_LATCH_HPP
