//
// Created by maxwe on 2021-03-05.
//

#ifndef VALKYRIE_MEMORY_ALIGNED_ALLOCATOR_HPP
#define VALKYRIE_MEMORY_ALIGNED_ALLOCATOR_HPP

#include <valkyrie/meta.hpp>
#include <valkyrie/primitives.hpp>

#include "detail/align.hpp"
#include "allocator_traits.hpp"

namespace valkyrie{
  /// A \concept{concept_rawallocator,RawAllocator} adapter that ensures a minimum alignment.
  /// It adjusts the alignment value so that it is always larger than the minimum and forwards to the specified allocator.
  /// \ingroup adapter
  template <class RawAllocator>
  class aligned_allocator : allocator_traits<RawAllocator>::allocator_type
      {
          using traits            = allocator_traits<RawAllocator>;
          using composable_traits = composable_allocator_traits<RawAllocator>;
          using composable        = is_composable_allocator<typename traits::allocator_type>;

          public:
          using allocator_type = typename allocator_traits<RawAllocator>::allocator_type;
          using is_stateful    = std::true_type;

          /// \effects Creates it passing it the minimum alignment value and the allocator object.
          /// \requires \c min_alignment must be less than \c this->max_alignment().
          explicit aligned_allocator(u64 min_alignment, allocator_type&& alloc = {})
          : allocator_type(std::move(alloc)), min_alignment_(min_alignment)
          {
            VK_assert(min_alignment_ <= max_alignment());
          }

          /// @{
          /// \effects Moves the \c aligned_allocator object.
          /// It simply moves the underlying allocator.
          aligned_allocator(aligned_allocator&& other) noexcept
          : allocator_type(std::move(other)), min_alignment_(other.min_alignment_)
          {
          }

          aligned_allocator& operator=(aligned_allocator&& other) noexcept
          {
            allocator_type::operator=(std::move(other));
            min_alignment_          = other.min_alignment_;
            return *this;
          }
          /// @}

          /// @{
          /// \effects Forwards to the underlying allocator through the \ref allocator_traits.
          /// If the \c alignment is less than the \c min_alignment(), it is set to the minimum alignment.
          void* allocate_node(u64 size, u64 alignment)
          {
            if (min_alignment_ > alignment)
              alignment = min_alignment_;
            return traits::allocate_node(get_allocator(), size, alignment);
          }

          void* allocate_array(u64 count, u64 size, u64 alignment)
          {
            if (min_alignment_ > alignment)
              alignment = min_alignment_;
            return traits::allocate_array(get_allocator(), count, size, alignment);
          }

          void deallocate_node(void* ptr, u64 size, u64 alignment) noexcept
          {
            if (min_alignment_ > alignment)
              alignment = min_alignment_;
            traits::deallocate_node(get_allocator(), ptr, size, alignment);
          }

          void deallocate_array(void* ptr, u64 count, u64 size,
          u64 alignment) noexcept
          {
            if (min_alignment_ > alignment)
              alignment = min_alignment_;
            traits::deallocate_array(get_allocator(), ptr, count, size, alignment);
          }
          /// @}

          /// @{
          /// \effects Forwards to the underlying allocator through the \ref composable_allocator_traits.
          /// If the \c alignment is less than the \c min_alignment(), it is set to the minimum alignment.
          /// \requires The underyling allocator must be composable.
          void* try_allocate_node(u64 size, u64 alignment) noexcept requires(composable::value)
          {
            if (min_alignment_ > alignment)
              alignment = min_alignment_;
            return composable_traits::try_allocate_node(get_allocator(), size, alignment);
          }
          void* try_allocate_array(u64 count, u64 size, u64 alignment) noexcept requires(composable::value)
          {
            if (min_alignment_ > alignment)
              alignment = min_alignment_;
            return composable_traits::try_allocate_array(get_allocator(), count, size,
                                                         alignment);
          }
          bool try_deallocate_node(void* ptr, u64 size, u64 alignment) noexcept requires(composable::value)
          {
            if (min_alignment_ > alignment)
              alignment = min_alignment_;
            return composable_traits::try_deallocate_node(get_allocator(), ptr, size,
                                                          alignment);
          }
          bool try_deallocate_array(void* ptr, u64 count, u64 size, u64 alignment) noexcept requires(composable::value)
          {
            if (min_alignment_ > alignment)
              alignment = min_alignment_;
            return composable_traits::try_deallocate_array(get_allocator(), ptr, count, size,
                                                           alignment);
          }
          /// @}

          /// @{
          /// \returns The value returned by the \ref allocator_traits for the underlying allocator.
          u64 max_node_size() const
          {
            return traits::max_node_size(get_allocator());
          }

          u64 max_array_size() const
          {
            return traits::max_array_size(get_allocator());
          }

          u64 max_alignment() const
          {
            return traits::max_alignment(get_allocator());
          }
          /// @}

          /// @{
          /// \returns A reference to the underlying allocator.
          allocator_type& get_allocator() noexcept
          {
            return *this;
          }

          const allocator_type& get_allocator() const noexcept
          {
            return *this;
          }
          /// @}

          /// \returns The minimum alignment.
          u64 min_alignment() const noexcept
          {
            return min_alignment_;
          }

          /// \effects Sets the minimum alignment to a new value.
          /// \requires \c min_alignment must be less than \c this->max_alignment().
          void set_min_alignment(u64 min_alignment)
          {
            VK_assert(min_alignment <= max_alignment());
            min_alignment_ = min_alignment;
          }

          private:
          u64 min_alignment_;
      };

  /// \returns A new \ref aligned_allocator created by forwarding the parameters to the constructor.
  /// \relates aligned_allocator
  template <class RawAllocator>
  auto make_aligned_allocator(u64 min_alignment, RawAllocator&& allocator) noexcept
  -> aligned_allocator<typename std::decay<RawAllocator>::type>
{
  return aligned_allocator<
      typename std::decay<RawAllocator>::type>{min_alignment,
  std::forward<RawAllocator>(allocator)};
}
}

#endif//VALKYRIE_MEMORY_ALIGNED_ALLOCATOR_HPP
