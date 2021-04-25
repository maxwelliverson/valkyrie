//
// Created by maxwe on 2021-03-12.
//

#ifndef VALKYRIE_MEMORY_SMART_PTR_HPP
#define VALKYRIE_MEMORY_SMART_PTR_HPP


#include <memory>
#include <type_traits>

#include "detail/utility.hpp"
#include "deleter.hpp"
#include "std_allocator.hpp"

namespace valkyrie{

  /// A \c std::unique_ptr that deletes using a \concept{concept_rawallocator,RawAllocator}.
  ///
  /// It is an alias template using \ref allocator_deleter as \c Deleter class.
  /// \ingroup adapter
  template <typename T, class RawAllocator>
  using unique_ptr = std::unique_ptr<T, allocator_deleter<T, RawAllocator>>;

  /// A \c std::unique_ptr that deletes using a \concept{concept_rawallocator,RawAllocator} and allows polymorphic types.
  ///
  /// It can only be created by converting a regular unique pointer to a pointer to a derived class,
  /// and is meant to be used inside containers.
  /// It is an alias template using \ref allocator_polymorphic_deleter as \c Deleter class.
  /// \note It has a relatively high overhead, so only use it if you have to.
  /// \ingroup adapter
  template <class BaseType, class RawAllocator>
  using unique_base_ptr = std::unique_ptr<BaseType, allocator_polymorphic_deleter<BaseType, RawAllocator>>;


  using std::shared_ptr;

  namespace detail {
    template <typename T, class RawAllocator, typename... Args>
    unique_ptr<T, RawAllocator> allocate_unique(allocator_reference<RawAllocator> alloc, Args&&... args) {
      using raw_ptr = std::unique_ptr<T, allocator_deallocator<T, RawAllocator>>;

      auto memory = alloc.allocate_node(sizeof(T), alignof(T));
      // raw_ptr deallocates memory in case of constructor exception
      raw_ptr result(static_cast<T*>(memory), {alloc});
      // call constructor
      ::new (memory) T(detail::forward<Args>(args)...);
      // pass ownership to return value using a deleter that calls destructor
      return {result.release(), {alloc}};
    }

    template <typename T, typename... Args>
    void construct(T* cur, T* end, Args&&... args) noexcept requires(std::is_nothrow_default_constructible_v<T>) {
      for (; cur != end; ++cur)
        ::new (static_cast<void*>(cur)) T(detail::forward<Args>(args)...);
    }

    template <typename T, typename... Args>
    void construct(T* begin, T* end, Args&&... args)
    {
#if VK_exceptions_enabled
      auto cur = begin;
                try
                {
                    for (; cur != end; ++cur)
                        ::new (static_cast<void*>(cur)) T(detail::forward<Args>(args)...);
                }
                catch (...)
                {
                    for (auto el = begin; el != cur; ++el)
                        el->~T();
                    throw;
                }
#else
      for (; cur != end; ++cur)
        ::new (static_cast<void*>(cur)) T(detail::forward<Args>(args)...);
#endif
    }

    template <typename T, class RawAllocator>
    unique_ptr<T[], RawAllocator>
    allocate_array_unique(u64 size, allocator_reference<RawAllocator> alloc)
    {
      using raw_ptr = std::unique_ptr<T[], allocator_deallocator<T[], RawAllocator>>;

      auto memory = alloc.allocate_array(size, sizeof(T), alignof(T));
      // raw_ptr deallocates memory in case of constructor exception
      raw_ptr result(static_cast<T*>(memory), {alloc, size});
      construct(result.get(), result.get() + size);
      // pass ownership to return value using a deleter that calls destructor
      return {result.release(), {alloc, size}};
    }
  } // namespace detail



  /// Creates a \c std::unique_ptr using a \concept{concept_rawallocator,RawAllocator} for the allocation.
  /// \effects Allocates memory for the given type using the allocator
  /// and creates a new object inside it passing the given arguments to its constructor.
  /// \returns A \c std::unique_ptr owning that memory.
  /// \note If the allocator is stateful a reference to the \c RawAllocator will be stored inside the deleter,
  /// the caller has to ensure that the object lives as long as the smart pointer.
  /// \ingroup adapter
  template <typename T, class RawAllocator, typename... Args>
  unique_ptr<T, std::decay_t<RawAllocator>> allocate_unique(RawAllocator&& alloc, Args&&... args)
  requires(!std::is_array_v<T>) {
  return detail::allocate_unique<T>(
        make_allocator_reference(detail::forward<RawAllocator>(alloc)),
        detail::forward<Args>(args)...);
}

/// Creates a \c std::unique_ptr using a type-erased \concept{concept_rawallocator,RawAllocator} for the allocation.
/// It is the same as the other overload but stores the reference to the allocator type-erased inside the \c std::unique_ptr.
/// \effects Allocates memory for the given type using the allocator
/// and creates a new object inside it passing the given arguments to its constructor.
/// \returns A \c std::unique_ptr with a type-erased allocator reference owning that memory.
/// \note If the allocator is stateful a reference to the \c RawAllocator will be stored inside the deleter,
/// the caller has to ensure that the object lives as long as the smart pointer.
/// \ingroup adapter
template <typename T, class RawAllocator, typename... Args>
unique_ptr<T, any_allocator> allocate_unique(any_allocator, RawAllocator&& alloc, Args&&... args)
    requires(!std::is_array_v<T>)  {
return detail::allocate_unique<T, any_allocator>(make_allocator_reference(
    detail::forward<RawAllocator>(
    alloc)),
    detail::forward<Args>(args)...);
}

/// Creates a \c std::unique_ptr owning an array using a \concept{concept_rawallocator,RawAllocator} for the allocation.
/// \effects Allocates memory for an array of given size and value initializes each element inside of it.
/// \returns A \c std::unique_ptr owning that array.
/// \note If the allocator is stateful a reference to the \c RawAllocator will be stored inside the deleter,
/// the caller has to ensure that the object lives as long as the smart pointer.
/// \ingroup adapter
template <typename T, class RawAllocator>
unique_ptr<T, std::decay_t<RawAllocator>> allocate_unique(RawAllocator&& alloc, u64 size)
requires(std::is_array_v<T>){
  return detail::allocate_array_unique<std::remove_extent_t<T>>(size, make_allocator_reference(detail::forward<RawAllocator>(alloc)));
}

/// Creates a \c std::unique_ptr owning an array using a type-erased \concept{concept_rawallocator,RawAllocator} for the allocation.
/// It is the same as the other overload but stores the reference to the allocator type-erased inside the \c std::unique_ptr.
/// \effects Allocates memory for an array of given size and value initializes each element inside of it.
/// \returns A \c std::unique_ptr with a type-erased allocator reference owning that array.
/// \note If the allocator is stateful a reference to the \c RawAllocator will be stored inside the deleter,
/// the caller has to ensure that the object lives as long as the smart pointer.
/// \ingroup adapter
template <typename T, class RawAllocator>
unique_ptr<T, any_allocator> allocate_unique(any_allocator, RawAllocator&& alloc, u64 size)
requires(std::is_array_v<T>)
{
return detail::allocate_array_unique<std::remove_extent_t<T>,
    any_allocator>(size, make_allocator_reference(
    detail::forward<RawAllocator>(
    alloc)));
}

/// Creates a \c std::shared_ptr using a \concept{concept_rawallocator,RawAllocator} for the allocation.
/// It is similar to \c std::allocate_shared but uses a \c RawAllocator (and thus also supports any \c Allocator).
/// \effects Calls \ref std_allocator::make_std_allocator to wrap the allocator and forwards to \c std::allocate_shared.
/// \returns A \c std::shared_ptr created using \c std::allocate_shared.
/// \note If the allocator is stateful a reference to the \c RawAllocator will be stored inside the shared pointer,
/// the caller has to ensure that the object lives as long as the smart pointer.
/// \ingroup adapter
template <typename T, class RawAllocator, typename... Args>
shared_ptr<T> allocate_shared(RawAllocator&& alloc, Args&&... args)
{
  return std::allocate_shared<T>(make_std_allocator<T>(
      detail::forward<RawAllocator>(alloc)),
                                 detail::forward<Args>(args)...);
}



  /*template <typename T>
  class handle;
  template <typename T>
  class unique_ptr;
  template <typename T>
  class shared_ptr;*/



}

#endif//VALKYRIE_MEMORY_SMART_PTR_HPP
