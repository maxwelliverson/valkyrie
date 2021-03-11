//
// Created by maxwe on 2021-03-05.
//

#ifndef VALKYRIE_MEMORY_ITERATION_ALLOCATOR_HPP
#define VALKYRIE_MEMORY_ITERATION_ALLOCATOR_HPP


#include "detail/debug_helpers.hpp"
#include "detail/memory_stack.hpp"
#include "default_allocator.hpp"
#include "error.hpp"
#include "memory_arena.hpp"

namespace valkyrie{
  namespace detail
  {
    template <class BlockOrRawAllocator>
    using iteration_block_allocator =
    make_block_allocator_t<BlockOrRawAllocator, fixed_block_allocator>;
  } // namespace detail

  /// A stateful \concept{concept_rawallocator,RawAllocator} that is designed for allocations in a loop.
  /// It uses `N` stacks for the allocation, one of them is always active.
  /// Allocation uses the currently active stack.
  /// Calling \ref iteration_allocator::next_iteration() at the end of the loop,
  /// will make the next stack active for allocation,
  /// effectively releasing all of its memory.
  /// Any memory allocated will thus be usable for `N` iterations of the loop.
  /// This type of allocator is a generalization of the double frame allocator.
  /// \ingroup allocator
  template <u64 N, class BlockOrRawAllocator = default_allocator>
  class iteration_allocator
      : detail::iteration_block_allocator<BlockOrRawAllocator>
      {
          public:
          using allocator_type = detail::iteration_block_allocator<BlockOrRawAllocator>;

          /// \effects Creates it with a given initial block size and and other constructor arguments for the \concept{concept_blockallocator,BlockAllocator}.
          /// It will allocate the first (and only) block and evenly divide it on all the stacks it uses.
          template <typename... Args>
          explicit iteration_allocator(u64 block_size, Args&&... args)
          : allocator_type(block_size, std::forward<Args>(args)...), cur_(0u)
          {
            block_         = get_allocator().allocate_block();
            auto cur       = static_cast<char*>(block_.memory);
            auto size_each = block_.size / N;
            for (auto i = 0u; i != N; ++i)
            {
              stacks_[i] = detail::fixed_memory_stack(cur);
              cur += size_each;
            }
          }

          iteration_allocator(iteration_allocator&& other) noexcept
          : allocator_type(std::move(other)),
          block_(other.block_),
          cur_(std::move(other.cur_))
          {
            for (auto i = 0u; i != N; ++i)
              stacks_[i] = std::move(other.stacks_[i]);

            other.cur_ = N;
          }

          ~iteration_allocator() noexcept
          {
            if (cur_ < N)
              get_allocator().deallocate_block(block_);
          }

          iteration_allocator& operator=(iteration_allocator&& other) noexcept
          {
            allocator_type::operator=(std::move(other));
            block_                  = other.block_;
            cur_                    = other.cur_;

            for (auto i = 0u; i != N; ++i)
              stacks_[i] = std::move(other.stacks_[i]);

            other.cur_ = N;

            return *this;
          }

          /// \effects Allocates a memory block of given size and alignment.
          /// It simply moves the top marker of the currently active stack.
          /// \returns A \concept{concept_node,node} with given size and alignment.
          /// \throws \ref out_of_fixed_memory if the current stack does not have any memory left.
          /// \requires \c size and \c alignment must be valid.
          void* allocate(u64 size, u64 alignment)
          {
            auto& stack = stacks_[cur_];

            auto fence  = detail::debug_fence_size;
            auto offset = detail::align_offset(stack.top() + fence, alignment);
            if (!stack.top()
                || (fence + offset + size + fence > u64(block_end(cur_) - stack.top())))
              //FOONATHAN_THROW(out_of_fixed_memory(info(), size));
              return nullptr;
            return stack.allocate_unchecked(size, offset);
          }

          /// \effects Allocates a memory block of given size and alignment
          /// similar to \ref allocate().
          /// \returns A \concept{concept_node,node} with given size and alignment
          /// or `nullptr` if the current stack does not have any memory left.
          void* try_allocate(u64 size, u64 alignment) noexcept
          {
            auto& stack = stacks_[cur_];
            return stack.allocate(block_end(cur_), size, alignment);
          }

          /// \effects Goes to the next internal stack.
          /// This will clear the stack whose \ref max_iterations() lifetime has reached,
          /// and use it for all allocations in this iteration.
          /// \note This function should be called at the end of the loop.
          void next_iteration() noexcept
          {
            VK_assert_msg(cur_ != N, "moved-from allocator");
            cur_ = (cur_ + 1) % N;
            stacks_[cur_].unwind(block_start(cur_));
          }

          /// \returns The number of iteration each allocation will live.
          /// This is the template parameter `N`.
          static u64 max_iterations() noexcept
          {
            return N;
          }

          /// \returns The index of the current iteration.
          /// This is modulo \ref max_iterations().
          u64 cur_iteration() const noexcept
          {
            return cur_;
          }

          /// \returns A reference to the \concept{concept_blockallocator,BlockAllocator} used for managing the memory.
          /// \requires It is undefined behavior to move this allocator out into another object.
          allocator_type& get_allocator() noexcept
          {
            return *this;
          }

          /// \returns The amount of memory remaining in the stack with the given index.
          /// This is the number of bytes that are available for allocation.
          u64 capacity_left(u64 i) const noexcept
          {
            return u64(block_end(i) - stacks_[i].top());
          }

          /// \returns The amount of memory remaining in the currently active stack.
          u64 capacity_left() const noexcept
          {
            return capacity_left(cur_iteration());
          }

          private:
          allocator_info info() const noexcept
          {
            return { "valkyrie::iteration_allocator", this};
          }

          char* block_start(u64 i) const noexcept
          {
            VK_assert_msg(i <= N, "moved from state");
            auto ptr = static_cast<char*>(block_.memory);
            return ptr + (i * block_.size / N);
          }

          char* block_end(u64 i) const noexcept
          {
            VK_assert_msg(i < N, "moved from state");
            return block_start(i + 1);
          }

          detail::fixed_memory_stack stacks_[N];
          memory_block               block_;
          u64                cur_;

          friend allocator_traits<iteration_allocator<N, BlockOrRawAllocator>>;
          friend composable_allocator_traits<iteration_allocator<N, BlockOrRawAllocator>>;
      };

  /// An alias for \ref iteration_allocator for two iterations.
  /// \ingroup allocator
  template <class BlockOrRawAllocator = default_allocator>
  using double_frame_allocator = iteration_allocator<2, BlockOrRawAllocator>;


  extern template class iteration_allocator<2>;


  /// Specialization of the \ref allocator_traits for \ref iteration_allocator.
  /// \note It is not allowed to mix calls through the specialization and through the member functions,
  /// i.e. \ref memory_stack::allocate() and this \c allocate_node().
  /// \ingroup allocator
  template <u64 N, class BlockAllocator>
  class allocator_traits<iteration_allocator<N, BlockAllocator>>
  {
  public:
    using allocator_type = iteration_allocator<N, BlockAllocator>;
    using is_stateful    = std::true_type;

    /// \returns The result of \ref iteration_allocator::allocate().
    static void* allocate_node(allocator_type& state, u64 size,
                               u64 alignment)
    {
      return state.allocate(size, alignment);
    }

    /// \returns The result of \ref memory_stack::allocate().
    static void* allocate_array(allocator_type& state, u64 count, u64 size,
                                u64 alignment)
    {
      return allocate_node(state, count * size, alignment);
    }

    /// @{
    /// \effects Does nothing.
    /// Actual deallocation can only be done via \ref memory_stack::unwind().
    static void deallocate_node(allocator_type&, void*, u64, u64) noexcept
    {
    }

    static void deallocate_array(allocator_type&, void*, u64, u64,
                                 u64) noexcept
    {
    }
    /// @}

    /// @{
    /// \returns The maximum size which is \ref iteration_allocator::capacity_left().
    static u64 max_node_size(const allocator_type& state) noexcept
    {
      return state.capacity_left();
    }

    static u64 max_array_size(const allocator_type& state) noexcept
    {
      return state.capacity_left();
    }
    /// @}

    /// \returns The maximum possible value since there is no alignment restriction
    /// (except indirectly through \ref memory_stack::next_capacity()).
    static u64 max_alignment(const allocator_type&) noexcept
    {
      return u64(-1);
    }
  };

  /// Specialization of the \ref composable_allocator_traits for \ref iteration_allocator classes.
  /// \ingroup allocator
  template <u64 N, class BlockAllocator>
  class composable_allocator_traits<iteration_allocator<N, BlockAllocator>>
  {
  public:
    using allocator_type = iteration_allocator<N, BlockAllocator>;

    /// \returns The result of \ref memory_stack::try_allocate().
    static void* try_allocate_node(allocator_type& state, u64 size,
                                   u64 alignment) noexcept
    {
      return state.try_allocate(size, alignment);
    }

    /// \returns The result of \ref memory_stack::try_allocate().
    static void* try_allocate_array(allocator_type& state, u64 count,
                                    u64 size, u64 alignment) noexcept
    {
      return state.try_allocate(count * size, alignment);
    }

    /// @{
    /// \effects Does nothing.
    /// \returns Whether the memory will be deallocated by \ref memory_stack::unwind().
    static bool try_deallocate_node(allocator_type& state, void* ptr, u64,
                                    u64) noexcept
    {
      return state.block_.contains(ptr);
    }

    static bool try_deallocate_array(allocator_type& state, void* ptr, u64 count,
                                     u64 size, u64 alignment) noexcept
    {
      return try_deallocate_node(state, ptr, count * size, alignment);
    }
    /// @}
  };


  extern template class allocator_traits<iteration_allocator<2>>;
        extern template class composable_allocator_traits<iteration_allocator<2>>;

}

#endif//VALKYRIE_MEMORY_ITERATION_ALLOCATOR_HPP
