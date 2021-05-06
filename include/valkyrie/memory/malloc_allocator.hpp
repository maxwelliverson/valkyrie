//
// Created by maxwe on 2021-03-05.
//

#ifndef VALKYRIE_MEMORY_MALLOC_ALLOCATOR_HPP
#define VALKYRIE_MEMORY_MALLOC_ALLOCATOR_HPP


#include <valkyrie/traits.hpp>
#include "detail/lowlevel_allocator.hpp"


namespace valkyrie{
  struct allocator_info;

  namespace detail
  {
    struct malloc_allocator_impl
    {
      static allocator_info info() noexcept;

      static void* allocate(u64 size, u64) noexcept
      {
        return std::malloc(size);
      }

      static void deallocate(void* ptr, u64, u64) noexcept
      {
        std::free(ptr);
      }

      static u64 max_node_size() noexcept
      {
        return std::allocator_traits<std::allocator<char>>::max_size({});
      }
    };

    VALKYRIE_LL_ALLOCATOR_LEAK_CHECKER(malloc_allocator_impl,
        malloc_alloator_leak_checker)
  } // namespace detail

  /// A stateless \concept{concept_rawallocator,RawAllocator} that allocates memory using <tt>std::malloc()</tt>.
  /// It throws \ref out_of_memory when the allocation fails.
  /// \ingroup allocator
  using malloc_allocator = detail::lowlevel_allocator<detail::malloc_allocator_impl>;


  extern template class detail::lowlevel_allocator<detail::malloc_allocator_impl>;
  VK_extern_trait_instantiation(allocator, malloc_allocator);
}

#endif//VALKYRIE_MEMORY_MALLOC_ALLOCATOR_HPP
