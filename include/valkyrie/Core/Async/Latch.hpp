//
// Created by Maxwell on 2020-11-18.
//

#ifndef VALKYRIE_CORE_ASYNC_LATCH_HPP
#define VALKYRIE_CORE_ASYNC_LATCH_HPP

#include <valkyrie/Core/Async/Semaphore.hpp>

#include <latch>

namespace valkyrie::Core{
  using Latch = std::latch;
}

#endif//VALKYRIE_CORE_ASYNC_LATCH_HPP
