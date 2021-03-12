//
// Created by maxwe on 2021-03-05.
//

#ifndef VALKYRIE_MEMORY_DETAIL_ALIGN_HPP
#define VALKYRIE_MEMORY_DETAIL_ALIGN_HPP

#include <valkyrie/preprocessor.hpp>
#include <valkyrie/primitives.hpp>

namespace valkyrie::detail{
  // whether or not an alignment is valid, i.e. a power of two not zero
  constexpr bool is_valid_alignment(u64 alignment) noexcept
{
  return alignment && (alignment & (alignment - 1)) == 0u;
}

// returns the offset needed to align ptr for given alignment
// alignment must be valid
inline u64 align_offset(std::uintptr_t address, u64 alignment) noexcept
{
VK_assert(is_valid_alignment(alignment));
auto misaligned = address & (alignment - 1);
return misaligned != 0 ? (alignment - misaligned) : 0;
}
inline u64 align_offset(void* ptr, u64 alignment) noexcept
{
return align_offset(reinterpret_cast<std::uintptr_t>(ptr), alignment);
}

// whether or not the pointer is aligned for given alignment
// alignment must be valid
bool is_aligned(void* ptr, u64 alignment) noexcept;

// maximum alignment value
constexpr u64 max_alignment = alignof(std::max_align_t);
static_assert(is_valid_alignment(max_alignment), "ehm..?");

// returns the minimum alignment required for a node of given size
u64 alignment_for(u64 size) noexcept;
}

#endif//VALKYRIE_MEMORY_DETAIL_ALIGN_HPP
