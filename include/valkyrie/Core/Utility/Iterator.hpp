//
// Created by Maxwell on 2020-11-16.
//

#ifndef VALKYRIE_UTILITY_ITERATOR_HPP
#define VALKYRIE_UTILITY_ITERATOR_HPP

#include <valkyrie/Core/Types.hpp>

#include <iterator>
#include <tuple>

namespace valkyrie::Core{
  using InputIterator = std::input_iterator_tag;
  using OutputIterator = std::output_iterator_tag;
  using ForwardIterator = std::forward_iterator_tag;
  using BidirectionalIterator = std::bidirectional_iterator_tag;
  using RandomAccessIterator = std::random_access_iterator_tag;
  using ContiguousIterator = std::contiguous_iterator_tag;
  
  namespace Detail{
    template<typename... Types>
    using CommonIterator = std::common_type_t<typename std::iterator_traits<Types>::iterator_category...>;
    template<typename... Types>
    using CommonDiffType = std::common_type_t<typename std::iterator_traits<Types>::difference_type...>;
  }

  template<typename... Types>
  class MultiIterator {
    std::tuple<Types...> Iters;

  public:
    using IteratorCategory = Detail::CommonIterator<Types...>;
    using iterator_category = IteratorCategory;

    using DifferenceType = Detail::CommonDiffType<Types...>;
    using difference_type = DifferenceType;

  private:
    inline constexpr static bool IsInputIter = std::convertible_to<IteratorCategory, InputIterator>;
    inline constexpr static bool IsOutputIter = std::convertible_to<IteratorCategory, OutputIterator>;
    inline constexpr static bool IsForwardIter = std::convertible_to<IteratorCategory, ForwardIterator>;
    inline constexpr static bool IsBidirIter = std::convertible_to<IteratorCategory, BidirectionalIterator>;
    inline constexpr static bool IsRandomIter = std::convertible_to<IteratorCategory, RandomAccessIterator>;
    inline constexpr static bool IsContigIter = std::convertible_to<IteratorCategory, ContiguousIterator>;

    inline constexpr static std::make_index_sequence<sizeof...(Types)> Indices{};


    template<size_t... N>
    VK_nodiscard VK_gpu_inline auto deref(std::index_sequence<N...>) const noexcept {
      return std::tuple{*std::get<N>(Iters)...};
    }
    template<size_t... N>
    VK_gpu_inline void inc(std::index_sequence<N...>) noexcept {
      (++std::get<N>(Iters), ...);
    }
    template<size_t... N>
    VK_gpu_inline void dec(std::index_sequence<N...>) noexcept {
      (--std::get<N>(Iters), ...);
    }

    template<size_t... N>
    VK_gpu_inline void plus_eq(DifferenceType Diff, std::index_sequence<N...>) noexcept {
      ((std::get<N>(Iters) += Diff), ...);
    }
    template<size_t... N>
    VK_gpu_inline void minus_eq(DifferenceType Diff, std::index_sequence<N...>) noexcept {
      ((std::get<N>(Iters) -= Diff), ...);
    }
    template<size_t... N>
    VK_gpu_inline decltype(auto) subscript(DifferenceType Diff, std::index_sequence<N...>) noexcept {
      return std::tuple{std::get<N>(Iters)[Diff]...};
    }
  public:
    MultiIterator(Types &&...args) noexcept :
        Iters(std::forward<Types>(args)...) {}
    MultiIterator(const std::tuple<Types...>& Tup) noexcept : Iters(Tup){}


    template <size_t N>
    VK_nodiscard VK_gpu_inline decltype(auto) get() & noexcept{
      return std::get<N>(Iters);
    }
    template <size_t N>
    VK_nodiscard VK_gpu_inline decltype(auto) get() const & noexcept{
      return std::get<N>(Iters);
    }
    template <size_t N>
    VK_nodiscard VK_gpu_inline decltype(auto) get() && noexcept{
      return std::get<N>((std::tuple<Types...>&&)Iters);
    }
    template <size_t N>
    VK_nodiscard VK_gpu_inline decltype(auto) get() const && noexcept{
      return std::get<N>((const std::tuple<Types...>&&)Iters);
    }


    VK_nodiscard VK_gpu_inline auto operator*() const noexcept requires(IsInputIter || IsOutputIter) {
      return deref(Indices);
    }
    VK_gpu_inline MultiIterator &operator++() noexcept requires(IsForwardIter) {
      inc(Indices);
      return *this;
    }
    VK_gpu_inline MultiIterator operator++(int) noexcept requires(IsForwardIter) {
      MultiIterator Copy = *this;
      inc(Indices);
      return std::move(Copy);
    }
    VK_gpu_inline MultiIterator &operator--() noexcept requires(IsBidirIter) {
      dec(Indices);
      return *this;
    }
    VK_gpu_inline MultiIterator operator--(int) noexcept requires(IsBidirIter) {
      MultiIterator Copy = *this;
      dec(Indices);
      return std::move(Copy);
    }


    VK_gpu_inline MultiIterator &operator+=(DifferenceType Diff) noexcept requires(IsRandomIter) {
      plus_eq(Diff, Indices);
      return *this;
    }
    VK_gpu_inline MultiIterator &operator-=(DifferenceType Diff) noexcept requires(IsRandomIter) {
      minus_eq(Diff, Indices);
      return *this;
    }


    VK_nodiscard VK_gpu_inline auto
    operator[](DifferenceType Diff) noexcept requires(IsRandomIter) {
      return subscript(Diff, Indices);
    }


    VK_nodiscard VK_gpu_inline friend MultiIterator
    operator+(const MultiIterator &A, DifferenceType B) noexcept requires(IsRandomIter) {
      auto Copy = A;
      Copy += B;
      return std::move(Copy);
    }
    VK_nodiscard VK_gpu_inline friend MultiIterator
    operator+(DifferenceType A, const MultiIterator &B) noexcept requires(IsRandomIter) {
      auto Copy = B;
      Copy += A;
      return std::move(Copy);
    }
    VK_nodiscard VK_gpu_inline friend MultiIterator
    operator-(const MultiIterator &A, DifferenceType B) noexcept requires(IsRandomIter) {
      auto Copy = A;
      Copy -= B;
      return std::move(Copy);
    }
    VK_nodiscard VK_gpu_inline friend MultiIterator
    operator-(DifferenceType B, const MultiIterator &A) noexcept = delete;
    VK_nodiscard VK_gpu_inline friend DifferenceType
    operator-(const MultiIterator &A, const MultiIterator &B) noexcept requires(IsRandomIter) {
      return std::get<0>(A.Iters) - std::get<0>(B.Iters);
    }


    VK_nodiscard friend bool operator==(const MultiIterator &A, const MultiIterator &B) noexcept {
      return A.Iters == B.Iters;
    }
    VK_nodiscard friend std::partial_ordering
    operator<=>(const MultiIterator &A, const MultiIterator &B) noexcept requires(IsRandomIter) {
      return A.Iters <=> B.Iters;
    }
  };

  template <typename ...Types>
  MultiIterator(Types&&...) -> MultiIterator<std::remove_cvref_t<Types>...>;


  namespace Concepts{
    template <typename T>
    concept InputIterator = std::input_iterator<T>;
    template <typename T, typename U = void>
    concept InputOrOutputIterator = InputIterator<T> || std::input_or_output_iterator<T>;
    template <typename T, typename U>
    concept OutputIterator = std::output_iterator<T, U>;
  }
}

template <typename ...Types>
struct std::tuple_size<valkyrie::Core::MultiIterator<Types...>> : std::integral_constant<size_t, sizeof...(Types)>{};
template <size_t N, typename ...Types>
struct std::tuple_element<N, valkyrie::Core::MultiIterator<Types...>> : std::tuple_element<N, std::tuple<Types...>>{};

template <typename ...Types>
struct std::tuple_size<std::reverse_iterator<valkyrie::Core::MultiIterator<Types...>>> : std::integral_constant<size_t, sizeof...(Types)>{};
template <size_t N, typename ...Types>
struct std::tuple_element<N, std::reverse_iterator<valkyrie::Core::MultiIterator<Types...>>> : std::tuple_element<N, std::tuple<Types...>>{};


#endif//VALKYRIE_UTILITY_ITERATOR_HPP
