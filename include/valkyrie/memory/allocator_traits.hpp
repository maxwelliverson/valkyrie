//
// Created by maxwe on 2021-03-05.
//

#ifndef VALKYRIE_MEMORY_ALLOCATOR_TRAITS_HPP
#define VALKYRIE_MEMORY_ALLOCATOR_TRAITS_HPP

#include <valkyrie/traits.hpp>

namespace valkyrie{

  namespace detail{
    template <typename T>
    struct protected_is_composable_test : T{

      meta::true_type test_is_composable() const requires(requires{ { this->is_composable() } -> same_as<bool>; });
      meta::false_type test_is_composable() const;

    };
  }

  class memory_block;

  template <typename T>
  struct allocator_traits;
  template <typename T>
  struct composable_allocator_traits;

  template <typename T>
  concept node = same_as<T, void*>;
  template <typename T>
  concept node_array = node<T>;

  /*template <typename T>
  concept raw_allocator = requires {
    typename allocator_traits<T>::allocator_type;
    typename allocator_traits<T>::is_stateful;
  }
                          &&requires(typename allocator_traits<remove_cvref_t<T>>::allocator_type &alloc,
  const typename allocator_traits<remove_cvref_t<T>>::allocator_type &calloc,
  void *n,
      u64 count,
  u64 size,
      u64 align) {
{ allocator_traits<remove_cvref_t<T>>::allocate_node(alloc, size, align) } -> node;
{ allocator_traits<remove_cvref_t<T>>::allocate_array(alloc, count, size, align) } -> node_array;
allocator_traits<remove_cvref_t<T>>::deallocate_node(alloc, n, size, align);
allocator_traits<remove_cvref_t<T>>::deallocate_array(alloc, n, count, size, align);
{ allocator_traits<remove_cvref_t<T>>::max_node_size(calloc) } -> std::convertible_to<u64>;
{ allocator_traits<remove_cvref_t<T>>::max_array_size(calloc) } -> std::convertible_to<u64>;
{ allocator_traits<remove_cvref_t<T>>::max_alignment(calloc) } -> std::convertible_to<u64>;
};*/





  namespace detail
  {
    template <typename A>
    concept has_construct = requires{
      typename A::pointer;
      typename A::value_type;
    } && requires(A& alloc, typename A::pointer p, typename A::value_type value){
      alloc.construct(p, std::move(value));
    };

    template <typename A>
    concept has_destroy = requires{
      typename A::pointer;
    } && requires(A& alloc, typename A::pointer p){
      alloc.destroy(p);
    };

    template <typename A>
    concept standard_allocator = !has_construct<A> && !has_destroy<A>;
  } // namespace detail

  template <typename A>
  struct allocator_is_raw_allocator : meta::value_wrapper<detail::standard_allocator<A>>{};
  template <typename T>
  struct allocator_is_raw_allocator<std::allocator<T>> : meta::true_type{};

namespace traits_detail // use seperate namespace to avoid name clashes
{
  // full_concept has the best conversion rank, error the lowest
  // used to give priority to the functions
  struct error
  {
    operator void*() const noexcept
    {
      VK_unreachable;
      return nullptr;
    }
  };
  struct std_concept : error
  {
  };
  struct min_concept : std_concept
  {
  };
  struct full_concept : min_concept
  {
  };

  // used to delay assert in handle_error() until instantiation
  template <typename T>
  struct invalid_allocator_concept
  {
    static const bool error = false;
  };

  //=== allocator_type ===//
  // if Allocator has a member template `rebind`, use that to rebind to `char`
  // else if Allocator has a member `value_type`, rebind by changing argument
  // else does nothing
  template <class Allocator>
  auto rebind_impl(int) -> typename Allocator::template rebind<char>::other&;

  template <class Allocator, typename T>
  struct allocator_rebinder
  {
    using type = Allocator&;
  };

  template <template <typename, typename...> class Alloc, typename U, typename... Args, typename T> requires(requires{ typename Alloc<T, Args...>; })
  struct allocator_rebinder<Alloc<U, Args...>, T>
  {
    using type = Alloc<T, Args...>&;
  };

  template <class Allocator, typename = typename Allocator::value_type>
  auto rebind_impl(char) -> typename allocator_rebinder<Allocator, char>::type;

  template <class Allocator>
  auto rebind_impl(...) -> Allocator&;

  template <class Allocator>
  struct allocator_type_impl // required for MSVC
  {
    using type = decltype(rebind_impl<Allocator>(0));
  };

  template <class Allocator>
  using allocator_type = typename std::decay<typename allocator_type_impl<Allocator>::type>::type;

  //=== is_stateful ===//
  // first try to access Allocator::is_stateful,
  // then use whether or not the type is empty
  template <class Allocator>
  auto is_stateful(full_concept) -> decltype(typename Allocator::is_stateful{});

  template <class Allocator, bool IsEmpty>
  struct is_stateful_impl;

  template <class Allocator>
  struct is_stateful_impl<Allocator, true>
  {
    static_assert(std::is_default_constructible<Allocator>::value,
    "RawAllocator is empty but not default constructible ."
    "This means it is not a stateless allocator. "
    "If this is actually intended provide the appropriate is_stateful "
    "typedef in your class.");
    using type = std::false_type;
  };

  template <class Allocator>
  struct is_stateful_impl<Allocator, false>
  {
    using type = std::true_type;
  };

  template <class Allocator>
  auto is_stateful(min_concept) ->
  typename is_stateful_impl<Allocator, std::is_empty<Allocator>::value>::type;

  
  template <typename T, typename U>
  struct valid_return_type;
  template <typename T>
  struct valid_return_type<T, T>{
    using type = T;
  };
  
  template <typename T, typename U>
  using valid_return_type_t = typename valid_return_type<T, U>::type;

  
#define VK_auto_return(expr) decltype(expr) { return expr; }
#define VK_auto_return_type(expr, type) valid_return_type_t<decltype(expr), type> { return expr; }

  //=== allocate_node() ===//
  // first try Allocator::allocate_node
  // then assume std_allocator and call Allocator::allocate
  // then error
  template <class Allocator>
  auto allocate_node(full_concept, Allocator& alloc, u64 size,
                     u64 alignment)
  -> VK_auto_return_type(alloc.allocate_node(size, alignment), void*)

  template <class Allocator>
  auto allocate_node(std_concept, Allocator& alloc, u64 size, u64)
  -> VK_auto_return(static_cast<void*>(alloc.allocate(size)))

  template <class Allocator>
  error allocate_node(error, Allocator&, u64, u64)
  {
    static_assert(invalid_allocator_concept<Allocator>::error,
                  "type is not a RawAllocator as it does not provide: void* "
                  "allocate_node(u64, "
                  "u64)");
    return {};
  }

  //=== deallocate_node() ===//
  // first try Allocator::deallocate_node
  // then assume std_allocator and call Allocator::deallocate
  // then error
  template <class Allocator>
  auto deallocate_node(full_concept, Allocator& alloc, void* ptr, u64 size,
                       u64 alignment) noexcept
  -> VK_auto_return_type(alloc.deallocate_node(ptr, size, alignment), void)

  template <class Allocator>
  auto deallocate_node(std_concept, Allocator& alloc, void* ptr, u64 size,
                       u64) noexcept
  -> VK_auto_return_type(alloc.deallocate(static_cast<char*>(ptr), size), void)

  template <class Allocator>
  error deallocate_node(error, Allocator&, void*, u64, u64)
  {
    static_assert(invalid_allocator_concept<Allocator>::error,
                  "type is not a RawAllocator as it does not provide: void "
                  "deallocate_node(void*, u64, "
                  "u64)");
    return error{};
  }

  //=== allocate_array() ===//
  // first try Allocator::allocate_array
  // then forward to allocate_node()
  template <class Allocator>
  auto allocate_array(full_concept, Allocator& alloc, u64 count, u64 size,
                      u64 alignment)
  -> VK_auto_return_type(alloc.allocate_array(count, size, alignment), void*)

  template <class Allocator>
  void* allocate_array(min_concept, Allocator& alloc, u64 count,
                       u64 size, u64 alignment)
  {
    return allocate_node(full_concept{}, alloc, count * size, alignment);
  }

  //=== deallocate_array() ===//
  // first try Allocator::deallocate_array
  // then forward to deallocate_node()
  template <class Allocator>
  auto deallocate_array(full_concept, Allocator& alloc, void* ptr, u64 count,
                        u64 size, u64 alignment) noexcept
  -> VK_auto_return_type(alloc.deallocate_array(ptr, count, size, alignment),
  void)

  template <class Allocator>
  void deallocate_array(min_concept, Allocator& alloc, void* ptr,
                        u64 count, u64 size,
                        u64 alignment) noexcept
{
  deallocate_node(full_concept{}, alloc, ptr, count * size, alignment);
}

//=== max_node_size() ===//
// first try Allocator::max_node_size()
// then return maximum value
template <class Allocator>
auto max_node_size(full_concept, const Allocator& alloc)
-> VK_auto_return_type(alloc.max_node_size(), u64)

template <class Allocator>
u64 max_node_size(min_concept, const Allocator&) noexcept
{
return u64(-1);
}

//=== max_node_size() ===//
// first try Allocator::max_array_size()
// then forward to max_node_size()
template <class Allocator>
auto max_array_size(full_concept, const Allocator& alloc)
-> VK_auto_return_type(alloc.max_array_size(), u64)

template <class Allocator>
u64 max_array_size(min_concept, const Allocator& alloc)
{
  return max_node_size(full_concept{}, alloc);
}

//=== max_alignment() ===//
// first try Allocator::max_alignment()
// then return detail::max_alignment
template <class Allocator>
auto max_alignment(full_concept, const Allocator& alloc)
-> VK_auto_return_type(alloc.max_alignment(), u64)

template <class Allocator>
u64 max_alignment(min_concept, const Allocator&)
{
  return alignof(std::max_align_t);
}
} // namespace traits_detail

/// The default specialization of the allocator_traits for a \concept{concept_rawallocator,RawAllocator}.
/// See the last link for the requirements on types that do not specialize this class and the interface documentation.
/// Any specialization must provide the same interface.
/// \ingroup core
template <class Allocator>
class allocator_traits
{
public:
  using allocator_type = traits_detail::allocator_type<Allocator>;
  using is_stateful = decltype(traits_detail::is_stateful<Allocator>(traits_detail::full_concept{}));

  static void* allocate_node(allocator_type& state, u64 size,
                             u64 alignment)
  {
    static_assert(allocator_is_raw_allocator<Allocator>::value,
                  "Allocator cannot be used as RawAllocator because it provides custom "
                  "construct()/destroy()");
    return traits_detail::allocate_node(traits_detail::full_concept{}, state, size,
                                        alignment);
  }

  static void* allocate_array(allocator_type& state, u64 count, u64 size,
                              u64 alignment)
  {
    static_assert(allocator_is_raw_allocator<Allocator>::value,
                  "Allocator cannot be used as RawAllocator because it provides custom "
                  "construct()/destroy()");
    return traits_detail::allocate_array(traits_detail::full_concept{}, state, count,
                                         size, alignment);
  }

  static void deallocate_node(allocator_type& state, void* node, u64 size,
                              u64 alignment) noexcept
  {
    static_assert(allocator_is_raw_allocator<Allocator>::value,
                  "Allocator cannot be used as RawAllocator because it provides custom "
                  "construct()/destroy()");
    traits_detail::deallocate_node(traits_detail::full_concept{}, state, node, size,
                                   alignment);
  }

  static void deallocate_array(allocator_type& state, void* array, u64 count,
                               u64 size, u64 alignment) noexcept
  {
    static_assert(allocator_is_raw_allocator<Allocator>::value,
                  "Allocator cannot be used as RawAllocator because it provides custom "
                  "construct()/destroy()");
    traits_detail::deallocate_array(traits_detail::full_concept{}, state, array, count,
                                    size, alignment);
  }

  static u64 max_node_size(const allocator_type& state)
  {
    static_assert(allocator_is_raw_allocator<Allocator>::value,
                  "Allocator cannot be used as RawAllocator because it provides custom "
                  "construct()/destroy()");
    return traits_detail::max_node_size(traits_detail::full_concept{}, state);
  }

  static u64 max_array_size(const allocator_type& state)
  {
    static_assert(allocator_is_raw_allocator<Allocator>::value,
                  "Allocator cannot be used as RawAllocator because it provides custom "
                  "construct()/destroy()");
    return traits_detail::max_array_size(traits_detail::full_concept{}, state);
  }

  static u64 max_alignment(const allocator_type& state)
  {
    static_assert(allocator_is_raw_allocator<Allocator>::value,
                  "Allocator cannot be used as RawAllocator because it provides custom "
                  "construct()/destroy()");
    return traits_detail::max_alignment(traits_detail::full_concept{}, state);
  }

#if !defined(DOXYGEN)
  using default_traits = std::true_type;
#endif
};

template <typename T>
concept raw_allocator = requires{
  typename allocator_traits<T>::allocator_type;
} && (allocator_traits<T>::default_traits::value ||
      requires(typename allocator_traits<T>::allocator_type& alloc, u64 n){
        { traits_detail::allocate_node(traits_detail::full_concept{}, alloc, n, n) }
        -> not_same_as<traits_detail::error>;
        { traits_detail::deallocate_node(traits_detail::full_concept{}, alloc, nullptr, n, n) } noexcept
        -> not_same_as<traits_detail::error>;
      });


namespace traits_detail
{
  //=== try_allocate_node() ===//
  // try Allocator::try_allocate_node
  // otherwise error
  template <class Allocator>
  auto try_allocate_node(full_concept, Allocator& alloc, u64 size,
  u64 alignment) noexcept
  -> VK_auto_return_type(alloc.try_allocate_node(size, alignment), void*)

  template <class Allocator>
  error try_allocate_node(error, Allocator&, u64, u64)
  {
    static_assert(invalid_allocator_concept<Allocator>::error,
                  "type is not a composable RawAllocator as it does not provide: void* "
                  "try_allocate_node(u64, "
                  "u64)");
    return {};
  }

  //=== try_deallocate_node() ===//
  // try Allocator::try_deallocate_node
  // otherwise error
  template <class Allocator>
  auto try_deallocate_node(full_concept, Allocator& alloc, void* ptr, u64 size,
  u64 alignment) noexcept
  -> VK_auto_return_type(alloc.try_deallocate_node(ptr, size, alignment), bool)

  template <class Allocator>
  error try_deallocate_node(error, Allocator&, void*, u64, u64)
  {
    static_assert(invalid_allocator_concept<Allocator>::error,
                  "type is not a composable RawAllocator as it does not provide: bool "
                  "try_deallocate_node(void*, u64, "
                  "u64)");
    return error{};
  }

  //=== try_allocate_array() ===//
  // first try Allocator::try_allocate_array
  // then forward to try_allocate_node()
  template <class Allocator>
  auto try_allocate_array(full_concept, Allocator& alloc, u64 count,
  u64 size, u64 alignment) noexcept
  -> VK_auto_return_type(alloc.try_allocate_array(count, size, alignment),
  void*)

  template <class Allocator>
  void* try_allocate_array(min_concept, Allocator& alloc, u64 count,
                           u64 size, u64 alignment)
  {
    return try_allocate_node(full_concept{}, alloc, count * size, alignment);
  }

  //=== try_deallocate_array() ===//
  // first try Allocator::try_deallocate_array
  // then forward to try_deallocate_node()
  template <class Allocator>
  auto try_deallocate_array(full_concept, Allocator& alloc, void* ptr, u64 count,
  u64 size, u64 alignment) noexcept
  -> VK_auto_return_type(alloc.try_deallocate_array(ptr, count, size,
      alignment),
  bool)

  template <class Allocator>
  bool try_deallocate_array(min_concept, Allocator& alloc, void* ptr,
                            u64 count, u64 size,
                            u64 alignment) noexcept
{
  return try_deallocate_node(full_concept{}, alloc, ptr, count * size, alignment);
}
} // namespace traits_detail

/// The default specialization of the composable_allocator_traits for a \concept{concept_composableallocator,ComposableAllocator}.
/// See the last link for the requirements on types that do not specialize this class and the interface documentation.
/// Any specialization must provide the same interface.
/// \ingroup core
template <class Allocator>
class composable_allocator_traits {
public:
  using allocator_type = typename allocator_traits<Allocator>::allocator_type;

  static void* try_allocate_node(allocator_type& state, u64 size,
                                 u64 alignment) noexcept
  {
    static_assert(raw_allocator<Allocator>);
    return traits_detail::try_allocate_node(traits_detail::full_concept{}, state, size, alignment);
  }

  static void* try_allocate_array(allocator_type& state, u64 count,
                                  u64 size, u64 alignment) noexcept
  {
    return traits_detail::try_allocate_array(traits_detail::full_concept{}, state,
                                             count, size, alignment);
  }

  static bool try_deallocate_node(allocator_type& state, void* node, u64 size,
                                  u64 alignment) noexcept
  {
    return traits_detail::try_deallocate_node(traits_detail::full_concept{}, state,
                                              node, size, alignment);
  }

  static bool try_deallocate_array(allocator_type& state, void* array, u64 count,
                                   u64 size, u64 alignment) noexcept {
    return traits_detail::try_deallocate_array(traits_detail::full_concept{}, state,
                                               array, count, size, alignment);
  }

#if !defined(DOXYGEN)
  using default_traits = std::true_type;
#endif
};


  template <typename A>
  concept composable_allocator = raw_allocator<A> && (composable_allocator_traits<A>::default_traits::value
                               || requires(typename composable_allocator_traits<A>::allocator_type& alloc,
                                           u64 n){
    { traits_detail::try_allocate_node(traits_detail::full_concept{}, alloc, n, n) }
      -> not_same_as<traits_detail::error>;
    { traits_detail::try_deallocate_node(traits_detail::full_concept{}, alloc, nullptr, n, n) } noexcept
        -> not_same_as<traits_detail::error>;
  });

template <typename T>
concept block_allocator = requires(T& alloc, const T& calloc, memory_block block){
  { alloc.allocate_block() } -> same_as<memory_block>;
  alloc.deallocate_block(block);
  { calloc.next_block_size() } -> std::convertible_to<u64>;
};

template <typename T>
concept memory_storage_policy = requires(T& policy, const T& cpolicy){
  typename T::allocator_type;
  { policy.get_allocator() } -> std::convertible_to<typename T::allocator_type&>;
  { cpolicy.get_allocator() } -> std::convertible_to<const typename T::allocator_type&>;
} && decltype(std::declval<const detail::protected_is_composable_test<T>&>().test_is_composable())::value;
template <typename T>
concept segregatable_allocator = requires(T& seg, const T& cseg, u64 count, u64 size, u64 align){
  requires raw_allocator<typename T::allocator_type>;
  { seg.get_allocator() } -> exact_same_as<typename T::allocator_type&>;
  { cseg.get_allocator() } -> exact_same_as<const typename T::allocator_type&>;
  { cseg.use_allocate_node(size, align) } -> same_as<bool>;
  { cseg.use_allocate_array(count, size, align) } -> same_as<bool>;
};
template <typename T>
concept memory_tracker      = requires(T& t, void* p, u64 count, u64 size, u64 align){
  t.on_node_allocation(p, size, align);
  t.on_array_allocation(p, count, size, align);
  t.on_node_deallocation(p, size, align);
  t.on_array_deallocation(p, count, size, align);
};
template <typename T>
concept memory_deep_tracker = memory_tracker<T> && requires(T& t, memory_block block){
  t.on_allocator_growth(block);
  t.on_allocator_shrinkage(block);
};


}

#endif//VALKYRIE_MEMORY_ALLOCATOR_TRAITS_HPP
