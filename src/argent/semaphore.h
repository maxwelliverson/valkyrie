//
// Created by maxwe on 2021-05-24.
//

#ifndef ARGENT_INTERNAL_SEMAPHORE_H
#define ARGENT_INTERNAL_SEMAPHORE_H

#include "atomic.h"

typedef struct agt_semaphore {
  atomic_u16_t counter;
  atomic_u16_t waitingThreads;
} *agt_semaphore_t;
typedef struct agt_binary_semaphore {
  atomic_u8_t counter;
} *agt_binary_semaphore_t;


#define acquire_semaphore(sem) \
  _Generic(sem,                \
           agt_semaphore_t: acquire_counting_semaphore, \
           agt_binary_semaphore_t: acquire_binary_semaphore)(sem)

#define try_acquire_semaphore(sem) \
  _Generic(sem,                \
           agt_semaphore_t: try_acquire_counting_semaphore, \
           agt_binary_semaphore_t: try_acquire_binary_semaphore)(sem)

#define try_acquire_semaphore_for(sem, timeout) \
  _Generic(sem,                \
           agt_semaphore_t: try_acquire_counting_semaphore_for, \
           agt_binary_semaphore_t: try_acquire_binary_semaphore_for)(sem, timeout)

#define try_acquire_semaphore_until(sem, deadline) \
  _Generic(sem,                \
           agt_semaphore_t: try_acquire_counting_semaphore_until, \
           agt_binary_semaphore_t: try_acquire_binary_semaphore_until)(sem, deadline)

#define release_semaphore(sem) \
  _Generic(sem,                \
           agt_semaphore_t:        release_counting_semaphore, \
           agt_binary_semaphore_t: release_binary_semaphore)(sem)

#define release_many_semaphores(sem, count) \
  _Generic(sem,                \
           agt_semaphore_t:        release_many_counting_semaphores, \
           agt_binary_semaphore_t: release_many_binary_semaphores)(sem, count)


AGT_forceinline static void acquire_counting_semaphore(agt_semaphore_t sem) {
  agt_u16_t current = atomic_load(&sem->counter, acquire);
  for ( ;; ) {

    while ( current == 0 ) {
      atomic_increment(&sem->waitingThreads, acquire);
      atomic_wait(&sem->counter, current);
      atomic_decrement(&sem->waitingThreads, relaxed);
      current = atomic_load(&sem->counter, relaxed);
    }

    if ( atomic_cas(&sem->counter, &current, current - 1, release) )
      return;
  }
}
AGT_forceinline static bool try_acquire_counting_semaphore(agt_semaphore_t sem) {
  agt_u16_t current = atomic_load(&sem->counter, acquire);
  if ( current == 0 )
    return false;
  atomic_fence();
  return atomic_cas(&sem->counter, &current, current - 1, release);
}
AGT_forceinline static bool try_acquire_counting_semaphore_until(agt_semaphore_t sem, agt_deadline_t deadline) {
  agt_u16_t current = atomic_load(&sem->counter, acquire);
  for ( ;; ) {

    if ( current == 0 ) {
      if ( is_past_deadline(deadline) )
        return false;
      do {
        atomic_increment(&sem->waitingThreads, acquire);
        if ( !atomic_wait_until(&sem->counter, current, deadline) ) {
          atomic_decrement(&sem->waitingThreads, relaxed);
          return false;
        }
        atomic_decrement(&sem->waitingThreads, relaxed);
        current = atomic_load(&sem->counter, relaxed);
      } while ( current == 0 );
    }

    if ( atomic_cas(&sem->counter, &current, current - 1, release) )
      return true;
  }
}
AGT_forceinline static bool try_acquire_counting_semaphore_for(agt_semaphore_t sem, agt_u64_t timeout) {
  return try_acquire_counting_semaphore_until(sem, agt_deadline_from_timeout(timeout));
}
AGT_forceinline static void release_counting_semaphore(agt_semaphore_t sem) {
  atomic_fence();
  (void)atomic_increment(&sem->counter, release);
  const agt_u16_t waiters = atomic_load(&sem->counter, acquire);

  if ( waiters > 0 )
    atomic_wake_one(&sem->counter);
}
AGT_forceinline static void release_many_counting_semaphores(agt_semaphore_t sem, agt_u16_t count) {

  if ( count == 0 )
    return;

  atomic_fence();
  (void)atomic_add_exchange(&sem->counter, count, seq);
  const agt_u16_t waiters = atomic_load(&sem->counter, acquire);

  if ( waiters == 0 ) {

  }
  else if ( waiters <= count ){
    atomic_wake_all(&sem->counter);
  }
  else {
    for (; count != 0; --count)
      atomic_wake_one(&sem->counter);
  }
}



AGT_forceinline static void acquire_binary_semaphore(agt_binary_semaphore_t sem) {
  for (;;) {
    agt_u8_t prev = atomic_exchange(&sem->counter, 0, acquire);

    if ( prev == 1 )
      break;
    assert(prev == 0);

    atomic_wait(&sem->counter, 0);
  }
}
AGT_forceinline static bool try_acquire_binary_semaphore(agt_binary_semaphore_t sem) {
  agt_u8_t prev = atomic_exchange(&sem->counter, 0, acquire);
  assert( (prev & ~1) == 0 && "value of a binary_semaphore must only ever be 0 or 1.");
  return prev;
}
AGT_forceinline static bool try_acquire_binary_semaphore_until(agt_binary_semaphore_t sem, agt_deadline_t deadline) {
  for (;;) {
    agt_u8_t prev = atomic_exchange(&sem->counter, 0, acquire);
    if ( prev == 1 )
      return true;

    if ( !atomic_wait_until(&sem->counter, 0, deadline) )
      return false;
  }
}
AGT_forceinline static bool try_acquire_binary_semaphore_for(agt_binary_semaphore_t sem, agt_u64_t timeout) {
  return try_acquire_binary_semaphore_until(sem, agt_deadline_from_timeout(timeout));
}
AGT_forceinline static void release_binary_semaphore(agt_binary_semaphore_t sem) {
  atomic_store(&sem->counter, 1, release);
  atomic_wake_one(&sem->counter);
}
AGT_forceinline static void release_many_binary_semaphores(agt_binary_semaphore_t sem, bool count) {

  if ( !count )
    return;

  release_binary_semaphore(sem);
}




#endif  //ARGENT_INTERNAL_SEMAPHORE_H
