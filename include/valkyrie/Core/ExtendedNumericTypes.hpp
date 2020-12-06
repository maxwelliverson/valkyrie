//
// Created by Maxwell on 2020-11-30.
//

#ifndef VALKYRIE_EXTENDED_NUMERIC_TYPES_HPP
#define VALKYRIE_EXTENDED_NUMERIC_TYPES_HPP

#include <valkyrie/Core/Types.hpp>
#include <valkyrie/Core/Meta.hpp>
#include <valkyrie/Core/Traits.hpp>

namespace valkyrie{

  namespace Core{

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
    template <decltype(auto) V, typename Den>
    class Rational<Meta::ValueWrapper<V>, Den>{

    };






    class NormalizedFloat64{};
    class NormalizedFloat32{};
    class NormalizedFloat16{};

    class FixedPoint64{};
    class FixedPoint32{};
    class FixedPoint16{};
    class FixedPoint8 {};
  }

  using f16 = Core::Float16;

  using norm64 = Core::NormalizedFloat64;
  using norm32 = Core::NormalizedFloat32;
  using norm16 = Core::NormalizedFloat16;

  using fixed64 = Core::FixedPoint64;
  using fixed32 = Core::FixedPoint32;
  using fixed16 = Core::FixedPoint16;
  using fixed8  = Core::FixedPoint8;
}


#endif //VALKYRIE_EXTENDED_NUMERIC_TYPES_HPP
