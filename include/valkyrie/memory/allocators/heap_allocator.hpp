//
// Created by maxwe on 2021-03-05.
//

#ifndef VALKYRIE_MEMORY_HEAP_ALLOCATOR_HPP
#define VALKYRIE_MEMORY_HEAP_ALLOCATOR_HPP

#include "../../traits.hpp"
#include "../detail/lowlevel_allocator.hpp"

namespace valkyrie{
  struct allocator_info;

  /// Allocates heap memory.
  /// This function is used by the \ref heap_allocator to allocate the heap memory.
  /// It is not defined on a freestanding implementation, a definition must be provided by the library user.
  /// \requiredbe This function shall return a block of uninitialized memory that is aligned for \c max_align_t and has the given size.
  /// The size parameter will not be zero.
  /// It shall return a \c nullptr if no memory is available.
  /// It must be thread safe.
  /// \defaultbe On a hosted implementation this function uses OS specific facilities, \c std::malloc is used as fallback.
  /// \ingroup allocator
  void* heap_alloc(u64 size) noexcept;

  /// Deallocates heap memory.
  /// This function is used by the \ref heap_allocator to allocate the heap memory.
  /// It is not defined on a freestanding implementation, a definition must be provided by the library user.
  /// \requiredbe This function gets a pointer from a previous call to \ref heap_alloc with the same size.
  /// It shall free the memory.
  /// The pointer will not be zero.
  /// It must be thread safe.
  /// \defaultbe On a hosted implementation this function uses OS specific facilities, \c std::free is used as fallback.
  /// \ingroup allocator
  void heap_dealloc(void* ptr, u64 size) noexcept;

  namespace detail
  {
    struct heap_allocator_impl
    {
      static allocator_info info() noexcept;

      static void* allocate(u64 size, u64) noexcept
      {
        return heap_alloc(size);
      }

      static void deallocate(void* ptr, u64 size, u64) noexcept
      {
        heap_dealloc(ptr, size);
      }

      static u64 max_node_size() noexcept;
    };

    VALKYRIE_LL_ALLOCATOR_LEAK_CHECKER(heap_allocator_impl,
        heap_alloator_leak_checker)
  } // namespace detail

  /// A stateless \concept{concept_rawallocator,RawAllocator} that allocates memory from the heap.
  /// It uses the two functions \ref heap_alloc and \ref heap_dealloc for the allocation,
  /// which default to \c std::malloc and \c std::free.
  /// \ingroup allocator
  using heap_allocator = detail::lowlevel_allocator<detail::heap_allocator_impl>;


  extern template class detail::lowlevel_allocator<detail::heap_allocator_impl>;
  VK_extern_trait_instantiation(allocator, heap_allocator);
}

#endif//VALKYRIE_MEMORY_HEAP_ALLOCATOR_HPP
