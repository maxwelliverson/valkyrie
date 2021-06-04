//
// Created by maxwe on 2021-05-10.
//

#ifndef VALKYRIE_MEMORY_NEW_DELETE_HPP
#define VALKYRIE_MEMORY_NEW_DELETE_HPP

#include "default_allocator.hpp"

namespace valkyrie{

  template <typename T, typename Alloc, typename ...Args> requires (allocator_c<Alloc> && constructible_from<T, Args...>)
  T* new_object(Alloc& allocator, Args&& ...args) {
    using trt = allocator_traits<Alloc>;
    T* ptr = static_cast<T*>(trt::allocate_node(allocator, sizeof(T), alignof(T)));

#if VK_exceptions_enabled
    if ( std::is_nothrow_constructible_v<T, Args...> ) {
      new (ptr) T{ std::forward<Args>(args)... };
    }
    else {
      try {
        new (ptr) T{ std::forward<Args>(args)... };
      }
      catch (...) {
        trt::deallocate_node(allocator, ptr, sizeof(T), alignof(T));
        throw;
      }
    }
#else
    new (ptr) T{ std::forward<Args>(args)... };
#endif
    trt::on_new_object(allocator, ptr);
    return ptr;
  }
  template <typename T, typename ...Args> requires constructible_from<T, Args...>
  T* new_object(Args&& ...args) {
    default_allocator allocator{};
    return new_object<T>(allocator, std::forward<Args>(args)...);
  }

  template <typename T, typename Alloc> requires (allocator_c<Alloc> && std::is_default_constructible_v<T>)
  T* new_array(Alloc& allocator, u64 size) {
    using trt = allocator_traits<Alloc>;
    T* ptr = static_cast<T*>(trt::allocate_array(allocator, size, sizeof(T), alignof(T)));
#if VK_exceptions_enabled
    if constexpr ( std::is_nothrow_default_constructible_v<T> ) {
      for (T* addr = ptr; addr < ptr + size; ++addr)
        new (addr) T;
    }
    else {
      T* addr = ptr;
      T* const end = ptr + size;
      try {
        for (; addr < end; ++addr)
          new (addr) T;
      }
      catch (...) {
        for (T* begin = ptr; begin < addr; ++begin)
          begin->~T();
        trt::deallocate_array(allocator, ptr, size, sizeof(T), alignof(T));
        throw;
      }
    }
#else
    for (T* addr = ptr; addr < ptr + size; ++addr)
      new (addr) T;
#endif

    trt::on_new_array(allocator, ptr, size);
    return ptr;
  }
  template <typename T, typename Alloc> requires (allocator_c<Alloc> && std::copy_constructible<T>)
  T* new_array(Alloc& allocator, u64 size, const T& value) {
    using trt = allocator_traits<Alloc>;
    T* ptr = static_cast<T*>(trt::allocate_array(allocator, size, sizeof(T), alignof(T)));
#if VK_exceptions_enabled
    if constexpr ( std::is_nothrow_copy_constructible_v<T> ) {
      for (T* addr = ptr; addr < ptr + size; ++addr)
        new (addr) T(value);
    }
    else {
      T* addr = ptr;
      T* const end = ptr + size;
      try {
        for (; addr < end; ++addr)
          new (addr) T(value);
      }
      catch (...) {
        for (T* begin = ptr; begin < addr; ++begin)
          begin->~T();
        trt::deallocate_array(allocator, ptr, size, sizeof(T), alignof(T));
        throw;
      }
    }
#else
    for (T* addr = ptr; addr < ptr + size; ++addr)
      new (addr) T(value);
#endif
    trt::on_new_array(allocator, ptr, size);
    return ptr;
  }
  template <typename T, typename Alloc> requires (allocator_c<Alloc> && std::copy_constructible<T>)
  T* new_array(Alloc& allocator, u64 size, std::initializer_list<T> list) {
    using trt = allocator_traits<Alloc>;
    T* ptr = static_cast<T*>(trt::allocate_array(allocator, size, sizeof(T), alignof(T)));

    T* addr = ptr;
    T* const end = ptr + size;

    if constexpr ( bitwise_movable_c<T> ) {
      trt::copy_bytes(allocator, addr, list.begin(), list.size() * sizeof(T));
      addr += list.size();

#if VK_exceptions_enabled
      if constexpr ( std::is_nothrow_default_constructible_v<T> ) {
        while ( addr < end )
          new ( addr++ ) T;
      }
      else {
        try {
          for (; addr < end; ++addr )
            new ( addr ) T;
        }
        catch (...) {
          for (T* begin = ptr; begin < addr; ++begin)
            begin->~T();
          trt::deallocate_array(allocator, ptr, size, sizeof(T), alignof(T));
          throw;
        }
      }
#else
      while ( addr < end )
        new ( addr++ ) T;
#endif

    }
#if VK_exceptions_enabled
    else if constexpr ( std::is_nothrow_copy_constructible_v<T> || std::is_nothrow_default_constructible_v<T> ) {
      if constexpr ( !std::is_nothrow_copy_constructible_v<T> ) {

        // copy constructor might throw, but default constructor won't.

        try {
          for ( const T& val : list ) {
            new ( addr ) T(val);
            ++addr;
          }
        }
        catch (...) {
          for (T* begin = ptr; begin < addr; ++begin)
            begin->~T();
          trt::deallocate_array(allocator, ptr, size, sizeof(T), alignof(T));
          throw;
        }

        while ( addr < end )
          new ( addr++ ) T;
      }
      else if constexpr ( !std::is_nothrow_default_constructible_v<T> ) {

        // copy constructor won't throw, but the default constructor might

        for ( const T& val : list )
          new ( addr++ ) T(val);
        try {
          for (; addr < end; ++addr )
            new ( addr ) T;
        }
        catch (...) {
          for (T* begin = ptr; begin < addr; ++begin)
            begin->~T();
          trt::deallocate_array(allocator, ptr, size, sizeof(T), alignof(T));
          throw;
        }
      }
      else {

        // neither the copy constructor nor the default constructor will throw,
        // allowing us to avoid the try catch block altogether :)

        for ( const T& val : list )
          new ( addr++ ) T(val);
        while ( addr < end )
          new ( addr++ ) T;
      }
    }
    else {

      // both the copy constructor and the default constructor might throw,
      // execute everything inside a try/catch block

      try {
        for ( const T& val : list ) {
          new ( addr ) T(val);
          ++addr;
        }
        for (; addr < end; ++addr )
          new ( addr ) T;
      }
      catch (...) {
        for (T* begin = ptr; begin < addr; ++begin)
          begin->~T();
        trt::deallocate_array(allocator, ptr, size, sizeof(T), alignof(T));
        throw;
      }
    }
#else
    else {
      for ( const T& val : list )
        new ( addr++ ) T(val);
      while ( addr < end )
        new ( addr++ ) T;
    }
#endif
    trt::on_new_array(allocator, ptr, size);
    return ptr;
  }




  template <typename T, typename Alloc, typename U> requires (allocator_c<Alloc> && pointer_castable_to<remove_cv_t<U>, T>)
  void delete_object(Alloc& allocator, U* ptr) noexcept {
    using trt = allocator_traits<Alloc>;
    if ( ptr ) {
      T* const real_ptr = const_cast<T*>(static_cast<const volatile T*>(static_cast<const volatile void*>(ptr)));
      trt::on_delete_object(allocator, real_ptr);
      if constexpr ( !std::is_trivially_destructible_v<T> )
        real_ptr->~T();
      trt::deallocate_node(allocator, real_ptr, sizeof(T), alignof(T));
    }
  }
  template <typename Alloc, typename T> requires (allocator_c<Alloc> && class_c<remove_cv_t<T>>)
  void delete_object(Alloc& allocator, T* ptr) noexcept {
    using trt = allocator_traits<Alloc>;
    using U   = remove_cv_t<T>;

    using kls = class_traits<U>;

    static_assert( !kls::is_polymorphic || std::has_virtual_destructor_v<U> );

    if ( ptr ) {
      U* const real_ptr = const_cast<U*>(ptr);
      trt::on_delete_object(allocator, real_ptr);
      if constexpr ( !std::is_trivially_destructible_v<U> )
        real_ptr->~U();
      trt::deallocate_node(
          allocator,
          real_ptr,
          kls::object_size(*real_ptr),
          kls::alignment );
    }

  }
  template <typename Alloc, typename T> requires (allocator_c<Alloc> && !std::is_polymorphic_v<remove_cv_t<T>> && !class_c<remove_cv_t<T>>)
  void delete_object(Alloc& allocator, T* ptr) noexcept {
    using trt = allocator_traits<Alloc>;
    using U   = remove_cv_t<T>;
    if ( ptr ) {
      U* real_ptr = const_cast<U*>(ptr);

      trt::on_delete_object(allocator, real_ptr);

      if constexpr ( !std::is_trivially_destructible_v<T> )
        real_ptr->~U();

      trt::deallocate_node(allocator, real_ptr, sizeof(U), alignof(U));
    }
  }


  template <typename T, typename Alloc, typename U> requires (allocator_c<Alloc> && pointer_castable_to<remove_cv_t<U>, T>)
  void delete_array(Alloc& allocator, U* ptr, u64 size) noexcept {
    using trt = allocator_traits<Alloc>;

    if ( ptr ) {
      T* const real_ptr = const_cast<T*>(static_cast<const volatile T*>(static_cast<const volatile void*>(ptr)));

      trt::on_delete_array(allocator, real_ptr, size);

      if constexpr ( !std::is_trivially_destructible_v<T> ) {
        for (T* obj = real_ptr; obj < real_ptr + size; ++obj)
          obj->~T();
      }
      trt::deallocate_array(allocator, real_ptr, size, sizeof(T), alignof(T));
    }


  }
  template <typename Alloc, typename T> requires (allocator_c<Alloc> && class_c<remove_cv_t<T>>)
  void delete_array(Alloc& allocator, T* ptr, u64 size) noexcept {
    using trt = allocator_traits<Alloc>;
    using U   = remove_cv_t<T>;

    using kls = class_traits<U>;

    static_assert( !kls::is_polymorphic || std::has_virtual_destructor_v<U> );

    if ( ptr ) {
      U* const real_ptr = const_cast<U*>(ptr);
      trt::on_delete_array(allocator, real_ptr, size);
      if constexpr ( !std::is_trivially_destructible_v<U> ) {
        for (U* addr = real_ptr; addr < real_ptr + size; ++addr )
          addr->~U();
      }

      trt::deallocate_array(
          allocator,
          real_ptr,
          size,
          kls::object_size(*real_ptr),
          kls::alignment );
    }

  }
  template <typename Alloc, typename T> requires (allocator_c<Alloc> && !std::is_polymorphic_v<remove_cv_t<T>> && !class_c<remove_cv_t<T>>)
  void delete_array(Alloc& allocator, T* ptr, u64 size) noexcept {
    using trt = allocator_traits<Alloc>;
    using U   = remove_cv_t<T>;
    if ( ptr ) {
      U* real_ptr = const_cast<U*>(ptr);

      trt::on_delete_array(allocator, real_ptr, size);

      if constexpr ( !std::is_trivially_destructible_v<T> ) {
        for (U* obj = real_ptr; obj < real_ptr + size; ++obj)
          obj->~U();
      }
      trt::deallocate_array(allocator, real_ptr, size, sizeof(U), alignof(U));
    }
  }

}

#endif  //VALKYRIE_MEMORY_NEW_DELETE_HPP
