//
// Created by Maxwell on 2020-11-09.
//

#ifndef VALKYRIE_UTILITY_HASH_HPP
#define VALKYRIE_UTILITY_HASH_HPP

#include <valkyrie/adt/array_ref.hpp>

#include <ranges>
#include <array>

namespace valkyrie{

  namespace impl{
    VK_constant u32 hash_offset32_v = 5381;
    VK_constant u64 hash_offset64_v = 14695981039346656037ULL;
    VK_constant u64 hash_prime64_v  = 1099511628211ULL;

    template <typename T> requires (std::has_unique_object_representations_v<T>)
    VK_constant u64 hash_append_bytes64(const T* address, u64 length, u64 val = hash_offset64_v) noexcept {
      for (const T* const end = address + length; address != end; ++address) {
        if constexpr ( sizeof(T) != 1 ) {
          for (const u8 b : bit_cast<std::array<u8, sizeof(T)>>(*address)) {
            val ^= static_cast<u64>(b);
            val *= hash_prime64_v;
          }
        }
        else {
          if constexpr ( !(std::integral<T> || std::is_enum_v<T>) ) {
            val ^= static_cast<u64>(bit_cast<u8>(*address));
          }
          else {
            val ^= static_cast<u64>(*address);
          }
          val *= hash_prime64_v;
        }
      }
      return val;
    }
    template <typename T> requires (std::has_unique_object_representations_v<T>)
    VK_constant u32 hash_append_bytes32(const T* address, u64 length, u32 val = hash_offset32_v) noexcept {
      for (const T* const end = address + length; address != end; ++address) {
        if constexpr ( sizeof(T) != 1 ) {
          for (const u8 b : bit_cast<std::array<u8, sizeof(T)>>(*address)) {
            val = (val << 5) + val + b;
          }
        }
        else {
          val = (val << 5) + val + bit_cast<u8>(*address);
        }
      }
      return val;
    }


    template <typename Rng>
    requires (contiguous_range_c<remove_ref_t<Rng>> &&
              std::has_unique_object_representations_v<typename range_traits<remove_ref_t<Rng>>::value_type>)
    VK_constant u64 hash_append_bytes64(Rng&& range, u64 val = hash_offset64_v) noexcept {
      return impl::hash_append_bytes64(std::ranges::cdata(range),
                                       std::ranges::size(range),
                                       val);
    }
    template <typename Rng>
    requires (contiguous_range_c<remove_ref_t<Rng>> &&
              std::has_unique_object_representations_v<typename range_traits<remove_ref_t<Rng>>::value_type>)
    VK_constant u64 hash_append_bytes32(Rng&& range, u32 val = hash_offset32_v) noexcept {
      return impl::hash_append_bytes32(std::ranges::cdata(range),
                                       std::ranges::size(range),
                                       val);
    }




    VK_constant u64 combine_hash(const u64 hashA, const u64 hashB) noexcept {
      return hash_append_bytes64(&hashA, 1ULL, hashB);
      //return (hashA ^ (hashB >> 1)) * hash_prime64_v;
    }
    VK_constant u32 combine_hash(const u32 hashA, const u32 hashB) noexcept {
      return hash_append_bytes32(&hashA, 1ULL, hashB);
      //return (hashA ^ (hashB >> 1)) * hash_prime64_v;
    }



    template <typename T, typename ...U>
    VK_constant u64 hash_all(u64 state, in_param_t<T> arg, in_param_t<U>... args) noexcept {
      if constexpr (sizeof...(U) == 0) {
        return combine_hash(hashable_traits<T>::call64(arg), state);
      }
      else {
        return combine_hash(hash_all<U...>(hashable_traits<T>::call64(arg), args...), state);
      }
    }
    template <typename T, typename ...U>
    VK_constant u32 hash_all(u32 state, in_param_t<T> arg, in_param_t<U>... args) noexcept {
      if constexpr (sizeof...(U) == 0) {
        return combine_hash(hashable_traits<T>::call32(arg), state);
      }
      else {
        return combine_hash(hash_all<U...>(hashable_traits<T>::call32(arg), args...), state);
      }
    }
  }

  template <typename ...Args> requires(hashable_c<remove_cvref_t<Args>> && ...)
  VK_constant u64 hash64(Args&& ...args) noexcept {
    return impl::hash_all<remove_cvref_t<Args>...>(impl::hash_offset64_v, args...);
  }
  template <typename ...Args> requires(hashable_c<remove_cvref_t<Args>> && ...)
  VK_constant u64 hash32(Args&& ...args) noexcept {
    return impl::hash_all<remove_cvref_t<Args>...>(impl::hash_offset32_v, args...);
  }

}

#endif//VALKYRIE_UTILITY_HASH_HPP
