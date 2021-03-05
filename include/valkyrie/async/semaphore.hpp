//
// Created by Maxwell on 2020-11-18.
//

#ifndef VALKYRIE_ASYNC_SEMAPHORE_HPP
#define VALKYRIE_ASYNC_SEMAPHORE_HPP

#include "atomic.hpp"
#include <valkyrie/preprocessor.hpp>
#include <valkyrie/primitives.hpp>

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

    class Semaphore{
      i64 counter;
    protected:
      constexpr explicit Semaphore(i64 counter) noexcept : counter(counter){}

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
      Semaphore(const Semaphore&) = delete;
      Semaphore& operator=(const Semaphore&) = delete;

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
  class CountingSemaphore : public detail::Semaphore{
  public:
    using detail::Semaphore::Semaphore;

    constexpr static i64 max() noexcept { return detail::Align2<LeastMaxValue>::value; }
  };
  template <>
  class CountingSemaphore<1>{
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
    constexpr explicit CountingSemaphore(bool initialState) noexcept : isAvailable(initialState){}
    CountingSemaphore(const CountingSemaphore&) = delete;

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

  using BinarySemaphore = CountingSemaphore<1>;
  using Semaphore       = CountingSemaphore<>;
#else
  template <size_t N = std::numeric_limits<ptrdiff_t>::max()>
  using CountingSemaphore = std::counting_semaphore<N>;
  using Semaphore = CountingSemaphore<>;
  using BinarySemaphore = std::binary_semaphore;
#endif
}

#endif//VALKYRIE_ASYNC_SEMAPHORE_HPP
