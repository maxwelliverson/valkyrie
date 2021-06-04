//
// Created by Maxwell on 2020-11-30.
//

#ifndef VALKYRIE_EXTENDED_NUMERIC_TYPES_HPP
#define VALKYRIE_EXTENDED_NUMERIC_TYPES_HPP

#include <valkyrie/traits.hpp>

namespace valkyrie{





  class f16{
  public:


  private:
    u16 value;
  };

  /*template <typename Num, typename Den>
  class rational{
    Num num;
    Den den;

    using InNum = param_t<Num>;
    using InDen = param_t<Den>;

    inline constexpr static bool NoexceptCopyConstructible = std::is_nothrow_constructible_v<Num, InNum> && std::is_nothrow_constructible_v<Den, InDen>;
  public:
    constexpr rational() = default;
    constexpr rational(InNum num, InDen den) noexcept(NoexceptCopyConstructible)
        : num(num), den(den){}
  };*/


  class norm16{};
  class norm32{};
  class norm64{};

  class fixed8{};
  class fixed16{};
  class fixed32{};
  class fixed64{};


  class bool32{
  public:
    constexpr bool32() = default;
    constexpr bool32(bool value) noexcept : value(value){}


    operator bool() const noexcept {
      return value;
    }
    bool32 operator!() const noexcept {
      return bool32(!value);
    }

  private:
    u32 value = 0;
  };

  VK_constant bool32 true32  = true;
  VK_constant bool32 false32 = false;

}


#endif //VALKYRIE_EXTENDED_NUMERIC_TYPES_HPP
