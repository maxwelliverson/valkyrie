//
// Created by Maxwell on 2020-11-15.
//

#ifndef VALKYRIE_VERSION_HPP
#define VALKYRIE_VERSION_HPP

#include <valkyrie/Core/Types.hpp>

#include <compare>
#include <bit>

namespace valkyrie::Core{
  class Version {
    u32 Major : 10;
    u32 Minor : 10;
    u32 Patch : 12;

  public:

    constexpr Version() = default;
    constexpr explicit Version(u32 Major, u32 Minor = 0, u32 Patch = 0) noexcept :
        Major(Major), Minor(Minor), Patch(Patch){}

    constexpr u32 major() const noexcept { return Major; }
    constexpr u32 minor() const noexcept { return Minor; }
    constexpr u32 patch() const noexcept { return Patch; }

    constexpr explicit operator u32() const noexcept {
      return std::bit_cast<u32>(*this);
    }

    friend constexpr bool operator==(Version A, Version B) noexcept = default;
    friend constexpr std::strong_ordering operator<=>(Version A, Version B) noexcept {
      return (u32)A <=> (u32)B;
    }
  };

  constexpr Version decodeCompactVersion(u32 version_) noexcept{
    return Version(version_ >> 22, (version_ >> 12) & 0x0000'03FFu, version_ & 0x0000'0FFFu);
  }
}

#endif//VALKYRIE_VERSION_HPP
