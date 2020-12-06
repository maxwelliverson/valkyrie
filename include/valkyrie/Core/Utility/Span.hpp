//
// Created by Maxwell on 2020-11-15.
//

#ifndef VALKYRIE_SPAN_HPP
#define VALKYRIE_SPAN_HPP

#include <valkyrie/Core/Config.hpp>
#include <valkyrie/Core/Utility/Casting.hpp>

namespace valkyrie::Core{
  using Byte = std::byte;

  inline constexpr static enum class DynamicExtentType{} dynamicExtent{};
  inline constexpr static DynamicExtentType _{};


  namespace Concepts{
    template <typename T, typename E = ptrdiff_t>
    concept Extent = std::same_as<DynamicExtentType, T> || std::convertible_to<T, E>;
    template <typename T, typename E = ptrdiff_t>
    concept StrictExtent = std::same_as<DynamicExtentType, T> || std::same_as<T, E>;
  }

  template <typename T, auto Extent_ = dynamicExtent>
  class Span;
  template <typename T, size_t N>
  class CArray;


  namespace Detail{
    template <typename T, auto Extent_>
    struct SpanStorage;
    template <typename T, size_t Extent_>
    struct SpanStorage<T, Extent_>{
      T* pData;
      inline constexpr static size_t Size = Extent_;

      SpanStorage() = delete;
      template <std::contiguous_iterator Iter> requires(std::convertible_to<T&, std::iter_reference_t<Iter>>)
      constexpr explicit SpanStorage(Iter first, size_t count) noexcept
          : pData(std::addressof(*first)){
        VK_constexpr_assert(count == Extent_);
      }
      template <std::contiguous_iterator Iter, std::sized_sentinel_for<Iter> Sent>
      requires(std::convertible_to<T&, std::iter_reference_t<Iter>>)
      constexpr explicit SpanStorage(Iter first, Sent last)
          : pData(std::addressof(*first)){
        VK_constexpr_assert(Size == std::distance(first, last));
      }
      template<DynamicMemorySpan<T> Rng>
      constexpr explicit SpanStorage(Rng&& rng) noexcept
          : pData(array_traits<Rng>::template getData<T>(rng)){}
      template<StaticMemorySpan<T> Rng> requires(array_traits<Rng>::size == Extent_)
      constexpr SpanStorage(Rng&& rng) noexcept
          : pData(array_traits<Rng>::template getData<T>(rng)){}
      constexpr SpanStorage(const SpanStorage&) noexcept = default;
      constexpr SpanStorage(SpanStorage&&) noexcept = default;
    };
    template <typename T>
    struct SpanStorage<T, dynamicExtent>{
      T* pData = nullptr;
      size_t Size = 0;

      constexpr SpanStorage() = default;
      constexpr SpanStorage(const SpanStorage&) noexcept = default;
      constexpr SpanStorage(SpanStorage&&) noexcept = default;

      template <std::contiguous_iterator Iter> requires(std::convertible_to<std::iter_reference_t<Iter>, T&>)
      constexpr SpanStorage(Iter first, size_t count) noexcept
          : pData(std::addressof(*first)),
            Size(count){}
      template <std::contiguous_iterator Iter, std::sized_sentinel_for<Iter> Sent>
      requires(std::convertible_to<T&, std::iter_reference_t<Iter>>)
      constexpr SpanStorage(Iter first, Sent last)
          : pData(std::addressof(*first)),
            Size(std::distance(first, last)){}
      template <AliasedBy<T> U, auto Ext>
      constexpr SpanStorage(const SpanStorage<U, Ext>& Other) noexcept
          : pData(alias_cast<T>(Other.pData)),
            Size(Other.Size * (sizeof(U) / sizeof(T))){}
      template<typename Rng>
      requires(!std::same_as<std::remove_cvref_t<Rng>, SpanStorage> && ContiguousRange<Rng, T>)
      constexpr SpanStorage(Rng&& rng) noexcept
          : pData(alias_cast<T>(rangeData(std::forward<Rng>(rng)))),
            Size(array_traits<Rng>::getSize(rng)){}
    };
  }

  template <typename T, auto Extent_>
  class Span : Detail::SpanStorage<T, Extent_> {
    using Storage = Detail::SpanStorage<T, Extent_>;
  public:

    using element_type = T;
    using value_type = std::remove_cv_t<element_type>;
    using pointer = std::add_pointer_t<element_type>;
    using const_pointer = std::add_pointer_t<std::add_const_t<element_type>>;
    using reference = std::add_lvalue_reference_t<element_type>;
    using const_reference = std::add_lvalue_reference_t<std::add_const_t<element_type>>;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using size_type = size_t;
    using difference_type = ptrdiff_t;


    constexpr Span(const Span&) noexcept = default;
    using Storage::Storage;
    constexpr Span& operator=(const Span&) noexcept = default;


    //[ Span.Access ]

    VK_nodiscard constexpr pointer data() const noexcept{
      return this->pData;
    }

    VK_nodiscard constexpr reference front() const noexcept{
      return this->pData[0];
    }
    VK_nodiscard constexpr reference back() const noexcept{
      return this->pData[size() - 1];
    }

    VK_nodiscard constexpr reference operator[](size_t offset) const noexcept {
      return this->pData[offset];
    }


    // [ Span.Observers ]

    VK_nodiscard constexpr size_type size() const noexcept { return Storage::Size; }
    VK_nodiscard constexpr size_type sizeBytes() const noexcept { return Storage::Size * sizeof(T); }
    VK_nodiscard constexpr bool empty() const noexcept { return !Storage::Size; }


    // [ Span.Iterators ]

    VK_nodiscard constexpr iterator begin() const noexcept{
      return data();
    }
    VK_nodiscard constexpr const_iterator cbegin() const noexcept{
      return data();
    }
    VK_nodiscard constexpr iterator end() const noexcept{
      return data() + size();
    }
    VK_nodiscard constexpr const_iterator cend() const noexcept{
      return data() + size();
    }

    VK_nodiscard constexpr reverse_iterator rbegin() const noexcept {
      return reverse_iterator(end());
    }
    VK_nodiscard constexpr const_reverse_iterator crbegin() const noexcept {
      return const_reverse_iterator(cend());
    }
    VK_nodiscard constexpr reverse_iterator rend() const noexcept {
      return reverse_iterator (begin());
    }
    VK_nodiscard constexpr const_reverse_iterator crend() const noexcept {
      return const_reverse_iterator (cbegin());
    }

    template <size_t Count>
    VK_nodiscard constexpr Span<T, Count> first() noexcept {
      VK_constexpr_assert(Count <= Storage::Size);
      return {data(), Count};
    }
    VK_nodiscard constexpr Span<T> first(size_t Count) noexcept{
      VK_constexpr_assert(Count <= Storage::Size);
      return {data(), Count};
    }

    template <size_t Count>
    VK_nodiscard constexpr Span<T, Count> last() noexcept {
      VK_constexpr_assert(Count <= Storage::Size);
      return {data() + size() - Count, Count};
    }
    VK_nodiscard constexpr Span<T> last(size_t Count) noexcept{
      VK_constexpr_assert(Count <= Storage::Size);
      return {data() + size() - Count, Count};
    }

    template <size_t Offset, auto NewExt = dynamicExtent>
    VK_nodiscard constexpr auto subspan() noexcept {
      if constexpr (NewExt == dynamicExtent) {
        using return_t = std::conditional_t<Extent_ == dynamicExtent, Span<T>, Span<T, (size_t)Extent_ - Offset>>;
        return return_t{data() + Offset, size() - Offset};
      }
      else {
        VK_constexpr_assert((NewExt + Offset) <= size());
        return Span<T, NewExt>{data() + Offset, NewExt};
      }
    }
    VK_nodiscard constexpr Span<element_type, dynamicExtent> subspan(size_t Offset) noexcept {
      return {data() + Offset, size() - Offset};
    }
    VK_nodiscard constexpr Span<element_type, dynamicExtent> subspan(size_t Offset, size_t Count) noexcept {
      VK_constexpr_assert(Count + Offset <= size());
      return {data() + Offset, Count};
    }
  };

  template <StaticMemorySpan Rng>
  Span(Rng&&) -> Span<typename array_traits<Rng>::element_type, array_traits<Rng>::size>;
  template <DynamicMemorySpan Rng>
  Span(Rng&&) -> Span<typename array_traits<Rng>::element_type>;
  template <typename T>
  Span(T*, size_t) -> Span<T>;
  template <typename Iter, typename Sent>
  Span(Iter, Sent) -> Span<typename std::iterator_traits<Iter>::value_type>;


  template <typename T, size_t N>
  class CArray{

  public:

    using element_type = T;
    using value_type = std::remove_cv_t<T>;
    using pointer = std::add_pointer_t<element_type>;
    using const_pointer = std::add_pointer_t<std::add_const_t<element_type>>;
    using reference = std::add_lvalue_reference_t<element_type>;
    using const_reference = std::add_lvalue_reference_t<std::add_const_t<element_type>>;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    constexpr CArray() = default;


    //[ CArray.Access ]

    VK_nodiscard constexpr pointer data() noexcept{
      return Arr;
    }
    VK_nodiscard constexpr const_pointer data() const noexcept{
      return Arr;
    }

    VK_nodiscard constexpr reference front() noexcept{
      return Arr[0];
    }
    VK_nodiscard constexpr const_reference front() const noexcept{
      return Arr[0];
    }
    VK_nodiscard constexpr reference back() noexcept{
      return Arr[size() - 1];
    }
    VK_nodiscard constexpr const_reference back() const noexcept{
      return Arr[size() - 1];
    }

    VK_nodiscard constexpr reference operator[](size_type pos) noexcept {
      VK_constexpr_assert(pos < N);
      return Arr[pos];
    }
    VK_nodiscard constexpr const_reference operator[](size_type pos) const noexcept {
      VK_constexpr_assert(pos < N);
      return Arr[pos];
    }
    VK_nodiscard constexpr reference at(size_type pos) {
      if (pos >= N) std::terminate();
      return Arr[pos];
    }
    VK_nodiscard constexpr const_reference at(size_type pos) const {
      if (pos >= N) std::terminate();
      return Arr[pos];
    }


    // [ CArray.Observers ]

    VK_nodiscard constexpr size_type size() const noexcept { return N; }
    VK_nodiscard constexpr size_type sizeBytes() const noexcept { return N * sizeof(T); }
    VK_nodiscard constexpr bool empty() const noexcept { return !N; }


    // [ CArray.Iterators ]

    VK_nodiscard constexpr iterator begin() noexcept{
      return data();
    }
    VK_nodiscard constexpr const_iterator begin() const noexcept{
      return data();
    }
    VK_nodiscard constexpr const_iterator cbegin() const noexcept{
      return data();
    }
    VK_nodiscard constexpr iterator end() noexcept{
      return data() + size();
    }
    VK_nodiscard constexpr const_iterator end() const noexcept{
      return data() + size();
    }
    VK_nodiscard constexpr const_iterator cend() const noexcept{
      return data() + size();
    }

    VK_nodiscard constexpr reverse_iterator rbegin() noexcept {
      return reverse_iterator(end());
    }
    VK_nodiscard constexpr const_reverse_iterator rbegin() const noexcept {
      return const_reverse_iterator(end());
    }
    VK_nodiscard constexpr const_reverse_iterator crbegin() const noexcept {
      return const_reverse_iterator(cend());
    }
    VK_nodiscard constexpr reverse_iterator rend() noexcept {
      return reverse_iterator (begin());
    }
    VK_nodiscard constexpr const_reverse_iterator rend() const noexcept {
      return const_reverse_iterator (begin());
    }
    VK_nodiscard constexpr const_reverse_iterator crend() const noexcept {
      return const_reverse_iterator (cbegin());
    }


    // [ CArray.Operations ]

    constexpr void fill(const T& value) noexcept(std::is_nothrow_copy_assignable_v<T>) {
      for (auto& val : Arr) {
        val = value;
      }
    }
    constexpr void swap(CArray& Other) noexcept(std::is_nothrow_swappable_v<T>) {
      if constexpr (BitwiseMovable<T>) {
        T TmpArr[N];
        std::memcpy(TmpArr, Arr, sizeBytes());
        std::memcpy(Arr, Other.Arr, sizeBytes());
        std::memcpy(Other.Arr, TmpArr, sizeBytes());
      }
      else {
        for (size_t I = 0; I < N; ++I)
          std::swap(Arr[I], Other.Arr[I]);
      }
    }

  private:
    T Arr[N]{};
  };
}

#endif//VALKYRIE_SPAN_HPP
