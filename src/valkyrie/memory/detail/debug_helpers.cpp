//
// Created by maxwe on 2021-03-05.
//

#include <valkyrie/memory/detail/debug_helpers.hpp>

#include <valkyrie/memory/debugging.hpp>

using namespace valkyrie;

#if VK_debug_build
void detail::debug_fill(void *memory, u64 size, debug_magic m) noexcept
{
  std::memset(memory, static_cast<int>(m), size);
}

void* detail::debug_is_filled(void *memory, u64 size, debug_magic m) noexcept
{
  auto byte = static_cast<unsigned char*>(memory);
  for (auto end = byte + size; byte != end; ++byte)
    if (*byte != static_cast<unsigned char>(m))
      return byte;
  return nullptr;
}

void* detail::debug_fill_new(void *memory,
                             u64 node_size, u64 fence_size) noexcept
{
  if (!debug_fence_size)
    fence_size = 0u; // force override of fence_size

  auto mem = static_cast<char*>(memory);
  debug_fill(mem, fence_size, debug_magic::fence_memory);

  mem += fence_size;
  debug_fill(mem, node_size, debug_magic::new_memory);

  debug_fill(mem + node_size, fence_size, debug_magic::fence_memory);

  return mem;
}

void* detail::debug_fill_free(void *memory,
                              u64 node_size, u64 fence_size) noexcept
{
  if constexpr (!debug_fence_size)
    fence_size = 0u; // force override of fence_size

  debug_fill(memory, node_size, debug_magic::freed_memory);

  auto pre_fence = static_cast<unsigned char*>(memory) - fence_size;
  if (auto pre_dirty = debug_is_filled(pre_fence, fence_size, debug_magic::fence_memory))
    get_buffer_overflow_handler()(memory, node_size, pre_dirty);

  auto post_mem = static_cast<unsigned char*>(memory) + node_size;
  if (auto post_dirty = debug_is_filled(post_mem, fence_size, debug_magic::fence_memory))
    get_buffer_overflow_handler()(memory, node_size, post_dirty);

  return pre_fence;
}

void detail::debug_fill_internal(void *memory, u64 size, bool free) noexcept
{
  debug_fill(memory, size, free ? debug_magic::internal_freed_memory : debug_magic::internal_memory);
}
#endif

void detail::debug_handle_invalid_ptr(const allocator_info &info, void *ptr)
{
  get_invalid_pointer_handler()(info, ptr);
}

void detail::debug_handle_memory_leak(const allocator_info &info, i64 amount)
{
  get_leak_handler()(info, amount);
}