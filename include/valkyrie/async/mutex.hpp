//
// Created by maxwe on 2021-04-05.
//

#ifndef VALKYRIE_ASYNC_MUTEX_HPP
#define VALKYRIE_ASYNC_MUTEX_HPP


#include "semaphore.hpp"
#include "atomic.hpp"


// Mutex implementations that are faster and smaller than the
// OS based STL mutex types in the majority of cases where they
// are only used in-process.

namespace valkyrie{

  
  
  namespace impl{
    template <typename Mtx>
    struct mutex_read_access{

      using mutex_type = Mtx;

      inline static mutex_type* lock(mutex_type& mut) noexcept {
        mut.read_lock();
        return std::addressof(mut);
      }
      inline static mutex_type* try_lock(mutex_type& mut) noexcept {
        if (mut.try_read_lock())
          return std::addressof(mut);
        return nullptr;
      }
      template <typename Clk, typename Dur>
      inline static mutex_type* try_lock_until(mutex_type& mut, const std::chrono::time_point<Clk, Dur>& time_point) {
        if (mut.try_read_lock_until(time_point))
          return std::addressof(mut);
        return nullptr;
      }
      template <typename Rep, typename Period>
      inline static mutex_type* try_lock_for(mutex_type& mut, const std::chrono::duration<Rep, Period>& time) {
        if (mut.try_read_lock_for(time))
          return std::addressof(mut);
        return nullptr;
      }

      inline static void unlock(mutex_type& mut) noexcept {
        mut.read_unlock();
      }
    };
    template <typename Mtx>
    struct mutex_write_access{
      using mutex_type = Mtx;

      inline static mutex_type* lock(mutex_type& mut) noexcept {
        mut.write_lock();
        return std::addressof(mut);
      }
      inline static mutex_type* try_lock(mutex_type& mut) noexcept {
        if (mut.try_write_lock())
          return std::addressof(mut);
        return nullptr;
      }
      template <typename Clk, typename Dur>
      inline static mutex_type* try_lock_until(mutex_type& mut, const std::chrono::time_point<Clk, Dur>& time_point) {
        if (mut.try_write_lock_until(time_point))
          return std::addressof(mut);
        return nullptr;
      }
      template <typename Rep, typename Period>
      inline static mutex_type* try_lock_for(mutex_type& mut, const std::chrono::duration<Rep, Period>& time) {
        if (mut.try_write_lock_for(time))
          return std::addressof(mut);
        return nullptr;
      }

      inline static void unlock(mutex_type& mut) noexcept {
        mut.read_unlock();
      }
    };
    template <typename Mtx>
    struct mutex_upgrade_access{
      using mutex_type = Mtx;

      inline static mutex_type* lock(mutex_type& mut) noexcept {
        mut.upgrade_lock();
        return std::addressof(mut);
      }
      inline static mutex_type* try_lock(mutex_type& mut) noexcept {
        if (mut.try_upgrade_lock())
          return std::addressof(mut);
        return nullptr;
      }
      template <typename Clk, typename Dur>
      inline static mutex_type* try_lock_until(mutex_type& mut, const std::chrono::time_point<Clk, Dur>& time_point) {
        if (mut.try_upgrade_lock_until(time_point))
          return std::addressof(mut);
        return nullptr;
      }
      template <typename Rep, typename Period>
      inline static mutex_type* try_lock_for(mutex_type& mut, const std::chrono::duration<Rep, Period>& time) {
        if (mut.try_upgrade_lock_for(time))
          return std::addressof(mut);
        return nullptr;
      }

      inline static void unlock(mutex_type& mut) noexcept {
        mut.write_unlock();
      }
    };

    struct access_tag_t{};
    struct read_access_t : access_tag_t{};
    struct write_access_t : access_tag_t{};
    struct upgrade_access_t : access_tag_t{};

    struct non_blocking_t{};


    template <typename T>
    struct mutex_access;
    template <>
    struct mutex_access<read_access_t>{
      template <typename T>
      using type = mutex_read_access<T>;
    };
    template <>
    struct mutex_access<write_access_t>{
      template <typename T>
      using type = mutex_write_access<T>;
    };
    template <>
    struct mutex_access<upgrade_access_t>{
      template <typename T>
      using type = mutex_upgrade_access<T>;
    };
  }

  class timeout{

    using duration_t = std::chrono::high_resolution_clock::duration;

  public:
    template <typename Rep, typename Period>
    explicit timeout(const std::chrono::duration<Rep, Period>& dur) noexcept
        : timeout_(std::chrono::duration_cast<duration_t>(dur)){}

    duration_t duration() const noexcept {
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


    time_point_t time_point() const noexcept {
      return deadline_;
    }

  private:
    time_point_t deadline_;
  };

  template <typename Rep, typename Per>
  inline static timeout try_for(const std::chrono::duration<Rep, Per>& dur) noexcept {
    return timeout(dur);
  }
  template <typename Clk, typename Dur>
  inline static deadline try_until(const std::chrono::time_point<Clk, Dur>& tm) noexcept {
    return deadline(tm);
  }
  
  inline constexpr static impl::read_access_t    read_access{};
  inline constexpr static impl::write_access_t   write_access{};
  inline constexpr static impl::upgrade_access_t upgrade_access{};

  inline constexpr static impl::non_blocking_t do_not_block{};
  
  template <typename Mtx, template <typename> typename Access = impl::mutex_write_access>
  class lock{
  public:
    
    using mutex_type = Mtx;
    
    VK_nodiscard lock(mutex_type& mut) noexcept
        : mut_(Access<Mtx>::lock(mut)){ }
    VK_nodiscard lock(mutex_type& mut, impl::access_tag_t) noexcept
        : mut_(Access<Mtx>::lock(mut)){ }
    VK_nodiscard lock(mutex_type& mut, impl::non_blocking_t) noexcept
        : mut_(Access<Mtx>::try_lock(mut)){ }
    VK_nodiscard lock(mutex_type& mut, impl::access_tag_t, impl::non_blocking_t) noexcept
        : mut_(Access<Mtx>::try_lock(mut)){ }
    VK_nodiscard lock(mutex_type& mut, deadline deadline_) noexcept
        : mut_(Access<Mtx>::try_lock_until(mut, deadline_.time_point())){ }
    VK_nodiscard lock(mutex_type& mut, impl::access_tag_t, deadline deadline_) noexcept
        : mut_(Access<Mtx>::try_lock_until(mut, deadline_.time_point())){ }
    VK_nodiscard lock(mutex_type& mut, timeout timeout_) noexcept
        : mut_(Access<Mtx>::try_lock_for(mut, timeout_.duration())){ }
    VK_nodiscard lock(mutex_type& mut, impl::access_tag_t, timeout timeout_) noexcept
        : mut_(Access<Mtx>::try_lock_for(mut, timeout_.duration())){ }



    lock(const lock&) = delete;
    lock(lock&& other) noexcept : mut_(other.release()){}

    lock& operator=(const lock&) = delete;
    lock& operator=(lock&& other) noexcept {
      this->~lock();
      new(this) lock(std::move(other));
      return *this;
    }


    ~lock() {
      this->do_unlock();
    }

    explicit operator bool() const noexcept {
      return bool(mut_);
    }


    void unlock() noexcept {
      do_unlock();
      mut_ = nullptr;
    }

    mutex_type* take() noexcept {
      do_unlock();
      return release();
    }
    mutex_type* release() noexcept {
      return std::exchange(mut_, nullptr);
    }

    void swap(lock& other) noexcept {
      mutex_type* tmp = release();
      mut_ = other.release();
      other.mut_ = tmp;
    }
    
  private:

    inline void do_unlock() noexcept {
      if (mut_)
        Access<Mtx>::unlock(*mut_);
    }

    mutex_type* mut_;
  };


  template <typename Mtx>
  lock(Mtx&)                       -> lock<Mtx>;
  template <typename Mtx>
  lock(Mtx&, impl::non_blocking_t) -> lock<Mtx>;
  template <typename Mtx>
  lock(Mtx&, deadline)             -> lock<Mtx>;
  template <typename Mtx>
  lock(Mtx&, timeout)              -> lock<Mtx>;


  template <typename Mtx, std::derived_from<impl::access_tag_t> Acc>
  lock(Mtx&, Acc) -> lock<Mtx, /*typename */impl::mutex_access<Acc>::template type>;
  template <typename Mtx, std::derived_from<impl::access_tag_t> Acc>
  lock(Mtx&, Acc, impl::non_blocking_t) -> lock<Mtx, /*typename */impl::mutex_access<Acc>::template type>;
  template <typename Mtx, std::derived_from<impl::access_tag_t> Acc>
  lock(Mtx&, Acc, deadline) -> lock<Mtx, /*typename */impl::mutex_access<Acc>::template type>;
  template <typename Mtx, std::derived_from<impl::access_tag_t> Acc>
  lock(Mtx&, Acc, timeout) -> lock<Mtx, /*typename */impl::mutex_access<Acc>::template type>;

  class mutex{
    binary_semaphore sem_{1};
  public:

    mutex()             = default;
    mutex(const mutex&) = delete;

    mutex& operator=(const mutex&) = delete;



    void              read_lock() noexcept {
      sem_.acquire();
    }
    VK_nodiscard bool try_read_lock() noexcept {
      return sem_.try_acquire();
    }
    template <typename Clk, typename Dur>
    VK_nodiscard bool try_read_lock_until(const std::chrono::time_point<Clk, Dur>& time_point) {
      return sem_.try_acquire_until(time_point);
    }
    template <typename Rep, typename Period>
    VK_nodiscard bool try_read_lock_for(const std::chrono::duration<Rep, Period>& time) {
      return sem_.try_acquire_for(time);
    }


    void              upgrade_lock() noexcept {}
    VK_nodiscard bool try_upgrade_lock() noexcept {
      return true;
    }
    template <typename Clk, typename Dur>
    VK_nodiscard bool try_upgrade_lock_until(const std::chrono::time_point<Clk, Dur>& time_point) noexcept {
      return true;
    }
    template <typename Rep, typename Period>
    VK_nodiscard bool try_upgrade_lock_for(const std::chrono::duration<Rep, Period>& time) noexcept {
      return true;
    }


    void              write_lock() noexcept {
      sem_.acquire();
    }
    VK_nodiscard bool try_write_lock() noexcept {
      return sem_.try_acquire();
    }
    template <typename Rep, typename Period>
    VK_nodiscard bool try_write_lock_for(const std::chrono::duration<Rep, Period>& time) {
      return sem_.try_acquire_for(time);
    }
    template <typename Clk, typename Dur>
    VK_nodiscard bool try_write_lock_until(const std::chrono::time_point<Clk, Dur>& time_point) {
      return sem_.try_acquire_until(time_point);
    }


    void read_unlock() noexcept {
      sem_.release();
    }
    void write_unlock() noexcept {
      sem_.release();
    }
  };

  class shared_mutex{
    atomic<u32> reader_count;
    mutex       read_mutex;
    mutex       write_mutex;
  public:
    shared_mutex() = default;
    shared_mutex(const shared_mutex&) = delete;

    shared_mutex& operator=(const shared_mutex&) = delete;


    void              read_lock() noexcept {
      lock outer(read_mutex);

      if (++reader_count == 1)
        write_mutex.write_lock();
    }
    VK_nodiscard bool try_read_lock() noexcept {
      lock outer(read_mutex);

      if (++reader_count == 1 && !write_mutex.try_write_lock()) {
        --reader_count;
        return false;
      }
      return true;
    }
    template <typename Clk, typename Dur>
    VK_nodiscard bool try_read_lock_until(const std::chrono::time_point<Clk, Dur>& time_point) {
      lock outer(read_mutex);

      if (++reader_count == 1 && !write_mutex.try_write_lock_until(time_point)) {
        --reader_count;
        return false;
      }

      return true;
    }
    template <typename Rep, typename Period>
    VK_nodiscard bool try_read_lock_for(const std::chrono::duration<Rep, Period>& time) {
      lock outer(read_mutex);

      if (++reader_count == 1 && !write_mutex.try_write_lock_for(time)) {
        --reader_count;
        return false;
      }

      return true;
    }


    void              upgrade_lock() noexcept {
      lock outer(read_mutex);

      if (--reader_count != 0)
        write_mutex.write_lock();
    }
    VK_nodiscard bool try_upgrade_lock() noexcept {
      lock outer(read_mutex);

      if (--reader_count != 0)
        return write_mutex.try_write_lock();

      return true;
    }
    template <typename Rep, typename Period>
    VK_nodiscard bool try_upgrade_lock_for(const std::chrono::duration<Rep, Period>& time) {
      lock outer(read_mutex);

      if (--reader_count != 0)
        return write_mutex.try_write_lock_for(time);

      return true;
    }
    template <typename Clk, typename Dur>
    VK_nodiscard bool try_upgrade_lock_until(const std::chrono::time_point<Clk, Dur>& time_point) {
      lock outer(read_mutex);

      if (--reader_count != 0)
        return write_mutex.try_write_lock_until(time_point);

      return true;
    }


    void              write_lock() noexcept {
      write_mutex.write_lock();
    }
    VK_nodiscard bool try_write_lock() noexcept {
      return write_mutex.try_write_lock();
    }
    template <typename Rep, typename Period>
    VK_nodiscard bool try_write_lock_for(const std::chrono::duration<Rep, Period>& time) {
      return write_mutex.try_write_lock_for(time);
    }
    template <typename Clk, typename Dur>
    VK_nodiscard bool try_write_lock_until(const std::chrono::time_point<Clk, Dur>& time_point) {
      return write_mutex.try_write_lock_until(time_point);
    }


    void read_unlock() noexcept {
      lock outer(read_mutex);

      if (--reader_count == 0)
        write_mutex.write_unlock();
    }
    void write_unlock() noexcept {
      write_mutex.write_unlock();
    }
  };

}

#endif//VALKYRIE_ASYNC_MUTEX_HPP
