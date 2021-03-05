//
// Created by Maxwell on 2020-11-30.
//

#ifndef VALKYRIE_EXTENDED_NUMERIC_TYPES_HPP
#define VALKYRIE_EXTENDED_NUMERIC_TYPES_HPP

#include <valkyrie/Types.hpp>
#include <valkyrie/Meta.hpp>
#include <valkyrie/traits.hpp>

namespace valkyrie{

  template <typename Num, typename Den>
  class Rational;
  class Float16;




  class Float16{};

  template <bool Signed, u32 Bits>
  class NormalizedFloat{};

  template <typename Num, typename Den>
  class Rational{
    Num num;
    Den den;

    using InNum = param_t<Num>;
    using InDen = param_t<Den>;

    inline constexpr static bool NoexceptCopyConstructible = std::is_nothrow_constructible_v<Num, InNum> && std::is_nothrow_constructible_v<Den, InDen>;
  public:
    constexpr Rational() = default;
    constexpr Rational(InNum num, InDen den) noexcept(NoexceptCopyConstructible)
        : num(num), den(den){}
  };
  /*template <decltype(auto) V, typename Den>
  class Rational<meta::ValueWrapper<V>, Den>{

};*/






  class NormalizedFloat64{};
  class NormalizedFloat32{};
  class NormalizedFloat16{};

  class FixedPoint64{};
  class FixedPoint32{};
  class FixedPoint16{};
  class FixedPoint8 {};

  using f16 = Float16;

  using norm64 = NormalizedFloat64;
  using norm32 = NormalizedFloat32;
  using norm16 = NormalizedFloat16;

  using fixed64 = FixedPoint64;
  using fixed32 = FixedPoint32;
  using fixed16 = FixedPoint16;
  using fixed8  = FixedPoint8;
}


#endif //VALKYRIE_EXTENDED_NUMERIC_TYPES_HPP
