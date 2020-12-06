//
// Created by Maxwell on 2020-11-30.
//

#ifndef VALKYRIE_GRAPHICS_SHAPES_HPP
#define VALKYRIE_GRAPHICS_SHAPES_HPP

#include <valkyrie/Core/Config.hpp>
#include <valkyrie/Core/Types.hpp>
#include <valkyrie/Core/Traits.hpp>

#include <tuple>

namespace valkyrie::Graphics{
  template <typename T = u32>
  struct Extent2D{
    T width;
    T height;

    constexpr Extent2D() = default;
    constexpr Extent2D(const Extent2D&) = default;
    constexpr Extent2D(Extent2D&&) noexcept = default;

    constexpr Extent2D& operator=(const Extent2D&) = default;
    constexpr Extent2D& operator=(Extent2D&&) noexcept = default;

    template <std::convertible_to<T> A>
    explicit constexpr Extent2D(A&& a) noexcept
        : width(std::forward<A>(a)),
          height{}{}
    template <std::convertible_to<T> A, std::convertible_to<T> B>
    constexpr Extent2D(A&& a, B&& b) noexcept
        : width(std::forward<A>(a)),
          height(std::forward<B>(b)){}

    template <typename U> requires(std::constructible_from<T, const U&>)
    explicit(!std::convertible_to<const U&, T>) Extent2D(const Extent2D<U>& other) noexcept(std::is_nothrow_constructible_v<T, const U&>)
        : width(other.width),
          height(other.height){}
    template <typename U> requires(std::constructible_from<T, U&&>)
    explicit(!std::convertible_to<U&&, T>) Extent2D(Extent2D<U>&& other) noexcept(std::is_nothrow_constructible_v<T, U&&>)
    : width(std::move(other.width)),
    height(std::move(other.height)){}
  };
  template <typename T = u32>
  struct Extent3D{
    T width;
    T height;
    T depth;

    constexpr Extent3D() = default;
    constexpr Extent3D(const Extent3D&) = default;
    constexpr Extent3D(Extent3D&&) noexcept = default;

    constexpr Extent3D& operator=(const Extent3D&) = default;
    constexpr Extent3D& operator=(Extent3D&&) noexcept = default;


    template <std::convertible_to<T> A>
    explicit constexpr Extent3D(A&& a) noexcept
        : width(std::forward<A>(a)),
          height{},
          depth{}{}
    template <std::convertible_to<T> A, std::convertible_to<T> B>
    constexpr Extent3D(A&& a, B&& b) noexcept
        : width( std::forward<A>(a)),
          height(std::forward<B>(b)),
          depth{}{}
    template <std::convertible_to<T> A, std::convertible_to<T> B, std::convertible_to<T> C>
    constexpr Extent3D(A&& a, B&& b, C&& c) noexcept
        : width( std::forward<A>(a)),
          height(std::forward<B>(b)),
          depth( std::forward<C>(c)){}



    template <typename U> requires(std::constructible_from<T, const U&>)
    explicit constexpr Extent3D(const Extent2D<U>& other) noexcept(std::is_nothrow_constructible_v<T, const U&>)
        : width( other.width),
          height(other.height),
          depth{}{}
    template <typename U> requires(std::constructible_from<T, U&&>)
    explicit constexpr Extent3D(Extent2D<U>&& other) noexcept(std::is_nothrow_constructible_v<T, U&&>)
    : width( std::move(other.width)),
    height(std::move(other.height)),
    depth{}{}



    template <typename U, typename V> requires(std::constructible_from<T, const U&> && std::constructible_from<T, V>)
    constexpr Extent3D(const Extent2D<U>& other, V&& depth) noexcept(std::is_nothrow_constructible_v<T, const U&> && std::is_nothrow_constructible_v<T, V>)
        : width( other.width),
          height(other.height),
          depth( std::forward<V>(depth)){}
    template <typename U, typename V> requires(std::constructible_from<T, U&&> && std::constructible_from<T, V>)
    constexpr Extent3D(Extent2D<U>&& other, V&& depth) noexcept(std::is_nothrow_constructible_v<T, U&&> && std::is_nothrow_constructible_v<T, V>)
        : width( std::move(other.width)),
          height(std::move(other.height)),
          depth( std::forward<V>(depth)){}



    template <typename U> requires(std::constructible_from<T, const U&>)
    explicit(!std::convertible_to<const U&, T>) constexpr Extent3D(const Extent3D<U>& other) noexcept(std::is_nothrow_constructible_v<T, const U&>)
        : width( other.width),
          height(other.height),
          depth( other.depth){}
    template <typename U> requires(std::constructible_from<T, U&&>)
    explicit(!std::convertible_to<U&&, T>) constexpr Extent3D(Extent3D<U>&& other) noexcept(std::is_nothrow_constructible_v<T, U&&>)
        : width( std::move(other.width)),
          height(std::move(other.height)),
          depth( std::move(other.depth)){}
  };

  template <Numeric T>
  Extent2D(T&&) -> Extent2D<std::remove_cvref_t<T>>;
  template <Numeric T, Numeric U>
  Extent2D(T&&, U&&) -> Extent2D<std::common_type_t<std::remove_cvref_t<T>, std::remove_cvref_t<U>>>;

  template <Numeric T>
  Extent3D(T&&) -> Extent3D<std::remove_cvref_t<T>>;
  template <Numeric T, Numeric U>
  Extent3D(T&&, U&&) -> Extent3D<std::common_type_t<std::remove_cvref_t<T>, std::remove_cvref_t<U>>>;
  template <Numeric T, Numeric U, Numeric V>
  Extent3D(T&&, U&&, V&&) -> Extent3D<std::common_type_t<std::remove_cvref_t<T>, std::remove_cvref_t<U>, std::remove_cvref_t<V>>>;
  template <Numeric T>
  Extent3D(const Extent2D<T>&) -> Extent3D<T>;
  template <Numeric T>
  Extent3D(Extent2D<T>&&) -> Extent3D<T>;
  template <Numeric T, Numeric U>
  Extent3D(const Extent2D<T>&, U&&) -> Extent3D<std::common_type_t<T, std::remove_cvref_t<U>>>;
  template <Numeric T, Numeric U>
  Extent3D(U&&, const Extent2D<T>&) -> Extent3D<std::common_type_t<T, std::remove_cvref_t<U>>>;
  template <Numeric T, Numeric U>
  Extent3D(Extent2D<T>&&, U&&) -> Extent3D<std::common_type_t<T, std::remove_cvref_t<U>>>;
  template <Numeric T, Numeric U>
  Extent3D(U&&, Extent2D<T>&&) -> Extent3D<std::common_type_t<T, std::remove_cvref_t<U>>>;


  template <typename T = u32>
  struct Position2D{
    T x;
    T y;

    constexpr Position2D() = default;
    constexpr Position2D(const Position2D&) = default;
    constexpr Position2D(Position2D&&) noexcept = default;

    constexpr Position2D& operator=(const Position2D&) = default;
    constexpr Position2D& operator=(Position2D&&) noexcept = default;

    template <std::convertible_to<T> A>
    explicit constexpr Position2D(A&& a) noexcept
        : x(std::forward<A>(a)),
          y{}{}
    template <std::convertible_to<T> A, std::convertible_to<T> B>
    constexpr Position2D(A&& a, B&& b) noexcept
        : x(std::forward<A>(a)),
          y(std::forward<B>(b)){}

    template <typename U> requires(std::constructible_from<T, const U&>)
    explicit(!std::convertible_to<const U&, T>) Position2D(const Position2D<U>& other) noexcept(std::is_nothrow_constructible_v<T, const U&>)
        : x(other.x),
          y(other.y){}
    template <typename U> requires(std::constructible_from<T, U&&>)
    explicit(!std::convertible_to<U&&, T>) Position2D(Position2D<U>&& other) noexcept(std::is_nothrow_constructible_v<T, U&&>)
    : x(std::move(other.x)),
    y(std::move(other.y)){}
  };
  template <typename T = u32>
  struct Position3D{
    T x;
    T y;
    T z;

    constexpr Position3D() = default;
    constexpr Position3D(const Position3D&) = default;
    constexpr Position3D(Position3D&&) noexcept = default;

    constexpr Position3D& operator=(const Position3D&) = default;
    constexpr Position3D& operator=(Position3D&&) noexcept = default;

    template <std::convertible_to<T> A>
    explicit constexpr Position3D(A&& a) noexcept
        : x(std::forward<A>(a)),
          y{},
          z{}{}
    template <std::convertible_to<T> A, std::convertible_to<T> B>
    constexpr Position3D(A&& a, B&& b) noexcept
        : x( std::forward<A>(a)),
          y(std::forward<B>(b)),
          z{}{}
    template <std::convertible_to<T> A, std::convertible_to<T> B, std::convertible_to<T> C>
    constexpr Position3D(A&& a, B&& b, C&& c) noexcept
        : x( std::forward<A>(a)),
          y(std::forward<B>(b)),
          z( std::forward<C>(c)){}



    template <typename U> requires(std::constructible_from<T, const U&>)
    explicit constexpr Position3D(const Position2D<U>& other) noexcept(std::is_nothrow_constructible_v<T, const U&>)
        : x( other.x),
          y(other.y),
          z{}{}
    template <typename U> requires(std::constructible_from<T, U&&>)
    explicit constexpr Position3D(Position2D<U>&& other) noexcept(std::is_nothrow_constructible_v<T, U&&>)
    : x( std::move(other.x)),
    y(std::move(other.y)),
    z{}{}



    template <typename U, typename V> requires(std::constructible_from<T, const U&> && std::constructible_from<T, V>)
    constexpr Position3D(const Position2D<U>& other, V&& z) noexcept(std::is_nothrow_constructible_v<T, const U&> && std::is_nothrow_constructible_v<T, V>)
        : x( other.x),
          y(other.y),
          z( std::forward<V>(z)){}
    template <typename U, typename V> requires(std::constructible_from<T, U&&> && std::constructible_from<T, V>)
    constexpr Position3D(Position2D<U>&& other, V&& z) noexcept(std::is_nothrow_constructible_v<T, U&&> && std::is_nothrow_constructible_v<T, V>)
        : x( std::move(other.x)),
          y(std::move(other.y)),
          z( std::forward<V>(z)){}



    template <typename U> requires(std::constructible_from<T, const U&>)
    explicit(!std::convertible_to<const U&, T>) constexpr Position3D(const Position3D<U>& other) noexcept(std::is_nothrow_constructible_v<T, const U&>)
        : x( other.x),
          y(other.y),
          z( other.z){}
    template <typename U> requires(std::constructible_from<T, U&&>)
    explicit(!std::convertible_to<U&&, T>) constexpr Position3D(Position3D<U>&& other) noexcept(std::is_nothrow_constructible_v<T, U&&>)
        : x( std::move(other.x)),
          y(std::move(other.y)),
          z( std::move(other.z)){}
  };
  
  
  template <Numeric T>
  Position2D(T&&) -> Position2D<std::remove_cvref_t<T>>;
  template <Numeric T, Numeric U>
  Position2D(T&&, U&&) -> Position2D<std::common_type_t<std::remove_cvref_t<T>, std::remove_cvref_t<U>>>;

  template <Numeric T>
  Position3D(T&&) -> Position3D<std::remove_cvref_t<T>>;
  template <Numeric T, Numeric U>
  Position3D(T&&, U&&) -> Position3D<std::common_type_t<std::remove_cvref_t<T>, std::remove_cvref_t<U>>>;
  template <Numeric T, Numeric U, Numeric V>
  Position3D(T&&, U&&, V&&) -> Position3D<std::common_type_t<std::remove_cvref_t<T>, std::remove_cvref_t<U>, std::remove_cvref_t<V>>>;
  template <Numeric T>
  Position3D(const Position2D<T>&) -> Position3D<T>;
  template <Numeric T>
  Position3D(Position2D<T>&&) -> Position3D<T>;
  template <Numeric T, Numeric U>
  Position3D(const Position2D<T>&, U&&) -> Position3D<std::common_type_t<T, std::remove_cvref_t<U>>>;
  template <Numeric T, Numeric U>
  Position3D(U&&, const Position2D<T>&) -> Position3D<std::common_type_t<T, std::remove_cvref_t<U>>>;
  template <Numeric T, Numeric U>
  Position3D(Position2D<T>&&, U&&) -> Position3D<std::common_type_t<T, std::remove_cvref_t<U>>>;
  template <Numeric T, Numeric U>
  Position3D(U&&, Position2D<T>&&) -> Position3D<std::common_type_t<T, std::remove_cvref_t<U>>>;
  
  

  template <size_t N, typename T>
  constexpr T& get(Extent2D<T>& ext) noexcept {
    if constexpr (N == 0)
      return ext.width;
    else if constexpr (N == 1)
      return ext.height;
    else {
      static_assert(N < 2, "Index out of range for Extent2D");
      return ext.height;
    }
  }
  template <size_t N, typename T>
  constexpr const T& get(const Extent2D<T>& ext) noexcept {
    if constexpr (N == 0)
      return ext.width;
    else if constexpr (N == 1)
      return ext.height;
    else {
      static_assert(N < 2, "Index out of range for Extent2D");
      return ext.height;
    }
  }
  template <size_t N, typename T>
  constexpr T&& get(Extent2D<T>&& ext) noexcept {
    if constexpr (N == 0)
      return std::move(ext.width);
    else if constexpr (N == 1)
      return std::move(ext.height);
    else {
      static_assert(N < 2, "Index out of range for Extent2D");
      return std::move(ext.height);
    }
  }
  template <size_t N, typename T>
  constexpr const T&& get(const Extent2D<T>&& ext) noexcept {
    if constexpr (N == 0)
      return std::move(ext.width);
    else if constexpr (N == 1)
      return std::move(ext.height);
    else {
      static_assert(N < 2, "Index out of range for Extent2D");
      return std::move(ext.height);
    }
  }

  template <size_t N, typename T>
  constexpr T& get(Extent3D<T>& ext) noexcept {
    if constexpr (N == 0)
      return ext.width;
    else if constexpr (N == 1)
      return ext.height;
    else if constexpr (N == 2)
      return ext.depth;
    else {
      static_assert(N < 3, "Index out of range for Extent3D");
      return ext.depth;
    }
  }
  template <size_t N, typename T>
  constexpr const T& get(const Extent3D<T>& ext) noexcept {
    if constexpr (N == 0)
      return ext.width;
    else if constexpr (N == 1)
      return ext.height;
    else if constexpr (N == 2)
      return ext.depth;
    else {
      static_assert(N < 3, "Index out of range for Extent3D");
      return ext.depth;
    }
  }
  template <size_t N, typename T>
  constexpr T&& get(Extent3D<T>&& ext) noexcept {
    if constexpr (N == 0)
      return std::move(ext.width);
    else if constexpr (N == 1)
      return std::move(ext.height);
    else if constexpr (N == 2)
      return std::move(ext.depth);
    else {
      static_assert(N < 3, "Index out of range for Extent3D");
      return std::move(ext.depth);
    }
  }
  template <size_t N, typename T>
  constexpr const T&& get(const Extent3D<T>&& ext) noexcept {
    if constexpr (N == 0)
      return std::move(ext.width);
    else if constexpr (N == 1)
      return std::move(ext.height);
    else if constexpr (N == 2)
      return std::move(ext.depth);
    else {
      static_assert(N < 3, "Index out of range for Extent3D");
      return std::move(ext.depth);
    }
  }


  template <size_t N, typename T>
  constexpr T& get(Position2D<T>& ext) noexcept {
    if constexpr (N == 0)
      return ext.x;
    else if constexpr (N == 1)
      return ext.y;
    else {
      static_assert(N < 2, "Index out of range for Position2D");
      return ext.y;
    }
  }
  template <size_t N, typename T>
  constexpr const T& get(const Position2D<T>& ext) noexcept {
    if constexpr (N == 0)
      return ext.x;
    else if constexpr (N == 1)
      return ext.y;
    else {
      static_assert(N < 2, "Index out of range for Position2D");
      return ext.y;
    }
  }
  template <size_t N, typename T>
  constexpr T&& get(Position2D<T>&& ext) noexcept {
    if constexpr (N == 0)
      return std::move(ext.x);
    else if constexpr (N == 1)
      return std::move(ext.y);
    else {
      static_assert(N < 2, "Index out of range for Position2D");
      return std::move(ext.y);
    }
  }
  template <size_t N, typename T>
  constexpr const T&& get(const Position2D<T>&& ext) noexcept {
    if constexpr (N == 0)
      return std::move(ext.x);
    else if constexpr (N == 1)
      return std::move(ext.y);
    else {
      static_assert(N < 2, "Index out of range for Position2D");
      return std::move(ext.y);
    }
  }

  template <size_t N, typename T>
  constexpr T& get(Position3D<T>& ext) noexcept {
    if constexpr (N == 0)
      return ext.x;
    else if constexpr (N == 1)
      return ext.y;
    else if constexpr (N == 2)
      return ext.z;
    else {
      static_assert(N < 3, "Index out of range for Position3D");
      return ext.z;
    }
  }
  template <size_t N, typename T>
  constexpr const T& get(const Position3D<T>& ext) noexcept {
    if constexpr (N == 0)
      return ext.x;
    else if constexpr (N == 1)
      return ext.y;
    else if constexpr (N == 2)
      return ext.z;
    else {
      static_assert(N < 3, "Index out of range for Position3D");
      return ext.z;
    }
  }
  template <size_t N, typename T>
  constexpr T&& get(Position3D<T>&& ext) noexcept {
    if constexpr (N == 0)
      return std::move(ext.x);
    else if constexpr (N == 1)
      return std::move(ext.y);
    else if constexpr (N == 2)
      return std::move(ext.z);
    else {
      static_assert(N < 3, "Index out of range for Position3D");
      return std::move(ext.z);
    }
  }
  template <size_t N, typename T>
  constexpr const T&& get(const Position3D<T>&& ext) noexcept {
    if constexpr (N == 0)
      return std::move(ext.x);
    else if constexpr (N == 1)
      return std::move(ext.y);
    else if constexpr (N == 2)
      return std::move(ext.z);
    else {
      static_assert(N < 3, "Index out of range for Position3D");
      return std::move(ext.z);
    }
  }


  template <typename T = u32>
  struct Box2D{
    Position2D<T> offsets;
    Extent2D<T> extents;
  };
  template <typename T = u32>
  struct Box3D{
    Position3D<T> offsets;
    Extent3D<T> extents;
  };

  template <typename T>
  inline constexpr static auto square(T&& sideLength) noexcept {
    using V = std::remove_cvref_t<T>;
    return Box2D<V>{ .extents = {sideLength, sideLength}, .offsets = { 0, 0 } };
  }
  template <typename T, typename U>
  inline constexpr static auto square(T&& sideLength, const Position2D<U>& offset) noexcept {
    using V = std::common_type_t<std::remove_cvref_t<T>, std::remove_cvref_t<U>>;
    return Box2D<V>{ .extents = {sideLength, sideLength}, .offsets = offset };
  }
  template <typename T>
  inline constexpr static auto cube(T&& sideLength) noexcept {
    using V = std::remove_cvref_t<T>;
    return Box3D<V>{ .extents = {sideLength, sideLength, sideLength}, .offsets = { 0, 0, 0 } };
  }
  template <typename T, typename U>
  inline constexpr static auto cube(T&& sideLength, const Position3D<U>& offset) noexcept {
    using V = std::common_type_t<std::remove_cvref_t<T>, std::remove_cvref_t<U>>;
    return Box3D<V>{ .extents = {sideLength, sideLength, sideLength}, .offsets = offset };
  }

}

template <typename T>
struct std::tuple_size<valkyrie::Graphics::Extent2D<T>> : std::integral_constant<size_t, 2>{};
template <typename T>
struct std::tuple_size<valkyrie::Graphics::Extent3D<T>> : std::integral_constant<size_t, 3>{};
template <size_t N, typename T>
struct std::tuple_element<N, valkyrie::Graphics::Extent2D<T>>{
  using type = T;
};
template <size_t N, typename T>
struct std::tuple_element<N, valkyrie::Graphics::Extent3D<T>>{
  using type = T;
};

template <typename T>
struct std::tuple_size<valkyrie::Graphics::Position2D<T>> : std::integral_constant<size_t, 2>{};
template <typename T>
struct std::tuple_size<valkyrie::Graphics::Position3D<T>> : std::integral_constant<size_t, 3>{};
template <size_t N, typename T>
struct std::tuple_element<N, valkyrie::Graphics::Position2D<T>>{
  using type = T;
};
template <size_t N, typename T>
struct std::tuple_element<N, valkyrie::Graphics::Position3D<T>>{
  using type = T;
};


#endif//VALKYRIE_GRAPHICS_SHAPES_HPP
