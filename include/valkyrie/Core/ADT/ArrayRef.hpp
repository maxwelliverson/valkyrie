//
// Created by Maxwell on 2021-01-12.
//

#ifndef VALKYRIE_CORE_ADT_ARRAY_REF_HPP
#define VALKYRIE_CORE_ADT_ARRAY_REF_HPP

#include <valkyrie/Core/Traits.hpp>

namespace valkyrie::Core{
  inline constexpr static enum class DynamicExtentType{} dynamicExtent{};
  inline constexpr static DynamicExtentType _{};

  template <typename T, auto Extent_ = dynamicExtent, auto Stride_ = dynamicExtent>
  class ArrayRef;

  namespace Detail{
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


    template <typename Ext, typename Ind = size_t>
    concept StrictExtent = SameAs<Ext, DynamicExtentType> || SameAs<Ind, Ext>;
    template <typename Ext, typename Ind = size_t>
    concept ExtentLike = SameAs<Ext, DynamicExtentType> || std::convertible_to<Ind, Ext>;



    /*class ArrayRefSentinel{
    public:


    private:
      byte* pEndAddress;
    };*/
  }

  template <typename T, auto Extent_, auto Stride_>
  class ArrayRef : public Detail::ArrayRefStorage<Extent_, Stride_>{
    using Storage = Detail::ArrayRefStorage<Extent_, Stride_>;
    using Self = ArrayRef;
    template <typename, auto, auto>
    friend class ArrayRef;

    template <typename U, auto UStr_>
    using PseudoType = std::conditional_t<SameAs<decltype(UStr_), DynamicExtentType>, U, U[UStr_]>;

    template <typename U, auto UExt_ = dynamicExtent>
    VK_constant bool LayoutCompatible = []{
      if constexpr (Self::isDynamic() || SameAs<DynamicExtentType, decltype(UExt_)>) {
        if constexpr (SameAs<DynamicExtentType, decltype(Stride_)>)
          return true;
        else
          return Stride_ == sizeof(U);
      } else {
        if constexpr (SameAs<DynamicExtentType, decltype(Stride_)>)
          return UExt_ == Extent_;
        else
          return Stride_ == sizeof(U) && UExt_ == Extent_;
      }
    }();
    template <auto UExt_>
    VK_constant bool ExtentNarrowing  = !Self::isDynamic() && SameAs<decltype(UExt_), DynamicExtentType>;
    template <auto UStr_>
    VK_constant bool StrideNarrowing  = !Self::dynamicStride() && SameAs<decltype(UStr_), DynamicExtentType>;
    template <auto UExt_>
    VK_constant bool ExtentCompatible = []{
      if constexpr (Self::isDynamic() || SameAs<decltype(UExt_), DynamicExtentType>)
        return true;
      else
        return Extent_ == UExt_;
    }();
    template <auto UStr_>
    VK_constant bool StrideCompatible = []{
      if constexpr (Self::dynamicStride() || SameAs<decltype(UStr_), DynamicExtentType>)
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
    using iterator = Detail::ArrayRefIterator<T, Stride_>;
    using const_iterator = Detail::ArrayRefIterator<const T, Stride_>;;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using size_type = size_t;
    using difference_type = ptrdiff_t;



    constexpr ArrayRef() = default;
    constexpr ArrayRef(const ArrayRef&) noexcept = default;
    constexpr ArrayRef(ArrayRef&&) noexcept = default;

    constexpr ArrayRef& operator=(const ArrayRef&) noexcept = default;
    constexpr ArrayRef& operator=(ArrayRef&&) noexcept = default;

    template <std::derived_from<T> U, auto UExt_, auto UStr_>
    requires(ExtentCompatible<UExt_> && StrideCompatible<UStr_>)
    constexpr explicit(ExtentNarrowing<UExt_> || StrideNarrowing<UStr_>)
    ArrayRef(const ArrayRef<U, UExt_, UStr_>& other) : Storage(other.size(), other.stride()), pArray(other.pArray){
      VK_assert(this->size() == other.size());
      VK_assert(this->stride() == other.stride());
    }
    template <std::derived_from<T> U, auto UExt_, auto UStr_>
    requires(ExtentCompatible<UExt_> && StrideCompatible<UStr_>)
    constexpr explicit(ExtentNarrowing<UExt_> || StrideNarrowing<UStr_>)
    ArrayRef(ArrayRef<U, UExt_, UStr_>&& other) noexcept : Storage(other.size(), other.stride()), pArray(other.pArray){
      VK_assert(this->size() == other.size());
      VK_assert(this->stride() == other.stride());
    }

    template <std::derived_from<T> U, size_t N>
    requires(LayoutCompatible<U, N>)
    constexpr ArrayRef(U(&array)[N]) noexcept : Storage(N, sizeof(U)), pArray(array){}
    template <std::derived_from<T> U, size_t N>
    requires(LayoutCompatible<U, N> && std::is_const_v<T>)
    constexpr ArrayRef(const U(&array)[N]) noexcept : Storage(N, sizeof(U)), pArray(array){}

    constexpr ArrayRef(T& singleValue) noexcept requires(SingleElementIsValid)
        : Storage(1, sizeof(T)), pArray(std::addressof(singleValue)){}

    template <std::derived_from<T> U>
    requires(LayoutCompatible<U>)
    constexpr ArrayRef(U* pArray, u64 extent) noexcept : Storage(extent, sizeof(U)), pArray(pArray) {}
    template <std::derived_from<T> U>
    requires(LayoutCompatible<U> && !Self::isDynamic())
    constexpr explicit ArrayRef(U* pArray) noexcept : Storage(Extent_, sizeof(U)), pArray(pArray){}

    template <NotSameAs<Self> Arr>
    requires(requires(Arr& a){
      { std::ranges::data(a) } -> std::convertible_to<pointer>;
      { std::ranges::size(a) } -> std::convertible_to<u64>;
    } && StrideCompatible<sizeof(decltype(*std::ranges::data(std::declval<Arr&>())))>)
    constexpr ArrayRef(Arr&& a) noexcept
        : Storage(std::ranges::size(a), sizeof(decltype(*std::ranges::data(a)))),
          pArray(std::ranges::data(a)){}


    template <typename C, typename U, auto UExt_, auto UStr_, std::derived_from<T> V>
    constexpr ArrayRef(ArrayRef<U, UExt_, UStr_> other, V C::* pMember) noexcept requires(requires{
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
      if constexpr (SameAs<DynamicExtentType, decltype(Stride_)>)
        return false;
      else
        return Stride_ == sizeof(T);
    }
  private:
    T* pArray;
  };

  template <StaticMemorySpan Arr>
  ArrayRef(Arr&&) -> ArrayRef<typename array_traits<Arr>::element_type, array_traits<Arr>::size, sizeof(typename array_traits<Arr>::element_type)>;
  template <DynamicMemorySpan Arr>
  ArrayRef(Arr&&) -> ArrayRef<typename array_traits<Arr>::element_type, dynamicExtent, sizeof(typename array_traits<Arr>::element_type)>;
  template <typename T>
  ArrayRef(T*, u64) -> ArrayRef<T, dynamicExtent, sizeof(T)>;


  template <typename T, auto Ext_ = dynamicExtent>
  using Span = ArrayRef<T, Ext_, sizeof(T)>;
}



#endif //VALKYRIE_CORE_ADT_ARRAY_REF_HPP

