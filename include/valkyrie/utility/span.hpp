//
// Created by Maxwell on 2020-11-15.
//

#ifndef VALKYRIE_UTILITY_SPAN_HPP
#define VALKYRIE_UTILITY_SPAN_HPP

#include <valkyrie/config.hpp>
#include <valkyrie/utility/casting.hpp>

namespace valkyrie{
  using Byte = std::byte;

  inline constexpr static enum class DynamicExtentType{} dynamicExtent{};
  inline constexpr static DynamicExtentType _{};


  namespace concepts{
    template <typename T, typename E = ptrdiff_t>
    concept Extent = exact_same_as<DynamicExtentType, T> || std::convertible_to<T, E>;
    template <typename T, typename E = ptrdiff_t>
    concept StrictExtent = exact_same_as<DynamicExtentType, T> || exact_same_as<T, E>;
  }

  template <typename T, auto Extent_ = dynamicExtent>
  class Span;
  template <typename T, auto Extent_ = dynamicExtent, auto Stride_ = dynamicExtent>
  class array_ref;
  template <typename T, size_t N>
  class CArray;



  namespace detail{
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
      template<dynamic_memory_span_type<T> Rng>
      constexpr explicit SpanStorage(Rng&& rng) noexcept
          : pData(array_traits<Rng>::template getData<T>(rng)){}
      template<static_memory_span_type<T> Rng> requires(array_traits<Rng>::size == Extent_)
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
      template <aliased_by<T> U, auto Ext>
      constexpr SpanStorage(const SpanStorage<U, Ext>& Other) noexcept
          : pData(alias_cast<T>(Other.pData)),
            Size(Other.Size * (sizeof(U) / sizeof(T))){}
      template<typename Rng>
      requires(!std::same_as<std::remove_cvref_t<Rng>, SpanStorage> && contiguous_range_type<Rng, T>)
      constexpr SpanStorage(Rng&& rng) noexcept
          : pData(alias_cast<T>(rangeData(std::forward<Rng>(rng)))),
            Size(array_traits<Rng>::getSize(rng)){}
    };
  }

  template <typename T, auto Extent_>
  class Span : detail::SpanStorage<T, Extent_> {
    using Storage = detail::SpanStorage<T, Extent_>;
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

  template <static_memory_span_type Rng>
  Span(Rng&&) -> Span<typename array_traits<Rng>::element_type, array_traits<Rng>::size>;
  template <dynamic_memory_span_type Rng>
  Span(Rng&&) -> Span<typename array_traits<Rng>::element_type>;
  template <typename T>
  Span(T*, size_t) -> Span<T>;
  template <typename Iter, typename Sent>
  Span(Iter, Sent) -> Span<typename std::iterator_traits<Iter>::value_type>;


  namespace detail{
    template <auto Extent_>
    class ArrayRefExtentStorage{
    public:
      constexpr ArrayRefExtentStorage() = default;
      constexpr explicit ArrayRefExtentStorage(u64 extent) {
        VK_assert(extent == Extent_);
      }

      inline static constexpr bool isDynamic() noexcept {
        return false;
      }

      inline constexpr u32 size() const noexcept {
        return u32(Extent_);
      }
    };
    template <>
    class ArrayRefExtentStorage<dynamicExtent>{
      u32 Extent_ = 0;
    public:
      constexpr ArrayRefExtentStorage() = default;
      constexpr explicit ArrayRefExtentStorage(u64 extent) : Extent_(extent){}

      inline static constexpr bool isDynamic() noexcept {
        return true;
      }

      inline constexpr u32 size() const noexcept {
        return Extent_;
      }
    };
    template <auto Stride_>
    class ArrayRefStrideStorage{
    public:
      constexpr ArrayRefStrideStorage() = default;
      constexpr explicit ArrayRefStrideStorage(u64 stride) {
        VK_assert(stride == Stride_);
      }

      inline constexpr static bool dynamicStride() noexcept {
        return false;
      }

      inline constexpr u32 stride() const noexcept {
        return u32(Stride_);
      }
    };
    template <>
    class ArrayRefStrideStorage<dynamicExtent>{
      u32 Stride_;
    public:
      constexpr ArrayRefStrideStorage() = default;
      constexpr explicit ArrayRefStrideStorage(u64 stride) noexcept : Stride_(stride) {}

      inline constexpr static bool dynamicStride() noexcept {
        return true;
      }

      inline constexpr u32 stride() const noexcept {
        return u32(Stride_);
      }
    };

    template <auto Extent_, auto Stride_>
    class ArrayRefStorage : public ArrayRefExtentStorage<Extent_>, public ArrayRefStrideStorage<Stride_>{
      using ExtStorage = ArrayRefExtentStorage<Extent_>;
      using StrStorage = ArrayRefStrideStorage<Stride_>;
      using Self = ArrayRefStorage;


    public:
      constexpr ArrayRefStorage() = default;
      constexpr ArrayRefStorage(u64 extent, u64 stride) noexcept requires(Self::isDynamic() && Self::dynamicStride())
          : ArrayRefExtentStorage<Extent_>(extent),
            ArrayRefStrideStorage<Stride_>(stride){}
      constexpr ArrayRefStorage(u64 extent, u64 stride) noexcept requires(!Self::isDynamic() && Self::dynamicStride())
          : ArrayRefStrideStorage<Stride_>(stride){}
      constexpr ArrayRefStorage(u64 extent, u64 stride) noexcept requires(Self::isDynamic() && !Self::dynamicStride())
          : ArrayRefExtentStorage<Extent_>(extent){}
      constexpr ArrayRefStorage(u64 extent, u64 stride) noexcept requires(!Self::isDynamic() && !Self::dynamicStride()){}

      /*constexpr explicit ArrayRefStorage(u64 extent) noexcept requires(Self::isDynamic && !Self::dynamicStride())
          : ArrayRefExtentStorage<Extent_>(extent){}
      constexpr explicit ArrayRefStorage(u64 stride) noexcept requires(!Self::isDynamic && Self::dynamicStride())
          : ArrayRefStrideStorage<Stride_>(stride){}*/
    };

    template <typename T, auto Stride_>
    class ArrayRefIterator : ArrayRefStrideStorage<Stride_>{
      using Self = ArrayRefIterator;
    public:

      using element_type = T;
      using value_type = std::remove_cv_t<T>;
      using size_type = u32;
      using difference_type = i32;
      using iterator_category = decltype([]{
        if constexpr (!Self::dynamicStride()) {
          if constexpr (Stride_ == sizeof(T))
           return std::contiguous_iterator_tag{};
          else
            return std::random_access_iterator_tag{};
        }
        else
          return std::random_access_iterator_tag{};
      }());
      using iterator_concept  = iterator_category;
      using pointer = T*;
      using const_pointer = const T*;
      using reference = T&;
      using const_reference = const T&;

    private:
      using byte_pointer = std::conditional_t<std::is_const_v<element_type>, const byte*, byte*>;
      using void_pointer = std::conditional_t<std::is_const_v<element_type>, const void*, void*>;

      inline auto cast(byte_pointer p) const noexcept {
        return static_cast<pointer>(static_cast<void_pointer>(pAddress));
      }



    public:

      ArrayRefIterator(const Self&) = default;
      explicit ArrayRefIterator(pointer pArray, ArrayRefStrideStorage<Stride_> str) noexcept
          : ArrayRefStrideStorage<Stride_>(str), pAddress(static_cast<byte_pointer>(static_cast<void_pointer>(pArray))){}

      reference operator*() noexcept {
        return *cast(pAddress);
      }
      const_reference operator*() const noexcept {
        return *cast(pAddress);
      }
      reference operator[](difference_type offset) noexcept {
        return *cast(pAddress + (offset * this->stride()));
      }
      const_reference operator[](difference_type offset) const noexcept {
        return *cast(pAddress + (offset * this->stride()));
      }
      pointer operator->() noexcept{
        return cast(pAddress);
      }
      const_pointer operator->() const noexcept{
        return cast(pAddress);
      }

      Self& operator++() noexcept{
        pAddress += this->stride();
        return *this;
      }
      Self operator++(int) noexcept{
        Self copy = *this;
        ++*this;
        return copy;
      }
      Self& operator--() noexcept{
        pAddress -= this->stride();
        return *this;
      }
      Self operator--(int) noexcept{
        Self copy = *this;
        --*this;
        return copy;
      }

      Self& operator+=(difference_type offset) noexcept {
        pAddress += (offset * this->stride());
        return *this;
      }
      Self& operator-=(difference_type offset) noexcept {
        pAddress -= (offset * this->stride());
        return *this;
      }


      friend Self operator+(const Self a, difference_type offset) noexcept {
        Self copy = a;
        copy += offset;
        return copy;
      }
      friend Self operator-(const Self a, difference_type offset) noexcept {
        Self copy = a;
        copy -= offset;
        return copy;
      }
      friend difference_type operator-(const Self a, const Self b) noexcept {
        VK_assert(a.stride() == b.stride());
        return (a.pAddress - b.pAddress) / a.stride();
      }

      friend bool operator==(const Self a, const Self b) noexcept{
        return a.pAddress == b.pAddress;
      }
      friend auto operator<=>(const Self a, const Self b) noexcept{
        return a.pAddress <=> b.pAddress;
      }

    private:

      using byte_pointer = std::conditional_t<std::is_const_v<element_type>, const byte*, byte*>;
      byte_pointer pAddress;
    };
    /*class ArrayRefSentinel{
    public:


    private:
      byte* pEndAddress;
    };*/
  }

  template <typename T, auto Extent_, auto Stride_>
  class array_ref : public detail::ArrayRefStorage<Extent_, Stride_>{
    using Storage = detail::ArrayRefStorage<Extent_, Stride_>;
    using Self = array_ref;
    template <typename, auto, auto>
    friend class array_ref;

    template <typename U, auto UStr_>
    using PseudoType = std::conditional_t<same_as<decltype(UStr_), DynamicExtentType>, U, U[UStr_]>;

    template <typename U, auto UExt_ = dynamicExtent>
    VK_constant bool LayoutCompatible = []{
      if constexpr (Self::isDynamic() || same_as<DynamicExtentType, decltype(UExt_)>) {
        if constexpr (same_as<DynamicExtentType, decltype(Stride_)>)
          return true;
        else
          return Stride_ == sizeof(U);
      } else {
        if constexpr (same_as<DynamicExtentType, decltype(Stride_)>)
          return UExt_ == Extent_;
        else
          return Stride_ == sizeof(U) && UExt_ == Extent_;
      }
    }();
    template <auto UExt_>
    VK_constant bool ExtentNarrowing  = !Self::isDynamic() && same_as<decltype(UExt_), DynamicExtentType>;
    template <auto UStr_>
    VK_constant bool StrideNarrowing  = !Self::dynamicStride() && same_as<decltype(UStr_), DynamicExtentType>;
    template <auto UExt_>
    VK_constant bool ExtentCompatible = []{
      if constexpr (Self::isDynamic() || same_as<decltype(UExt_), DynamicExtentType>)
        return true;
      else
        return Extent_ == UExt_;
    }();
    template <auto UStr_>
    VK_constant bool StrideCompatible = []{
      if constexpr (Self::dynamicStride() || same_as<decltype(UStr_), DynamicExtentType>)
        return true;
      else
        return Stride_ == UStr_;
    }();

    VK_constant bool SingleElementIsValid = []{
      if constexpr (Self::isDynamic())
        return true;
      else
        return Extent_ == 1;
    }();


  public:
    using element_type = T;
    using value_type = std::remove_cv_t<element_type>;
    using pointer = std::add_pointer_t<element_type>;
    using const_pointer = std::add_pointer_t<std::add_const_t<element_type>>;
    using reference = std::add_lvalue_reference_t<element_type>;
    using const_reference = std::add_lvalue_reference_t<std::add_const_t<element_type>>;
    using iterator = detail::ArrayRefIterator<T, Stride_>;
    using const_iterator = detail::ArrayRefIterator<const T, Stride_>;;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using size_type = size_t;
    using difference_type = ptrdiff_t;



    constexpr array_ref() = default;
    constexpr array_ref(const array_ref&) noexcept = default;
    constexpr array_ref(array_ref&&) noexcept = default;

    constexpr array_ref& operator=(const array_ref&) noexcept = default;
    constexpr array_ref& operator=(array_ref&&) noexcept = default;

    template <std::derived_from<T> U, auto UExt_, auto UStr_>
    requires(ExtentCompatible<UExt_> && StrideCompatible<UStr_>)
    constexpr explicit(ExtentNarrowing<UExt_> || StrideNarrowing<UStr_>)
    array_ref(const array_ref<U, UExt_, UStr_>& other) : Storage(other.size(), other.stride()), pArray(other.pArray){
      VK_assert(this->size() == other.size());
      VK_assert(this->stride() == other.stride());
    }
    template <std::derived_from<T> U, auto UExt_, auto UStr_>
    requires(ExtentCompatible<UExt_> && StrideCompatible<UStr_>)
    constexpr explicit(ExtentNarrowing<UExt_> || StrideNarrowing<UStr_>)
    array_ref(array_ref<U, UExt_, UStr_>&& other) noexcept : Storage(other.size(), other.stride()), pArray(other.pArray){
      VK_assert(this->size() == other.size());
      VK_assert(this->stride() == other.stride());
    }

    template <std::derived_from<T> U, size_t N>
    requires(LayoutCompatible<U, N>)
    constexpr array_ref(U(&array)[N]) noexcept : Storage(N, sizeof(U)), pArray(array){}
    template <std::derived_from<T> U, size_t N>
    requires(LayoutCompatible<U, N> && std::is_const_v<T>)
    constexpr array_ref(const U(&array)[N]) noexcept : Storage(N, sizeof(U)), pArray(array){}

    constexpr array_ref(T& singleValue) noexcept requires(SingleElementIsValid)
        : Storage(1, sizeof(T)), pArray(std::addressof(singleValue)){}

    template <std::derived_from<T> U>
    requires(LayoutCompatible<U>)
    constexpr array_ref(U* pArray, u64 extent) noexcept : Storage(extent, sizeof(U)), pArray(pArray) {}
    template <std::derived_from<T> U>
    requires(LayoutCompatible<U> && !Self::isDynamic())
    constexpr explicit array_ref(U* pArray) noexcept : Storage(Extent_, sizeof(U)), pArray(pArray){}

    template <not_same_as<Self> Arr>
    requires(requires(Arr& a){
      { std::ranges::data(a) } -> std::convertible_to<pointer>;
      { std::ranges::size(a) } -> std::convertible_to<u64>;
    } && StrideCompatible<sizeof(decltype(*std::ranges::data(std::declval<Arr&>())))>)
    constexpr array_ref(Arr&& a) noexcept
        : Storage(std::ranges::size(a), sizeof(decltype(*std::ranges::data(a)))),
          pArray(std::ranges::data(a)){}


    template <typename C, typename U, auto UExt_, auto UStr_, std::derived_from<T> V>
    constexpr array_ref(array_ref<U, UExt_, UStr_> other, V C::* pMember) noexcept requires(requires{
      { std::addressof(other.front().*pMember) } -> std::convertible_to<T*>;
    } && LayoutCompatible<PseudoType<U, UStr_>, UExt_>)
        : Storage(other.size(), other.stride()), pArray(std::addressof(other.front().*pMember)){}


    constexpr bool empty() const noexcept {
      return this->size() == 0;
    }



    reference front() noexcept {
      VK_assert(!empty());
      return *begin();
    }
    const_reference front() const noexcept {
      VK_assert(!empty());
      return *begin();
    }
    reference back() noexcept {
      VK_assert(!empty());
      return *rbegin();
    }
    const_reference back() const noexcept {
      VK_assert(!empty());
      return *rbegin();
    }


    inline iterator        begin()       noexcept {
      return iterator(pArray, *this);
    }
    inline const_iterator  begin() const noexcept {
      return iterator(pArray, *this);
    }
    inline const_iterator cbegin() const noexcept {
      return iterator(pArray, *this);
    }

    inline iterator        end()       noexcept {
      return begin() + this->size();
    }
    inline const_iterator  end() const noexcept {
      return begin() + this->size();
    }
    inline const_iterator cend() const noexcept {
      return cbegin() + this->size();
    }

    inline reverse_iterator        rbegin()       noexcept {
      return reverse_iterator(end());
    }
    inline const_reverse_iterator  rbegin() const noexcept {
      return reverse_iterator(end());
    }
    inline const_reverse_iterator crbegin() const noexcept {
      return reverse_iterator(cend());
    }

    inline reverse_iterator        rend()       noexcept {
      return reverse_iterator(begin());
    }
    inline const_reverse_iterator  rend() const noexcept {
      return reverse_iterator(begin());
    }
    inline const_reverse_iterator crend() const noexcept {
      return reverse_iterator(cbegin());
    }




    inline constexpr bool isContiguous() const noexcept {
      return this->stride() == sizeof(T);
    }
    inline constexpr static bool isAlwaysContiguous() noexcept {
      if constexpr (same_as<DynamicExtentType, decltype(Stride_)>)
        return false;
      else
        return Stride_ == sizeof(T);
    }
  private:
    T* pArray;
  };

  template <static_memory_span_type Arr>
  array_ref(Arr&&) -> array_ref<typename array_traits<Arr>::element_type, array_traits<Arr>::size, sizeof(typename array_traits<Arr>::element_type)>;
  template <dynamic_memory_span_type Arr>
  array_ref(Arr&&) -> array_ref<typename array_traits<Arr>::element_type, dynamicExtent, sizeof(typename array_traits<Arr>::element_type)>;
  template <typename T>
  array_ref(T*, u64) -> array_ref<T, dynamicExtent, sizeof(T)>;

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
      if constexpr (bitwise_movable<T>) {
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

#endif//VALKYRIE_UTILITY_SPAN_HPP
