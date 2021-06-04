//
// Created by maxwe on 2021-05-06.
//

#ifndef VALKYRIE_UTILITY_ALIGN_HPP
#define VALKYRIE_UTILITY_ALIGN_HPP

#include "pow2.hpp"

namespace valkyrie {

  // whether or not an alignment is valid, i.e. a power of two not zero
  constexpr bool is_valid_alignment(u64 alignment) noexcept {
    return alignment && (alignment & (alignment - 1)) == 0u;
  }

  // returns the offset needed to align ptr for given alignment
  // alignment must be valid
  inline u64 align_offset(u64 address, u64 alignment) noexcept {
    VK_assert(is_valid_alignment(alignment));
    auto misaligned = address & (alignment - 1);
    return misaligned != 0 ? (alignment - misaligned) : 0;
  }
  inline u64 align_offset(void *ptr, u64 alignment) noexcept {
    return align_offset(reinterpret_cast<u64>(ptr), alignment);
  }

  // whether or not the pointer is aligned for given alignment
  // alignment must be valid
  inline bool is_aligned(void *ptr, u64 alignment) noexcept {
    VK_assert(is_valid_alignment(alignment));
    auto address = reinterpret_cast<uintptr_t>(ptr);
    return (address & (alignment - 1)) == 0u;
  }

  // maximum alignment value
  constexpr u64 max_alignment      = alignof(std::max_align_t);
  constexpr u64 max_alignment_log2 = ilog2(max_alignment);
  static_assert(is_valid_alignment(max_alignment), "ehm..?");

  // returns the minimum alignment required for a node of given size
  inline u64 alignment_for(u64 size) noexcept {
    return size >= max_alignment ? max_alignment : (u64(1) << ilog2(size));
  }


  template <u64 Align, u64 AlignOffset = Align - 1, u64 AlignMask = ~AlignOffset>
  VK_forceinline constexpr static u64 align_to(u64 size) noexcept {
    return (size + AlignOffset) & AlignMask;
  }


  namespace impl{
    template <typename T, typename ...U>
    struct buffer_aligner{
      T t;
      buffer_aligner<U...> tail;
      buffer_aligner() = delete;
    };
    template <typename T>
    struct buffer_aligner<T>{
      T t;
      buffer_aligner() = delete;
    };
    template <typename T, typename ...U>
    union buffer_sizer{
      char buffer[sizeof(T)];
      buffer_sizer<U...> tail;
    };
    template <typename T>
    union buffer_sizer<T>{
      char buffer[sizeof(T)];
    };
  }

  template <typename T, typename ...U>
  struct aligned_union{
    alignas(impl::buffer_aligner<T, U...>) char buffer[sizeof(impl::buffer_sizer<T, U...>)];
  };



}  // namespace valkyrie

#endif  //VALKYRIE_UTILITY_ALIGN_HPP
