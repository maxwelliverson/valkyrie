//
// Created by maxwe on 2021-04-27.
//

#ifndef VALKYRIE_MEMORY_LITERALS_HPP
#define VALKYRIE_MEMORY_LITERALS_HPP

#include <valkyrie/primitives.hpp>

namespace valkyrie{
  namespace literals{
    inline namespace memory{
        /// Syntax sugar to express sizes with unit prefixes.
        /// \returns The number of bytes `value` is in the given unit.
        /// \ingroup core
        /// @{
        constexpr u64 operator"" _KiB(unsigned long long value) noexcept
        {
          return u64(value * 1024);
        }

        constexpr u64 operator"" _KB(unsigned long long value) noexcept
        {
          return u64(value * 1000);
        }

        constexpr u64 operator"" _MiB(unsigned long long value) noexcept
        {
          return u64(value * 1024 * 1024);
        }

        constexpr u64 operator"" _MB(unsigned long long value) noexcept
        {
          return u64(value * 1000 * 1000);
        }

        constexpr u64 operator"" _GiB(unsigned long long value) noexcept
        {
          return u64(value * 1024 * 1024 * 1024);
        }

        constexpr u64 operator"" _GB(unsigned long long value) noexcept
        {
          return u64(value * 1000 * 1000 * 1000);
        }
    }
  }
  namespace memory_literals = literals::memory;
}

#endif  //VALKYRIE_MEMORY_LITERALS_HPP
