//
// Created by Maxwell on 2020-11-18.
//

#ifndef VALKYRIE_ASYNC_SEMAPHORE_HPP
#define VALKYRIE_ASYNC_SEMAPHORE_HPP



#include <valkyrie/traits.hpp>
#include <valkyrie/utility/time.hpp>

#include "atomic.hpp"

#if __has_include(<semaphore>)
#include <semaphore>
#if defined(__cpp_lib_semaphore) && __cpp_lib_semaphore >= 201907L
#define VK_std_has_semaphore true
#else
#define VK_std_has_semaphore false
#endif
#else
#define VK_std_has_semaphore false
#include <chrono>
#endif


namespace valkyrie{


  inline namespace concepts{
    template <typename S>
    concept semaphore_c = requires(S& sem, timeout time, deadline until, i64 count){
      S::max();
      sem.acquire();
      sem.release();
      sem.release(count);
      { sem.try_acquire() } noexcept -> exact_same_as<bool>;
      { sem.try_acquire(time) } noexcept -> exact_same_as<bool>;
      { sem.try_acquire(until) } noexcept -> exact_same_as<bool>;
    };
  }


#if !VK_std_has_semaphore
  namespace detail{

    template <i64 Val>
    struct AlignNext2 : AlignNext2<Val & (Val - 1)>{};
    template <i64 Val> requires((Val & (Val - 1)) == 0)
    struct AlignNext2<Val>{
      inline constexpr static i64 value = Val << 1;
    };


    template <i64 Val>
    struct Align2 : AlignNext2<Val & (Val - 1)>{  };
    template <i64 Val> requires((Val & (Val - 1)) == 0)
    struct Align2<Val>{
      inline constexpr static i64 value = Val;
    };

    class semaphore{
      i64 counter;
    protected:
      constexpr explicit semaphore(i64 counter) noexcept : counter(counter){}

      bool doTryAcquireFor(const std::chrono::nanoseconds& ns) noexcept;
      bool doTryAcquireFor(const std::chrono::microseconds& ns) noexcept;
      bool doTryAcquireFor(const std::chrono::milliseconds& ns) noexcept;
      bool doTryAcquireFor(const std::chrono::seconds& ns) noexcept;
      template <typename Rep, typename Period>
      inline bool doTryAcquireFor(const std::chrono::duration<Rep, Period>& duration) noexcept {
        return this->doTryAcquireFor(std::chrono::duration_cast<std::chrono::nanoseconds>(duration));
      }

      bool doTryAcquireUntil(const std::chrono::steady_clock::time_point& absTime) noexcept;
      bool doTryAcquireUntil(const std::chrono::system_clock::time_point& absTime) noexcept;
      template <typename Clock, typename Duration>
      inline bool doTryAcquireUntil(const std::chrono::time_point<Clock, Duration>& absoluteTime) noexcept {
        return this->doTryAcquireUntil(std::chrono::time_point_cast<std::chrono::steady_clock::time_point>(absoluteTime));
      }

    public:
      semaphore(const semaphore&) = delete;
      semaphore& operator=(const semaphore&) = delete;

      void release(i64 incValue = 1) noexcept;
      void acquire() noexcept;
      bool tryAcquire() noexcept;
      template <typename Rep, typename Period>
      inline bool tryAcquireFor(const std::chrono::duration<Rep, Period>& duration) noexcept {
        return this->doTryAcquireFor(duration);
      }
      template <typename Clock, typename Duration>
      inline bool tryAcquireUntil(const std::chrono::time_point<Clock, Duration>& absoluteTime) noexcept {
        return this->doTryAcquireUntil(absoluteTime);
      }
    };
  }


  template <i64 LeastMaxValue = 16>
  class counting_semaphore : public detail::semaphore{
  public:
    using detail::semaphore::semaphore;

    constexpr static i64 max() noexcept { return detail::Align2<LeastMaxValue>::value; }
  };
  template <>
  class counting_semaphore<1>{
    bool isAvailable;

    bool doTryAcquireFor(const std::chrono::nanoseconds& ns) noexcept;
    bool doTryAcquireFor(const std::chrono::microseconds& ns) noexcept;
    bool doTryAcquireFor(const std::chrono::milliseconds& ns) noexcept;
    bool doTryAcquireFor(const std::chrono::seconds& ns) noexcept;
    template <typename Rep, typename Period>
    inline bool doTryAcquireFor(const std::chrono::duration<Rep, Period>& duration) noexcept{
      return this->doTryAcquireFor(std::chrono::duration_cast<std::chrono::nanoseconds>(duration));
    }

    bool doTryAcquireUntil(const std::chrono::steady_clock::time_point& absTime) noexcept;
    bool doTryAcquireUntil(const std::chrono::system_clock::time_point& absTime) noexcept;
    template <typename Clock, typename Duration>
    inline bool doTryAcquireUntil(const std::chrono::time_point<Clock, Duration>& absoluteTime) noexcept{
      return this->doTryAcquireUntil(std::chrono::time_point_cast<std::chrono::steady_clock::time_point>(absoluteTime));
    }


  public:
    constexpr explicit counting_semaphore(bool initialState) noexcept : isAvailable(initialState){}
    counting_semaphore(const counting_semaphore&) = delete;

    void release() noexcept;
    void acquire() noexcept;
    bool tryAcquire() noexcept;
    template <typename Rep, typename Period>
    inline bool tryAcquireFor(const std::chrono::duration<Rep, Period>& duration) noexcept{
      return this->doTryAcquireFor(duration);
    }
    template <typename Clock, typename Duration>
    inline bool tryAcquireUntil(const std::chrono::time_point<Clock, Duration>& absoluteTime) noexcept{
      return this->doTryAcquireUntil(absoluteTime);
    }
  };

  using binary_semaphore = counting_semaphore<1>;
  using semaphore       = counting_semaphore<>;
#else
  template <u64 MaxCount = std::numeric_limits<i64>::max()>
  class counting_semaphore{
  public:

    explicit constexpr counting_semaphore(i64 initial_value) noexcept : sem_(initial_value) { }

    counting_semaphore(const counting_semaphore&) = delete;
    counting_semaphore& operator=(const counting_semaphore&) = delete;

    inline constexpr static i64(max)() noexcept {
      return MaxCount;
    }

    void acquire() noexcept {
      this->sem_.acquire();
    }
    void release(i64 update = 1) noexcept {
      this->sem_.release(update);
    }

    VK_nodiscard bool try_acquire()         noexcept {
      return this->sem_.try_acquire();
    }
    VK_nodiscard bool try_acquire(timeout t)  noexcept {
      return this->sem_.try_acquire_for(t.duration());
    }
    VK_nodiscard bool try_acquire(deadline until) noexcept {
      return this->sem_.try_acquire_until(until.time_point());
    }

  private:
    std::counting_semaphore<MaxCount> sem_;
  };
  using semaphore = counting_semaphore<>;
  using binary_semaphore = counting_semaphore<1>;
#endif


  class noop_semaphore{
  public:

    explicit constexpr noop_semaphore(i64) noexcept { }

    noop_semaphore(const noop_semaphore&) = delete;
    noop_semaphore& operator=(const noop_semaphore&) = delete;


    inline constexpr static i64(max)() noexcept {
      return std::numeric_limits<i64>::max();
    }


    void acquire() noexcept { }
    void release(i64 update = 1) noexcept { }

    VK_nodiscard bool try_acquire()         noexcept { return true; }
    VK_nodiscard bool try_acquire(timeout)  noexcept { return true; }
    VK_nodiscard bool try_acquire(deadline) noexcept { return true; }
  };


  static_assert(semaphore_c<counting_semaphore<10>>);
  static_assert(semaphore_c<semaphore>);
  static_assert(semaphore_c<binary_semaphore>);
  static_assert(semaphore_c<noop_semaphore>);
}

#endif//VALKYRIE_ASYNC_SEMAPHORE_HPP
