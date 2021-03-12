//
// Created by maxwe on 2021-03-05.
//

#ifndef VALKYRIE_MEMORY_FALLBACK_ALLOCATOR_HPP
#define VALKYRIE_MEMORY_FALLBACK_ALLOCATOR_HPP


#include "detail/ebo_storage.hpp"
#include "detail/utility.hpp"
#include "allocator_traits.hpp"

#include <valkyrie/meta.hpp>

namespace valkyrie{
  /// A \concept{raw_allocator,RawAllocator} with a fallback.
  /// Allocation first tries `Default`, if it fails,
  /// it uses `Fallback`.
  /// \requires `Default` must be a composable \concept{concept_rawallocator,RawAllocator},
  /// `Fallback` must be a \concept{concept_rawallocator,RawAllocator}.
  /// \ingroup adapter
  template <class Default, class Fallback>
  class fallback_allocator
      : detail::ebo_storage<0, typename allocator_traits<Default>::allocator_type>,
        detail::ebo_storage<1, typename allocator_traits<Fallback>::allocator_type>
  {
    using default_traits             = allocator_traits<Default>;
    using default_composable_traits  = composable_allocator_traits<Default>;
    using fallback_traits            = allocator_traits<Fallback>;
    using fallback_composable_traits = composable_allocator_traits<Fallback>;
    using fallback_composable        = is_composable_allocator<typename fallback_traits::allocator_type>;

    public:
    using default_allocator_type     = typename allocator_traits<Default>::allocator_type;
    using fallback_allocator_type    = typename allocator_traits<Fallback>::allocator_type;

    using is_stateful                = std::disjunction<default_traits::is_stateful, fallback_traits::is_stateful>;

    /// \effects Default constructs both allocators.
    /// \notes This function only participates in overload resolution, if both allocators are not stateful.
    fallback_allocator() requires(!is_stateful::value)
    : detail::ebo_storage<0, default_allocator_type>({}),
        detail::ebo_storage<1, fallback_allocator_type>({})
    {
    }

    /// \effects Constructs the allocator by passing in the two allocators it has.
    explicit fallback_allocator(default_allocator_type&&  default_alloc,
                                fallback_allocator_type&& fallback_alloc = {})
    : detail::ebo_storage<0, default_allocator_type>(std::move(default_alloc)),
        detail::ebo_storage<1, fallback_allocator_type>(std::move(fallback_alloc))
    {
    }

    /// @{
    /// \effects First calls the compositioning (de)allocation function on the `default_allocator_type`.
    /// If that fails, uses the non-compositioning function of the `fallback_allocator_type`.
    void* allocate_node(u64 size, u64 alignment)
    {
      auto ptr = default_composable_traits::try_allocate_node(get_default_allocator(),
                                                              size, alignment);
      if (!ptr)
        ptr = fallback_traits::allocate_node(get_fallback_allocator(), size, alignment);
      return ptr;
    }

    void* allocate_array(u64 count, u64 size, u64 alignment)
    {
      auto ptr = default_composable_traits::try_allocate_array(get_default_allocator(),
                                                               count, size, alignment);
      if (!ptr)
        ptr = fallback_traits::allocate_array(get_fallback_allocator(), count, size,
                                              alignment);
      return ptr;
    }

    void deallocate_node(void* ptr, u64 size, u64 alignment) noexcept
    {
      auto res = default_composable_traits::try_deallocate_node(get_default_allocator(),
                                                                ptr, size, alignment);
      if (!res)
        fallback_traits::deallocate_node(get_fallback_allocator(), ptr, size,
                                         alignment);
    }

    void deallocate_array(void* ptr, u64 count, u64 size,
    u64 alignment) noexcept
    {
      auto res =
          default_composable_traits::try_deallocate_array(get_default_allocator(), ptr,
                                                          count, size, alignment);
      if (!res)
        fallback_traits::deallocate_array(get_fallback_allocator(), ptr, count, size,
                                          alignment);
    }
    /// @}

    /// @{
    /// \effects First calls the compositioning (de)allocation function on the `default_allocator_type`.
    /// If that fails, uses the compositioning function of the `fallback_allocator_type`.
    /// \requires The `fallback_allocator_type` msut be composable.
    void* try_allocate_node(u64 size, u64 alignment) noexcept requires(fallback_composable::value)
    {
      auto ptr = default_composable_traits::try_allocate_node(get_default_allocator(),
                                                              size, alignment);
      if (!ptr)
        ptr = fallback_composable_traits::try_allocate_node(get_fallback_allocator(),
                                                            size, alignment);
      return ptr;
    }
    void* allocate_array(u64 count, u64 size, u64 alignment) noexcept requires(fallback_composable::value)
    {
      auto ptr = default_composable_traits::try_allocate_array(get_default_allocator(),
                                                               count, size, alignment);
      if (!ptr)
        ptr = fallback_composable_traits::try_allocate_array(get_fallback_allocator(),
                                                             count, size, alignment);
      return ptr;
    }

    bool try_deallocate_node(void* ptr, u64 size, u64 alignment) noexcept requires(fallback_composable::value)
    {
      auto res = default_composable_traits::try_deallocate_node(get_default_allocator(),
                                                                ptr, size, alignment);
      if (!res)
        res = fallback_composable_traits::try_deallocate_node(get_fallback_allocator(),
                                                              ptr, size, alignment);
      return res;
    }

    bool try_deallocate_array(void* ptr, u64 count, u64 size, u64 alignment) noexcept requires(fallback_composable::value)
    {
      auto res =
          default_composable_traits::try_deallocate_array(get_default_allocator(), ptr,
                                                          count, size, alignment);
      if (!res)
        res = fallback_composable_traits::try_deallocate_array(get_fallback_allocator(),
                                                               ptr, count, size,
                                                               alignment);
      return res;
    }
    /// @}

    /// @{
    /// \returns The maximum of the two values from both allocators.
    u64 max_node_size() const
    {
      auto def      = default_traits::max_node_size(get_default_allocator());
      auto fallback = fallback_traits::max_node_size(get_fallback_allocator());
      return fallback > def ? fallback : def;
    }

    u64 max_array_size() const
    {
      auto def      = default_traits::max_array_size(get_default_allocator());
      auto fallback = fallback_traits::max_array_size(get_fallback_allocator());
      return fallback > def ? fallback : def;
    }

    u64 max_alignment() const
    {
      auto def      = default_traits::max_alignment(get_default_allocator());
      auto fallback = fallback_traits::max_alignment(get_fallback_allocator());
      return fallback > def ? fallback : def;
    }
    /// @}

    /// @{
    /// \returns A (`const`) reference to the default allocator.
    default_allocator_type& get_default_allocator() noexcept
    {
      return detail::ebo_storage<0, default_allocator_type>::get();
    }

    const default_allocator_type& get_default_allocator() const noexcept
    {
      return detail::ebo_storage<0, default_allocator_type>::get();
    }
    /// @}

    /// @{
    /// \returns A (`const`) reference to the fallback allocator.
    fallback_allocator_type& get_fallback_allocator() noexcept
    {
      return detail::ebo_storage<1, fallback_allocator_type>::get();
    }

    const fallback_allocator_type& get_fallback_allocator() const noexcept
    {
      return detail::ebo_storage<1, fallback_allocator_type>::get();
    }
    /// @}
  };
}

#endif//VALKYRIE_MEMORY_FALLBACK_ALLOCATOR_HPP
