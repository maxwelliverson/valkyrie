//
// Created by maxwe on 2021-03-05.
//

#ifndef VALKYRIE_MEMORY_NEW_ALLOCATOR_HPP
#define VALKYRIE_MEMORY_NEW_ALLOCATOR_HPP

#include <valkyrie/traits.hpp>

#include "detail/lowlevel_allocator.hpp"

namespace valkyrie{
  struct allocator_info;

  namespace detail {
    struct new_allocator_impl {
      static allocator_info info() noexcept;

      static void* allocate(u64 size, u64) noexcept;

      static void deallocate(void* ptr, u64 size, u64) noexcept;

      static u64 max_node_size() noexcept;
    };

    VALKYRIE_LL_ALLOCATOR_LEAK_CHECKER(new_allocator_impl, new_alloator_leak_checker)
  } // namespace detail

  /// A stateless \concept{concept_rawallocator,RawAllocator} that allocates memory using (nothrow) <tt>operator new</tt>.
  /// If the operator returns \c nullptr, it behaves like \c new and loops calling \c std::new_handler,
  /// but instead of throwing a \c std::bad_alloc exception, it throws \ref out_of_memory.
  /// \ingroup allocator
  using new_allocator = detail::lowlevel_allocator<detail::new_allocator_impl>;


  extern template class detail::lowlevel_allocator<detail::new_allocator_impl>;
  VK_extern_trait_instantiation(allocator, new_allocator);
}

#endif//VALKYRIE_MEMORY_NEW_ALLOCATOR_HPP
