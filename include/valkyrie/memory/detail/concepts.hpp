//
// Created by maxwe on 2021-03-04.
//

#ifndef VALKYRIE_MEMORY_DETAIL_CONCEPTS_HPP
#define VALKYRIE_MEMORY_DETAIL_CONCEPTS_HPP

#include <valkyrie/traits.hpp>

namespace valkyrie{

  class memory_block;

  template <typename T>
  struct allocator_traits;
  template <typename T>
  struct composable_allocator_traits;

  template <typename T>
  concept node = same_as<T, void*>;
  template <typename T>
  concept node_array = node<T>;

  template <typename T>
  concept raw_allocator = requires {
    typename allocator_traits<T>::allocator_type;
    typename allocator_traits<T>::is_stateful;
  }
  &&requires(typename allocator_traits<T>::allocator_type &alloc,
             const typename allocator_traits<T>::allocator_type &calloc,
             void *n,
             u64 count,
             u64 size,
             u64 align) {
    { allocator_traits<T>::allocate_node(alloc, size, align) } -> node;
    { allocator_traits<T>::allocate_array(alloc, count, size, align) } -> node_array;
    allocator_traits<T>::deallocate_node(alloc, n, size, align);
    allocator_traits<T>::deallocate_array(alloc, n, count, size, align);
    { allocator_traits<T>::max_node_size(calloc) } -> std::convertible_to<u64>;
    { allocator_traits<T>::max_array_size(calloc) } -> std::convertible_to<u64>;
    { allocator_traits<T>::max_alignment(calloc) } -> std::convertible_to<u64>;
  };

  template <typename T>
  concept composable_raw_allocator = requires{
    typename composable_allocator_traits<T>::allocator_type;
  }
  && requires(typename composable_allocator_traits<T>::allocator_type&       alloc,
              void*                                                          n,
              u64                                                            count,
              u64                                                            size,
              u64                                                            align){
    { composable_allocator_traits<T>::try_allocate_node(alloc, size, align) } noexcept -> node;
    { composable_allocator_traits<T>::try_allocate_array(alloc, count, size, align) } noexcept -> node_array;
    { composable_allocator_traits<T>::try_deallocate_node(alloc, n, size, align) } -> same_as<bool>;
    { composable_allocator_traits<T>::try_deallocate_array(alloc, n, count, size, align) } -> same_as<bool>;
  };
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
    { cpolicy.is_composable() } -> same_as<bool>;
  };
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

#endif//VALKYRIE_MEMORY_DETAIL_CONCEPTS_HPP
