//
// Created by Maxwell on 2020-11-22.
//

#ifndef VALKYRIE_ASYNC_ATOMIC_HPP
#define VALKYRIE_ASYNC_ATOMIC_HPP

#include <valkyrie/traits.hpp>

#include <atomic>


namespace valkyrie{
  namespace detail{
    /*template <typename T>
    struct GetAtomicType{
      using type = std::atomic_ref<T>;
    };
    template <typename T> requires(requires{ typename std::atomic<T>; { sizeof(std::atomic<T>) }; })
    struct GetAtomicType<T>{
      using type = std::atomic<T>;
    };*/

    template <typename T>
    struct get_atomic_type{
      using type = std::atomic<T>;
    };
    template <typename T>
    struct get_atomic_type<T&>{
      using type = std::atomic_ref<T>;
    };
  }

  template <typename T>
  using atomic = typename detail::get_atomic_type<T>::type;


#if defined(__cpp_using_enum) && __cpp_using_enum < 201907L
  using std::memory_order;
#define VK_using_enum using enum
#else // workaround lack of "using enum"
  namespace memory_order{
    VK_constant auto acquire = std::memory_order_acquire;
    VK_constant auto release = std::memory_order_release;
    VK_constant auto relaxed = std::memory_order_relaxed;
    VK_constant auto acq_rel = std::memory_order_acq_rel;
    VK_constant auto consume = std::memory_order_consume;
    VK_constant auto seq_cst = std::memory_order_seq_cst;
  }
#define VK_using_enum using namespace
#endif






  template <std::memory_order Order = std::memory_order_acq_rel, typename T>
  inline static bool atomic_cas(std::atomic<T>& location, T& expected, param_t<identity_type_t<T>> value) noexcept {
    return location.compare_exchange_strong(expected, value, Order);
  }
  template <std::memory_order Order = std::memory_order_release, typename T>
  inline static T    atomic_exchange(std::atomic<T>& location, param_t<identity_type_t<T>> value) noexcept {
    return location.exchange(value, Order);
  }
  template <std::memory_order Order = std::memory_order_release, typename T>
  inline static void atomic_store(std::atomic<T>& location, param_t<identity_type_t<T>> value) noexcept {
    location.store(value, Order);
  }
  template <std::memory_order Order = std::memory_order_acquire, typename T>
  inline static T    atomic_load(const std::atomic<T>& location) noexcept {
    return location.load(Order);
  }

  template <std::memory_order Order = std::memory_order_acq_rel, typename T> requires numeric_type<T>
  inline static T    atomic_fetch_add(std::atomic<T>& location, param_t<identity_type_t<T>> value) noexcept {
    return location.fetch_add(value, Order);
  }
  template <std::memory_order Order = std::memory_order_acq_rel, typename T> requires numeric_type<T>
  inline static T    atomic_fetch_sub(std::atomic<T>& location, param_t<identity_type_t<T>> value) noexcept {
    return location.fetch_sub(value, Order);
  }

  template <std::memory_order Order = std::memory_order_acq_rel, typename T> requires integral_type<T>
  inline static T    atomic_fetch_and(std::atomic<T>& location, param_t<identity_type_t<T>> value) noexcept {
    return location.fetch_and(value, Order);
  }
  template <std::memory_order Order = std::memory_order_acq_rel, typename T> requires integral_type<T>
  inline static T    atomic_fetch_or(std::atomic<T>& location, param_t<identity_type_t<T>> value) noexcept {
    return location.fetch_or(value, Order);
  }
  template <std::memory_order Order = std::memory_order_acq_rel, typename T> requires integral_type<T>
  inline static T    atomic_fetch_xor(std::atomic<T>& location, param_t<identity_type_t<T>> value) noexcept {
    return location.fetch_xor(value, Order);
  }

  template <std::memory_order Order = std::memory_order_acquire, typename T>
  inline static void atomic_wait(const std::atomic<T>& location, param_t<identity_type_t<T>> value) noexcept {
    atomic<T&>(location).wait(value, Order);
  }
  template <typename T>
  inline static void atomic_notify_one(std::atomic<T>& location) noexcept {
    atomic<T&>(location).notify_one();
  }
  template <typename T>
  inline static void atomic_notify_all(std::atomic<T>& location) noexcept {
    atomic<T&>(location).notify_all();
  }




  template <std::memory_order Order = std::memory_order_acq_rel, typename T>
  inline static bool atomic_cas(T& location, T& expected, param_t<identity_type_t<T>> value) noexcept {
    return atomic<T&>(location)
        .compare_exchange_strong(expected, value, Order);
  }
  template <std::memory_order Order = std::memory_order_release, typename T>
  inline static T    atomic_exchange(T& location, param_t<identity_type_t<T>> value) noexcept {
    return atomic<T&>(location).exchange(value, Order);
  }
  template <std::memory_order Order = std::memory_order_release, typename T>
  inline static void atomic_store(T& location, param_t<identity_type_t<T>> value) noexcept {
    atomic<T&>(location).store(value, Order);
  }
  template <std::memory_order Order = std::memory_order_acquire, typename T>
  inline static T    atomic_load(const T& location) noexcept {
    return atomic<T&>(const_cast<T&>(location)).load(Order);
  }

  template <std::memory_order Order = std::memory_order_acq_rel, typename T> requires numeric_type<T>
  inline static T    atomic_fetch_add(T& location, param_t<identity_type_t<T>> value) noexcept {
    return atomic<T&>(location).fetch_add(value, Order);
  }
  template <std::memory_order Order = std::memory_order_acq_rel, typename T> requires numeric_type<T>
  inline static T    atomic_fetch_sub(T& location, param_t<identity_type_t<T>> value) noexcept {
    return atomic<T&>(location).fetch_sub(value, Order);
  }
  template <std::memory_order Order = std::memory_order_acq_rel, typename T> requires integral_type<T>
  inline static T    atomic_fetch_and(T& location, param_t<identity_type_t<T>> value) noexcept {
    return atomic<T&>(location).fetch_and(value, Order);
  }
  template <std::memory_order Order = std::memory_order_acq_rel, typename T> requires integral_type<T>
  inline static T    atomic_fetch_or(T& location, param_t<identity_type_t<T>> value) noexcept {
    return atomic<T&>(location).fetch_or(value, Order);
  }
  template <std::memory_order Order = std::memory_order_acq_rel, typename T> requires integral_type<T>
  inline static T    atomic_fetch_xor(T& location, param_t<identity_type_t<T>> value) noexcept {
    return atomic<T&>(location).fetch_xor(value, Order);
  }

  template <std::memory_order Order = std::memory_order_acquire, typename T>
  inline static void atomic_wait(const T& location, param_t<identity_type_t<T>> value) noexcept {
    atomic<T&>(const_cast<T&>(location)).wait(value, Order);
  }
  template <typename T>
  inline static void atomic_notify_one(T& location) noexcept {
    atomic<T&>(location).notify_one();
  }
  template <typename T>
  inline static void atomic_notify_all(T& location) noexcept {
    atomic<T&>(location).notify_all();
  }
}

#endif//VALKYRIE_ASYNC_ATOMIC_HPP
