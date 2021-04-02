//
// Created by maxwe on 2021-04-01.
//

#include <valkyrie/primitives.hpp>

#ifndef VALKYRIE_UTILITY_ORDERING_HPP
#define VALKYRIE_UTILITY_ORDERING_HPP

#include <compare>

namespace valkyrie{

  using std::strong_ordering, std::weak_ordering, std::partial_ordering;

  struct not_ordered{};


  namespace impl{
    template <typename T>
    struct _order_fn;
    template <>
    struct _order_fn<strong_ordering>{
      template <typename A, typename B>
      inline constexpr auto operator()(A&& a, B&& b) const noexcept {
        return std::compare_strong_order_fallback(std::forward<A>(a), std::forward<B>(b));
      }
    };
    template <>
    struct _order_fn<weak_ordering>{
      template <typename A, typename B>
      inline constexpr auto operator()(A&& a, B&& b) const noexcept {
        return std::compare_weak_order_fallback(std::forward<A>(a), std::forward<B>(b));
      }
    };
    template <>
    struct _order_fn<partial_ordering>{
      template <typename A, typename B>
      inline constexpr auto operator()(A&& a, B&& b) const noexcept {
        return std::compare_partial_order_fallback(std::forward<A>(a), std::forward<B>(b));
      }
    };
  }

  template <typename T>
  inline constexpr static impl::_order_fn<T> three_way_order{};
}

#endif//VALKYRIE_UTILITY_ORDERING_HPP
