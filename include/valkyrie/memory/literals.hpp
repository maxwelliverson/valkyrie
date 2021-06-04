//
// Created by maxwe on 2021-04-27.
//

#ifndef VALKYRIE_MEMORY_LITERALS_HPP
#define VALKYRIE_MEMORY_LITERALS_HPP

#include <valkyrie/primitives.hpp>

namespace valkyrie{
  
  
  struct mem {
    enum unit_t : u64 {
      bytes = 1,
      KiB   = 1024,
      KB    = 1000,
      MiB   = KiB * KiB,
      MB    = KB * KB,
      GiB   = MiB * KiB,
      GB    = MB * KB,
      TiB   = GiB * KiB,
      TB    = GB * KB,
      PiB   = TiB * KiB,
      PB    = TB * KB,
      EiB   = PiB * KiB,
      EB    = PB * KB
    };
    
    VK_constant u64 full_precision = 0xF;

    u64    total_bytes;
    unit_t unit          : 60;
    u64    max_precision : 4;

    operator u64() const noexcept {
      return total_bytes;
    }
  };

  constexpr mem operator+(mem a, mem b) noexcept {
    mem::unit_t ret_unit = std::max(a.unit, b.unit);
    return {
        .total_bytes = a.total_bytes + b.total_bytes,
        .unit = std::max(a.unit, b.unit),
        .max_precision = std::min(a.max_precision, b.max_precision)
    };
  }
  constexpr mem operator-(mem a, mem b) noexcept {
    mem::unit_t ret_unit = std::max(a.unit, b.unit);
    return {
        .total_bytes = a.total_bytes - b.total_bytes,
        .unit = std::max(a.unit, b.unit),
        .max_precision = std::min(a.max_precision, b.max_precision)
    };
  }
  constexpr mem operator*(mem a, u64 mul) noexcept {
    return {
        .total_bytes   = a.total_bytes * mul,
        .unit          = a.unit,
        .max_precision = a.max_precision
    };
  }
  constexpr mem operator/(mem a, u64 div) noexcept {
    return {
        .total_bytes   = a.total_bytes / div,
        .unit          = a.unit,
        .max_precision = a.max_precision
    };
  }





  namespace literals{
    inline namespace memory{
      
      constexpr mem operator"" _bytes(unsigned long long value) noexcept {
        return { .total_bytes = value, .unit = mem::bytes, .max_precision = mem::full_precision };
      }
        
      constexpr mem operator"" _KiB(unsigned long long value) noexcept {
        return { .total_bytes = value * mem::KiB,
                 .unit = mem::KiB,
                 .max_precision = mem::full_precision };
      }

      constexpr mem operator"" _KB(unsigned long long value) noexcept {
        return { .total_bytes = value * mem::KB,
                 .unit = mem::KB,
                 .max_precision = mem::full_precision };
      }

      constexpr mem operator"" _MiB(unsigned long long value) noexcept {
        return { .total_bytes = value * mem::MiB,
                 .unit = mem::MiB,
                 .max_precision = mem::full_precision };
      }

      constexpr mem operator"" _MB(unsigned long long value) noexcept {
        return { .total_bytes = value * mem::MB,
                 .unit = mem::MB,
                 .max_precision = mem::full_precision };
      }

      constexpr mem operator"" _GiB(unsigned long long value) noexcept {
        return { .total_bytes   = value * mem::GiB,
                 .unit          = mem::GiB,
                 .max_precision = mem::full_precision };
      }

      constexpr mem operator"" _GB(unsigned long long value) noexcept {
        return { .total_bytes   = value * mem::GB,
                 .unit          = mem::GB,
                 .max_precision = mem::full_precision };
      }

      constexpr mem operator"" _TiB(unsigned long long value) noexcept {
        return { .total_bytes   = value * mem::TiB,
            .unit          = mem::TiB,
            .max_precision = mem::full_precision };
      }

      constexpr mem operator"" _TB(unsigned long long value) noexcept {
        return { .total_bytes   = value * mem::TB,
            .unit          = mem::TB,
            .max_precision = mem::full_precision };
      }
    }
  }
  namespace memory_literals = literals::memory;
}

#endif  //VALKYRIE_MEMORY_LITERALS_HPP
