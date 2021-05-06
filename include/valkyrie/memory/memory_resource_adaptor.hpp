//
// Created by maxwe on 2021-03-05.
//

#ifndef VALKYRIE_MEMORY_MEMORY_RESOURCE_ADAPTOR_HPP
#define VALKYRIE_MEMORY_MEMORY_RESOURCE_ADAPTOR_HPP

#include <memory_resource>

#include "detail/utility.hpp"
#include <valkyrie/traits.hpp>

namespace valkyrie{
  /// The \c std_memory_resource abstract base class used in the implementation.
  /// \ingroup adapter
  using std_memory_resource = std::pmr::memory_resource;

  /// Wraps a \concept{concept_rawallocator,RawAllocator} and makes it a \ref std_memory_resource.
  /// \ingroup adapter
  template <class RawAllocator>
  class memory_resource_adapter : public std_memory_resource, allocator_traits<RawAllocator>::allocator_type
      {
          public:
          using allocator_type = typename allocator_traits<RawAllocator>::allocator_type;

          /// \effects Creates the resource by moving in the allocator.
          memory_resource_adapter(allocator_type&& other) noexcept : allocator_type(std::move(other)){}

          /// @{
          /// \returns A reference to the wrapped allocator.
          allocator_type& get_allocator() noexcept {
            return *this;
          }
          const allocator_type& get_allocator() const noexcept {
            return *this;
          }
          /// @}

          protected:
          using traits_type = allocator_traits<RawAllocator>;

          /// \effects Allocates raw memory with given size and alignment.
          /// It forwards to \c allocate_node() or \c allocate_array() depending on the size.
          /// \returns The new memory as returned by the \concept{concept_rawallocator,RawAllocator}.
          /// \throws Anything thrown by the allocation function.
          VK_nodiscard void* do_allocate(u64 bytes, u64 alignment) override {
            const auto max = traits_type::max_node_size(*this);
            if (bytes <= max) VK_likely
              return traits_type::allocate_node(*this, bytes, alignment);
            auto div = bytes / max;
            auto mod = bytes % max;
            auto n   = div + (mod != 0);
            return traits_type::allocate_array(*this, n, max, alignment);
          }

          /// \effects Deallocates memory previously allocated by \ref do_allocate.
          /// It forwards to \c deallocate_node() or \c deallocate_array() depending on the size.
          /// \throws Nothing.
          void do_deallocate(void* p, u64 bytes, u64 alignment) override {
            auto max = traits_type::max_node_size(*this);
            if (bytes <= max) VK_likely
              traits_type::deallocate_node(*this, p, bytes, alignment);
            else {
              auto div = bytes / max;
              auto mod = bytes % max;
              auto n   = div + (mod != 0);
              traits_type::deallocate_array(*this, p, n, max, alignment);
            }
          }

          /// \returns Whether or not \c *this is equal to \c other
          /// by comparing the addresses.
          VK_nodiscard bool do_is_equal(const std_memory_resource& other) const noexcept override {
            return this == &other;
          }
      };

  /// Wraps a \ref std_memory_resource and makes it a \concept{concept_rawallocator,RawAllocator}.
  /// \ingroup adapter
  class memory_resource_allocator {
  public:
    /// \effects Creates it by giving it a pointer to the \ref std_memory_resource.
    /// \requires \c ptr must not be \c nullptr.
    memory_resource_allocator(std_memory_resource* ptr) noexcept : ptr_(ptr)
        {
            VK_assert(ptr);
        }

    /// \effects Allocates a node by forwarding to the \c allocate() function.
    /// \returns The node as returned by the \ref std_memory_resource.
    /// \throws Anything thrown by the \c allocate() function.
    void* allocate_node(u64 size, u64 alignment)
    {
      return ptr_->allocate(size, alignment);
    }

    /// \effects Deallocates a node by forwarding to the \c deallocate() function.
    void deallocate_node(void* ptr, u64 size, u64 alignment) noexcept {
      ptr_->deallocate(ptr, size, alignment);
    }

    /// \returns The maximum alignment which is the maximum value of type \c u64.
    VK_nodiscard u64 max_alignment() const noexcept {
      return u64(-1);
    }

    /// \returns A pointer to the used \ref std_memory_resource, this is never \c nullptr.
    VK_nodiscard std_memory_resource* resource() const noexcept {
      return ptr_;
    }

  private:
    std_memory_resource* ptr_;
  };

  /// @{
  /// \returns Whether `lhs` and `rhs` share the same resource.
  /// \relates memory_resource_allocator
  inline bool operator==(const memory_resource_allocator& lhs,
                         const memory_resource_allocator& rhs) noexcept
{
  return lhs.resource() == rhs.resource();
}

inline bool operator!=(const memory_resource_allocator& lhs,
                       const memory_resource_allocator& rhs) noexcept
{
return !(lhs == rhs);
}
/// @}

template <class RawAllocator>
struct is_shared_allocator;

/// Specialization of \ref is_shared_allocator to mark \ref memory_resource_allocator as shared.
/// This allows using it as \ref allocator_reference directly.
/// \ingroup adapter
template <>
struct is_shared_allocator<memory_resource_allocator> : std::true_type
{
};


}

#endif//VALKYRIE_MEMORY_MEMORY_RESOURCE_ADAPTOR_HPP
