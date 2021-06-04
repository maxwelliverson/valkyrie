//
// Created by maxwe on 2021-03-05.
//

#include <valkyrie/memory/allocators/new_allocator.hpp>
#include <valkyrie/memory/error.hpp>

using namespace valkyrie;


allocator_info detail::new_allocator_impl::info() noexcept {
  return {"valkyrie::new_allocator", nullptr};
}

void *detail::new_allocator_impl::allocate(u64 size, u64 align) noexcept {
  void *memory = nullptr;
  while (true) {
    memory = ::operator new(size, std::align_val_t{align}, std::nothrow);
    if (memory)
      break;

    auto handler = std::get_new_handler();
    if (handler) {
#if VK_exceptions_enabled
      try {
        handler();
      }
      catch (...) {
        return nullptr;
      }
#else
      handler();
#endif
    }
    else {
      return nullptr;
    }
  }
  return memory;
}
void detail::new_allocator_impl::deallocate(void *ptr, u64 size, u64 align) noexcept {
  ::operator delete(ptr, size, std::align_val_t{align});
}

u64 detail::new_allocator_impl::max_node_size() noexcept {
  return std::allocator_traits<std::allocator<char>>::max_size({});
}

static_assert(allocator_c<new_allocator>);
static_assert(VK_satisfies_subtrait(new_allocator, allocator, typedef_allocator_type));
static_assert(VK_satisfies_subtrait(new_allocator, allocator, function_allocate_node));
static_assert(VK_satisfies_subtrait(new_allocator, allocator, function_deallocate_node));
static_assert(VK_satisfies_subtrait(new_allocator, allocator, function_try_allocate_node));
static_assert(VK_satisfies_subtrait(new_allocator, allocator, function_try_deallocate_node));
static_assert(VK_satisfies_subtrait(new_allocator, allocator, function_allocate_array));
static_assert(VK_satisfies_subtrait(new_allocator, allocator, function_deallocate_array));
static_assert(VK_satisfies_subtrait(new_allocator, allocator, function_try_allocate_array));
static_assert(VK_satisfies_subtrait(new_allocator, allocator, function_try_deallocate_array));
static_assert(VK_satisfies_subtrait(new_allocator, allocator, bool_is_stateful));
static_assert(VK_satisfies_subtrait(new_allocator, allocator, bool_is_composable));
static_assert(VK_satisfies_subtrait(new_allocator, allocator, bool_is_thread_safe));


template class detail::lowlevel_allocator<detail::new_allocator_impl>;
VK_trait_instantiation(allocator, new_allocator);
