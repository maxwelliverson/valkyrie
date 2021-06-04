//
// Created by maxwe on 2021-05-21.
//

#ifndef ARGENT_INTERNAL_ATOMIC_H
#define ARGENT_INTERNAL_ATOMIC_H

#include "internal.h"
#include "timing.h"


#include <intrin.h>
#include <string.h>




typedef struct AGT_alignas(16) {
  agt_u64_t low;
  agt_u64_t high;
} agt_u128_t;

typedef volatile bool      atomic_flag_t;

typedef volatile agt_i8_t  atomic_i8_t;
typedef volatile agt_u8_t  atomic_u8_t;
typedef volatile agt_i16_t atomic_i16_t;
typedef volatile agt_u16_t atomic_u16_t;


#if AGT_system_windows
typedef volatile long          atomic_i32_t;
typedef volatile unsigned long atomic_u32_t;
#else
typedef volatile int          atomic_i32_t;
typedef volatile unsigned int atomic_u32_t;
#endif

typedef volatile agt_i64_t  atomic_i64_t;
typedef volatile agt_u64_t  atomic_u64_t;
typedef volatile agt_u128_t atomic_u128_t;


#define assert_timed_out() do {} while(false)



#define atomic_cas(dst, comp, write, order) \
  _Generic(dst,                             \
           atomic_u8_t*: atomic_cas_##order##_u8, \
           atomic_i8_t*: atomic_cas_##order##_i8, \
           atomic_u16_t*: atomic_cas_##order##_u16, \
           atomic_i16_t*: atomic_cas_##order##_i16, \
           atomic_u32_t*: atomic_cas_##order##_u32, \
           atomic_i32_t*: atomic_cas_##order##_i32, \
           atomic_u64_t*: atomic_cas_##order##_u64, \
           atomic_i64_t*: atomic_cas_##order##_i64, \
           atomic_u128_t*: atomic_cas_##order##_u128)(dst, comp, write)

#define atomic_exchange(dst, write, order) \
  _Generic(dst,                            \
           atomic_u8_t*: atomic_exchange_##order##_u8, \
           atomic_i8_t*: atomic_exchange_##order##_i8, \
           atomic_u16_t*: atomic_exchange_##order##_u16, \
           atomic_i16_t*: atomic_exchange_##order##_i16, \
           atomic_u32_t*: atomic_exchange_##order##_u32, \
           atomic_i32_t*: atomic_exchange_##order##_i32, \
           atomic_u64_t*: atomic_exchange_##order##_u64, \
           atomic_i64_t*: atomic_exchange_##order##_i64, \
           atomic_u128_t*: atomic_exchange_##order##_u128)(dst, write)

#define atomic_load(dst, order) \
  _Generic(dst,                 \
           atomic_u8_t*: atomic_load_##order##_u8, \
           atomic_i8_t*: atomic_load_##order##_i8, \
           atomic_u16_t*: atomic_load_##order##_u16, \
           atomic_i16_t*: atomic_load_##order##_i16, \
           atomic_u32_t*: atomic_load_##order##_u32, \
           atomic_i32_t*: atomic_load_##order##_i32, \
           atomic_u64_t*: atomic_load_##order##_u64, \
           atomic_i64_t*: atomic_load_##order##_i64, \
           atomic_u128_t*: atomic_load_##order##_u128, \
           const atomic_u8_t*: atomic_load_##order##_u8, \
           const atomic_i8_t*: atomic_load_##order##_i8, \
           const atomic_u16_t*: atomic_load_##order##_u16, \
           const atomic_i16_t*: atomic_load_##order##_i16, \
           const atomic_u32_t*: atomic_load_##order##_u32, \
           const atomic_i32_t*: atomic_load_##order##_i32, \
           const atomic_u64_t*: atomic_load_##order##_u64, \
           const atomic_i64_t*: atomic_load_##order##_i64, \
           const atomic_u128_t*: atomic_load_##order##_u128)(dst)

#define atomic_store(dst, write, order) \
  _Generic(dst,                         \
           atomic_u8_t*: atomic_store_##order##_u8, \
           atomic_i8_t*: atomic_store_##order##_i8, \
           atomic_u16_t*: atomic_store_##order##_u16, \
           atomic_i16_t*: atomic_store_##order##_i16, \
           atomic_u32_t*: atomic_store_##order##_u32, \
           atomic_i32_t*: atomic_store_##order##_i32, \
           atomic_u64_t*: atomic_store_##order##_u64, \
           atomic_i64_t*: atomic_store_##order##_i64, \
           atomic_u128_t*: atomic_store_##order##_u128)(dst, write)

#define atomic_add_exchange(dst, val, order) \
  _Generic(dst,                              \
           atomic_u8_t*:  atomic_add_exchange_##order##_u8, \
           atomic_u16_t*: atomic_add_exchange_##order##_u16, \
           atomic_u32_t*: atomic_add_exchange_##order##_u32, \
           atomic_u64_t*: atomic_add_exchange_##order##_u64)(dst, val)


#define atomic_increment(dst, order) \
  _Generic(dst,                      \
           atomic_u8_t*: atomic_increment_##order##_u8, \
           atomic_i8_t*: atomic_increment_##order##_i8, \
           atomic_u16_t*: atomic_increment_##order##_u16, \
           atomic_i16_t*: atomic_increment_##order##_i16, \
           atomic_u32_t*: atomic_increment_##order##_u32, \
           atomic_i32_t*: atomic_increment_##order##_i32, \
           atomic_u64_t*: atomic_increment_##order##_u64, \
           atomic_i64_t*: atomic_increment_##order##_i64)(dst)

#define atomic_decrement(dst, order) \
  _Generic(dst,                      \
           atomic_u8_t*: atomic_decrement_##order##_u8, \
           atomic_i8_t*: atomic_decrement_##order##_i8, \
           atomic_u16_t*: atomic_decrement_##order##_u16, \
           atomic_i16_t*: atomic_decrement_##order##_i16, \
           atomic_u32_t*: atomic_decrement_##order##_u32, \
           atomic_i32_t*: atomic_decrement_##order##_i32, \
           atomic_u64_t*: atomic_decrement_##order##_u64, \
           atomic_i64_t*: atomic_decrement_##order##_i64)(dst)

#define atomic_wait(dest, value) \
  _Generic(dest,                 \
           atomic_u8_t*: atomic_wait_u8, \
           atomic_i8_t*: atomic_wait_i8, \
           atomic_u16_t*: atomic_wait_u16, \
           atomic_i16_t*: atomic_wait_i16, \
           atomic_u32_t*: atomic_wait_u32, \
           atomic_i32_t*: atomic_wait_i32, \
           atomic_u64_t*: atomic_wait_u64, \
           atomic_i64_t*: atomic_wait_i64, \
           const atomic_u8_t*: atomic_wait_u8, \
           const atomic_i8_t*: atomic_wait_i8, \
           const atomic_u16_t*: atomic_wait_u16, \
           const atomic_i16_t*: atomic_wait_i16, \
           const atomic_u32_t*: atomic_wait_u32, \
           const atomic_i32_t*: atomic_wait_i32, \
           const atomic_u64_t*: atomic_wait_u64, \
           const atomic_i64_t*: atomic_wait_i64)(dest, value)

#define atomic_wait_update(dest, value) \
  _Generic(dest,                 \
           atomic_u8_t*: atomic_wait_update_u8, \
           atomic_i8_t*: atomic_wait_update_i8, \
           atomic_u16_t*: atomic_wait_update_u16, \
           atomic_i16_t*: atomic_wait_update_i16, \
           atomic_u32_t*: atomic_wait_update_u32, \
           atomic_i32_t*: atomic_wait_update_i32, \
           atomic_u64_t*: atomic_wait_update_u64, \
           atomic_i64_t*: atomic_wait_update_i64, \
           const atomic_u8_t*: atomic_wait_update_u8, \
           const atomic_i8_t*: atomic_wait_update_i8, \
           const atomic_u16_t*: atomic_wait_update_u16, \
           const atomic_i16_t*: atomic_wait_update_i16, \
           const atomic_u32_t*: atomic_wait_update_u32, \
           const atomic_i32_t*: atomic_wait_update_i32, \
           const atomic_u64_t*: atomic_wait_update_u64, \
           const atomic_i64_t*: atomic_wait_update_i64)(dest, value)


#define atomic_wait_until(dest, value, deadline) \
  _Generic(dest,                                 \
           atomic_u8_t*: atomic_wait_until_u8,   \
           atomic_i8_t*: atomic_wait_until_i8,   \
           atomic_u16_t*: atomic_wait_until_u16, \
           atomic_i16_t*: atomic_wait_until_i16, \
           atomic_u32_t*: atomic_wait_until_u32, \
           atomic_i32_t*: atomic_wait_until_i32, \
           atomic_u64_t*: atomic_wait_until_u64, \
           atomic_i64_t*: atomic_wait_until_i64, \
           const atomic_u8_t*: atomic_wait_until_u8,   \
           const atomic_i8_t*: atomic_wait_until_i8,   \
           const atomic_u16_t*: atomic_wait_until_u16, \
           const atomic_i16_t*: atomic_wait_until_i16, \
           const atomic_u32_t*: atomic_wait_until_u32, \
           const atomic_i32_t*: atomic_wait_until_i32, \
           const atomic_u64_t*: atomic_wait_until_u64, \
           const atomic_i64_t*: atomic_wait_until_i64)(dest, value, deadline)

#define atomic_wake_one(dest) WakeByAddressSingle((void*)dest)
#define atomic_wake_all(dest) WakeByAddressAll((void*)dest)


#define atomic_fence() _ReadWriteBarrier()

















// atomic_cas => (relaxed, acquire, release)

AGT_forceinline static bool atomic_cas_relaxed_u8(atomic_u8_t* destination, agt_u8_t* comp, const agt_u8_t write) {
  const agt_u32_t result = _InterlockedCompareExchange8((volatile char*)destination, write, *comp);
  if ( result != *comp ) {
    memcpy(comp, &result, sizeof(result));
    return false;
  }
  return true;
}
AGT_forceinline static bool atomic_cas_relaxed_i8(atomic_i8_t* destination, agt_i8_t* comp, const agt_i8_t write) {
  const agt_i32_t result = _InterlockedCompareExchange8((volatile char*)destination, write, *comp);
  if ( result != *comp ) {
    memcpy(comp, &result, sizeof(result));
    return false;
  }
  return true;
}
AGT_forceinline static bool atomic_cas_relaxed_u16(atomic_u16_t* destination, agt_u16_t* comp, const agt_u16_t write) {
  const agt_u32_t result = _InterlockedCompareExchange16_np((atomic_i16_t*)destination, write, *comp);
  if ( result != *comp ) {
    memcpy(comp, &result, sizeof(result));
    return false;
  }
  return true;
}
AGT_forceinline static bool atomic_cas_relaxed_i16(atomic_i16_t* destination, agt_i16_t* comp, const agt_i16_t write) {
  const agt_i32_t result = _InterlockedCompareExchange16_np(destination, write, *comp);
  if ( result != *comp ) {
    memcpy(comp, &result, sizeof(result));
    return false;
  }
  return true;
}
AGT_forceinline static bool atomic_cas_relaxed_u32(atomic_u32_t* destination, agt_u32_t* comp, const agt_u32_t write) {
  const agt_u32_t result = _InterlockedCompareExchange_np((atomic_i32_t*)destination, write, *comp);
  if ( result != *comp ) {
    memcpy(comp, &result, sizeof(result));
    return false;
  }
  return true;
}
AGT_forceinline static bool atomic_cas_relaxed_i32(atomic_i32_t* destination, agt_i32_t* comp, const agt_i32_t write) {
  const agt_i32_t result = _InterlockedCompareExchange_np(destination, write, *comp);
  if ( result != *comp ) {
    memcpy(comp, &result, sizeof(result));
    return false;
  }
  return true;
}
AGT_forceinline static bool atomic_cas_relaxed_u64(atomic_u64_t* destination, agt_u64_t* comp, const agt_u64_t write) {
  const agt_u64_t result = _InterlockedCompareExchange64_np((atomic_i64_t*)destination, write, *comp);
  if ( result != *comp ) {
    memcpy(comp, &result, sizeof(agt_u64_t));
    return false;
  }
  return true;
}
AGT_forceinline static bool atomic_cas_relaxed_i64(atomic_i64_t* destination, agt_i64_t* comp, const agt_i64_t write) {
  const agt_i64_t result = _InterlockedCompareExchange64_np(destination, write, *comp);
  if ( result != *comp ) {
    memcpy(comp, &result, sizeof(result));
    return false;
  }
  return true;
}
AGT_forceinline static bool atomic_cas_relaxed_u128(atomic_u128_t* destination, agt_u128_t* comp, const agt_u128_t write) {
  return _InterlockedCompareExchange128_np((atomic_i64_t*)destination->low, write.high, write.low, (agt_i64_t*)comp);
}

AGT_forceinline static bool atomic_cas_acquire_u8(atomic_u8_t* destination, agt_u8_t* comp, const agt_u8_t write) {
  const agt_u32_t result = _InterlockedCompareExchange8((volatile char*)destination, write, *comp);
  if ( result != *comp ) {
    memcpy(comp, &result, sizeof(result));
    return false;
  }
  return true;
}
AGT_forceinline static bool atomic_cas_acquire_i8(atomic_i8_t* destination, agt_i8_t* comp, const agt_i8_t write) {
  const agt_i32_t result = _InterlockedCompareExchange8((volatile char*)destination, write, *comp);
  if ( result != *comp ) {
    memcpy(comp, &result, sizeof(result));
    return false;
  }
  return true;
}
AGT_forceinline static bool atomic_cas_acquire_u16(atomic_u16_t* destination, agt_u16_t* comp, const agt_u16_t write) {
  const agt_u32_t result = _InterlockedCompareExchange16_np((atomic_i16_t*)destination, write, *comp);
  if ( result != *comp ) {
    memcpy(comp, &result, sizeof(result));
    return false;
  }
  return true;
}
AGT_forceinline static bool atomic_cas_acquire_i16(atomic_i16_t* destination, agt_i16_t* comp, const agt_i16_t write) {
  const agt_i32_t result = _InterlockedCompareExchange16_np(destination, write, *comp);
  if ( result != *comp ) {
    memcpy(comp, &result, sizeof(result));
    return false;
  }
  return true;
}
AGT_forceinline static bool atomic_cas_acquire_u32(atomic_u32_t* destination, agt_u32_t* comp, const agt_u32_t write) {
  const agt_u32_t result = _InterlockedCompareExchange_HLEAcquire((atomic_i32_t*)destination, write, *comp);
  if ( result != *comp ) {
    memcpy(comp, &result, sizeof(result));
    return false;
  }
  return true;
}
AGT_forceinline static bool atomic_cas_acquire_i32(atomic_i32_t* destination, agt_i32_t* comp, const agt_i32_t write) {
  const agt_i32_t result = _InterlockedCompareExchange_HLEAcquire(destination, write, *comp);
  if ( result != *comp ) {
    memcpy(comp, &result, sizeof(result));
    return false;
  }
  return true;
}
AGT_forceinline static bool atomic_cas_acquire_u64(atomic_u64_t* destination, agt_u64_t* comp, const agt_u64_t write) {
  const agt_u64_t result = _InterlockedCompareExchange64_HLEAcquire((atomic_i64_t*)destination, write, *comp);
  if ( result != *comp ) {
    memcpy(comp, &result, sizeof(agt_u64_t));
    return false;
  }
  return true;
}
AGT_forceinline static bool atomic_cas_acquire_i64(atomic_i64_t* destination, agt_i64_t* comp, const agt_i64_t write) {
  const agt_i64_t result = _InterlockedCompareExchange64_HLEAcquire(destination, write, *comp);
  if ( result != *comp ) {
    memcpy(comp, &result, sizeof(result));
    return false;
  }
  return true;
}
AGT_forceinline static bool atomic_cas_acquire_u128(atomic_u128_t* destination, agt_u128_t* comp, const agt_u128_t write) {
  return _InterlockedCompareExchange128_np((atomic_i64_t*)destination->low, write.high, write.low, (agt_i64_t*)comp);
}

AGT_forceinline static bool atomic_cas_release_u8(atomic_u8_t* destination, agt_u8_t* comp, const agt_u8_t write) {
  const agt_u32_t result = _InterlockedCompareExchange8((volatile char*)destination, write, *comp);
  if ( result != *comp ) {
    memcpy(comp, &result, sizeof(result));
    return false;
  }
  return true;
}
AGT_forceinline static bool atomic_cas_release_i8(atomic_i8_t* destination, agt_i8_t* comp, const agt_i8_t write) {
  const agt_i32_t result = _InterlockedCompareExchange8((volatile char*)destination, write, *comp);
  if ( result != *comp ) {
    memcpy(comp, &result, sizeof(result));
    return false;
  }
  return true;
}
AGT_forceinline static bool atomic_cas_release_u16(atomic_u16_t* destination, agt_u16_t* comp, const agt_u16_t write) {
  const agt_u32_t result = _InterlockedCompareExchange16_np((atomic_i16_t*)destination, write, *comp);
  if ( result != *comp ) {
    memcpy(comp, &result, sizeof(result));
    return false;
  }
  return true;
}
AGT_forceinline static bool atomic_cas_release_i16(atomic_i16_t* destination, agt_i16_t* comp, const agt_i16_t write) {
  const agt_i32_t result = _InterlockedCompareExchange16_np(destination, write, *comp);
  if ( result != *comp ) {
    memcpy(comp, &result, sizeof(result));
    return false;
  }
  return true;
}
AGT_forceinline static bool atomic_cas_release_u32(atomic_u32_t* destination, agt_u32_t* comp, const agt_u32_t write) {
  const agt_u32_t result = _InterlockedCompareExchange_HLERelease((atomic_i32_t*)destination, write, *comp);
  if ( result != *comp ) {
    memcpy(comp, &result, sizeof(result));
    return false;
  }
  return true;
}
AGT_forceinline static bool atomic_cas_release_i32(atomic_i32_t* destination, agt_i32_t* comp, const agt_i32_t write) {
  const agt_i32_t result = _InterlockedCompareExchange_HLERelease(destination, write, *comp);
  if ( result != *comp ) {
    memcpy(comp, &result, sizeof(result));
    return false;
  }
  return true;
}
AGT_forceinline static bool atomic_cas_release_u64(atomic_u64_t* destination, agt_u64_t* comp, const agt_u64_t write) {
  const agt_u64_t result = _InterlockedCompareExchange64_HLERelease((atomic_i64_t*)destination, write, *comp);
  if ( result != *comp ) {
    memcpy(comp, &result, sizeof(agt_u64_t));
    return false;
  }
  return true;
}
AGT_forceinline static bool atomic_cas_release_i64(atomic_i64_t* destination, agt_i64_t* comp, const agt_i64_t write) {
  const agt_i64_t result = _InterlockedCompareExchange64_HLERelease(destination, write, *comp);
  if ( result != *comp ) {
    memcpy(comp, &result, sizeof(result));
    return false;
  }
  return true;
}
AGT_forceinline static bool atomic_cas_release_u128(atomic_u128_t* destination, agt_u128_t* comp, const agt_u128_t write) {
  return _InterlockedCompareExchange128_np((atomic_i64_t*)destination->low, write.high, write.low, (agt_i64_t*)comp);
}



// atomic_exchange => (relaxed, acquire, release)

AGT_forceinline static agt_u8_t   atomic_exchange_relaxed_u8(atomic_u8_t* destination, const agt_u8_t write) {
  return _InterlockedExchange8((volatile char*)destination, write);
}
AGT_forceinline static agt_i8_t   atomic_exchange_relaxed_i8(atomic_i8_t* destination, const agt_i8_t write) {
  return _InterlockedExchange8((volatile char*)destination, write);
}
AGT_forceinline static agt_u16_t  atomic_exchange_relaxed_u16(atomic_u16_t* destination, const agt_u16_t write) {
  return _InterlockedExchange16((atomic_i16_t*)destination, write);
}
AGT_forceinline static agt_i16_t  atomic_exchange_relaxed_i16(atomic_i16_t* destination, const agt_i16_t write) {
  return _InterlockedExchange16(destination, write);
}
AGT_forceinline static agt_u32_t  atomic_exchange_relaxed_u32(atomic_u32_t* destination, const agt_u32_t write) {
  return _InterlockedExchange((atomic_i32_t*)destination, write);
}
AGT_forceinline static agt_i32_t  atomic_exchange_relaxed_i32(atomic_i32_t* destination, const agt_i32_t write) {
  return _InterlockedExchange(destination, write);
}
AGT_forceinline static agt_u64_t  atomic_exchange_relaxed_u64(atomic_u64_t* destination, const agt_u64_t write) {
  return _InterlockedExchange64((atomic_i64_t*)destination, (const agt_i64_t)write);
}
AGT_forceinline static agt_i64_t  atomic_exchange_relaxed_i64(atomic_i64_t* destination, const agt_i64_t write) {
  return _InterlockedExchange64(destination, write);
}
AGT_forceinline static agt_u128_t atomic_exchange_relaxed_u128(atomic_u128_t* destination, const agt_u128_t write) {
  agt_u128_t comp = { .low = 0, .high = 0 };
  while ( !atomic_cas(destination, &comp, write, relaxed) );
  return comp;
}

AGT_forceinline static agt_u8_t   atomic_exchange_acquire_u8(atomic_u8_t* destination, const agt_u8_t write) {
  _ReadWriteBarrier();
  return _InterlockedExchange8((volatile char*)destination, write);
}
AGT_forceinline static agt_i8_t   atomic_exchange_acquire_i8(atomic_i8_t* destination, const agt_i8_t write) {
  _ReadWriteBarrier();
  return _InterlockedExchange8((volatile char*)destination, write);
}
AGT_forceinline static agt_u16_t  atomic_exchange_acquire_u16(atomic_u16_t* destination, const agt_u16_t write) {
  _ReadWriteBarrier();
  return _InterlockedExchange16((atomic_i16_t*)destination, write);
}
AGT_forceinline static agt_i16_t  atomic_exchange_acquire_i16(atomic_i16_t* destination, const agt_i16_t write) {
  _ReadWriteBarrier();
  return _InterlockedExchange16(destination, write);
}
AGT_forceinline static agt_u32_t  atomic_exchange_acquire_u32(atomic_u32_t* destination, const agt_u32_t write) {
  _ReadWriteBarrier();
  return _InterlockedExchange_HLEAcquire((atomic_i32_t*)destination, write);
}
AGT_forceinline static agt_i32_t  atomic_exchange_acquire_i32(atomic_i32_t* destination, const agt_i32_t write) {
  _ReadWriteBarrier();
  return _InterlockedExchange_HLEAcquire(destination, write);
}
AGT_forceinline static agt_u64_t  atomic_exchange_acquire_u64(atomic_u64_t* destination, const agt_u64_t write) {
  _ReadWriteBarrier();
  return _InterlockedExchange64_HLEAcquire((atomic_i64_t*)destination, (const agt_i64_t)write);
}
AGT_forceinline static agt_i64_t  atomic_exchange_acquire_i64(atomic_i64_t* destination, const agt_i64_t write) {
  _ReadWriteBarrier();
  return _InterlockedExchange64_HLEAcquire(destination, write);
}
AGT_forceinline static agt_u128_t atomic_exchange_acquire_u128(atomic_u128_t* destination, const agt_u128_t write) {
  agt_u128_t comp = { .low = 0, .high = 0 };
  while ( !atomic_cas(destination, &comp, write, acquire) );
  return comp;
}

AGT_forceinline static agt_u8_t   atomic_exchange_release_u8(atomic_u8_t* destination, const agt_u8_t write) {
  const agt_u8_t value = _InterlockedExchange8((volatile char*)destination, write);
  _ReadWriteBarrier();
  return value;
}
AGT_forceinline static agt_i8_t   atomic_exchange_release_i8(atomic_i8_t* destination, const agt_i8_t write) {
  const agt_i8_t value = _InterlockedExchange8((volatile char*)destination, write);
  _ReadWriteBarrier();
  return value;
}
AGT_forceinline static agt_u16_t  atomic_exchange_release_u16(atomic_u16_t* destination, const agt_u16_t write) {
  const agt_u16_t value = _InterlockedExchange16((atomic_i16_t*)destination, write);
  _ReadWriteBarrier();
  return value;
}
AGT_forceinline static agt_i16_t  atomic_exchange_release_i16(atomic_i16_t* destination, const agt_i16_t write) {
  const agt_i16_t value = _InterlockedExchange16(destination, write);
  _ReadWriteBarrier();
  return value;
}
AGT_forceinline static agt_u32_t  atomic_exchange_release_u32(atomic_u32_t* destination, const agt_u32_t write) {
  const agt_u32_t value = _InterlockedExchange_HLERelease((atomic_i32_t*)destination, write);
  _ReadWriteBarrier();
  return value;
}
AGT_forceinline static agt_i32_t  atomic_exchange_release_i32(atomic_i32_t* destination, const agt_i32_t write) {
  const agt_i32_t value = _InterlockedExchange_HLERelease(destination, write);
  _ReadWriteBarrier();
  return value;
}
AGT_forceinline static agt_u64_t  atomic_exchange_release_u64(atomic_u64_t* destination, const agt_u64_t write) {
  const agt_u64_t value = _InterlockedExchange64_HLERelease((atomic_i64_t*)destination, (const agt_i64_t)write);
  _ReadWriteBarrier();
  return value;
}
AGT_forceinline static agt_i64_t  atomic_exchange_release_i64(atomic_i64_t* destination, const agt_i64_t write) {
  const agt_i64_t value = _InterlockedExchange64_HLERelease(destination, write);
  _ReadWriteBarrier();
  return value;
}
AGT_forceinline static agt_u128_t atomic_exchange_release_u128(atomic_u128_t* destination, const agt_u128_t write) {
  agt_u128_t comp = { .low = 0, .high = 0 };
  while ( !atomic_cas(destination, &comp, write, release) );
  return comp;
}




// atomic_load => (relaxed, acquire)

AGT_forceinline static agt_u32_t  atomic_load_relaxed_u8(const atomic_u8_t* from) {
  return __iso_volatile_load8((const volatile char*)from);
}
AGT_forceinline static agt_i32_t  atomic_load_relaxed_i8(const atomic_i8_t* from) {
  return __iso_volatile_load8((const volatile char*)from);
}
AGT_forceinline static agt_u32_t  atomic_load_relaxed_u16(const atomic_u16_t* from) {
  return __iso_volatile_load16((const atomic_i16_t*)from);
}
AGT_forceinline static agt_i32_t  atomic_load_relaxed_i16(const atomic_i16_t* from) {
  return __iso_volatile_load16(from);
}
AGT_forceinline static agt_u32_t  atomic_load_relaxed_u32(const atomic_u32_t* from) {
  return __iso_volatile_load32((const volatile int*)from);
}
AGT_forceinline static agt_i32_t  atomic_load_relaxed_i32(const atomic_i32_t* from) {
  return __iso_volatile_load32((const volatile int*)from);
}
AGT_forceinline static agt_u64_t  atomic_load_relaxed_u64(const atomic_u64_t* from) {
  return __iso_volatile_load64((const atomic_i64_t*)from);
}
AGT_forceinline static agt_i64_t  atomic_load_relaxed_i64(const atomic_i64_t* from) {
  return __iso_volatile_load64(from);
}
AGT_forceinline static agt_u128_t atomic_load_relaxed_u128(const atomic_u128_t* from) {
  agt_u128_t result = (agt_u128_t){ 0, 0 };
  atomic_cas((atomic_u128_t*)from, &result, result, relaxed);
  return result;
}

AGT_forceinline static agt_u32_t  atomic_load_acquire_u8(const atomic_u8_t* from) {
  _ReadBarrier();
  return __iso_volatile_load8((const volatile char*)from);
}
AGT_forceinline static agt_i32_t  atomic_load_acquire_i8(const atomic_i8_t* from) {
  _ReadBarrier();
  return __iso_volatile_load8((const volatile char*)from);
}
AGT_forceinline static agt_u32_t  atomic_load_acquire_u16(const atomic_u16_t* from) {
  _ReadBarrier();
  return __iso_volatile_load16((const atomic_i16_t*)from);
}
AGT_forceinline static agt_i32_t  atomic_load_acquire_i16(const atomic_i16_t* from) {
  _ReadBarrier();
  return __iso_volatile_load16(from);
}
AGT_forceinline static agt_u32_t  atomic_load_acquire_u32(const atomic_u32_t* from) {
  _ReadBarrier();
  return __iso_volatile_load32((const volatile int*)from);
}
AGT_forceinline static agt_i32_t  atomic_load_acquire_i32(const atomic_i32_t* from) {
  _ReadBarrier();
  return __iso_volatile_load32((const volatile int*)from);
}
AGT_forceinline static agt_u64_t  atomic_load_acquire_u64(const atomic_u64_t* from) {
  _ReadBarrier();
  return __iso_volatile_load64((const atomic_i64_t*)from);
}
AGT_forceinline static agt_i64_t  atomic_load_acquire_i64(const atomic_i64_t* from) {
  _ReadBarrier();
  return __iso_volatile_load64(from);
}
AGT_forceinline static agt_u128_t atomic_load_acquire_u128(const atomic_u128_t* from) {
  agt_u128_t result = (agt_u128_t){ 0, 0 };
  atomic_cas((atomic_u128_t*)from, &result, result, acquire);
  return result;
}



// atomic_store => (relaxed, release)

AGT_forceinline static void       atomic_store_relaxed_u8(atomic_u8_t* to, const agt_u8_t value) {
  __iso_volatile_store8((volatile char*)to, value);
}
AGT_forceinline static void       atomic_store_relaxed_i8(atomic_i8_t* to, const agt_i8_t value) {
  __iso_volatile_store8((volatile char*)to, value);
}
AGT_forceinline static void       atomic_store_relaxed_u16(atomic_u16_t* to, const agt_u16_t value) {
  __iso_volatile_store16((atomic_i16_t*)to, value);
}
AGT_forceinline static void       atomic_store_relaxed_i16(atomic_i16_t* to, const agt_i16_t value) {
  __iso_volatile_store16(to, value);
}
AGT_forceinline static void       atomic_store_relaxed_u32(atomic_u32_t* to, const agt_u32_t value) {
  __iso_volatile_store32((volatile __int32*)to, (const __int32)value);
}
AGT_forceinline static void       atomic_store_relaxed_i32(atomic_i32_t* to, const agt_i32_t value) {
  __iso_volatile_store32((volatile __int32*)to, value);
}
AGT_forceinline static void       atomic_store_relaxed_u64(atomic_u64_t* to, const agt_u64_t value) {
  __iso_volatile_store64((atomic_i64_t*)to, value);
}
AGT_forceinline static void       atomic_store_relaxed_i64(atomic_i64_t* to, const agt_i64_t value) {
  __iso_volatile_store64(to, value);
}
AGT_forceinline static void       atomic_store_relaxed_u128(atomic_u128_t* to, const agt_u128_t value) {
  agt_u128_t result = (agt_u128_t){ 0, 0 };
  while ( !atomic_cas(to, &result, value, relaxed) );
}


AGT_forceinline static void       atomic_store_release_u8(atomic_u8_t* to, const agt_u8_t value) {
  __iso_volatile_store8((volatile char*)to, value);
  _ReadWriteBarrier();
}
AGT_forceinline static void       atomic_store_release_i8(atomic_i8_t* to, const agt_i8_t value) {
  __iso_volatile_store8((volatile char*)to, value);
  _ReadWriteBarrier();
}
AGT_forceinline static void       atomic_store_release_u16(atomic_u16_t* to, const agt_u16_t value) {
  __iso_volatile_store16((atomic_i16_t*)to, value);
  _ReadWriteBarrier();
}
AGT_forceinline static void       atomic_store_release_i16(atomic_i16_t* to, const agt_i16_t value) {
  __iso_volatile_store16(to, value);
  _ReadWriteBarrier();
}
AGT_forceinline static void       atomic_store_release_u32(atomic_u32_t* to, const agt_u32_t value) {
  __iso_volatile_store32((volatile __int32*)to, (const __int32)value);
  _ReadWriteBarrier();
}
AGT_forceinline static void       atomic_store_release_i32(atomic_i32_t* to, const agt_i32_t value) {
  __iso_volatile_store32((volatile __int32*)to, (const __int32)value);
  _ReadWriteBarrier();
}
AGT_forceinline static void       atomic_store_release_u64(atomic_u64_t* to, const agt_u64_t value) {
  __iso_volatile_store64((volatile __int64*)to, (const __int64)value);
  _ReadWriteBarrier();
}
AGT_forceinline static void       atomic_store_release_i64(atomic_i64_t* to, const agt_i64_t value) {
  __iso_volatile_store64(to, value);
  _ReadWriteBarrier();
}
AGT_forceinline static void       atomic_store_release_u128(atomic_u128_t* to, const agt_u128_t value) {
  agt_u128_t result = (agt_u128_t){ 0, 0 };
  while ( !atomic_cas(to, &result, value, release) );
}



// atomic_increment => (relaxed, acquire, release)

AGT_forceinline static agt_u8_t   atomic_increment_relaxed_u8(atomic_u8_t* destination) {
  return _InterlockedExchangeAdd8((volatile char*)destination, 1) + 1;
}
AGT_forceinline static agt_i8_t   atomic_increment_relaxed_i8(atomic_i8_t* destination) {
  return _InterlockedExchangeAdd8((volatile char*)destination, 1) + 1;
}
AGT_forceinline static agt_u16_t  atomic_increment_relaxed_u16(atomic_u16_t* destination) {
  return _InterlockedIncrement16((atomic_i16_t*)destination);
}
AGT_forceinline static agt_i16_t  atomic_increment_relaxed_i16(atomic_i16_t* destination) {
  return _InterlockedIncrement16(destination);
}
AGT_forceinline static agt_u32_t  atomic_increment_relaxed_u32(atomic_u32_t* destination) {
  return _InterlockedIncrement((atomic_i32_t*)destination);
}
AGT_forceinline static agt_i32_t  atomic_increment_relaxed_i32(atomic_i32_t* destination) {
  return _InterlockedIncrement(destination);
}
AGT_forceinline static agt_u64_t  atomic_increment_relaxed_u64(atomic_u64_t* destination) {
  return _InterlockedIncrement64((atomic_i64_t*)destination);
}
AGT_forceinline static agt_i64_t  atomic_increment_relaxed_i64(atomic_i64_t* destination) {
  return _InterlockedIncrement64(destination);
}

AGT_forceinline static agt_u8_t   atomic_increment_acquire_u8(atomic_u8_t* destination) {
  _ReadWriteBarrier();
  return _InterlockedExchangeAdd8((volatile char*)destination, 1) + 1;
}
AGT_forceinline static agt_i8_t   atomic_increment_acquire_i8(atomic_i8_t* destination) {
  _ReadWriteBarrier();
  return _InterlockedExchangeAdd8((volatile char*)destination, 1) + 1;
}
AGT_forceinline static agt_u16_t  atomic_increment_acquire_u16(atomic_u16_t* destination) {
  _ReadWriteBarrier();
  return _InterlockedIncrement16((atomic_i16_t*)destination);
}
AGT_forceinline static agt_i16_t  atomic_increment_acquire_i16(atomic_i16_t* destination) {
  _ReadWriteBarrier();
  return _InterlockedIncrement16(destination);
}
AGT_forceinline static agt_u32_t  atomic_increment_acquire_u32(atomic_u32_t* destination) {
  _ReadWriteBarrier();
  return _InterlockedIncrement((atomic_i32_t*)destination);
}
AGT_forceinline static agt_i32_t  atomic_increment_acquire_i32(atomic_i32_t* destination) {
  _ReadWriteBarrier();
  return _InterlockedIncrement(destination);
}
AGT_forceinline static agt_u64_t  atomic_increment_acquire_u64(atomic_u64_t* destination) {
  _ReadWriteBarrier();
  return _InterlockedIncrement64((atomic_i64_t*)destination);
}
AGT_forceinline static agt_i64_t  atomic_increment_acquire_i64(atomic_i64_t* destination) {
  _ReadWriteBarrier();
  return _InterlockedIncrement64(destination);
}

AGT_forceinline static agt_u8_t   atomic_increment_release_u8(atomic_u8_t* destination) {
  const agt_u8_t value = _InterlockedExchangeAdd8((volatile char*)destination, 1) + 1;
  _ReadWriteBarrier();
  return value;
}
AGT_forceinline static agt_i8_t   atomic_increment_release_i8(atomic_i8_t* destination) {
  const agt_i8_t value = _InterlockedExchangeAdd8((volatile char*)destination, 1) + 1;
  _ReadWriteBarrier();
  return value;
}
AGT_forceinline static agt_u16_t  atomic_increment_release_u16(atomic_u16_t* destination) {
  const agt_u16_t value = _InterlockedIncrement16((atomic_i16_t*)destination);
  _ReadWriteBarrier();
  return value;
}
AGT_forceinline static agt_i16_t  atomic_increment_release_i16(atomic_i16_t* destination) {
  const agt_i16_t value = _InterlockedIncrement16(destination);
  _ReadWriteBarrier();
  return value;
}
AGT_forceinline static agt_u32_t  atomic_increment_release_u32(atomic_u32_t* destination) {
  const agt_u32_t result = _InterlockedIncrement((atomic_i32_t*)destination);
  _ReadWriteBarrier();
  return result;
}
AGT_forceinline static agt_i32_t  atomic_increment_release_i32(atomic_i32_t* destination) {
  const agt_i32_t result = _InterlockedIncrement(destination);
  _ReadWriteBarrier();
  return result;
}
AGT_forceinline static agt_u64_t  atomic_increment_release_u64(atomic_u64_t* destination) {
  const agt_u64_t result = _InterlockedIncrement64((atomic_i64_t*)destination);
  _ReadWriteBarrier();
  return result;
}
AGT_forceinline static agt_i64_t  atomic_increment_release_i64(atomic_i64_t* destination) {
  const agt_i64_t result = _InterlockedIncrement64(destination);
  _ReadWriteBarrier();
  return result;
}




// atomic_add_exchange


AGT_forceinline static agt_u8_t  atomic_add_exchange_seq_u8(atomic_u8_t* destination, agt_u8_t value) {
  atomic_fence();
  const agt_u8_t result = _InterlockedExchangeAdd8((volatile char*)destination, value);
  atomic_fence();
  return result;
}
AGT_forceinline static agt_u16_t atomic_add_exchange_seq_u16(atomic_u16_t* destination, agt_u16_t value) {
  atomic_fence();
  const agt_u16_t result = _InterlockedExchangeAdd16((atomic_i16_t*)destination, value);
  atomic_fence();
  return result;
}
AGT_forceinline static agt_u32_t atomic_add_exchange_seq_u32(atomic_u32_t* destination, agt_u32_t value) {
  atomic_fence();
  const agt_u32_t result = _InterlockedExchangeAdd((atomic_i32_t*)destination, value);
  atomic_fence();
  return result;
}
AGT_forceinline static agt_u64_t atomic_add_exchange_seq_u64(atomic_u64_t* destination, agt_u64_t value) {
  atomic_fence();
  const agt_u64_t result = _InterlockedExchangeAdd64((atomic_i64_t*)destination, value);
  atomic_fence();
  return result;
}


// atomic_decrement => (relaxed, acquire, release)

AGT_forceinline static agt_u8_t   atomic_decrement_relaxed_u8(atomic_u8_t* destination) {
  return _InterlockedExchangeAdd8((volatile char*)destination, -1) - 1;
}
AGT_forceinline static agt_i8_t   atomic_decrement_relaxed_i8(atomic_i8_t* destination) {
  return _InterlockedExchangeAdd8((volatile char*)destination, -1) - 1;
}
AGT_forceinline static agt_u16_t  atomic_decrement_relaxed_u16(atomic_u16_t* destination) {
  return _InterlockedDecrement16((atomic_i16_t*)destination);
}
AGT_forceinline static agt_i16_t  atomic_decrement_relaxed_i16(atomic_i16_t* destination) {
  return _InterlockedDecrement16(destination);
}
AGT_forceinline static agt_u32_t  atomic_decrement_relaxed_u32(atomic_u32_t* destination) {
  return _InterlockedDecrement((atomic_i32_t*)destination);
}
AGT_forceinline static agt_i32_t  atomic_decrement_relaxed_i32(atomic_i32_t* destination) {
  return _InterlockedDecrement(destination);
}
AGT_forceinline static agt_u64_t  atomic_decrement_relaxed_u64(atomic_u64_t* destination) {
  return _InterlockedDecrement64((atomic_i64_t*)destination);
}
AGT_forceinline static agt_i64_t  atomic_decrement_relaxed_i64(atomic_i64_t* destination) {
  return _InterlockedDecrement64(destination);
}

AGT_forceinline static agt_u8_t   atomic_decrement_acquire_u8(atomic_u8_t* destination) {
  _ReadWriteBarrier();
  return _InterlockedExchangeAdd8((volatile char*)destination, -1) - 1;
}
AGT_forceinline static agt_i8_t   atomic_decrement_acquire_i8(atomic_i8_t* destination) {
  _ReadWriteBarrier();
  return _InterlockedExchangeAdd8((volatile char*)destination, -1) - 1;
}
AGT_forceinline static agt_u16_t  atomic_decrement_acquire_u16(atomic_u16_t* destination) {
  _ReadWriteBarrier();
  return _InterlockedDecrement16((atomic_i16_t*)destination);
}
AGT_forceinline static agt_i16_t  atomic_decrement_acquire_i16(atomic_i16_t* destination) {
  _ReadWriteBarrier();
  return _InterlockedDecrement16(destination);
}
AGT_forceinline static agt_u32_t  atomic_decrement_acquire_u32(atomic_u32_t* destination) {
  _ReadBarrier();
  return _InterlockedDecrement((atomic_i32_t*)destination);
}
AGT_forceinline static agt_i32_t  atomic_decrement_acquire_i32(atomic_i32_t* destination) {
  _ReadBarrier();
  return _InterlockedDecrement(destination);
}
AGT_forceinline static agt_u64_t  atomic_decrement_acquire_u64(atomic_u64_t* destination) {
  _ReadBarrier();
  return _InterlockedDecrement64((atomic_i64_t*)destination);
}
AGT_forceinline static agt_i64_t  atomic_decrement_acquire_i64(atomic_i64_t* destination) {
  _ReadBarrier();
  return _InterlockedDecrement64(destination);
}

AGT_forceinline static agt_u8_t   atomic_decrement_release_u8(atomic_u8_t* destination) {
  const agt_u8_t value = _InterlockedExchangeAdd8((volatile char*)destination, -1) - 1;
  _ReadWriteBarrier();
  return value;
}
AGT_forceinline static agt_i8_t   atomic_decrement_release_i8(atomic_i8_t* destination) {
  const agt_i8_t value = _InterlockedExchangeAdd8((volatile char*)destination, -1) - 1;
  _ReadWriteBarrier();
  return value;
}
AGT_forceinline static agt_u16_t  atomic_decrement_release_u16(atomic_u16_t* destination) {
  const agt_u16_t value = _InterlockedDecrement16((atomic_i16_t*)destination);
  _ReadWriteBarrier();
  return value;
}
AGT_forceinline static agt_i16_t  atomic_decrement_release_i16(atomic_i16_t* destination) {
  const agt_i16_t value = _InterlockedDecrement16(destination);
  _ReadWriteBarrier();
  return value;
}
AGT_forceinline static agt_u32_t  atomic_decrement_release_u32(atomic_u32_t* destination) {
  const agt_u32_t result = _InterlockedDecrement((atomic_i32_t*)destination);
  _WriteBarrier();
  return result;
}
AGT_forceinline static agt_i32_t  atomic_decrement_release_i32(atomic_i32_t* destination) {
  const agt_i32_t result = _InterlockedDecrement(destination);
  _WriteBarrier();
  return result;
}
AGT_forceinline static agt_u64_t  atomic_decrement_release_u64(atomic_u64_t* destination) {
  const agt_u64_t result = _InterlockedDecrement64((atomic_i64_t*)destination);
  _WriteBarrier();
  return result;
}
AGT_forceinline static agt_i64_t  atomic_decrement_release_i64(atomic_i64_t* destination) {
  const agt_i64_t result = _InterlockedDecrement64(destination);
  _WriteBarrier();
  return result;
}



// atomic_wait
#define define_atomic_wait_impl(type) \
  AGT_forceinline static void atomic_wait_##type(const atomic_##type##_t* dest, agt_##type##_t value) { \
    if ( atomic_load(dest, acquire) == value ) { \
      if ( !WaitOnAddress((volatile void*)dest, (void*)&value, sizeof(*dest), INFINITE) ) \
        assert_timed_out();\
    } \
  }

define_atomic_wait_impl(i8)
define_atomic_wait_impl(u8)
define_atomic_wait_impl(i16)
define_atomic_wait_impl(u16)
define_atomic_wait_impl(i32)
define_atomic_wait_impl(u32)
define_atomic_wait_impl(i64)
define_atomic_wait_impl(u64)

#undef define_atomic_wait_impl

// UPDATES BEFORE THE WAIT. Meant to be used in loops.
#define define_atomic_wait_update_impl(type) \
  AGT_forceinline static void atomic_wait_update_##type(const atomic_##type##_t* dest, agt_##type##_t* value) { \
    agt_##type##_t tmp = *value;                                         \
    if ((*value = atomic_load(dest, relaxed)) == tmp ) { \
      WaitOnAddress((volatile void*)dest, (void*)&value, sizeof(*dest), INFINITE);\
      *value = atomic_load(dest, relaxed); \
    } \
  }

define_atomic_wait_update_impl(i8)
define_atomic_wait_update_impl(u8)
define_atomic_wait_update_impl(i16)
define_atomic_wait_update_impl(u16)
define_atomic_wait_update_impl(i32)
define_atomic_wait_update_impl(u32)
define_atomic_wait_update_impl(i64)
define_atomic_wait_update_impl(u64)



#undef define_atomic_wait_update_impl


// atomic_wait_until

AGT_forceinline static bool atomic_wait_until_u8(const atomic_u8_t* dest, agt_u8_t value,    agt_deadline_t deadline) {
  if ( WaitOnAddress((volatile void*)dest, &value, sizeof(value), deadline_max_timeout_ms(deadline)) )
    return true;

  assert_timed_out();

  if ( !deadline )
    return false;

  while ( !is_past_deadline(deadline) ) {
    if ( atomic_load(dest, relaxed) != value )
      return true;
    yield_processor();
  }

  return false;
}
AGT_forceinline static bool atomic_wait_until_i8(const atomic_i8_t* dest, agt_i8_t value,    agt_deadline_t deadline) {
  if ( WaitOnAddress((volatile void*)dest, &value, sizeof(value), deadline_max_timeout_ms(deadline)) )
    return true;

  assert_timed_out();

  if ( !deadline )
    return false;

  while ( !is_past_deadline(deadline) ) {
    if ( atomic_load(dest, relaxed) != value )
      return true;
    yield_processor();
  }

  return false;
}
AGT_forceinline static bool atomic_wait_until_u16(const atomic_u16_t* dest, agt_u16_t value, agt_deadline_t deadline) {
  if ( WaitOnAddress((volatile void*)dest, &value, sizeof(value), deadline_max_timeout_ms(deadline)) )
    return true;

  assert_timed_out();

  if ( !deadline )
    return false;

  while ( !is_past_deadline(deadline) ) {
    if ( atomic_load(dest, relaxed) != value )
      return true;
    yield_processor();
  }

  return false;
}
AGT_forceinline static bool atomic_wait_until_i16(const atomic_i16_t* dest, agt_i16_t value, agt_deadline_t deadline) {
  if ( WaitOnAddress((volatile void*)dest, &value, sizeof(value), deadline_max_timeout_ms(deadline)) )
    return true;

  assert_timed_out();

  if ( !deadline )
    return false;

  while ( !is_past_deadline(deadline) ) {
    if ( atomic_load(dest, relaxed) != value )
      return true;
    yield_processor();
  }

  return false;
}
AGT_forceinline static bool atomic_wait_until_u32(const atomic_u32_t* dest, agt_u32_t value, agt_deadline_t deadline) {
  if ( WaitOnAddress((volatile void*)dest, &value, sizeof(value), deadline_max_timeout_ms(deadline)) )
    return true;

  assert_timed_out();

  if ( !deadline )
    return false;

  while ( !is_past_deadline(deadline) ) {
    if ( atomic_load(dest, relaxed) != value )
      return true;
    yield_processor();
  }

  return false;
}
AGT_forceinline static bool atomic_wait_until_i32(const atomic_i32_t* dest, agt_i32_t value, agt_deadline_t deadline) {
  if ( WaitOnAddress((volatile void*)dest, &value, sizeof(value), deadline_max_timeout_ms(deadline)) )
    return true;

  assert_timed_out();

  if ( !deadline )
    return false;

  while ( !is_past_deadline(deadline) ) {
    if ( atomic_load(dest, relaxed) != value )
      return true;
    yield_processor();
  }

  return false;
}
AGT_forceinline static bool atomic_wait_until_u64(const atomic_u64_t* dest, agt_u64_t value, agt_deadline_t deadline) {
  if ( WaitOnAddress((volatile void*)dest, &value, sizeof(value), deadline_max_timeout_ms(deadline)) )
    return true;

  assert_timed_out();

  if ( !deadline )
    return false;

  while ( !is_past_deadline(deadline) ) {
    if ( atomic_load(dest, relaxed) != value )
      return true;
    yield_processor();
  }

  return false;
}
AGT_forceinline static bool atomic_wait_until_i64(const atomic_i64_t* dest, agt_i64_t value, agt_deadline_t deadline) {
  if ( WaitOnAddress((volatile void*)dest, &value, sizeof(value), deadline_max_timeout_ms(deadline)) )
    return true;

  assert_timed_out();

  if ( !deadline )
    return false;

  while ( !is_past_deadline(deadline) ) {
    if ( atomic_load(dest, relaxed) != value )
      return true;
    yield_processor();
  }

  return false;
}







AGT_forceinline static bool atomic_flag_test(const atomic_flag_t* flag) {
  return _InterlockedCompareExchange8((volatile char*)flag, true, true);
}
AGT_forceinline static void atomic_flag_set(atomic_flag_t* flag) {
  atomic_store((atomic_u8_t*)flag, true, release);
}
AGT_forceinline static void atomic_flag_reset(atomic_flag_t* flag) {
  atomic_store((atomic_u8_t*)flag, false, release);
}
AGT_forceinline static bool atomic_flag_test_and_set(atomic_flag_t* flag) {
  atomic_fence();
  return atomic_exchange((atomic_u8_t*)flag, true, release);
}
AGT_forceinline static bool atomic_flag_test_and_reset(atomic_flag_t* flag) {
  atomic_fence();
  return atomic_exchange((atomic_u8_t*)flag, false, release);
}
AGT_forceinline static bool atomic_flag_wait(const atomic_flag_t* flag) {
  atomic_wait((const atomic_u8_t*)flag, false);
}


#endif//ARGENT_INTERNAL_ATOMIC_H
