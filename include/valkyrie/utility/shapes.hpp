//
// Created by Maxwell on 2020-12-30.
//

#ifndef VALKYRIE_UTILITY_SHAPES_HPP
#define VALKYRIE_UTILITY_SHAPES_HPP



#include <valkyrie/traits.hpp>

#include <tuple>

namespace valkyrie{
  template <typename T = u32>
  struct extent2d{
    T width;
    T height;

    constexpr extent2d() = default;
    constexpr extent2d(const extent2d&) = default;
    constexpr extent2d(extent2d&&) noexcept = default;

    constexpr extent2d& operator=(const extent2d&) = default;
    constexpr extent2d& operator=(extent2d&&) noexcept = default;

    template <convertible_to<T> A>
    explicit constexpr extent2d(A&& a) noexcept
        : width(std::forward<A>(a)),
          height{}{}
    template <convertible_to<T> A, convertible_to<T> B>
    constexpr extent2d(A&& a, B&& b) noexcept
        : width(std::forward<A>(a)),
          height(std::forward<B>(b)){}

    template <typename U> requires(constructible_from<T, const U&>)
    explicit(!convertible_to<const U&, T>) extent2d(const extent2d<U>& other) noexcept(std::is_nothrow_constructible_v<T, const U&>)
        : width(other.width),
          height(other.height){}
    template <typename U> requires(constructible_from<T, U&&>)
    explicit(!convertible_to<U&&, T>) extent2d(extent2d<U>&& other) noexcept(std::is_nothrow_constructible_v<T, U&&>)
        : width(std::move(other.width)),
          height(std::move(other.height)){}
  };
  template <typename T = u32>
  struct extent3d{
    T width;
    T height;
    T depth;

    constexpr extent3d() = default;
    constexpr extent3d(const extent3d&) = default;
    constexpr extent3d(extent3d&&) noexcept = default;

    constexpr extent3d& operator=(const extent3d&) = default;
    constexpr extent3d& operator=(extent3d&&) noexcept = default;


    template <convertible_to<T> A>
    explicit constexpr extent3d(A&& a) noexcept
        : width(std::forward<A>(a)),
          height{},
          depth{}{}
    template <convertible_to<T> A, convertible_to<T> B>
    constexpr extent3d(A&& a, B&& b) noexcept
        : width( std::forward<A>(a)),
          height(std::forward<B>(b)),
          depth{}{}
    template <convertible_to<T> A, convertible_to<T> B, convertible_to<T> C>
    constexpr extent3d(A&& a, B&& b, C&& c) noexcept
        : width( std::forward<A>(a)),
          height(std::forward<B>(b)),
          depth( std::forward<C>(c)){}



    template <typename U> requires(constructible_from<T, const U&>)
    explicit constexpr extent3d(const extent2d<U>& other) noexcept(std::is_nothrow_constructible_v<T, const U&>)
        : width( other.width),
          height(other.height),
          depth{}{}
    template <typename U> requires(constructible_from<T, U&&>)
    explicit constexpr extent3d(extent2d<U>&& other) noexcept(std::is_nothrow_constructible_v<T, U&&>)
        : width( std::move(other.width)),
          height(std::move(other.height)),
          depth{}{}



    template <typename U, typename V> requires(constructible_from<T, const U&> && constructible_from<T, V>)
    constexpr extent3d(const extent2d<U>& other, V&& depth) noexcept(std::is_nothrow_constructible_v<T, const U&> && std::is_nothrow_constructible_v<T, V>)
        : width( other.width),
          height(other.height),
          depth( std::forward<V>(depth)){}
    template <typename U, typename V> requires(constructible_from<T, U&&> && constructible_from<T, V>)
    constexpr extent3d(extent2d<U>&& other, V&& depth) noexcept(std::is_nothrow_constructible_v<T, U&&> && std::is_nothrow_constructible_v<T, V>)
        : width( std::move(other.width)),
          height(std::move(other.height)),
          depth( std::forward<V>(depth)){}



    template <typename U> requires(constructible_from<T, const U&>)
    explicit(!convertible_to<const U&, T>) constexpr extent3d(const extent3d<U>& other) noexcept(std::is_nothrow_constructible_v<T, const U&>)
        : width( other.width),
          height(other.height),
          depth( other.depth){}
    template <typename U> requires(constructible_from<T, U&&>)
    explicit(!convertible_to<U&&, T>) constexpr extent3d(extent3d<U>&& other) noexcept(std::is_nothrow_constructible_v<T, U&&>)
        : width( std::move(other.width)),
          height(std::move(other.height)),
          depth( std::move(other.depth)){}
  };

  template <typename T> requires numeric_c<T>
  extent2d(T) -> extent2d<T>;
  template <typename T, typename U> requires(numeric_c<T> && numeric_c<U>)
  extent2d(T, U) -> extent2d<std::common_type_t<T, U>>;

  template <typename T> requires numeric_c<T>
  extent3d(T) -> extent3d<T>;
  template <typename T, typename U> requires(numeric_c<T> && numeric_c<U>)
  extent3d(T, U) -> extent3d<std::common_type_t<T, U>>;
  template <typename T, typename U, typename V> requires(numeric_c<T> && numeric_c<U> && numeric_c<V>)
  extent3d(T, U, V) -> extent3d<std::common_type_t<T, U, V>>;
  template <typename T> requires numeric_c<T>
  extent3d(const extent2d<T>&) -> extent3d<T>;
  template <typename T> requires numeric_c<T>
  extent3d(extent2d<T>&&) -> extent3d<T>;
  template <typename T, typename U> requires(numeric_c<T> && numeric_c<U>)
  extent3d(const extent2d<T>&, U) -> extent3d<std::common_type_t<T, U>>;
  template <typename T, typename U> requires(numeric_c<T> && numeric_c<U>)
  extent3d(U, const extent2d<T>&) -> extent3d<std::common_type_t<T, U>>;
  template <typename T, typename U> requires(numeric_c<T> && numeric_c<U>)
  extent3d(extent2d<T>&&, U) -> extent3d<std::common_type_t<T, U>>;
  template <typename T, typename U> requires(numeric_c<T> && numeric_c<U>)
  extent3d(U, extent2d<T>&&) -> extent3d<std::common_type_t<T, U>>;


  template <typename T = u32>
  struct position2d{
    T x;
    T y;

    constexpr position2d() = default;
    constexpr position2d(const position2d&) = default;
    constexpr position2d(position2d&&) noexcept = default;

    constexpr position2d& operator=(const position2d&) = default;
    constexpr position2d& operator=(position2d&&) noexcept = default;

    template <convertible_to<T> A>
    explicit constexpr position2d(A&& a) noexcept
        : x(std::forward<A>(a)),
          y{}{}
    template <convertible_to<T> A, convertible_to<T> B>
    constexpr position2d(A&& a, B&& b) noexcept
        : x(std::forward<A>(a)),
          y(std::forward<B>(b)){}

    template <typename U> requires(constructible_from<T, const U&>)
    explicit(!convertible_to<const U&, T>) position2d(const position2d<U>& other) noexcept(std::is_nothrow_constructible_v<T, const U&>)
        : x(other.x),
          y(other.y){}
    template <typename U> requires(constructible_from<T, U&&>)
    explicit(!convertible_to<U&&, T>) position2d(position2d<U>&& other) noexcept(std::is_nothrow_constructible_v<T, U&&>)
        : x(std::move(other.x)),
          y(std::move(other.y)){}
  };
  template <typename T = u32>
  struct position3d{
    T x;
    T y;
    T z;

    constexpr position3d() = default;
    constexpr position3d(const position3d&) = default;
    constexpr position3d(position3d&&) noexcept = default;

    constexpr position3d& operator=(const position3d&) = default;
    constexpr position3d& operator=(position3d&&) noexcept = default;

    template <convertible_to<T> A>
    explicit constexpr position3d(A&& a) noexcept
        : x(std::forward<A>(a)),
          y{},
          z{}{}
    template <convertible_to<T> A, convertible_to<T> B>
    constexpr position3d(A&& a, B&& b) noexcept
        : x( std::forward<A>(a)),
          y(std::forward<B>(b)),
          z{}{}
    template <convertible_to<T> A, convertible_to<T> B, convertible_to<T> C>
    constexpr position3d(A&& a, B&& b, C&& c) noexcept
        : x( std::forward<A>(a)),
          y(std::forward<B>(b)),
          z( std::forward<C>(c)){}



    template <typename U> requires(constructible_from<T, const U&>)
    explicit constexpr position3d(const position2d<U>& other) noexcept(std::is_nothrow_constructible_v<T, const U&>)
        : x( other.x),
          y(other.y),
          z{}{}
    template <typename U> requires(constructible_from<T, U&&>)
    explicit constexpr position3d(position2d<U>&& other) noexcept(std::is_nothrow_constructible_v<T, U&&>)
        : x( std::move(other.x)),
          y(std::move(other.y)),
          z{}{}



    template <typename U, typename V> requires(constructible_from<T, const U&> && constructible_from<T, V>)
    constexpr position3d(const position2d<U>& other, V&& z) noexcept(std::is_nothrow_constructible_v<T, const U&> && std::is_nothrow_constructible_v<T, V>)
        : x( other.x),
          y(other.y),
          z( std::forward<V>(z)){}
    template <typename U, typename V> requires(constructible_from<T, U&&> && constructible_from<T, V>)
    constexpr position3d(position2d<U>&& other, V&& z) noexcept(std::is_nothrow_constructible_v<T, U&&> && std::is_nothrow_constructible_v<T, V>)
        : x( std::move(other.x)),
          y(std::move(other.y)),
          z( std::forward<V>(z)){}



    template <typename U> requires(constructible_from<T, const U&>)
    explicit(!convertible_to<const U&, T>) constexpr position3d(const position3d<U>& other) noexcept(std::is_nothrow_constructible_v<T, const U&>)
        : x( other.x),
          y(other.y),
          z( other.z){}
    template <typename U> requires(constructible_from<T, U&&>)
    explicit(!convertible_to<U&&, T>) constexpr position3d(position3d<U>&& other) noexcept(std::is_nothrow_constructible_v<T, U&&>)
        : x( std::move(other.x)),
          y(std::move(other.y)),
          z( std::move(other.z)){}
  };


  template <typename T> requires numeric_c<T>
  position2d(T&&) -> position2d<remove_cvref_t<T>>;
  template <typename T, typename U> requires(numeric_c<T> && numeric_c<U>)
  position2d(T&&, U&&) -> position2d<std::common_type_t<remove_cvref_t<T>, remove_cvref_t<U>>>;

  template <typename T> requires numeric_c<T>
  position3d(T&&) -> position3d<remove_cvref_t<T>>;
  template <typename T, typename U> requires(numeric_c<T> && numeric_c<U>)
  position3d(T&&, U&&) -> position3d<std::common_type_t<remove_cvref_t<T>, remove_cvref_t<U>>>;
  template <typename T, typename U, typename V> requires(numeric_c<T> && numeric_c<U> && numeric_c<V>)
  position3d(T&&, U&&, V&&) -> position3d<std::common_type_t<remove_cvref_t<T>, remove_cvref_t<U>, remove_cvref_t<V>>>;
  template <typename T> requires numeric_c<T>
  position3d(const position2d<T>&) -> position3d<T>;
  template <typename T> requires numeric_c<T>
  position3d(position2d<T>&&) -> position3d<T>;
  template <typename T, typename U> requires(numeric_c<T> && numeric_c<U>)
  position3d(const position2d<T>&, U&&) -> position3d<std::common_type_t<T, remove_cvref_t<U>>>;
  template <typename T, typename U> requires(numeric_c<T> && numeric_c<U>)
  position3d(U&&, const position2d<T>&) -> position3d<std::common_type_t<T, remove_cvref_t<U>>>;
  template <typename T, typename U> requires(numeric_c<T> && numeric_c<U>)
  position3d(position2d<T>&&, U&&) -> position3d<std::common_type_t<T, remove_cvref_t<U>>>;
  template <typename T, typename U> requires(numeric_c<T> && numeric_c<U>)
  position3d(U&&, position2d<T>&&) -> position3d<std::common_type_t<T, remove_cvref_t<U>>>;



  template <size_t N, typename T> requires(N < 2)
  constexpr T& get(extent2d<T>& ext) noexcept {
    if constexpr (N == 0)
      return ext.width;
    else
      return ext.height;
  }
  template <size_t N, typename T> requires(N < 2)
  constexpr const T& get(const extent2d<T>& ext) noexcept {
    if constexpr (N == 0)
      return ext.width;
    else
      return ext.height;
  }
  template <size_t N, typename T> requires(N < 2)
  constexpr T&& get(extent2d<T>&& ext) noexcept {
    if constexpr (N == 0)
      return std::move(ext.width);
    else
      return std::move(ext.height);
  }
  template <size_t N, typename T> requires(N < 2)
  constexpr const T&& get(const extent2d<T>&& ext) noexcept {
    if constexpr (N == 0)
      return std::move(ext.width);
    else
      return std::move(ext.height);
  }

  template <size_t N, typename T> requires(N < 3)
  constexpr T& get(extent3d<T>& ext) noexcept {
    if constexpr (N == 0)
      return ext.width;
    else if constexpr (N == 1)
      return ext.height;
    else
      return ext.depth;
  }
  template <size_t N, typename T> requires(N < 3)
  constexpr const T& get(const extent3d<T>& ext) noexcept {
    if constexpr (N == 0)
      return ext.width;
    else if constexpr (N == 1)
      return ext.height;
    else
      return ext.depth;
  }
  template <size_t N, typename T> requires(N < 3)
  constexpr T&& get(extent3d<T>&& ext) noexcept {
    if constexpr (N == 0)
      return std::move(ext.width);
    else if constexpr (N == 1)
      return std::move(ext.height);
    else
      return std::move(ext.depth);
  }
  template <size_t N, typename T> requires(N < 3)
  constexpr const T&& get(const extent3d<T>&& ext) noexcept {
    if constexpr (N == 0)
      return std::move(ext.width);
    else if constexpr (N == 1)
      return std::move(ext.height);
    else
      return std::move(ext.depth);
  }


  template <size_t N, typename T> requires(N < 2)
  constexpr T& get(position2d<T>& pos) noexcept {
    if constexpr (N == 0)
      return pos.x;
    else
      return pos.y;
  }
  template <size_t N, typename T> requires(N < 2)
  constexpr const T& get(const position2d<T>& pos) noexcept {
    if constexpr (N == 0)
      return pos.x;
    else
      return pos.y;
  }
  template <size_t N, typename T> requires(N < 2)
  constexpr T&& get(position2d<T>&& pos) noexcept {
    if constexpr (N == 0)
      return std::move(pos.x);
    else
      return std::move(pos.y);
  }
  template <size_t N, typename T> requires(N < 2)
  constexpr const T&& get(const position2d<T>&& pos) noexcept {
    if constexpr (N == 0)
      return std::move(pos.x);
    else
      return std::move(pos.y);
  }

  template <size_t N, typename T> requires(N < 3)
  constexpr T& get(position3d<T>& pos) noexcept {
    if constexpr (N == 0)
      return pos.x;
    else if constexpr (N == 1)
      return pos.y;
    else
      return pos.z;
  }
  template <size_t N, typename T> requires(N < 3)
  constexpr const T& get(const position3d<T>& pos) noexcept {
    if constexpr (N == 0)
      return pos.x;
    else if constexpr (N == 1)
      return pos.y;
    else
      return pos.z;
  }
  template <size_t N, typename T> requires(N < 3)
  constexpr T&& get(position3d<T>&& pos) noexcept {
    if constexpr (N == 0)
      return std::move(pos.x);
    else if constexpr (N == 1)
      return std::move(pos.y);
    else
      return std::move(pos.z);
  }
  template <size_t N, typename T> requires(N < 3)
  constexpr const T&& get(const position3d<T>&& pos) noexcept {
    if constexpr (N == 0)
      return std::move(pos.x);
    else if constexpr (N == 1)
      return std::move(pos.y);
    else
      return std::move(pos.z);
  }


  template <typename OffsetType = u32, typename ExtentType = OffsetType>
  struct box2d {
    position2d<OffsetType> offsets;
    extent2d<ExtentType>   extents;
  };
  template <typename OffsetType = u32, typename ExtentType = OffsetType>
  struct box3d {
    position3d<OffsetType> offsets;
    extent3d<ExtentType>   extents;
  };

  template <typename T>
  inline constexpr static auto square(T sideLength) noexcept {
    return box2d<T>{  .offsets = { 0, 0 }, .extents = { sideLength, sideLength } };
  }
  template <typename T, typename U>
  inline constexpr static auto square(T sideLength, const position2d<U>& offset) noexcept {
    return box2d<U, T>{ .offsets = offset, .extents = {sideLength, sideLength} };
  }
  template <typename T>
  inline constexpr static auto cube(T sideLength) noexcept {
    return box3d<T>{ .offsets = { 0, 0, 0 }, .extents = { sideLength, sideLength, sideLength } };
  }
  template <typename T, typename U>
  inline constexpr static auto cube(T sideLength, const position3d<U>& offset) noexcept {
    return box3d<U, T>{ .offsets = offset, .extents = {sideLength, sideLength, sideLength} };
  }

}

template <typename T>
struct std::tuple_size<valkyrie::extent2d<T>> : std::integral_constant<size_t, 2>{};
template <typename T>
struct std::tuple_size<valkyrie::extent3d<T>> : std::integral_constant<size_t, 3>{};
template <size_t N, typename T>
struct std::tuple_element<N, valkyrie::extent2d<T>>{
  using type = T;
};
template <size_t N, typename T>
struct std::tuple_element<N, valkyrie::extent3d<T>>{
  using type = T;
};

template <typename T>
struct std::tuple_size<valkyrie::position2d<T>> : std::integral_constant<size_t, 2>{};
template <typename T>
struct std::tuple_size<valkyrie::position3d<T>> : std::integral_constant<size_t, 3>{};
template <size_t N, typename T>
struct std::tuple_element<N, valkyrie::position2d<T>>{
  using type = T;
};
template <size_t N, typename T>
struct std::tuple_element<N, valkyrie::position3d<T>>{
  using type = T;
};

#endif//VALKYRIE_UTILITY_SHAPES_HPP
