//
// Created by maxwe on 2021-04-24.
//

#ifndef VALKYRIE_UTILITY_TIME_HPP
#define VALKYRIE_UTILITY_TIME_HPP


#include <chrono>

namespace valkyrie{
  inline namespace time_literals{
    using namespace std::chrono_literals;
  }


  class clock{
  public:
    using rep                       = long long;
    using period                    = std::nano;
    using duration                  = std::chrono::nanoseconds;
    using time_point                = std::chrono::time_point<clock>;

    VK_constant bool is_steady      = true;

    VK_nodiscard static time_point now() noexcept {

    }
  };


  class timer{

  };

  class timeout{

    using duration_t = std::chrono::high_resolution_clock::duration;

  public:
    template <typename Rep, typename Period>
    explicit timeout(const std::chrono::duration<Rep, Period>& dur) noexcept
        : timeout_(std::chrono::duration_cast<duration_t>(dur)){}

    VK_nodiscard duration_t duration() const noexcept {
      return timeout_;
    }

  private:
    duration_t timeout_;
  };
  class deadline{

    using time_point_t = typename std::chrono::high_resolution_clock::time_point;

  public:



    template <typename Clk, typename Dur>
    explicit deadline(const std::chrono::time_point<Clk, Dur>& tm) noexcept
        : deadline_(std::chrono::time_point_cast<time_point_t>(tm)){}


    VK_nodiscard time_point_t time_point() const noexcept {
      return deadline_;
    }

  private:
    time_point_t deadline_;
  };


  template <typename Rep, typename Per>
  inline static timeout  try_for(const std::chrono::duration<Rep, Per>& dur) noexcept {
    return timeout(dur);
  }
  template <typename Clk, typename Dur>
  inline static deadline try_until(const std::chrono::time_point<Clk, Dur>& tm) noexcept {
    return deadline(tm);
  }

}

#endif  //VALKYRIE_UTILITY_TIME_HPP
