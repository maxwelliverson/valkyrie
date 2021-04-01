//
// Created by maxwe on 2021-03-28.
//

#ifndef VALKYRIE_POINTER_INT_PAIR_HPP
#define VALKYRIE_POINTER_INT_PAIR_HPP

#include <valkyrie/adt/ptr.hpp>

#include <limits>
#include <bit>
#include <cassert>
#include <memory>

namespace valkyrie{

  namespace impl{


    constexpr u64 ilog2(u64 value) noexcept{
      return std::countr_zero(std::bit_floor(value));
    }


    template <typename T>
    struct pointer_low_bits;
    template <typename T>
    struct pointer_low_bits<T*> : meta::value_wrapper<u32(ilog2(alignof(T)))>{};
    template <typename T>
    struct pointer_high_bits;
    template <typename T>
    struct pointer_high_bits<T*> : meta::value_wrapper<16u>{}; // only 64bit platforms are supported...
  }





  template <typename PointerType,
            typename IntType = i32,
            u32 bits = impl::pointer_low_bits<PointerType>::value>
  class pointer_int_pair{

    static_assert(bits <= impl::pointer_low_bits<PointerType>::value, "integer bit width exceeds alignment of pointed to type");





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


    using ptr_param_t      = param_t<pointer_type>;



  public:




    inline raw_pointer_type get_pointer() const noexcept {
      return this->get_ptr_();
    }
    template <typename Ptr>
    inline auto set_pointer(Ptr&& ptr) noexcept -> decltype(set_ptr_(std::forward<Ptr>(ptr))){
      this->set_ptr_(std::forward<Ptr>(ptr));
    }


    inline raw_pointer_type exchange_pointer() noexcept {}



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
      tmp    |= (u64)i & integer_mask;
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
