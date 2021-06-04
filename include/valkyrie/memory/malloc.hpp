//
// Created by maxwe on 2021-05-14.
//

#ifndef VALKYRIE_MEMORY_MALLOC_HPP
#define VALKYRIE_MEMORY_MALLOC_HPP

#include "default_allocator.hpp"

namespace valkyrie{


  template <typename Alloc> requires allocator_c<Alloc>
  void raw_memcpy(Alloc& alloc, void* to, const void* from, u64 bytes) noexcept {
    allocator_traits<Alloc>::copy_bytes(alloc, to, from, bytes);
  }
  template <typename Alloc> requires allocator_c<Alloc>
  void raw_memmove(Alloc& alloc, void* to, const void* from, u64 bytes) noexcept {
    allocator_traits<Alloc>::move_bytes(alloc, to, from, bytes);
  }
  template <typename Alloc> requires allocator_c<Alloc>
  void raw_memset(Alloc& alloc, void* to, int val, u64 bytes) noexcept {
    allocator_traits<Alloc>::set_bytes(alloc, to, val, bytes);
  }
  template <typename Alloc> requires allocator_c<Alloc>
  void raw_memzero(Alloc& alloc, void* to, u64 bytes) noexcept {
    allocator_traits<Alloc>::zero_bytes(alloc, to, bytes);
  }




  template <typename Alloc> requires allocator_c<Alloc>
  void* raw_malloc(Alloc& allocator, u64 size) noexcept {
    return allocator_traits<Alloc>::allocate_array(allocator, size, 1, max_alignment);
  }
  template <typename Alloc> requires allocator_c<Alloc>
  void* raw_aligned_malloc(Alloc& allocator, u64 size, u64 align) noexcept {
    return allocator_traits<Alloc>::allocate_array(allocator, size, 1, align);
  }
  template <typename Alloc> requires allocator_c<Alloc>
  void* raw_calloc(Alloc& allocator, u64 size) noexcept {
    void* ptr = raw_malloc(allocator, size);
    raw_memzero(allocator, ptr, size);
    return ptr;
  }
  template <typename Alloc> requires allocator_c<Alloc>
  void* raw_aligned_calloc(Alloc& allocator, u64 size, u64 align) noexcept {
    void* ptr = raw_aligned_malloc(allocator, size, align);
    raw_memzero(allocator, ptr, size);
    return ptr;
  }
  template <typename Alloc> requires allocator_c<Alloc>
  void* raw_realloc(Alloc& allocator, void* ptr, u64 new_size, u64 old_size) noexcept {
    return allocator_traits<Alloc>::reallocate_array(
        allocator,
        ptr,
        new_size,
        old_size,
        1,
        max_alignment);
  }
  template <typename Alloc> requires allocator_c<Alloc>
  void* raw_aligned_realloc(Alloc& allocator, void* ptr, u64 new_size, u64 old_size, u64 align) noexcept {
    return allocator_traits<Alloc>::reallocate_array(
        allocator,
        ptr,
        new_size,
        old_size,
        1,
        align);
  }
  template <typename Alloc> requires allocator_c<Alloc>
  void  raw_free(Alloc& allocator, void* ptr, u64 size) noexcept {
    allocator_traits<Alloc>::deallocate_array(allocator, ptr, size, 1, max_alignment);
  }
  template <typename Alloc> requires allocator_c<Alloc>
  void  raw_aligned_free(Alloc& allocator, void* ptr, u64 size, u64 align) noexcept {
    allocator_traits<Alloc>::deallocate_array(allocator, ptr, size, 1, align);
  }



  template <typename T, typename Alloc> requires ( allocator_c<Alloc> && trivial_c<T>)
  T*   typed_malloc(Alloc& allocator, u64 count) noexcept {
    return static_cast<T*>(allocator_traits<Alloc>::allocate_array(
        allocator,
        count,
        sizeof(T),
        alignof(T)));
  }
  template <typename T, typename Alloc> requires ( allocator_c<Alloc> && trivial_c<T> )
  T*   typed_calloc(Alloc& allocator, u64 count) noexcept {
    using trt = allocator_traits<Alloc>;
    T* ptr = typed_malloc<T>(allocator, count);
    raw_memzero(allocator, ptr, count * sizeof(T));
    allocator_traits<Alloc>::zero_bytes(allocator, ptr, count * sizeof(T));
    return ptr;
  }
  template <typename T, typename Alloc> requires ( allocator_c<Alloc> && trivial_c<T> )
  T*   typed_realloc(Alloc& allocator, T* ptr, u64 new_count, u64 old_count) noexcept {
    return static_cast<T*>(allocator_traits<Alloc>::reallocate_array(
        allocator,
        ptr,
        new_count,
        old_count,
        sizeof(T),
        alignof(T)));
  }
  template <typename T, typename Alloc> requires ( allocator_c<Alloc> && trivial_c<T> )
  void typed_free(Alloc& allocator, T* ptr, u64 count) noexcept {
    allocator_traits<Alloc>::deallocate_array(allocator, ptr, count, sizeof(T), alignof(T));
  }
}

#endif  //VALKYRIE_MEMORY_MALLOC_HPP
