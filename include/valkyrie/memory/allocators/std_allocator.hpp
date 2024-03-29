//
// Created by maxwe on 2021-03-05.
//

#ifndef VALKYRIE_MEMORY_STD_ALLOCATOR_HPP
#define VALKYRIE_MEMORY_STD_ALLOCATOR_HPP


#include <valkyrie/adt/ptr.hpp>

#include "../allocator_storage.hpp"

namespace valkyrie {
  /*namespace traits_detail {
    template <class RawAllocator>
    auto propagate_on_container_swap(std_concept) ->
        typename RawAllocator::propagate_on_container_swap;

    template <class RawAllocator>
    auto propagate_on_container_swap(min_concept) -> std::true_type;

    template <class RawAllocator>
    auto propagate_on_container_move_assignment(std_concept) ->
        typename RawAllocator::propagate_on_container_move_assignment;

    template <class RawAllocator>
    auto propagate_on_container_move_assignment(min_concept) -> std::true_type;

    template <class RawAllocator>
    auto propagate_on_container_copy_assignment(std_concept) ->
        typename RawAllocator::propagate_on_container_copy_assignment;

    template <class RawAllocator>
    auto propagate_on_container_copy_assignment(min_concept) -> std::true_type;
  }  // namespace traits_detail

  /// Controls the propagation of a \ref std_allocator for a certain \concept{concept_rawallocator,RawAllocator}.
  /// \ingroup adapter
  template <class RawAllocator>
  struct propagation_traits {
    using propagate_on_container_swap =
        decltype(traits_detail::propagate_on_container_swap<RawAllocator>(
            traits_detail::full_concept{}));

    using propagate_on_container_move_assignment =
        decltype(traits_detail::propagate_on_container_move_assignment<RawAllocator>(
            traits_detail::full_concept{}));

    using propagate_on_container_copy_assignment =
        decltype(traits_detail::propagate_on_container_copy_assignment<RawAllocator>(
            traits_detail::full_concept{}));

    template <class AllocReference>
    static AllocReference select_on_container_copy_construction(const AllocReference &alloc) {
      return alloc;
    }
  };*/


  namespace impl {
    template <typename T>
    struct reference_typedef {
      using reference       = T &;
      using const_reference = const T &;
    };
    template <>
    struct reference_typedef<void> {};
  }  // namespace impl

#define VK_std_allocator_base(base_type) VK_if(VK_and(VK_compiler_msvc, VK_compiler_clang)(protected)) base_type

  /// Wraps a \concept{concept_rawallocator,RawAllocator} and makes it a "normal" \c Allocator.
  /// It allows using a \c RawAllocator anywhere a \c Allocator is required.
  /// \ingroup adapter
  template <typename T, allocator_c RawAllocator, typename Ptr = T *>
  class std_allocator : public impl::reference_typedef<T>, VK_std_allocator_base(allocator_reference<RawAllocator>) {
    using alloc_reference = allocator_reference<RawAllocator>;
    // if it is any_allocator_reference an optimized implementation can be used
    VK_constant bool is_any = same_as<alloc_reference, any_allocator_reference>;

    using base = allocator_reference<RawAllocator>;

    template <typename U>
    using ptr_t = ptr_rebind_t<U, Ptr>;

    template <typename U>
    using param_ptr_t = copy_cvref_t<ptr_t<U>, in_param_t<Ptr>>;

  public:
    //=== typedefs ===//
    using value_type                             = T;
    using pointer                                = Ptr;
    using const_pointer                          = ptr_t<const T>;
    using size_type                              = u64;
    using difference_type                        = ptr_difference_t<pointer>;


    using propagate_on_container_copy_assignment = std::bool_constant<base::propagate_on_container_copy_assignment>;
    using propagate_on_container_move_assignment = std::bool_constant<base::propagate_on_container_move_assignment>;
    using propagate_on_container_swap            = std::bool_constant<base::propagate_on_container_swap>;

    template <typename U>
    struct rebind {
      using other = std_allocator<U, RawAllocator>;
    };

    using allocator_type = typename alloc_reference::allocator_type;

    //=== constructor ===//
    /// \effects Default constructs it by storing a default constructed, stateless \c RawAllocator inside the reference.
    /// \requires The \c RawAllocator type is stateless, otherwise the body of this function will not compile.
    std_allocator() noexcept requires(base::is_stateless) : alloc_reference(allocator_type{}) {}

    /// \effects Creates it from a reference to a \c RawAllocator.
    /// It will store an \ref allocator_reference to it.
    /// \requires The expression <tt>allocator_reference<RawAllocator>(alloc)</tt> is well-formed,
    /// that is either \c RawAlloc is the same as \c RawAllocator or \c RawAllocator is the tag type \ref any_allocator.
    /// If the requirement is not fulfilled this function does not participate in overload resolution.
    /// \note The caller has to ensure that the lifetime of the \c RawAllocator is at least as long as the lifetime
    /// of this \ref std_allocator object.
    template <typename RawAlloc>
    std_allocator(RawAlloc &alloc) noexcept requires(!std::is_base_of_v<std_allocator, RawAlloc> && constructible_from<alloc_reference, RawAlloc &>)
        : alloc_reference(alloc) {
    }

    /// \effects Creates it from a stateless, temporary \c RawAllocator object.
    /// It will not store a reference but create it on the fly.
    /// \requires The \c RawAllocator is stateless
    /// and the expression <tt>allocator_reference<RawAllocator>(alloc)</tt> is well-formed as above,
    /// otherwise this function does not participate in overload resolution.
    template <typename RawAlloc>
    std_allocator(const RawAlloc &alloc) noexcept requires(!std::is_base_of_v<std_allocator, RawAlloc> && constructible_from<alloc_reference, const RawAlloc &>)
        : alloc_reference(alloc) {
    }

    /// \effects Creates it from another \ref allocator_reference using the same allocator type.
    std_allocator(const alloc_reference &alloc) noexcept : alloc_reference(alloc) {}

    /// \details Implicit conversion from any other \ref allocator_storage is forbidden
    /// to prevent accidentally wrapping another \ref allocator_storage inside a \ref allocator_reference.
    template <class StoragePolicy, class OtherMut>
    std_allocator(const allocator_storage<StoragePolicy, OtherMut> &) = delete;

    /// @{
    /// \effects Creates it from another \ref std_allocator allocating a different type.
    /// This is required by the \c Allcoator concept and simply takes the same \ref allocator_reference.
    template <typename U>
    std_allocator(const std_allocator<U, RawAllocator> &alloc) noexcept
        : alloc_reference(alloc) {
    }

    template <typename U>
    std_allocator(std_allocator<U, RawAllocator> &alloc) noexcept : alloc_reference(alloc.get_allocator()) {
    }
    /// @}

    /*/// \returns A copy of the allocator.
    /// This is required by the \c Allocator concept and forwards to the \ref propagation_traits.
    std_allocator select_on_container_copy_construction() const noexcept {
      return std_allocator(base::select_on_container_copy_construction());
    }*/

    //=== allocation/deallocation ===//
    /// \effects Allocates memory using the underlying \concept{concept_rawallocator,RawAllocator}.
    /// If \c n is \c 1, it will call <tt>allocate_node(sizeof(T), alignof(T))</tt>,
    /// otherwise <tt>allocate_array(n, sizeof(T), alignof(T))</tt>.
    /// \returns A pointer to a memory block suitable for \c n objects of type \c T.
    /// \throws Anything thrown by the \c RawAllocator.
    pointer allocate(size_type n, void * = nullptr) {
      return static_cast<pointer>(allocate_impl<T>(n));
    }

    /// \effects Deallcoates memory using the underlying \concept{concept_rawallocator,RawAllocator}.
    /// It will forward to the deallocation function in the same way as in \ref allocate().
    /// \requires The pointer must come from a previous call to \ref allocate() with the same \c n on this object or any copy of it.
    void deallocate(pointer p, size_type n) noexcept {
      deallocate_impl(std::to_address(p), n);
    }

    template <typename U>
    ptr_t<U> allocate_object(size_type n = 1) {
      VK_assert(n != 0);
      return ptr_t<U>(this->template allocate_impl<U>(n));
    }
    template <typename U>
    void deallocate_object(param_ptr_t<U> p, size_type n = 1) noexcept {
      VK_assert(n != 0);
      deallocate_impl(std::to_address(p), n);
    }


    //=== construction/destruction ===//
    /// \effects Creates an object of type \c U at given address using the passed arguments.
    template <typename U, typename... Args>
    void construct(in_param_t<ptr_t<U>> p, Args &&...args) {
      void *mem = std::to_address(p);
      ::new (mem) U(std::forward<Args>(args)...);
    }

    /// \effects Calls the destructor for an object of type \c U at given address.
    template <typename U>
    void destroy(param_ptr_t<U> p) noexcept {
      p->~U();
    }


    template <typename U, typename... Args>
    requires std::constructible_from<U, Args...>
        ptr_t<U> new_object(Args &&...args) {
      auto result = this->allocate_object<U>();

#if VK_exceptions_enabled
      try {
        this->construct(result, std::forward<Args>(args)...);
      }
      catch (...) {
        this->deallocate(result);
        throw;
      }
#else
      this->construct(result, std::forward<Args>(args)...);
#endif
      return result;
    }
    template <typename U>
    void delete_object(param_ptr_t<U> p) noexcept {
      this->destroy(p);
      this->deallocate_object(p);
    }


    //=== getter ===//
    /// \returns The maximum size for an allocation which is <tt>max_array_size() / sizeof(value_type)</tt>.
    /// This is only an upper bound, not the exact maximum.
    size_type max_size() const noexcept {
      return this->max_array_size() / sizeof(value_type);
    }

    /// @{
    /// \effects Returns a reference to the referenced allocator.
    /// \returns For stateful allocators: A (\c const) reference to the stored allocator.
    /// For stateless allocators: A temporary constructed allocator.
    auto get_allocator() noexcept -> decltype(std::declval<alloc_reference>().get_allocator()) {
      return alloc_reference::get_allocator();
    }

    auto get_allocator() const noexcept -> decltype(std::declval<const alloc_reference>().get_allocator()) {
      return alloc_reference::get_allocator();
    }
    /// @}

  private:
    template <typename U>
    U *allocate_impl(size_type n) {
      void *p;
      if constexpr (is_any) {
        p = get_allocator().allocate_impl();
      }
      else {
        if (n == 1)
          p = this->allocate_node(sizeof(U), alignof(U));
        else
          p = this->allocate_array(n, sizeof(U), alignof(U));
      }
      return static_cast<U *>(p);
    }
    template <typename U>
    void deallocate_impl(U *p, size_type n) noexcept {
      if constexpr (is_any) {
        get_allocator().deallocate_impl(p, n, sizeof(U), alignof(U));
      }
      else {
        if (n == 1)
          this->deallocate_node(p, sizeof(U), alignof(U));
        else
          this->deallocate_array(p, n, sizeof(U), alignof(U));
      }
    }

    template <typename U>
    bool equal_to(const std_allocator<U, RawAllocator> &other) const noexcept {
      if constexpr ( base::is_shared )
        return get_allocator() == other.get_allocator();
      else if constexpr (allocator_traits<RawAllocator>::is_stateful)
        return &get_allocator() == &other.get_allocator();
      else
        return true;
    }


    template <typename T1, typename T2, class Impl>
    friend bool operator==(const std_allocator<T1, Impl> &lhs, const std_allocator<T2, Impl> &rhs) noexcept;

    template <typename, allocator_c, typename>
    friend class std_allocator;
  };

  /// \effects Compares two \ref std_allocator object, they are equal if either stateless or reference the same allocator.
  /// \returns The result of the comparision for equality.
  /// \relates std_allocator
  template <typename T, typename U, class Impl>
  bool operator==(const std_allocator<T, Impl> &lhs,
                  const std_allocator<U, Impl> &rhs) noexcept {
    return lhs.equal_to(rhs);
  }

  /*/// \effects Compares two \ref std_allocator object, they are equal if either stateless or reference the same allocator.
/// \returns The result of the comparision for inequality.
/// \relates std_allocator
template <typename T, typename U, class Impl>
bool operator!=(const std_allocator<T, Impl>& lhs,
                const std_allocator<U, Impl>& rhs) noexcept
{
return !(lhs == rhs);
}*/

  /// \returns A new \ref std_allocator for a given type using a certain allocator object.
  /// \relates std_allocator
  template <typename T, class RawAllocator>
  auto make_std_allocator(RawAllocator &&allocator) noexcept
      -> std_allocator<T, typename std::decay<RawAllocator>::type> {
    return {std::forward<RawAllocator>(allocator)};
  }

  /// An alias template for \ref std_allocator using a type-erased \concept{concept_rawallocator,RawAllocator}.
  /// This is the same as using a \ref std_allocator with the tag type \ref any_allocator.
  /// The implementation is optimized to call fewer virtual functions.
  /// \ingroup adapter
  template <typename T>
  using any_std_allocator = std_allocator<T, any_allocator>;

  /// \returns A new \ref any_std_allocator for a given type using a certain allocator object.
  /// \relates any_std_allocator
  template <typename T, class RawAllocator>
  any_std_allocator<T> make_any_std_allocator(RawAllocator &&allocator) noexcept {
    return {std::forward<RawAllocator>(allocator)};
  }
}  // namespace valkyrie

#endif  //VALKYRIE_MEMORY_STD_ALLOCATOR_HPP
