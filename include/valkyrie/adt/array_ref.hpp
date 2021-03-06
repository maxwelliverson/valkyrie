//
// Created by Maxwell on 2021-01-12.
//

#ifndef VALKYRIE_ADT_ARRAY_REF_HPP
#define VALKYRIE_ADT_ARRAY_REF_HPP

#include <valkyrie/constants.hpp>
#include <valkyrie/traits.hpp>
#include <valkyrie/utility/casting.hpp>

namespace valkyrie{

  template <typename T, auto Extent = dynamic, auto Stride = dynamic>
  class array_ref;

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
    class ArrayRefExtentStorage<dynamic>{
      u32 Extent_ = 0;
    public:
      constexpr ArrayRefExtentStorage() = default;
      constexpr explicit ArrayRefExtentStorage(u64 extent) : Extent_(narrow_cast<u32>(extent)){}

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
    class ArrayRefStrideStorage<dynamic>{
      u32 Stride_;
    public:
      constexpr ArrayRefStrideStorage() = default;
      constexpr explicit ArrayRefStrideStorage(u64 stride) noexcept : Stride_(narrow_cast<u32>(stride)) {}

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
          : ArrayRefStrideStorage<Stride_>(str),
              pAddress(static_cast<byte_pointer>(static_cast<void_pointer>(pArray))){}

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
    using PseudoType = std::conditional_t<same_as<decltype(UStr_), dynamic_t>, U, U[UStr_]>;

    template <typename U, auto UExt_ = dynamic>
    VK_constant bool LayoutCompatible = []{
      if constexpr (Self::isDynamic() || same_as<dynamic_t, decltype(UExt_)>) {
        if constexpr (same_as<dynamic_t, decltype(Stride_)>)
          return true;
        else
          return Stride_ == sizeof(U);
      } else {
        if constexpr (same_as<dynamic_t, decltype(Stride_)>)
          return UExt_ == Extent_;
        else
          return Stride_ == sizeof(U) && UExt_ == Extent_;
      }
    }();
    template <auto UExt_>
    VK_constant bool ExtentNarrowing  = !Self::isDynamic() && same_as<decltype(UExt_), dynamic_t>;
    template <auto UStr_>
    VK_constant bool StrideNarrowing  = !Self::dynamicStride() && same_as<decltype(UStr_), dynamic_t>;
    template <auto UExt_>
    VK_constant bool ExtentCompatible = []{
      if constexpr (Self::isDynamic() || same_as<decltype(UExt_), dynamic_t>)
        return true;
      else
        return Extent_ == UExt_;
    }();
    template <auto UStr_>
    VK_constant bool StrideCompatible = []{
      if constexpr (Self::dynamicStride() || same_as<decltype(UStr_), dynamic_t>)
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
    using difference_type = i64;



    constexpr array_ref() = default;
    constexpr array_ref(const array_ref &) noexcept = default;
    constexpr array_ref(array_ref &&) noexcept = default;

    constexpr array_ref & operator=(const array_ref &) noexcept = default;
    constexpr array_ref & operator=(array_ref &&) noexcept = default;

    template <pointer_convertible_to<T> U, auto UExt_, auto UStr_>
    requires(ExtentCompatible<UExt_> && StrideCompatible<UStr_>)
    constexpr explicit(ExtentNarrowing<UExt_> || StrideNarrowing<UStr_>)
    array_ref(const array_ref<U, UExt_, UStr_>& other) : Storage(other.size(), other.stride()), pArray(other.pArray){
      VK_assert(this->size() == other.size());
      VK_assert(this->stride() == other.stride());
    }
    template <pointer_convertible_to<T> U, auto UExt_, auto UStr_>
    requires(ExtentCompatible<UExt_> && StrideCompatible<UStr_>)
    constexpr explicit(ExtentNarrowing<UExt_> || StrideNarrowing<UStr_>)
    array_ref(array_ref<U, UExt_, UStr_>&& other) noexcept : Storage(other.size(), other.stride()), pArray(other.pArray){
      VK_assert(this->size() == other.size());
      VK_assert(this->stride() == other.stride());
    }

    template <pointer_convertible_to<T> U, size_t N>
    requires(LayoutCompatible<U, N>)
    constexpr array_ref(U(&array)[N]) noexcept : Storage(N, sizeof(U)), pArray(array){}
    template <pointer_convertible_to<T> U, size_t N>
    requires(LayoutCompatible<U, N> && std::is_const_v<T>)
    constexpr array_ref(const U(&array)[N]) noexcept : Storage(N, sizeof(U)), pArray(array){}

    constexpr array_ref(T& singleValue) noexcept requires(SingleElementIsValid)
        : Storage(1, sizeof(T)), pArray(std::addressof(singleValue)){}

    template <pointer_convertible_to<T> U>
    requires(LayoutCompatible<U>)
    constexpr array_ref(U* pArray, u64 extent) noexcept : Storage(extent, sizeof(U)), pArray(pArray) {}
    template <pointer_convertible_to<T> U>
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


    template <typename C, typename U, auto UExt_, auto UStr_, pointer_convertible_to<T> V>
    constexpr array_ref(array_ref<U, UExt_, UStr_> other, V C::* pMember) noexcept requires(requires{
      { std::addressof(other.front().*pMember) } -> std::convertible_to<T*>;
    } && LayoutCompatible<PseudoType<U, UStr_>, UExt_>)
        : Storage(other.size(), other.stride()),
          pArray(std::addressof(other.front().*pMember)){}


    constexpr bool empty() const noexcept {
      return this->size() == 0;
    }



    constexpr reference front() noexcept {
      VK_assert(!empty());
      return *begin();
    }
    constexpr const_reference front() const noexcept {
      VK_assert(!empty());
      return *begin();
    }
    constexpr reference back() noexcept {
      VK_assert(!empty());
      return *rbegin();
    }
    constexpr const_reference back() const noexcept {
      VK_assert(!empty());
      return *rbegin();
    }


    inline constexpr pointer data()       noexcept requires StrideCompatible<sizeof(T)> {
      if constexpr (Self::dynamicStride()) {
        if (this->stride() != sizeof(T))
          return nullptr;
      }
      return pArray;
    }
    inline constexpr const_pointer data() const noexcept requires StrideCompatible<sizeof(T)> {
      if constexpr (Self::dynamicStride()) {
        if (this->stride() != sizeof(T))
          return nullptr;
      }
      return pArray;
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




    VK_nodiscard constexpr bool is_contiguous() const noexcept {
      return this->stride() == sizeof(T);
    }
    VK_nodiscard constexpr static bool is_always_contiguous() noexcept {
      if constexpr (same_as<dynamic_t, decltype(Stride_)>)
        return false;
      else
        return Stride_ == sizeof(T);
    }
  private:
    T* pArray;
  };

  template <typename Arr> requires(contiguous_range_c<remove_ref_t<Arr>> && !range_traits<remove_ref_t<Arr>>::is_dynamic)
  array_ref(Arr&&) -> array_ref<typename range_traits<remove_ref_t<Arr>>::element_type, range_traits<remove_ref_t<Arr>>::static_size, sizeof(typename range_traits<remove_ref_t<Arr>>::element_type)>;
  template <typename Arr> requires(contiguous_range_c<remove_ref_t<Arr>> &&  range_traits<remove_ref_t<Arr>>::is_dynamic)
  array_ref(Arr&&) -> array_ref<typename range_traits<remove_ref_t<Arr>>::element_type, dynamic, sizeof(typename range_traits<remove_ref_t<Arr>>::element_type)>;

  template <typename T>
  array_ref(T*, u64) -> array_ref<T, dynamic, sizeof(T)>;
  template <typename U, auto ExtA, auto StrA, typename T, typename C>
  requires(requires(U& value, T C::* pMem){ value.*pMem; })
  array_ref(array_ref<U, ExtA, StrA>, T C::*) -> array_ref<T, ExtA, StrA>;


  /*template <auto MemPtr, typename Arr>
  array_ref<>*/


  template <member_pointer auto MemPtr,
            pointer_convertible_to<member_pointer_class_t<decltype(MemPtr)>> T,
            auto Extent,
            auto Stride>
  auto subobject_span(const array_ref<T, Extent, Stride>& array) noexcept {
    //using element_type           = member_pointer_pointee_t<decltype(MemPtr)>;
    //constexpr static auto size   = array_traits<Arr>::size;
    //constexpr static auto stride =
    return array_ref(array, MemPtr);
  }




  template <typename T, auto Ext_ = dynamic>
  using span = array_ref<T, Ext_, sizeof(T)>;



}



#endif //VALKYRIE_ADT_ARRAY_REF_HPP

