//
// Created by Maxwell on 2020-11-22.
//

#ifndef VALKYRIE_SHARED_RESOURCE_HPP
#define VALKYRIE_SHARED_RESOURCE_HPP

#include <valkyrie/Core/Policies.hpp>
#include <valkyrie/Core/Error/Status.hpp>
#include <valkyrie/Core/Async/Semaphore.hpp>


namespace valkyrie::Core{




  enum class Capacity{
    Zero,
    One,
    Few,
    Many
  };

  /*namespace Detail{
    class SingleProducerManyConsumerMutex {
      Atomic<i32> ownerCount;
      Atomic<u32> readLockWaiters;
    public:
      SingleProducerManyConsumerMutex() noexcept : ownerCount(), readLockWaiters(){}
      SingleProducerManyConsumerMutex(const SingleProducerManyConsumerMutex &) = delete;

      void writeLock() noexcept {
        i32 storedValue = 0;
        if (!ownerCount.compare_exchange_weak(storedValue, -1, std::memory_order_acq_rel)) {
          ownerCount.wait(0, std::memory_order_acquire);
          ownerCount.store(-1, std::memory_order_release);
        }
      }
      void readLock() noexcept {
        i32 lastOwnerCount = ownerCount.load(std::memory_order_acquire);
        if (!ownerCount.compare_exchange_weak(lastOwnerCount, lastOwnerCount + 1)) {
          ++readLockWaiters;
          readLockWaiters.wait(0, std::memory_order_acq_rel);
        }
      }
      void writeUnlock() noexcept {
        u32 waitingReaders = readLockWaiters.exchange(0u, std::memory_order_acq_rel);
        ownerCount.store(waitingReaders, std::memory_order_release);
        readLockWaiters.notify_all();
      }
      void readUnlock() noexcept {
        --ownerCount;
        ownerCount.notify_one();
      }

      bool tryWriteLock() noexcept {
        i32 storedValue = 0;
        return ownerCount.compare_exchange_weak(storedValue, -1, std::memory_order_acq_rel);
      }
      bool tryReadLock() noexcept {
        i32 lastOwnerCount = ownerCount.load(std::memory_order_acquire);
        return ownerCount.compare_exchange_weak(lastOwnerCount, lastOwnerCount + 1);
      }
    };
    class ManyProducerManyConsumerMutex {

      Atomic<i32> ownerCount;
      Atomic<i32> writeLockWaiterCount;
      Atomic<i32> readLockWaiterCount;

    public:
      ManyProducerManyConsumerMutex() noexcept : ownerCount(0), writeLockWaiterCount(0), readLockWaiterCount(0){}
      ManyProducerManyConsumerMutex(const ManyProducerManyConsumerMutex&) = delete;

      void writeLock() noexcept {
        i32 storedValue = 0;
        ++writeLockWaiterCount;
        while (!ownerCount.compare_exchange_weak(storedValue, -1, std::memory_order_acq_rel)) {
          ownerCount.wait(0);
          storedValue = 0;
        }
        --writeLockWaiterCount;
      }
      void readLock() noexcept {
        if (ownerCount.load(std::memory_order_acquire) == -1) {
          ++readLockWaiterCount;
          writeLockWaiterCount.wait(0, std::memory_order_acquire);
        } else {
          ++ownerCount;
        }
      }
      void writeUnlock() noexcept {
        VK_assert(ownerCount.load() == -1);
        if (writeLockWaiterCount.load(std::memory_order_acquire)) {
          ownerCount.store(0, std::memory_order_release);
          ownerCount.notify_one();
        }
        else {
          i32 waitingReaders = readLockWaiterCount.exchange(0, std::memory_order_acq_rel);
          ownerCount.store(waitingReaders, std::memory_order_release);
          writeLockWaiterCount.notify_all();
        }
      }
      void readUnlock() noexcept {
        --ownerCount;
        ownerCount.notify_one();
      }


      bool tryWriteLock() noexcept {
        i32 storedValue = 0;
        return ownerCount.compare_exchange_strong(storedValue, -1, std::memory_order_acq_rel);
      }
      bool tryReadLock() noexcept {
        if (ownerCount.load(std::memory_order_acquire) != -1) {
          ++ownerCount;
          return true;
        }
        return false;
      }
    };
    class SingleProducerSingleConsumerMutex {

      enum State{
        NoOwner,
        WriteLocked,
        ReadLocked
      };

      BinarySemaphore isOwned{0};

    public:
      SingleProducerSingleConsumerMutex() noexcept = default;

      void writeLock() noexcept {
        State comparison = NoOwner;
        if (!state.compare_exchange_strong(comparison, WriteLocked)) {
          VK_assert(comparison == ReadLocked && "Illegal write lock reentry");
          state.wait(NoOwner);
        }
        isOwned.acquire();
      }
      void readLock() noexcept {
        isOwned.acquire();
      }
      void writeUnlock() noexcept {
        isOwned.release();
      }
      void readUnlock() noexcept {
        isOwned.release();
      }

      bool tryWriteLock() noexcept {
        return isOwned.try_acquire();
      }
      bool tryReadLock() noexcept {
        return isOwned.try_acquire();
      }
    };
  }*/

  template <Capacity Producers, Capacity Consumers>
  class Mutex;
  template <>
  class Mutex<Capacity::One, Capacity::Many>{
    Atomic<i32> ownerCount;
    Atomic<u32> readLockWaiters;
  public:
    Mutex() noexcept : ownerCount(), readLockWaiters(){}
    Mutex(const Mutex &) = delete;

    Mutex clone() const noexcept {
      return Mutex();
    }

    void writeLock() noexcept {
      i32 storedValue = 0;
      if (!ownerCount.compare_exchange_weak(storedValue, -1, std::memory_order_acq_rel)) {
        ownerCount.wait(0, std::memory_order_acquire);
        ownerCount.store(-1, std::memory_order_release);
      }
    }
    void readLock() noexcept {
      i32 lastOwnerCount = ownerCount.load(std::memory_order_acquire);
      if (!ownerCount.compare_exchange_weak(lastOwnerCount, lastOwnerCount + 1)) {
        ++readLockWaiters;
        readLockWaiters.wait(0, std::memory_order_acq_rel);
      }
    }
    void writeUnlock() noexcept {
      u32 waitingReaders = readLockWaiters.exchange(0u, std::memory_order_acq_rel);
      ownerCount.store(waitingReaders, std::memory_order_release);
      readLockWaiters.notify_all();
    }
    void readUnlock() noexcept {
      --ownerCount;
      ownerCount.notify_one();
    }

    bool tryWriteLock() noexcept {
      i32 storedValue = 0;
      return ownerCount.compare_exchange_weak(storedValue, -1, std::memory_order_acq_rel);
    }
    bool tryReadLock() noexcept {
      i32 lastOwnerCount = ownerCount.load(std::memory_order_acquire);
      return ownerCount.compare_exchange_weak(lastOwnerCount, lastOwnerCount + 1);
    }
  };
  template <>
  class Mutex<Capacity::Many, Capacity::Many>{
    Atomic<i32> ownerCount;
    Atomic<i32> writeLockWaiterCount;
    Atomic<i32> readLockWaiterCount;

  public:
    Mutex() noexcept : ownerCount(0), writeLockWaiterCount(0), readLockWaiterCount(0){}
    Mutex(const Mutex&) = delete;

    Mutex clone() const noexcept {
      return Mutex();
    }

    void writeLock() noexcept {
      i32 storedValue = 0;
      ++writeLockWaiterCount;
      while (!ownerCount.compare_exchange_weak(storedValue, -1, std::memory_order_acq_rel)) {
        ownerCount.wait(0);
        storedValue = 0;
      }
      --writeLockWaiterCount;
    }
    void readLock() noexcept {
      if (ownerCount.load(std::memory_order_acquire) == -1) {
        ++readLockWaiterCount;
        writeLockWaiterCount.wait(0, std::memory_order_acquire);
      } else {
        ++ownerCount;
      }
    }
    void writeUnlock() noexcept {
      VK_assert(ownerCount.load() == -1);
      if (writeLockWaiterCount.load(std::memory_order_acquire)) {
        ownerCount.store(0, std::memory_order_release);
        ownerCount.notify_one();
      }
      else {
        i32 waitingReaders = readLockWaiterCount.exchange(0, std::memory_order_acq_rel);
        ownerCount.store(waitingReaders, std::memory_order_release);
        writeLockWaiterCount.notify_all();
      }
    }
    void readUnlock() noexcept {
      --ownerCount;
      ownerCount.notify_one();
    }


    bool tryWriteLock() noexcept {
      i32 storedValue = 0;
      return ownerCount.compare_exchange_strong(storedValue, -1, std::memory_order_acq_rel);
    }
    bool tryReadLock() noexcept {
      if (ownerCount.load(std::memory_order_acquire) != -1) {
        ++ownerCount;
        return true;
      }
      return false;
    }
  };
  template <>
  class Mutex<Capacity::One, Capacity::One>{
    BinarySemaphore isOwned{0};

  public:
    Mutex() noexcept = default;
    Mutex(const Mutex&) = delete;

    Mutex clone() const noexcept {
      return {};
    }

    void writeLock() noexcept {
      /*State comparison = NoOwner;
      if (!state.compare_exchange_strong(comparison, WriteLocked)) {
        VK_assert(comparison == ReadLocked && "Illegal write lock reentry");
        state.wait(NoOwner);
      }*/
      isOwned.acquire();
    }
    void readLock() noexcept {
      isOwned.acquire();
    }
    void writeUnlock() noexcept {
      isOwned.release();
    }
    void readUnlock() noexcept {
      isOwned.release();
    }

    bool tryWriteLock() noexcept {
      return isOwned.try_acquire();
    }
    bool tryReadLock() noexcept {
      return isOwned.try_acquire();
    }
  };

  namespace Detail{
    using mutex_t = void*;
    using const_mutex_t = const void*;
    struct DynamicInterface{
      using clone_fn_t = mutex_t(*)(const_mutex_t) noexcept;
      using lock_fn_t = void(*)(mutex_t) noexcept;
      using unlock_fn_t = lock_fn_t;
      using try_lock_fn_t = bool(*)(mutex_t) noexcept;

      clone_fn_t    clone;
      lock_fn_t     lock;
      unlock_fn_t   unlock;
      try_lock_fn_t tryLock;
    };
    template <typename M> requires(requires(M& mutex/*, Allocator& alloc*/){
      { mutex.clone() } -> RawPointer<M>;
  /*  { mutex.clone(alloc) } -> Pointer<M>;  */
      { mutex.readLock() } noexcept -> ExactSameAs<void>;
      { mutex.readUnlock() } noexcept -> ExactSameAs<void>;
      { mutex.tryReadLock() } noexcept -> ExactSameAs<bool>;
    })
    inline constexpr static DynamicInterface readLockInterface{
      .clone    = [](const_mutex_t mutex) noexcept { return (void*)static_cast<M*>(mutex)->clone(); },
      .lock     = [](mutex_t mutex) noexcept { static_cast<M*>(mutex)->readLock(); },
      .unlock   = [](mutex_t mutex) noexcept { static_cast<M*>(mutex)->readUnlock(); },
      .tryLock  = [](mutex_t mutex) noexcept -> bool { return static_cast<M*>(mutex)->tryReadLock(); }
    };
    template <typename M> requires(requires(M& mutex/*, Allocator& alloc*/){
      { mutex.clone() } -> RawPointer<M>;
      /*  { mutex.clone(alloc) } -> Pointer<M>;  */
      { mutex.writeLock() } noexcept -> ExactSameAs<void>;
      { mutex.writeUnlock() } -> ExactSameAs<void>;
      { mutex.tryWriteLock() } -> ExactSameAs<bool>;
    })
    inline constexpr static DynamicInterface writeLockInterface{
      .clone    = [](const_mutex_t mutex) noexcept { return (void*)static_cast<M*>(mutex)->clone(); },
      .lock     = [](mutex_t mutex) noexcept { static_cast<M*>(mutex)->writeLock(); },
      .unlock   = [](mutex_t mutex) noexcept { static_cast<M*>(mutex)->writeUnlock(); },
      .tryLock  = [](mutex_t mutex) noexcept -> bool { return static_cast<M*>(mutex)->tryWriteLock(); }
    };
  }

  class Lock;

  template <typename T, typename Mut>
  class IntrusiveMutex{
    mutable Mut mutex;
  protected:
    template <typename ...Args> requires(ConstructibleFrom<Mut, Args...>)
    explicit IntrusiveMutex(Args&& ...args) noexcept : mutex{std::forward<Args>(args)...}{}
  public:
    Lock lock() noexcept;
    Lock lock() const noexcept;
    Lock writeLock() noexcept;
    Lock readLock() const noexcept;
  };




  class Lock{
    template <typename T, typename Mut>
    friend class IntrusiveMutex;


    const Detail::DynamicInterface* const pInterface;
    Detail::mutex_t mutex;

    template <typename M>
    Lock(M& mut, const Detail::DynamicInterface& interface) noexcept
        : pInterface(&interface), mutex(std::addressof(mut)){}

  public:
    Lock(const Lock&) = delete;
    Lock(Lock&& other) noexcept
        : pInterface(other.pInterface),
          mutex(other.mutex){
      other.mutex = nullptr;
    }
    ~Lock() {
      if (mutex)
        pInterface->unlock(mutex);
    }

    void release() noexcept {
      if (mutex) {
        pInterface->unlock(mutex);
        mutex = nullptr;
      }
    }
  };

  template <typename T, typename Mut>
  Lock IntrusiveMutex<T, Mut>::lock() noexcept {
    return Lock(mutex, Detail::writeLockInterface<Mut>);
  }
  template <typename T, typename Mut>
  Lock IntrusiveMutex<T, Mut>::lock() const noexcept {
    return Lock(mutex, Detail::readLockInterface<Mut>);
  }
  template <typename T, typename Mut>
  Lock IntrusiveMutex<T, Mut>::writeLock() noexcept {
    return Lock(mutex, Detail::writeLockInterface<Mut>);
  }
  template <typename T, typename Mut>
  Lock IntrusiveMutex<T, Mut>::readLock() const noexcept {
    return Lock(mutex, Detail::readLockInterface<Mut>);
  }





  template <typename T, Capacity Producers, Capacity Consumers>
  class shared_handle{};
  template <typename T, Capacity Producers, Capacity Consumers>
  class shared_ptr{};

  template <typename T, Capacity Producers, Capacity Consumers>
  class locked_handle{

  };
  template <typename T, Capacity Producers, Capacity Consumers>
  class locked_ptr{

  };
}

#endif//VALKYRIE_SHARED_RESOURCE_HPP
