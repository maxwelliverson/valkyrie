//
// Created by maxwe on 2021-03-05.
//


#include <valkyrie/preprocessor.hpp>
#include <valkyrie/primitives.hpp>
#include <valkyrie/memory/detail/align.hpp>
#include <valkyrie/memory/detail/ilog2.hpp>

using namespace valkyrie;

bool valkyrie::detail::is_aligned(void *ptr, u64 alignment) noexcept {
  VK_assert(is_valid_alignment(alignment));
  auto address = reinterpret_cast<uintptr_t>(ptr);
  return address % alignment == 0u;
}

u64 valkyrie::detail::alignment_for(u64 size) noexcept {
  return size >= max_alignment ? max_alignment : (u64(1) << ilog2(size));
}