//
// Created by maxwe on 2021-03-05.
//

#ifndef VALKYRIE_MEMORY_DETAIL_ILOG2_HPP
#define VALKYRIE_MEMORY_DETAIL_ILOG2_HPP

#include <valkyrie/traits.hpp>

namespace valkyrie::detail{
  // undefined for 0
  template <std::unsigned_integral UInt>
  VK_constant bool is_power_of_two(UInt x) {
    return (x & (x - 1)) == 0;
  }

  inline u64 ilog2_base(u64 x)
  {
#if defined(__GNUC__)
    unsigned long long value = x;
    return sizeof(value) * CHAR_BIT - __builtin_clzll(value);
#else
    // Adapted from https://stackoverflow.com/a/40943402
    u64 clz = 64;
    u64 c   = 32;
    do
    {
      auto tmp = x >> c;
      if (tmp != 0)
      {
        clz -= c;
        x = tmp;
      }
      c = c >> 1;
    } while (c != 0);
    clz -= x ? 1 : 0;

    return 64 - clz;
#endif
  }

  // ilog2() implementation, cuts part after the comma
  // e.g. 1 -> 0, 2 -> 1, 3 -> 1, 4 -> 2, 5 -> 2
  inline u64 ilog2(u64 x) {
    return ilog2_base(x) - 1;
  }

  // ceiling ilog2() implementation, adds one if part after comma
  // e.g. 1 -> 0, 2 -> 1, 3 -> 2, 4 -> 2, 5 -> 3
  inline u64 ilog2_ceil(u64 x) {
    // only subtract one if power of two
    return ilog2_base(x) - u64(is_power_of_two(x));
  }
}

#endif//VALKYRIE_MEMORY_DETAIL_ILOG2_HPP
