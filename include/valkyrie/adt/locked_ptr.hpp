//
// Created by maxwe on 2021-05-05.
//

#ifndef VALKYRIE_LOCKED_PTR_HPP
#define VALKYRIE_LOCKED_PTR_HPP

#include <valkyrie/async/mutex.hpp>

#include "ptr.hpp"

namespace valkyrie{
  template <typename T, mutex_c Mtx = shared_mutex>
  class locked_ptr : public ptr_facade<locked_ptr<T, Mtx>, T*, T&, not_ordered, void>{

    using base = ptr_facade<locked_ptr<T, Mtx>, T*, T&, not_ordered, void>;

    VK_constant bool is_read_only = std::is_const_v<T>;
    VK_constant auto access_tag = []{
      if constexpr (is_read_only)
        return read_access;
      else
        return write_access;
    }();

  public:

    using mutex_type = Mtx;
    using lock_type = decltype(lock(std::declval<mutex_type&>(), access_tag));

    locked_ptr() = delete;
    locked_ptr(const locked_ptr&) = delete;

    locked_ptr(locked_ptr&& other) noexcept
        : lock(std::move(other.lock)), ptr(std::exchange(other.ptr, nullptr)){}

    locked_ptr(T* ptr, mutex_type& mutex) noexcept
        : lock(mutex, access_tag), ptr(ptr){ }
    locked_ptr(T* ptr, mutex_type& mutex, impl::non_blocking_t) noexcept
        : lock(mutex, access_tag, do_not_block), ptr(lock ? ptr : nullptr){ }
    locked_ptr(T* ptr, mutex_type& mutex, timeout timeout) noexcept
        : lock(mutex, access_tag, timeout), ptr(lock ? ptr : nullptr){ }
    locked_ptr(T* ptr, mutex_type& mutex, deadline deadline) noexcept
        : lock(mutex, access_tag, deadline), ptr(lock ? ptr : nullptr){ }

    locked_ptr& operator=(const locked_ptr&) = delete;
    locked_ptr& operator=(locked_ptr&& other) noexcept {
      other.lock = std::move(other.lock);
      ptr = std::exchange(other.ptr, nullptr);
      return *this;
    }

    void assign(T* pointer, mutex_type& mutex) noexcept {
      lock = { mutex, access_tag };
      ptr  = pointer;
    }
    bool assign(T* pointer, mutex_type& mutex, impl::non_blocking_t) noexcept {
      lock = { mutex, access_tag, do_not_block };
      if (lock)
        ptr  = pointer;
    }
    bool assign(T* pointer, mutex_type& mutex, timeout timeout) noexcept {
      lock = { mutex, access_tag, timeout };
      if (lock)
        ptr  = pointer;
    }
    bool assign(T* pointer, mutex_type& mutex, deadline deadline) noexcept {
      lock = { mutex, access_tag, deadline };
      if (lock)
        ptr  = pointer;
    }


    T* get() const noexcept {
      return ptr;
    }
    T* release() noexcept {
      lock.release();
      return std::exchange(ptr, nullptr);
    }
    void reset() noexcept {
      lock.release();
      ptr = nullptr;
    }


  private:

    lock_type lock;
    T*        ptr;
  };

  template <typename T, mutex_c Mtx, typename ...Args>
  locked_ptr(T*, Mtx&, Args&&...) -> locked_ptr<T, Mtx>;
}

#endif  //VALKYRIE_LOCKED_PTR_HPP
