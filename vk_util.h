//
// Created by Maxwell on 2020-11-04.
//

#ifndef VULKAN_WRAPPER_VK_UTIL_H
#define VULKAN_WRAPPER_VK_UTIL_H

#include <type_traits>
#include <string>
#include <string_view>
#include <vector>
#include <set>
#include <algorithm>
#include <stdexcept>
#include <variant>
#include <ranges>
#include <optional>
#include <span>

namespace Vk{
  template <typename T>
  concept Enum = std::is_enum_v<T>;


  template <Enum E>
  class BitFlags {
      using UnderlyingType = std::underlying_type_t<E>;


  public:
      constexpr BitFlags() noexcept = default;
      constexpr BitFlags(const BitFlags&) noexcept = default;
      constexpr BitFlags(BitFlags&&) noexcept = default;

      constexpr BitFlags(E enumValue) noexcept : m_bits_(static_cast<UnderlyingType>(enumValue)) {}
      constexpr explicit BitFlags(UnderlyingType intValue) noexcept : m_bits_(intValue){}


      constexpr BitFlags& operator=(const BitFlags&) noexcept = default;
      constexpr BitFlags& operator=(BitFlags&&) noexcept = default;


      constexpr BitFlags& operator&=(const BitFlags& Other) noexcept {
          m_bits_ &= Other.m_bits_;
          return *this;
      }
      constexpr BitFlags& operator|=(const BitFlags& Other) noexcept {
          m_bits_ |= Other.m_bits_;
          return *this;
      }
      constexpr BitFlags& operator^=(const BitFlags& Other) noexcept {
          m_bits_ ^= Other.m_bits_;
          return *this;
      }


      constexpr operator E() const noexcept { return static_cast<E>(m_bits_); }
      constexpr explicit operator UnderlyingType() const noexcept {
          return m_bits_;
      }
      constexpr explicit operator bool() const noexcept { return m_bits_; }


      friend constexpr BitFlags operator~(const BitFlags& Flags) noexcept { return BitFlags{ ~Flags.m_bits_ }; }
      friend constexpr BitFlags operator&(const BitFlags& A, const BitFlags& B) noexcept { return BitFlags{ A.m_bits_ & B.m_bits_ }; }
      friend constexpr BitFlags operator|(const BitFlags& A, const BitFlags& B) noexcept { return BitFlags{ A.m_bits_ | B.m_bits_ }; }
      friend constexpr BitFlags operator^(const BitFlags& A, const BitFlags& B) noexcept { return BitFlags{ A.m_bits_ ^ B.m_bits_ }; }


  private:
      UnderlyingType m_bits_;
  };
  template <std::totally_ordered T, typename Alloc = std::allocator<T>>
  class FlatSet {
      inline constexpr static bool is_register_copyable_v =
              std::is_trivially_destructible_v<T> &&
              std::is_trivially_copyable_v<T> &&
              (sizeof(T) <= 16);

      using in_param_t = std::conditional_t<is_register_copyable_v, T, const T&>;
      using container_type = std::vector<T, Alloc>;
  public:
      using value_type = T;
      using allocator_type = Alloc;
      using size_type = size_t;
      using iterator = typename container_type::iterator;
      using const_iterator = typename container_type::const_iterator;
      using reverse_iterator = std::reverse_iterator<iterator>;
      using const_reverse_iterator = std::reverse_iterator<const_iterator>;
      using reference = typename container_type::reference;
      using const_reference = typename container_type::const_reference;

      using maybe_iterator = std::optional<iterator>;
      using maybe_const_iterator = std::optional<const_iterator>;

      FlatSet() = default;
      FlatSet(std::initializer_list<T> List) : m_set_(List) {
          priv_prune_();
      }


      FlatSet& operator&=(const FlatSet& Other) noexcept {
          container_type result;
        std::set_intersection(m_set_.begin(), m_set_.end(), Other.m_set_.begin(), Other.m_set_.end(), std::back_inserter(result));
          //std::ranges::set_intersection(m_set_, Other.m_set_, std::back_inserter(result));
          m_set_.swap(result);
          return *this;
      }
      FlatSet& operator|=(const FlatSet& Other) noexcept {
          container_type result;
        std::set_union(m_set_.begin(), m_set_.end(), Other.m_set_.begin(), Other.m_set_.end(), std::back_inserter(result));
          //std::ranges::set_union(m_set_, Other.m_set_, std::back_inserter(result));
          m_set_.swap(result);
          return *this;
      }
      FlatSet& operator^=(const FlatSet& Other) noexcept {
          container_type result;
          std::set_symmetric_difference(m_set_.begin(), m_set_.end(), Other.m_set_.begin(), Other.m_set_.end(), std::back_inserter(result));
          //std::ranges::set_symmetric_difference(m_set_, Other.m_set_, std::back_inserter(result));
          m_set_.swap(result);
          return *this;
      }





      bool empty() const noexcept { return m_set_.empty(); }
      size_type size() const noexcept { return m_set_.size(); }

      T* data() noexcept { return m_set_.data(); }
      const T* data() const noexcept { return m_set_.data(); }

      iterator        begin()       noexcept { return m_set_.begin(); }
      const_iterator  begin() const noexcept { return m_set_.begin(); }
      const_iterator cbegin() const noexcept { return m_set_.cbegin(); }

      iterator        end()       noexcept { return m_set_.end(); }
      const_iterator  end() const noexcept { return m_set_.end(); }
      const_iterator cend() const noexcept { return m_set_.cend(); }

      reverse_iterator        rbegin()       noexcept { return reverse_iterator(end()); }
      const_reverse_iterator  rbegin() const noexcept { return const_reverse_iterator(end()); }
      const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }

      reverse_iterator        rend()       noexcept { return reverse_iterator(begin()); }
      const_reverse_iterator  rend() const noexcept { return const_reverse_iterator(begin()); }
      const_reverse_iterator crend() const noexcept { return const_reverse_iterator(cbegin()); }

      size_type count(in_param_t Value) const noexcept {
          return this->contains(Value);
      }
      template <std::totally_ordered_with<T> Key>
      size_type count(const Key& Value) const noexcept {
          return this->contains(Value);
      }


      bool contains(in_param_t Value) const noexcept {
        return std::binary_search(m_set_.begin(), m_set_.end(), Value);
          //return std::ranges::binary_search(m_set_, Value);
      }
      template <std::totally_ordered_with<T> Key>
      bool contains(const Key& Value) const noexcept {
        return std::binary_search(m_set_.begin(), m_set_.end(), Value);
      }


      iterator       find(in_param_t Value) noexcept {
          iterator result = this->lower_bound(Value);
          if (result == end() || *result != Value) return end();
          return result;
      }
      const_iterator find(in_param_t Value) const noexcept {
          const_iterator result = this->lower_bound(Value);
          if (result == cend() || *result != Value) return cend();
          return result;
      }
      template <std::totally_ordered_with<T> Key>
      iterator       find(const Key& Value) noexcept {
          iterator result = this->lower_bound(Value);
          if (result == end() || *result != Value) return end();
          return result;
      }
      template <std::totally_ordered_with<T> Key>
      const_iterator find(const Key& Value) const noexcept {
          const_iterator result = this->lower_bound(Value);
          if (result == cend() || *result != Value) return cend();
          return result;
      }


      maybe_iterator insert(in_param_t Value) noexcept {
          //iterator iter = std::ranges::lower_bound(m_set_, Value);
        iterator iter = std::lower_bound(m_set_.begin(), m_set_.end(), Value);
        if (iter != end() && *iter == Value) return std::nullopt;
        return m_set_.insert(iter, Value);
      }
      template <std::input_iterator I, std::sentinel_for<I> S> requires(std::convertible_to<std::iter_reference_t<I>, T>)
      void           insert(I&& first, S&& last) noexcept {
        while(first != last) {
          m_set_.push_back(*first);
          ++first;
        }
          //m_set_.insert(first, last);
        priv_prune_();
      }
      template <std::ranges::range Rng>  requires(std::convertible_to<std::ranges::range_reference_t<Rng>, T>)
      void           insert(Rng&& range) noexcept {
          this->insert(std::ranges::begin(range), std::ranges::end(range));
      }


      iterator erase(const_iterator at) {
          return m_set_.erase(at);
      }
      iterator erase(const_iterator from, const_iterator to) noexcept {
          return m_set_.erase(from, to);
      }
      bool     erase(in_param_t Val) noexcept {
          iterator position = this->find(Val);
          if (position == end()) return false;
          m_set_.erase(position);
          return true;
      }
      template <std::totally_ordered_with<T> Key>
      bool     erase(const Key& Val) noexcept {
          iterator position = this->find(Val);
          if (position == end()) return false;
          m_set_.erase(position);
          return true;
      }





      iterator       lower_bound(in_param_t Val) noexcept {
        return std::lower_bound(m_set_.begin(), m_set_.end(), Val);
      }
      const_iterator lower_bound(in_param_t Val) const noexcept {
        return std::lower_bound(m_set_.begin(), m_set_.end(), Val);
      }
      template <std::totally_ordered_with<T> Key>
      iterator       lower_bound(const Key& Val) noexcept {
        return std::lower_bound(m_set_.begin(), m_set_.end(), Val);
      }
      template <std::totally_ordered_with<T> Key>
      const_iterator lower_bound(const Key& Val) const noexcept {
        return std::lower_bound(m_set_.begin(), m_set_.end(), Val);
      }


      iterator       upper_bound(in_param_t Val) noexcept {
        return std::upper_bound(m_set_.begin(), m_set_.end(), Val);
      }
      const_iterator upper_bound(in_param_t Val) const noexcept {
        return std::upper_bound(m_set_.begin(), m_set_.end(), Val);
      }
      template <std::totally_ordered_with<T> Key>
      iterator       upper_bound(const Key& Val) noexcept {
        return std::upper_bound(m_set_.begin(), m_set_.end(), Val);
      }
      template <std::totally_ordered_with<T> Key>
      const_iterator upper_bound(const Key& Val) const noexcept {
        return std::upper_bound(m_set_.begin(), m_set_.end(), Val);
      }


      void swap(FlatSet& Other) noexcept {
          m_set_.swap(Other.m_set_);
      }


      allocator_type get_allocator() const noexcept { return m_set_.get_allocator(); }




      friend FlatSet operator&(const FlatSet& A, const FlatSet& B) noexcept {
          FlatSet C;
          std::set_intersection(A.m_set_.front(), A.m_set_.end(), B.m_set_.begin(), B.m_set_.end(), std::back_inserter(C.m_set_));
          C.priv_prune_();
          return std::move(C);
      }
      friend FlatSet operator|(const FlatSet& A, const FlatSet& B) noexcept {
        FlatSet C;
        std::set_union(A.m_set_.front(), A.m_set_.end(), B.m_set_.begin(), B.m_set_.end(), std::back_inserter(C.m_set_));
        C.priv_prune_();
        return std::move(C);
      }
      friend FlatSet operator^(const FlatSet& A, const FlatSet& B) noexcept {
        FlatSet C;
        std::set_symmetric_difference(A.m_set_.front(), A.m_set_.end(), B.m_set_.begin(), B.m_set_.end(), std::back_inserter(C.m_set_));
        C.priv_prune_();
        return std::move(C);
      }


      friend bool operator==(const FlatSet& A, const FlatSet& B) noexcept {
          return std::ranges::equal(A, B);
      }
      friend auto operator<=>(const FlatSet& A, const FlatSet& B) noexcept {
          auto&& [iterA, iterB] = std::ranges::mismatch(A, B);
          if (iterA == A.end() || iterB == B.end()) return A.size() <=> B.size();
          return *iterA <=> *iterB;
      }

  private:
      inline void priv_sort_() noexcept {
          std::sort(m_set_.begin(), m_set_.end());
      }
      inline void priv_unique_() noexcept {
          m_set_.erase(std::unique(m_set_.begin(), m_set_.end()), m_set_.end());
      }
      inline void priv_prune_() noexcept {
          priv_sort_();
          priv_unique_();
      }

      container_type m_set_;
  };

  class UniqueString {
    const char* pString = "";

  public:
    constexpr UniqueString() noexcept = default;
    constexpr UniqueString(const char* pString) noexcept : pString(pString) {}


    constexpr const char* data() const noexcept { return pString; }
    constexpr size_t      size() const noexcept { return std::char_traits<char>::length(pString); }

    constexpr std::string_view view() const noexcept { return { data(), size() }; }

    friend constexpr bool operator==(const UniqueString A, const UniqueString B) noexcept {
      return A.view() == B.view();
    }
    friend constexpr bool operator!=(const UniqueString A, const UniqueString B) noexcept {
      return A.view() != B.view();
    }
    friend constexpr bool operator<(const UniqueString A, const UniqueString B) noexcept {
      return A.view() < B.view();
    }
    friend constexpr bool operator>(const UniqueString A, const UniqueString B) noexcept {
      return A.view() > B.view();
    }
    friend constexpr bool operator<=(const UniqueString A, const UniqueString B) noexcept {
      return A.view() <= B.view();
    }
    friend constexpr bool operator>=(const UniqueString A, const UniqueString B) noexcept {
      return A.view() >= B.view();
    }
  };

  template <size_t N>
  class StringLiteral{
    const char(&literal)[N];
  public:
    constexpr StringLiteral() = delete;
    constexpr StringLiteral(const char(&literal)[N]) noexcept : literal(literal){}
    constexpr StringLiteral(const StringLiteral&) noexcept = default;


    [[nodiscard]] inline constexpr const char* data() const noexcept{
      return literal;
    }

    [[nodiscard]] inline constexpr char front() const noexcept { return literal[0]; }
    [[nodiscard]] inline constexpr char back() const noexcept { return literal[size()]; }

    [[nodiscard]] inline constexpr const char* begin() const noexcept{
      return literal;
    }
    [[nodiscard]] inline constexpr const char* end() const noexcept{
      return literal + size();
    }
    [[nodiscard]] inline constexpr const char* cbegin() const noexcept{
      return literal;
    }
    [[nodiscard]] inline constexpr const char* cend() const noexcept{
      return literal + size();
    }

    [[nodiscard]] inline constexpr bool empty() const noexcept{
      return N == 1;
    }
    [[nodiscard]] inline constexpr size_t size() const noexcept{
      return N - 1;
    }

    [[nodiscard]] friend inline constexpr bool operator==(StringLiteral A, StringLiteral B) noexcept{
      return std::ranges::equal(A, B);
    }
    [[nodiscard]] friend inline constexpr std::strong_ordering operator<=>(StringLiteral A, StringLiteral B) noexcept{
      switch(std::char_traits<char>::compare(A.data(), B.data())) {
        case -1:
          return std::strong_ordering::less;
        case 0:
          return std::strong_ordering::equal;
        case 1:
          return std::strong_ordering::greater;
        default:
          __assume(0);
      }
    }

    friend std::ostream& operator<<(std::ostream& os, StringLiteral str) noexcept{
      return os << str.literal;
    }
  };
  template <size_t N>
  StringLiteral(const char(&)[N]) -> StringLiteral<N>;

  class Extent2D{
  public:
    uint32_t width;
    uint32_t height;

    friend bool operator==(Extent2D A, Extent2D B) noexcept = default;
    friend std::partial_ordering operator<=>(Extent2D A, Extent2D B) noexcept {
      auto widthComp = A.width <=> B.width;
      auto heightComp = A.height <=> B.height;
      if (widthComp == heightComp)
        return widthComp;
      return std::partial_ordering::unordered;
    }
  };


  /*class Serializer{
  public:

  };*/


  /*class BadOwnershipException : public std::runtime_error {
      const void* pFirstOwner;
      const void* pSecondOwner;
      const void* pOwnedObject;

      BadOwnershipException(const void* pFirst, const void* pSecond, const void* pOwned, std::string Msg) noexcept :
              std::runtime_error(std::move(Msg)), pFirstOwner(pFirst), pSecondOwner(pSecond), pOwnedObject(pOwned) {}
  public:
      BadOwnershipException(const void* pFirst, const void* pSecond, const void* pOwned) noexcept :
              BadOwnershipException(pFirst, pSecond, pOwned,
                                    std::to_string((uintptr_t)pSecond) +
                                    " attempted to claim ownership of " +
                                    std::to_string((uintptr_t)pOwned) +
                                    ", which was already owned by " +
                                    std::to_string((uintptr_t)pFirst)) {}
  };


  template <typename ...T>
  class OwnedBy;
  template <typename ...T>
  class Owns;
  template <typename T>
  class OwnsImpl;
  template <typename Derived, typename T>
  class OwnedByImpl;

  template <typename T>
  class Many;
  template <typename T>
  class AFew;
  template <typename ...Args>
  class OneOf;
  template <typename ...Args>
  class AnyNumberOf;


  template <typename Derived, typename T>
  class OwnedByImpl {
      T* pOwner{};
  public:
      void giveTo(T* pOwner) {
          if (this->pOwner)
              throw new std::runtime_error("bad giveTo call");
          this->pOwner = pOwner;
      }
      void takeFrom(T* pOwner) {
          if (this->pOwner != pOwner) {
              throw new std::runtime_error("bad takeFrom call");
          }
          static_cast<Derived*>(this)->~Derived();
      }
      void transferTo(T* pFrom, T* pTo) {
          if (pOwner != pFrom)
              throw new std::runtime_error("bad transferTo call");
          pOwner = pTo;
      }
      bool ownedBy(T* pOwner) const noexcept {
          return this->pOwner == pOwner;
      }
  };
  template <typename Derived, typename ...T>
  class OwnedByImpl<Derived, OneOf<T...>> {

      using Variant = std::variant<T*...>;

      Variant pOwner{};
  public:
      void giveTo(Variant pOwner) {
          if (this->pOwner.index() != std::variant_npos)
              throw new std::runtime_error("bad giveTo call");
          this->pOwner = pOwner;
      }
      void takeFrom(Variant pOwner) {
          if (this->pOwner != pOwner) {
              throw new std::runtime_error("bad takeFrom call");
          }
          static_cast<Derived*>(this)->~Derived();
      }
      void transferTo(Variant pFrom, Variant pTo) {
          if (pOwner != pFrom)
              throw new std::runtime_error("bad transferTo call");
          pOwner = pTo;
      }
      bool ownedBy(Variant pOwner) const noexcept {
          return this->pOwner == pOwner;
      }
  };
  template <typename Derived, typename ...T>
  class OwnedByImpl<Derived, AnyNumberOf<T...>> {

      using Variant = std::variant<T*...>;

      FlatSet<Variant> owners_{};
  public:
      void giveTo(Variant pOwner) {
          if (!owners_.insert(pOwner))
              throw std::runtime_error("bad giveTo call");
      }
      void takeFrom(Variant pOwner) {
          if (!owners_.erase(pOwner))
              throw std::runtime_error("bad takeFrom call");
          if (owners_.empty()) {
              static_cast<Derived*>(this)->~Derived();
          }
      }
      void transferTo(Variant pFrom, Variant pTo) {
          if (!owners_.erase(pFrom) || !owners_.insert(pTo))
              throw std::runtime_error("bad transferTo call");
      }


      bool ownedBy(Variant pOwner) const noexcept {
          return owners_.contains(pOwner);
      }

      std::span<const Variant> owners() const noexcept { return { owners_.data(), owners_.size() }; }
  };



  template <typename Derived, typename T>
  class OwnedByImpl<Derived, AFew<T>> {
      FlatSet<T*> owners_{};
  public:
      void giveTo(T* pOwner) noexcept {
          if (!owners_.insert(pOwner))
              throw std::runtime_error("bad giveTo call");
      }
      void takeFrom(T* pOwner) {
          if (!owners_.erase(pOwner))
              throw new std::runtime_error("bad takeFrom call");
          if (owners_.empty()) {
              static_cast<Derived*>(this)->~Derived();
          }
      }
      void transferTo(T* pFrom, T* pTo) {
          if (!(owners_.erase(pFrom) && owners_.insert(pTo)))
              throw std::runtime_error("bad transferTo call");
      }

      bool ownedBy(T* pOwner) const noexcept {
          return owners_.contains(pOwner);
      }

      std::span<T* const> owners() const noexcept { return { owners_.data(), owners_.size() }; }
  };
  template <typename Derived, typename T>
  class OwnedByImpl<Derived, Many<T>> {
      std::set<T*> owners_{};
  public:
      void giveTo(T* pOwner) noexcept {
          if (!owners_.insert(pOwner))
              throw std::runtime_error("bad giveTo call");
      }
      void takeFrom(T* pOwner) {
          if (!owners_.erase(pOwner)) {
              throw std::runtime_error("bad takeFrom call");
          }
          if (owners_.empty()) {
              static_cast<Derived*>(this)->~Derived();
          }
      }
      void transferTo(T* pFrom, T* pTo) {
          if (!owners_.erase(pFrom))
              throw std::runtime_error("bad transferTo call");
          if (auto&& [iter, success] = owners_.insert(pTo); !success)
              throw std::runtime_error("bad transferTo call");
      }

      bool ownedBy(T* pOwner) const noexcept {
          return owners_.contains(pOwner);
      }
  };

  template <typename T>
  class OwnsImpl {
      T* pInstance;
  public:
      OwnsImpl(const OwnsImpl& Other) requires(requires{ { Other.pInstance->clone() } -> std::convertible_to<T*>; }) :
              pInstance(Other.pInstance->clone()) {}
      OwnsImpl(const OwnsImpl&) = delete;
      OwnsImpl(OwnsImpl&& Other) noexcept : pInstance(Other.pInstance) {
          Other.pInstance = nullptr;
          pInstance->transferTo(&Other, this);
      }

      void retain(T* pInstance) noexcept {
          if (pInstance) this->release();
          this->pInstance = pInstance;
          this->pInstance->giveTo(this);
      }
      void release() {
          if (pInstance)
              pInstance->takeFrom(this);
      }
      bool owns(T* pInstance) const noexcept {
          return this->pInstance == pInstance;
      }

  protected:
      T* get() const noexcept { return pInstance; }
  };
  template <typename T>
  class OwnsImpl<AFew<T>> {
      FlatSet<T*> instances_;
  public:
      void retain(T* pInstance) noexcept {
          if (instances_.insert(pInstance))
              pInstance->giveTo(this);
      }
      void release() {
          for (auto* instance : instances_) {
              instance->takeFrom(this);
          }
      }
      bool owns(T* pInstance) const noexcept {
          return instances_.contains(pInstance);
      }

  protected:
      const FlatSet<T*>& get() const noexcept { return instances_; }
  };
  template <typename T>
  class OwnsImpl<Many<T>> {
      std::set<T*> instances_;
  public:
      void retain(T* pInstance) noexcept {
          if (auto && [iter, success] = instances_.insert(pInstance); success)
              pInstance->giveTo(this);
      }
      void release() {
          for (auto* instance : instances_) {
              instance->takeFrom(this);
          }
      }
      bool owns(T* pInstance) const noexcept {
          return instances_.contains(pInstance);
      }

  protected:
      const std::set<T*>& get() const noexcept { return instances_; }
  };


  template <typename ...Args>
  class OwnedBy : public OwnedByImpl<OwnedBy<Args...>, Args>...{
  public:
      virtual ~OwnedBy() = default;

      using OwnedByImpl<OwnedBy<Args...>, Args>::giveTo...;
      using OwnedByImpl<OwnedBy<Args...>, Args>::takeFrom...;
      using OwnedByImpl<OwnedBy<Args...>, Args>::transferTo...;
      using OwnedByImpl<OwnedBy<Args...>, Args>::ownedBy...;
  };




  template <typename ...T>
  class Owns : public OwnsImpl<T>...{
  private:
    using OwnsImpl<T>::get...;
  public:

      using OwnsImpl<T>::retain...;
      using OwnsImpl<T>::release...;
      using OwnsImpl<T>::owns...;

  protected:
      ~Owns() {
          ((static_cast<const OwnsImpl<T>*>(this)->release()), ...);
      }

      template <typename U> requires(std::same_as<U, T> || ...)
      decltype(auto) get() const noexcept { return static_cast<const OwnsImpl<U>*>(this)->get(); }
  };*/

  template <typename T>
  inline constexpr bool IsRegisterCopyable = std::conjunction_v<std::is_trivially_copyable<T>, std::is_trivially_destructible<T>> && sizeof(T) <= 16;
  template <typename T>
  using param_t = std::conditional_t<IsRegisterCopyable<T>, T, const T&>;

  template <typename Comp, typename T>
  concept order_on = requires(Comp&& comp, const T& a, const T& b){
    { std::forward<Comp>(comp)(a, b) } -> std::convertible_to<std::partial_ordering>;
  };

  inline constexpr auto DefaultOrder = []<typename T, typename U>(T&& A, U&& B){ return std::forward<T>(A) <=> std::forward<U>(B); };

  template <auto Value>
  struct LowerBound;
  template <typename T>
  struct Mixin;
  template <auto Value>
  struct UpperBound;
  template <auto minValue, auto maxValue>
  struct Bounds;



  namespace _detail{
    enum IntervalLocation{
      EntirelyLessThan,
      IntersectionLessThan,
      Equals,
      ContainedWithin,
      EntirelyContains,
      IntersectionGreaterThan,
      EntirelyGreaterThan
    };

    template <typename T, typename Opt>
    class IntervalStorage;
    template <typename T>
    class IntervalStorage<T, void> {
      using param_t = Vk::param_t<T>;
    public:
      constexpr IntervalStorage() = default;
      constexpr IntervalStorage(param_t maxValue, param_t minValue) noexcept : min(minValue), max(maxValue){}

      T min;
      T max;
    };
    template <typename T, std::convertible_to<T> U, U Value>
    class IntervalStorage<T, LowerBound<Value>>{
      using param_t = Vk::param_t<T>;
    public:
      constexpr IntervalStorage() = default;
      constexpr IntervalStorage(param_t maxValue) noexcept : max(maxValue){}

      constexpr static T min = Value;
      T max;
    };
    template <typename T, std::convertible_to<T> U, U Value>
    class IntervalStorage<T, UpperBound<Value>>{
      using param_t = Vk::param_t<T>;
    public:
      constexpr IntervalStorage() = default;
      constexpr IntervalStorage(param_t minValue) noexcept : min(minValue){}

      T min;
      constexpr static T max = Value;
    };
    template <typename T, std::convertible_to<T> U, std::convertible_to<T> V, U MinVal, V MaxVal>
    class IntervalStorage<T, Bounds<MinVal, MaxVal>>{
      using param_t = Vk::param_t<T>;
    public:
      constexpr IntervalStorage() = default;

      constexpr static T min = MinVal;
      constexpr static T max = MaxVal;
    };
    template <typename T, typename U>
    class IntervalStorage<T, Mixin<U>> : public U{};

    template <typename T, typename ...Opt>
    class IntervalMixin : public IntervalStorage<T, Opt> ... { };
    template <typename T>
    class IntervalMixin<T> : public IntervalStorage<T, void> {
    public:
      using IntervalStorage<T, void>::IntervalStorage;
    };
  }

  template <typename T, typename ...Opt>
  class Interval : public _detail::IntervalMixin<T, Opt...> {
    using param_t = Vk::param_t<T>;
    using base_t = _detail::IntervalMixin<T, Opt...>;
  public:
    using base_t::base_t;

    [[nodiscard]] inline constexpr size_t size() const noexcept requires(requires{
      { this->max - this->min } -> std::convertible_to<size_t>;
    }){
      return this->max - this->min;
    }

    [[nodiscard]] inline constexpr bool contains(param_t Value) const noexcept{
      return this->min <= Value && Value < this->max;
    }
    template <std::predicate<param_t, param_t> Compare>
    [[nodiscard]] inline constexpr bool contains(param_t Value, Compare&& Comp) const noexcept{
      return !Comp(Value, this->min) && Comp(Value, this->max);
    }
    template <order_on<T> Compare>
    [[nodiscard]] inline constexpr bool contains(param_t Value, Compare&& Comp) const noexcept{
      return (Comp(Value, this->min) != std::partial_ordering::greater) && (Comp(Value, this->max) == std::partial_ordering::less);
    }

    [[nodiscard]] inline constexpr _detail::IntervalLocation relativeLocation(const Interval& Other) const noexcept{
      auto Lower = this->min <=> Other.min;
      auto Upper = this->max <=> Other.max;
      if (Lower == Upper) [[likely]] {
        if (Lower == std::partial_ordering::less)
          return this->max < Other.min ? _detail::EntirelyLessThan : _detail::IntersectionLessThan;
        if (Lower == std::partial_ordering::greater)
          return Other.max < this->min ? _detail::EntirelyGreaterThan : _detail::IntersectionGreaterThan;
        return _detail::Equals;
      }

      if (Lower == std::partial_ordering::less || Upper == std::partial_ordering::greater)
        return _detail::EntirelyContains;
      return _detail::ContainedWithin;
    }
    template <std::predicate<param_t, param_t> Compare>
    [[nodiscard]] inline constexpr _detail::IntervalLocation relativeLocation(const Interval& Other, Compare&& Comp) const noexcept{
      if (Comp(this->min, Other.min)) {
        if (Comp(this->max, Other.min))
          return _detail::EntirelyLessThan;
        if (Comp(this->max, Other.max))
          return _detail::IntersectionLessThan;
        return _detail::EntirelyContains;
      }
      if (Comp(Other.max, this->max)) {
        if (Comp(Other.max, this->min))
          return _detail::EntirelyGreaterThan;
        if (Comp(Other.min, this->min))
          return _detail::IntersectionGreaterThan;
        return _detail::EntirelyContains;
      }
      [[unlikely]]
      if (Comp(Other.min, this->min) || Comp(this->max, Other.max))
        return _detail::ContainedWithin;
      return _detail::Equals;
    }
    template <order_on<T> Compare>
    [[nodiscard]] inline constexpr _detail::IntervalLocation relativeLocation(const Interval& Other, Compare&& Comp) const noexcept{
      auto Lower = Comp(this->min, Other.min);
      auto Upper = Comp(this->max, Other.max);
      if (Lower == Upper) [[likely]] {
        if (Lower == std::partial_ordering::less)
          return (Comp(this->max, Other.min) == std::partial_ordering::less) ? _detail::EntirelyLessThan : _detail::IntersectionLessThan;
        if (Lower == std::partial_ordering::greater)
          return (Comp(Other.max, this->min) == std::partial_ordering::less) ? _detail::EntirelyGreaterThan : _detail::IntersectionGreaterThan;
        return _detail::Equals;
      }

      if (Lower == std::partial_ordering::less || Upper == std::partial_ordering::greater)
        return _detail::EntirelyContains;
      return _detail::ContainedWithin;
    }

    [[nodiscard]] inline constexpr Interval intersect(const Interval& Other) const noexcept{
      switch(this->relativeLocation(Other)) {
        case _detail::Equals:
        case _detail::ContainedWithin:
          return *this;
        case _detail::EntirelyContains:
          return Other;
        case _detail::IntersectionLessThan:
          return Interval{Other.min, this->max};
        case _detail::IntersectionGreaterThan:
          return Interval{this->min, Other.max};
        case _detail::EntirelyGreaterThan:
        case _detail::EntirelyLessThan:
          return {};
        default:
          __assume(0);
      }
    }
    template <std::predicate<param_t, param_t> Compare>
    [[nodiscard]] inline constexpr Interval intersect(const Interval& Other, Compare&& Comp) const noexcept{
      return this->intersect(Other, std::forward<Compare>(Comp));
    }
    template <order_on<T> Compare>
    [[nodiscard]] inline constexpr Interval intersect(const Interval& Other, Compare&& Comp) const noexcept{
      return this->intersect(Other, std::forward<Compare>(Comp));
    }


    [[nodiscard]] inline constexpr std::partial_ordering compare(const Interval& Other) const noexcept{
      return compareInternal(this->relativeLocation(Other));
    }
    template <std::predicate<param_t, param_t> Compare>
    [[nodiscard]] inline constexpr std::partial_ordering compare(const Interval& Other, Compare&& Comp) const noexcept{
      return compareInternal(this->relativeLocation(Other, std::forward<Compare>(Comp)));
    }
    template <order_on<T> Compare>
    [[nodiscard]] inline constexpr std::partial_ordering compare(const Interval& Other, Compare&& Comp) const noexcept{
      return compareInternal(this->relativeLocation(Other, std::forward<Compare>(Comp)));
    }


    constexpr explicit operator bool() const noexcept{
      return this->min != this->max;
    }



    friend constexpr bool operator==(const Interval& A, const Interval& B) noexcept = default;
    friend constexpr std::partial_ordering operator<=>(const Interval& A, const Interval& B) noexcept{
      return compareInternal(A.relativeLocation(B));
    }

  private:

    [[nodiscard]] inline static constexpr std::partial_ordering compareInternal(_detail::IntervalLocation Loc) noexcept{
      switch(Loc){
        case _detail::EntirelyLessThan:
        case _detail::IntersectionLessThan:
          return std::partial_ordering::less;
        case _detail::Equals:
          return std::partial_ordering::equivalent;
        case _detail::IntersectionGreaterThan:
        case _detail::EntirelyGreaterThan:
          return std::partial_ordering::greater;
        case _detail::ContainedWithin:
        case _detail::EntirelyContains:
          return std::partial_ordering::unordered;
        default:
          __assume(0);
      }
    }
  };

  template <typename T>
  using LimitMax = Interval<T, LowerBound<std::numeric_limits<T>::min()>>;
  template <typename T>
  using LimitMin = Interval<T, UpperBound<std::numeric_limits<T>::max()>>;
  template <typename T>
  using LimitRange = Interval<T>;

  using MaxU32 = LimitMax<uint32_t>;
  using MinU32 = LimitMin<uint32_t>;
  using RangeU32 = LimitRange<uint32_t>;
  using MaxI32 = LimitMax<int32_t>;
  using MinI32 = LimitMin<int32_t>;
  using RangeI32 = LimitRange<int32_t>;
  using MaxU64 = LimitMax<uint64_t>;
  using MinU64 = LimitMin<uint64_t>;
  using RangeU64 = LimitRange<uint64_t>;
  using RangeF32 = LimitRange<float>;

  struct UpdateAfterBind{
    MaxU32 updateAfterBind;
  };
  using MaxU32WithUAB = Interval<uint32_t, LowerBound<0u>, Mixin<UpdateAfterBind>>;






}

#endif //VULKAN_WRAPPER_VK_UTIL_H
