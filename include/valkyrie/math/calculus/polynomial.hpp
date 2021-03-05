//
// Created by Maxwell on 2020-11-13.
//

#ifndef VALKYRIE_MATH_CALCULUS_POLYNOMIAL_HPP
#define VALKYRIE_MATH_CALCULUS_POLYNOMIAL_HPP

#include <valkyrie/primitives.hpp>

namespace valkyrie::Math::Calculus{
  template <typename T>
  class Polynomial{};

  template <typename T>
  class Coefficient{

  };

  template <typename T>
  class HornerTerm{

  public:

    inline constexpr static bool isHornerTerm = true;

    constexpr HornerTerm(T c, T v) noexcept : coefficient(c), variable(v){}




    friend inline constexpr T operator|(HornerTerm A, T B) noexcept{
      return A.coefficient + (A.variable * B);
    }
    friend inline constexpr T operator|(T A, HornerTerm B) noexcept{
      return B.coefficient + (B.variable * A);
    }

    T coefficient;
    T variable;
  };

  template <typename T, typename U>
  HornerTerm(T, U) -> HornerTerm<std::common_type_t<T, U>>;

  namespace detail{
    inline constexpr static struct HornerHook{} hornerHook;

    template <typename T>
    inline constexpr T operator|(HornerTerm<T> value, HornerHook) noexcept{
      return value.coefficient;
    }
    template <typename T>
    inline constexpr T operator|(HornerHook, HornerTerm<T> value) noexcept{
      return value.coefficient;
    }
  }
}

#define PP_VK_impl_EVAL_HORNER_RIGHT_redirect(...) (::valkyrie::Math::Calculus::detail::hornerHook | ... | ::valkyrie::Math::Calculus::HornerTerm{__VA_ARGS__})
#define PP_VK_impl_EVAL_HORNER_LEFT_redirect(...) (::valkyrie::Math::Calculus::HornerTerm{__VA_ARGS__} | ... | ::valkyrie::Math::Calculus::detail::hornerHook)

#define PP_VK_impl_EVAL_HORNER_RIGHT PP_VK_impl_EVAL_HORNER_RIGHT_redirect
#define PP_VK_impl_EVAL_HORNER_LEFT PP_VK_impl_EVAL_HORNER_LEFT_redirect

#define PP_VK_impl_EVAL_HORNER_right VK_lazy(PP_VK_impl_EVAL_HORNER_RIGHT) (
#define PP_VK_impl_EVAL_HORNER_left VK_lazy(PP_VK_impl_EVAL_HORNER_LEFT) (
#define PP_VK_impl_HORNER(...) VK_variadic_concat(PP_VK_impl_EVAL_HORNER_, __VA_ARGS__) )
#define VK_horner PP_VK_impl_HORNER

#endif//VALKYRIE_MATH_CALCULUS_POLYNOMIAL_HPP
