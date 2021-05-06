//
// Created by maxwe on 2021-05-03.
//

#ifndef VALKYRIE_UTILITY_POW2_HPP
#define VALKYRIE_UTILITY_POW2_HPP

#include <valkyrie/preprocessor.hpp>
#include <valkyrie/primitives.hpp>

#include <concepts>
#include <bit>

namespace valkyrie{

  template <std::unsigned_integral UInt>
  VK_constant bool is_pow2(UInt x) {
    return (x & (x - 1)) == 0;
  }

  template <std::unsigned_integral UInt>
  VK_constant i32 ilog2(UInt value) noexcept{
    return std::countr_zero(std::bit_floor(value));
  }
  template <std::unsigned_integral UInt>
  VK_constant i32 ilog2_base(UInt value) noexcept{
    return ilog2(value);
  }
  template <std::unsigned_integral UInt>
  VK_constant i32 ilog2_ceil(UInt value) noexcept{
    return std::countr_zero(std::bit_ceil(value));
  }
}

#endif  //VALKYRIE_UTILITY_POW2_HPP
