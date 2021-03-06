//
// Created by maxwe on 2021-03-05.
//

#ifndef VALKYRIE_MEMORY_DETAIL_LOWLEVEL_ALLOCATOR_HPP
#define VALKYRIE_MEMORY_DETAIL_LOWLEVEL_ALLOCATOR_HPP

#include <valkyrie/utility/align.hpp>
#include "debug_helpers.hpp"

namespace valkyrie::detail{
  template <class Functor>
  struct lowlevel_allocator_leak_handler {
    void operator()(i64 amount) {
      debug_handle_memory_leak(Functor::info(), amount);
    }
  };

  // Functor controls low-level allocation:
  // static allocator_info info()
  // static void* allocate(u64 size, u64 alignment);
  // static void deallocate(void *memory, u64 size, u64 alignment);
  // static u64 max_node_size();
  template <class Functor>
  class lowlevel_allocator : global_leak_checker<lowlevel_allocator_leak_handler<Functor>> {
  public:
    VK_constant bool is_stateful = false;

    lowlevel_allocator() noexcept = default;
    lowlevel_allocator(lowlevel_allocator&&) noexcept = default;

    //~lowlevel_allocator() = default;

    lowlevel_allocator& operator=(lowlevel_allocator&&) noexcept {
      return *this;
    }

    void* allocate_node(u64 size, u64 alignment) noexcept {
      auto actual_size = size + (debug_fence_size ? 2 * max_alignment : 0u);

      auto memory = Functor::allocate(actual_size, alignment);

      if ( memory ) VK_likely {
        this->on_allocate(actual_size);

        return debug_fill_new(memory, size, max_alignment);
      } else {
        return nullptr;
      }

    }
    void* reallocate_array(void* ptr, u64 new_size, u64 old_size, u64 align) noexcept requires(requires{ &Functor::reallocate; }) {
      return Functor::reallocate(ptr, new_size, old_size, align);
    }
    void  deallocate_node(void* node, u64 size, u64 alignment) noexcept {
      auto actual_size = size + (debug_fence_size ? 2 * max_alignment : 0u);

      auto memory = debug_fill_free(node, size, max_alignment);
      Functor::deallocate(memory, actual_size, alignment);

      this->on_deallocate(actual_size);
    }

    VK_nodiscard u64 max_node_size() const noexcept {
      return Functor::max_node_size();
    }
  };

#define VALKYRIE_LL_ALLOCATOR_LEAK_CHECKER(functor, var_name)                              \
    VALKYRIE_GLOBAL_LEAK_CHECKER(lowlevel_allocator_leak_handler<functor>, var_name)
}

#endif//VALKYRIE_MEMORY_DETAIL_LOWLEVEL_ALLOCATOR_HPP
