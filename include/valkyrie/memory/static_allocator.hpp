//
// Created by maxwe on 2021-03-05.
//

#ifndef VALKYRIE_MEMORY_STATIC_ALLOCATOR_HPP
#define VALKYRIE_MEMORY_STATIC_ALLOCATOR_HPP


#include <valkyrie/utility/align.hpp>
#include "detail/memory_stack.hpp"
#include "detail/utility.hpp"
#include <valkyrie/traits.hpp>

#include <valkyrie/traits.hpp>

namespace valkyrie{
  /// Storage for a \ref static_allocator.
  /// Its constructor will take a reference to it and use it for its allocation.
  /// The storage type is simply a \c char array aligned for maximum alignment.
  /// \note It is not allowed to access the memory of the storage.
  /// \ingroup allocator
  template <u64 Size>
  struct static_allocator_storage
  {
    alignas(detail::max_alignment) char storage[Size];
  };

  static_assert(sizeof(static_allocator_storage<1024>) == 1024, "");
  static_assert(alignof(static_allocator_storage<1024>) == detail::max_alignment, "");

  struct allocator_info;

  /// A stateful \concept{concept_rawallocator,RawAllocator} that uses a fixed sized storage for the allocations.
  /// It works on a \ref static_allocator_storage and uses its memory for all allocations.
  /// Deallocations are not supported, memory cannot be marked as freed.<br>
  /// \note It is not allowed to share an \ref static_allocator_storage between multiple \ref static_allocator objects.
  /// \ingroup allocator
  class static_allocator
  {
  public:
    using is_stateful = meta::true_type;

    /// \effects Creates it by passing it a \ref static_allocator_storage by reference.
    /// It will take the address of the storage and use its memory for the allocation.
    /// \requires The storage object must live as long as the allocator object.
    /// It must not be shared between multiple allocators,
    /// i.e. the object must not have been passed to a constructor before.
    template <u64 Size>
    static_allocator(static_allocator_storage<Size>& storage) noexcept
        : stack_(&storage), end_(stack_.top() + Size)
    {
    }

    /// \effects A \concept{concept_rawallocator,RawAllocator} allocation function.
    /// It uses the specified \ref static_allocator_storage.
    /// \returns A pointer to a \concept{concept_node,node}, it will never be \c nullptr.
    /// \throws An exception of type \ref out_of_memory or whatever is thrown by its handler if the storage is exhausted.
    void* allocate_node(u64 size, u64 alignment);

    /// \effects A \concept{concept_rawallocator,RawAllocator} deallocation function.
    /// It does nothing, deallocation is not supported by this allocator.
    void deallocate_node(void*, u64, u64) noexcept {}

    /// \returns The maximum node size which is the capacity remaining inside the \ref static_allocator_storage.
    u64 max_node_size() const noexcept
    {
      return static_cast<u64>(end_ - stack_.top());
    }

    /// \returns The maximum possible value since there is no alignment restriction
    /// (except indirectly through the size of the \ref static_allocator_storage).
    u64 max_alignment() const noexcept
    {
      return u64(-1);
    }

  private:
    allocator_info info() const noexcept;

    detail::fixed_memory_stack stack_;
    const char*                end_;
  };


  VK_extern_trait_instantiation(allocator, static_allocator);


  struct memory_block;

  /// A \concept{concept_blockallocator,BlockAllocator} that allocates the blocks from a fixed size storage.
  /// It works on a \ref static_allocator_storage and uses it for all allocations,
  /// deallocations are only allowed in reversed order which is guaranteed by \ref memory_arena.
  /// \note It is not allowed to share an \ref static_allocator_storage between multiple \ref static_allocator objects.
  /// \ingroup allocator
  class static_block_allocator
  {
  public:
    /// \effects Creates it by passing it the block size and a \ref static_allocator_storage by reference.
    /// It will take the address of the storage and use it to allocate \c block_size'd blocks.
    /// \requires The storage object must live as long as the allocator object.
    /// It must not be shared between multiple allocators,
    /// i.e. the object must not have been passed to a constructor before.
    /// The size of the \ref static_allocator_storage must be a multiple of the (non-null) block size.
    template <u64 Size>
    static_block_allocator(u64                     block_size,
                           static_allocator_storage<Size>& storage) noexcept
        : cur_(static_cast<char*>(static_cast<void*>(&storage))),
        end_(cur_ + Size),
    block_size_(block_size)
        {
            VK_assert(block_size <= Size);
        VK_assert(Size % block_size == 0u);
        }

    ~static_block_allocator() noexcept = default;

    /// @{
    /// \effects Moves the block allocator, it transfers ownership over the \ref static_allocator_storage.
    /// This does not invalidate any memory blocks.
    static_block_allocator(static_block_allocator&& other) noexcept
        : cur_(other.cur_), end_(other.end_), block_size_(other.block_size_)
    {
      other.cur_ = other.end_ = nullptr;
      other.block_size_       = 0;
    }

    static_block_allocator& operator=(static_block_allocator&& other) noexcept
    {
      static_block_allocator tmp(std::move(other));
      swap(*this, tmp);
      return *this;
    }
    /// @}

    /// \effects Swaps the ownership over the \ref static_allocator_storage.
    /// This does not invalidate any memory blocks.
    friend void swap(static_block_allocator& a, static_block_allocator& b) noexcept
    {
      std::swap(a.cur_, b.cur_);
      std::swap(a.end_, b.end_);
      std::swap(a.block_size_, b.block_size_);
    }

    /// \effects Allocates a new block by returning the \ref next_block_size() bytes.
    /// \returns The new memory block.
    memory_block allocate_block();

    /// \effects Deallocates the last memory block by marking the block as free again.
    /// This block will be returned again by the next call to \ref allocate_block().
    /// \requires \c block must be the current top block of the memory,
    /// this is guaranteed by \ref memory_arena.
    void deallocate_block(memory_block block) noexcept;

    /// \returns The next block size, this is the size passed to the constructor.
    u64 next_block_size() const noexcept
    {
      return block_size_;
    }

  private:
    allocator_info info() const noexcept;

    char *      cur_, *end_;
    u64 block_size_;
  };
}

#endif//VALKYRIE_MEMORY_STATIC_ALLOCATOR_HPP
