//
// Created by maxwe on 2021-04-06.
//

#ifndef VALKYRIE_UTILITY_REFLECT_HPP
#define VALKYRIE_UTILITY_REFLECT_HPP

#include <valkyrie/meta.hpp>

//#include <boost/pfr/detail/core17_generated.hpp>

#include <tuple>

namespace valkyrie{
  // Basic Struct Reflection
  namespace impl{

    struct any_type{
      template <typename T>
      operator T() const noexcept;
    };

    inline constexpr static any_type any{};

    template <typename Str, size_t ...I>
    concept has_member_count_indirect = requires{
      Str{(I, any)...};
    };
    template <typename Str, size_t ...I>
    concept member_count_exact       = has_member_count_indirect<Str, I...> && !has_member_count_indirect<Str, I..., 0>;

    template <typename Str, size_t N, typename = std::make_index_sequence<N>>
    struct do_has_member_count;
    template <typename Str, size_t N, size_t ...I>
    struct do_has_member_count<Str, N, std::index_sequence<I...>>
        : meta::boolean_constant<has_member_count_indirect<Str, I...>>{};


    template <typename Str, size_t N>
    concept has_member_count = do_has_member_count<Str, N>::value;
    template <typename Str, size_t N>
    concept has_exact_member_count = has_member_count<Str, N> && !has_member_count<Str, N + 1>;


    /*template <typename Str, size_t ...I>
    concept member_count_lower_bound = has_member_count<Str, I...> && has_member_count<Str, I..., 0>;*/

    inline constexpr static size_t max_member_count = 64;


    template <typename Struct>
    struct get_member_count_{

      template <size_t Upper,
          size_t Lower = (Upper >> 1),
          size_t N     = ((Upper + Lower) >> 1)>
      struct bounded_query                  : bounded_query<N, Lower>{};
      template <size_t Upper, size_t Lower, size_t N> requires has_member_count<Struct, N>
      struct bounded_query<Upper, Lower, N> : bounded_query<Upper, N>{};
      template <size_t Upper, size_t Lower, size_t N> requires has_exact_member_count<Struct, N>
      struct bounded_query<Upper, Lower, N> : meta::value_wrapper<N>{};



      template <size_t N>
      struct initial_query;

      template <size_t N>
      struct safe_initial_query : initial_query<N>{};
      template <size_t N> requires (N > max_member_count)
      struct safe_initial_query<N>{
        static_assert(N <= max_member_count,
                      "Query exceeded the maximum member count the reflection API can handle");
      };

      template <size_t N>
      struct initial_query    : bounded_query<N>{};
      template <size_t N> requires has_member_count<Struct, N>
      struct initial_query<N> : safe_initial_query<N * 2>{};
      template <size_t N> requires has_exact_member_count<Struct, N>
      struct initial_query<N> : meta::value_wrapper<N>{};




      using type = std::conditional_t<std::is_empty_v<Struct>, meta::value_wrapper<0ull>, safe_initial_query<1>>;
    };
  }



  inline namespace concepts{
    template <typename Struct>
    concept record_c = std::is_aggregate_v<Struct>;
  }


  template <record_c Struct>
  inline constexpr static size_t member_count = impl::get_member_count_<Struct>::type::value;


  namespace impl{

    template <size_t N, typename T>
    struct tuple_member{
      T member;
    };

    template <typename IndexSeq, typename ...T>
    struct tuple_base;
    template <size_t ...I, typename ...T>
    struct tuple_base<std::index_sequence<I...>, T...> : tuple_member<I, T> ...{

      constexpr tuple_base() = default;
      constexpr tuple_base(tuple_base&&) = default;
      constexpr tuple_base(const tuple_base&) = default;

      constexpr tuple_base(T... v) noexcept : tuple_member<I, T>{ v }...{}
    };

    template <size_t N, typename T>
    inline static constexpr T& get_member(tuple_member<N, T>& tup) noexcept {
      return tup.member;
    }
    template <size_t N, typename T>
    inline static constexpr const T& get_member(const tuple_member<N, T>& tup) noexcept {
      return tup.member;
    }
    template <size_t N, typename T>
    inline static constexpr volatile T& get_member(volatile tuple_member<N, T>& tup) noexcept {
      return tup.member;
    }
    template <size_t N, typename T>
    inline static constexpr const volatile T& get_member(const volatile tuple_member<N, T>& tup) noexcept {
      return tup.member;
    }
    template <size_t N, typename T>
    inline static constexpr T&& get_member(tuple_member<N, T>&& tup) noexcept {
      return std::forward<T>(tup.member);
    }


    template <typename ...T>
    struct reference_tuple : public tuple_base<std::make_index_sequence<sizeof...(T)>, T...>{
      using tuple_base<std::make_index_sequence<sizeof...(T)>, T...>::tuple_base;
    };

    template <>
    struct reference_tuple<>{};

    template <typename ...T>
    reference_tuple(T&&...) -> reference_tuple<T...>;


    template <size_t N, typename ...T>
    inline static constexpr decltype(auto) get(reference_tuple<T...>& tup) noexcept {
      static_assert(N < sizeof...(T));
      return impl::get_member<N>(tup);
    }
    template <size_t N, typename ...T>
    inline static constexpr decltype(auto) get(const reference_tuple<T...>& tup) noexcept {
      static_assert(N < sizeof...(T));
      return impl::get_member<N>(tup);
    }
    template <size_t N, typename ...T>
    inline static constexpr decltype(auto) get(volatile reference_tuple<T...>& tup) noexcept {
      static_assert(N < sizeof...(T));
      return impl::get_member<N>(tup);
    }
    template <size_t N, typename ...T>
    inline static constexpr decltype(auto) get(const volatile reference_tuple<T...>& tup) noexcept {
      static_assert(N < sizeof...(T));
      return impl::get_member<N>(tup);
    }
    template <size_t N, typename ...T>
    inline static constexpr decltype(auto) get(reference_tuple<T...>&& tup) noexcept {
      static_assert(N < sizeof...(T));
      return impl::get_member<N>(std::move(tup));
    }



    template <size_t N>
    struct to_tuple;

    template <>
    struct to_tuple<0>{
      template <typename Struct> requires record_c<remove_cvref_t<Struct>>
      inline static decltype(auto) call(Struct&& str) noexcept {
        return reference_tuple<>{};
      }
    };
    template <>
    struct to_tuple<1>{
      template <typename Struct> requires record_c<remove_cvref_t<Struct>>
      inline static decltype(auto) call(Struct&& str) noexcept {
        auto& [a] = std::forward<Struct>(str);
        return reference_tuple{a};
      }
    };
    template <>
    struct to_tuple<2>{
      template <typename Struct> requires record_c<remove_cvref_t<Struct>>
      inline static decltype(auto) call(Struct&& str) noexcept {
        auto& [a, b] = std::forward<Struct>(str);
        return reference_tuple{a, b};
      }
    };
    template <>
    struct to_tuple<3>{
      template <typename Struct> requires record_c<remove_cvref_t<Struct>>
      inline static decltype(auto) call(Struct&& str) noexcept {
        auto& [a, b, c] = std::forward<Struct>(str);
        return reference_tuple{a, b, c};
      }
    };
    template <>
    struct to_tuple<4>{
      template <typename Struct> requires record_c<remove_cvref_t<Struct>>
      inline static decltype(auto) call(Struct&& str) noexcept {
        auto& [a, b, c, d] = std::forward<Struct>(str);
        return reference_tuple{a, b, c, d};
      }
    };
    template <>
    struct to_tuple<5>{
      template <typename Struct> requires record_c<remove_cvref_t<Struct>>
      inline static decltype(auto) call(Struct&& str) noexcept {
        auto& [a, b, c, d, e] = std::forward<Struct>(str);
        return reference_tuple{a, b, c, d, e};
      }
    };
    template <>
    struct to_tuple<6>{
      template <typename Struct> requires record_c<remove_cvref_t<Struct>>
      inline static decltype(auto) call(Struct&& str) noexcept {
        auto& [a, b, c, d, e, f] = std::forward<Struct>(str);
        return reference_tuple{a, b, c, d, e, f};
      }
    };
    template <>
    struct to_tuple<7>{
      template <typename Struct> requires record_c<remove_cvref_t<Struct>>
      inline static decltype(auto) call(Struct&& str) noexcept {
        auto& [a, b, c, d, e, f, g] = std::forward<Struct>(str);
        return reference_tuple{a, b, c, d, e, f, g};
      }
    };
    template <>
    struct to_tuple<8>{
      template <typename Struct> requires record_c<remove_cvref_t<Struct>>
      inline static decltype(auto) call(Struct&& str) noexcept {
        auto& [a, b, c, d, e, f, g, h] = std::forward<Struct>(str);
        return reference_tuple{a, b, c, d, e, f, g, h};
      }
    };
    template <>
    struct to_tuple<9>{
      template <typename Struct> requires record_c<remove_cvref_t<Struct>>
      inline static decltype(auto) call(Struct&& str) noexcept {
        auto& [a, b, c, d, e, f, g, h, i] = std::forward<Struct>(str);
        return reference_tuple{a, b, c, d, e, f, g, h, i};
      }
    };
    template <>
    struct to_tuple<10>{
      template <typename Struct> requires record_c<remove_cvref_t<Struct>>
      inline static decltype(auto) call(Struct&& str) noexcept {
        auto& [a, b, c, d, e, f, g, h, i, j] = std::forward<Struct>(str);
        return reference_tuple{a, b, c, d, e, f, g, h, i, j};
      }
    };
    template <>
    struct to_tuple<11>{
      template <typename Struct> requires record_c<remove_cvref_t<Struct>>
      inline static decltype(auto) call(Struct&& str) noexcept {
        auto& [a, b, c, d, e, f, g, h, i, j, k] = std::forward<Struct>(str);
        return reference_tuple{a, b, c, d, e, f, g, h, i, j, k};
      }
    };
    template <>
    struct to_tuple<12>{
      template <typename Struct> requires record_c<remove_cvref_t<Struct>>
      inline static decltype(auto) call(Struct&& str) noexcept {
        auto& [a, b, c, d, e, f, g, h, i, j, k, l] = std::forward<Struct>(str);
        return reference_tuple{a, b, c, d, e, f, g, h, i, j, k, l};
      }
    };
    template <>
    struct to_tuple<13>{
      template <typename Struct> requires record_c<remove_cvref_t<Struct>>
      inline static decltype(auto) call(Struct&& str) noexcept {
        auto& [a, b, c, d, e, f, g, h, i, j, k, l, m] = std::forward<Struct>(str);
        return reference_tuple{a, b, c, d, e, f, g, h, i, j, k, l, m};
      }
    };
    template <>
    struct to_tuple<14>{
      template <typename Struct> requires record_c<remove_cvref_t<Struct>>
      inline static decltype(auto) call(Struct&& str) noexcept {
        auto& [a, b, c, d, e, f, g, h, i, j, k, l, m, n] = std::forward<Struct>(str);
        return reference_tuple{a, b, c, d, e, f, g, h, i, j, k, l, m, n};
      }
    };
    template <>
    struct to_tuple<15>{
      template <typename Struct> requires record_c<remove_cvref_t<Struct>>
      inline static decltype(auto) call(Struct&& str) noexcept {
        auto& [a, b, c, d, e, f, g, h, i, j, k, l, m, n, o] = std::forward<Struct>(str);
        return reference_tuple{a, b, c, d, e, f, g, h, i, j, k, l, m, n, o};
      }
    };
    template <>
    struct to_tuple<16>{
      template <typename Struct> requires record_c<remove_cvref_t<Struct>>
      inline static decltype(auto) call(Struct&& str) noexcept {
        auto& [a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p] = std::forward<Struct>(str);
        return reference_tuple{a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p};
      }
    };
    template <>
    struct to_tuple<17>{
      template <typename Struct> requires record_c<remove_cvref_t<Struct>>
      inline static decltype(auto) call(Struct&& str) noexcept {
        auto& [a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q] = std::forward<Struct>(str);
        return reference_tuple{a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q};
      }
    };
    template <>
    struct to_tuple<18>{
      template <typename Struct> requires record_c<remove_cvref_t<Struct>>
      inline static decltype(auto) call(Struct&& str) noexcept {
        auto& [a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r] = std::forward<Struct>(str);
        return reference_tuple{a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r};
      }
    };
    template <>
    struct to_tuple<19>{
      template <typename Struct> requires record_c<remove_cvref_t<Struct>>
      inline static decltype(auto) call(Struct&& str) noexcept {
        auto& [a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s] = std::forward<Struct>(str);
        return reference_tuple{a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s};
      }
    };
    template <>
    struct to_tuple<20>{
      template <typename Struct> requires record_c<remove_cvref_t<Struct>>
      inline static decltype(auto) call(Struct&& str) noexcept {
        auto& [a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t] = std::forward<Struct>(str);
        return reference_tuple{a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t};
      }
    };
    template <>
    struct to_tuple<21>{
      template <typename Struct> requires record_c<remove_cvref_t<Struct>>
      inline static decltype(auto) call(Struct&& str) noexcept {
        auto& [a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u] = std::forward<Struct>(str);
        return reference_tuple{a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u};
      }
    };
    template <>
    struct to_tuple<22>{
      template <typename Struct> requires record_c<remove_cvref_t<Struct>>
      inline static decltype(auto) call(Struct&& str) noexcept {
        auto& [a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v] = std::forward<Struct>(str);
        return reference_tuple{a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v};
      }
    };
    template <>
    struct to_tuple<23>{
      template <typename Struct> requires record_c<remove_cvref_t<Struct>>
      inline static decltype(auto) call(Struct&& str) noexcept {
        auto& [a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w] = std::forward<Struct>(str);
        return reference_tuple{a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w};
      }
    };
    template <>
    struct to_tuple<24>{
      template <typename Struct> requires record_c<remove_cvref_t<Struct>>
      inline static decltype(auto) call(Struct&& str) noexcept {
        auto& [a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x] = std::forward<Struct>(str);
        return reference_tuple{a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x};
      }
    };
    template <>
    struct to_tuple<25>{
      template <typename Struct> requires record_c<remove_cvref_t<Struct>>
      inline static decltype(auto) call(Struct&& str) noexcept {
        auto& [a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y] = std::forward<Struct>(str);
        return reference_tuple{a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y};
      }
    };
    template <>
    struct to_tuple<26>{
      template <typename Struct> requires record_c<remove_cvref_t<Struct>>
      inline static decltype(auto) call(Struct&& str) noexcept {
        auto& [a, b, c, d, e,
               f, g, h, i, j,
               k, l, m, n, o,
               p, q, r, s, t,
               u, v, w, x, y, z] = std::forward<Struct>(str);
        return reference_tuple{a, b, c, d, e,
                               f, g, h, i, j,
                               k, l, m, n, o,
                               p, q, r, s, t,
                               u, v, w, x, y, z};
      }
    };


    template <typename Struct> requires record_c<remove_cvref_t<Struct>>
    inline static constexpr decltype(auto) to_ref_tuple(Struct&& str) noexcept {
      return to_tuple<member_count<remove_cvref_t<Struct>>>::call(std::forward<Struct>(str));
    }
  }


  template <size_t N, typename Struct> requires record_c<remove_cvref_t<Struct>>
  inline constexpr static decltype(auto) get(Struct&& str) /* noexcept */ {
    return get<N>(impl::to_ref_tuple(std::forward<Struct>(str)));
  }


  /*namespace impl{
    class reflinfo{

    };
  }*/


  /*struct test_struct{
    int a;
    int b;
    double c;
    test_struct* next;
    void*(* pfn_this)(test_struct*);
  };
  static_assert(member_count<test_struct> == 5);

  static_assert(std::same_as<decltype(get<0>(std::declval<const test_struct&>())), const int&>);
  static_assert(std::same_as<decltype(get<1>(std::declval<const test_struct&>())), const int&>);
  static_assert(std::same_as<decltype(get<2>(std::declval<const test_struct&>())), const double&>);
  static_assert(std::same_as<decltype(get<3>(std::declval<const test_struct&>())), test_struct* const &>);
  static_assert(std::same_as<decltype(get<4>(std::declval<test_struct&>())), void*(* &)(test_struct*)>);*/

}

#endif//VALKYRIE_UTILITY_REFLECT_HPP
