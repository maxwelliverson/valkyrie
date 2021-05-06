//
// Created by maxwe on 2021-03-28.
//

#ifndef VALKYRIE_POINTER_INT_PAIR_HPP
#define VALKYRIE_POINTER_INT_PAIR_HPP

#include <valkyrie/adt/ptr.hpp>

#include <limits>
#include <cassert>
#include <memory>

namespace valkyrie{

  template <typename PointerType,
            typename IntType = i32,
            u32 bits = pointer_traits<PointerType>::free_low_bits>
  class pointer_int_pair{
    static_assert(bits <= pointer_traits<PointerType>::free_low_bits, "integer bit width exceeds alignment of pointed to type");
    static_assert(bits > 0, "Define traits::pointer<T>::free_low_bits to enable safe pointer_int_pair usage");

    template <typename, typename, u32>
    friend class pointer_int_pair;

  public:

    using pointer_type = PointerType;
    using element_type = typename std::pointer_traits<PointerType>::element_type;
    using value_type   = std::remove_cv_t<element_type>;
    using integer_type = IntType;
    using raw_pointer_type = element_type*;

    constexpr static integer_type max_value() noexcept {
      return integer_mask;
    }


  private:

    inline constexpr static bool is_unique_ptr_kind =
        !std::copy_constructible<pointer_type> &&
        std::move_constructible<pointer_type>;


    using ptr_param_t      = in_param_t<pointer_type>;

  public:

    pointer_int_pair() = default;
    pointer_int_pair(const pointer_int_pair&)
        requires(std::copy_constructible<pointer_type> &&
                 std::is_trivially_copy_constructible_v<pointer_type>) = default;
    pointer_int_pair(pointer_int_pair&&) noexcept
    requires(std::move_constructible<pointer_type> &&
             std::is_trivially_move_constructible_v<pointer_type>) = default;

    pointer_int_pair(const pointer_type& ptr, integer_type i)
        noexcept(std::is_nothrow_copy_constructible_v<pointer_type>)
    requires(std::copy_constructible<pointer_type>){
      new(&value) pointer_type(ptr);
      value |= (static_cast<u64>(i) & integer_mask);
    }

    pointer_int_pair(pointer_type&& ptr, integer_type i)
        noexcept(std::is_nothrow_move_constructible_v<pointer_type>)
    requires(std::move_constructible<pointer_type>){
      new(&value) pointer_type(std::move(ptr));
      value |= (static_cast<u64>(i) & integer_mask);
    }

    template <typename OPtr, typename OInt, u32 OBits>
    explicit(!(convertible_to<const OPtr&, PointerType> &&
               convertible_to<const OInt&, IntType> &&
               (OBits <= bits)))
    pointer_int_pair(const pointer_int_pair<OPtr, OInt, OBits>& other)
    requires(constructible_from<PointerType, const OPtr&> &&
             !std::is_trivially_constructible_v<pointer_type, const OPtr&>){
      u64 tmp_int = other.value & other.integer_mask;

      other.value &= other.pointer_mask;
      new(&value) pointer_type(reinterpret_cast<const OPtr&>(other.value));

      other.value |= tmp_int;

      if constexpr ( bits < OBits )
        value |= (tmp_int & integer_mask);
      else
        value |= tmp_int;
    }
    template <typename OPtr, typename OInt, u32 OBits>
    explicit(!(convertible_to<OPtr&&, PointerType> &&
               convertible_to<OInt&&, IntType> &&
               (OBits <= bits)))
    pointer_int_pair(pointer_int_pair<OPtr, OInt, OBits>&& other) noexcept
    requires(constructible_from<PointerType, OPtr&&> &&
             !std::is_trivially_constructible_v<pointer_type, OPtr&&>){
      u64 tmp_int = other.value & other.integer_mask;

      other.value &= other.pointer_mask;
      new(&value) pointer_type(std::move(reinterpret_cast<OPtr&>(other.value)));

      other.value |= tmp_int;

      if constexpr ( bits < OBits )
        value |= (tmp_int & integer_mask);
      else
        value |= tmp_int;
    }

    ~pointer_int_pair() requires(std::is_trivially_destructible_v<pointer_type>) = default;
    ~pointer_int_pair() {
      value &= pointer_mask;
      reinterpret_cast<pointer_type&>(value).~pointer_type();
    }


    inline integer_type    get_integer() const noexcept {
      return this->get_int_();
    }
    inline void            set_integer(integer_type i) noexcept {
      this->set_int_(i);
    }


    inline raw_pointer_type get_pointer() const noexcept {
      return this->get_ptr_();
    }
    template <typename Ptr>
    inline auto set_pointer(Ptr&& ptr) noexcept -> decltype(set_ptr_(std::forward<Ptr>(ptr))){
      this->set_ptr_(std::forward<Ptr>(ptr));
    }


    inline integer_type     exchange_integer(integer_type i) noexcept {
      auto tmp = this->get_int_();
      this->set_int_(i);
      return tmp;
    }
    inline raw_pointer_type exchange_pointer(ptr_param_t ptr) noexcept {
      auto tmp = this->get_ptr_();
      this->set_ptr_(ptr);
      return tmp;
    }


    void swap(pointer_int_pair& other) noexcept {
      pointer_int_pair tmp{std::move(*this)};
      *this = std::move(other);
      other = std::move(tmp);
    }


  private:

    inline constexpr static u64          pointer_mask = std::numeric_limits<u64>::max() << bits;
    inline constexpr static u64          integer_mask = ~pointer_mask;

    inline raw_pointer_type get_ptr_() const noexcept {
      return reinterpret_cast<raw_pointer_type>(value & pointer_mask);
    }
    inline integer_type     get_int_() const noexcept {
      return static_cast<integer_type>(value & integer_mask);
    }
    template <typename Ptr>
    requires(
        std::assignable_from<pointer_type&, Ptr&&> &&
       (std::is_trivially_assignable_v<pointer_type&, Ptr&&> || sizeof(Ptr) <= sizeof(void*)))
    inline void             set_ptr_(Ptr&& p) noexcept {
      if constexpr (!std::is_trivially_assignable_v<pointer_type&, Ptr&&>) {
        u64 tmp_int = value & integer_mask;
        value &= pointer_mask;
        reinterpret_cast<pointer_type&>(value) = std::forward<Ptr>(p);
        VK_assert(((value & integer_mask) == 0));
        value |= tmp_int;
      } else {
        u64 tmp = value & integer_mask;
        tmp    |= reinterpret_cast<u64>(std::to_address(std::forward<Ptr>(p))) & pointer_mask;
        value = tmp;
      }
    }
    inline void             set_int_(integer_type i) noexcept {
      assert(i <= max_value());
      u64 tmp = value & pointer_mask;
      tmp    |= static_cast<u64>(i) & integer_mask;
      value = tmp;
    }


    inline static raw_pointer_type ptr_to_raw_(ptr_param_t p) noexcept {
      return pointer_traits<pointer_type>::to_address(p);
    }
    inline static pointer_type     raw_to_ptr_(raw_pointer_type p) noexcept {
      return pointer_traits<pointer_type>::to_pointer(*p);
    }

    u64 value;
  };


}

#endif//VALKYRIE_POINTER_INT_PAIR_HPP
