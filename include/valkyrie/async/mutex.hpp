//
// Created by maxwe on 2021-04-05.
//

#ifndef VALKYRIE_ASYNC_MUTEX_HPP
#define VALKYRIE_ASYNC_MUTEX_HPP

#include <valkyrie/utility/time.hpp>

#include "semaphore.hpp"



// Mutex implementations that are faster and smaller than the
// OS based STL mutex types in the majority of cases where they
// are only used in-process.

namespace valkyrie{


  inline namespace concepts{
    template <typename M>
    concept mutex_c = requires(M& mut, timeout t, deadline d){

      { mut.read_lock()      } noexcept;
      { mut.try_read_lock()  } noexcept -> exact_same_as<bool>;
      { mut.try_read_lock(t) } noexcept -> exact_same_as<bool>;
      { mut.try_read_lock(d) } noexcept -> exact_same_as<bool>;
      { mut.read_unlock()    } noexcept;

      { mut.upgrade_lock()      } noexcept;
      { mut.try_upgrade_lock()  } noexcept -> exact_same_as<bool>;
      { mut.try_upgrade_lock(t) } noexcept -> exact_same_as<bool>;
      { mut.try_upgrade_lock(d) } noexcept -> exact_same_as<bool>;

      { mut.write_lock()        } noexcept;
      { mut.try_write_lock()    } noexcept -> exact_same_as<bool>;
      { mut.try_write_lock(t)   } noexcept -> exact_same_as<bool>;
      { mut.try_write_lock(d)   } noexcept -> exact_same_as<bool>;
      { mut.write_unlock()      } noexcept;
    };
  }

  
  
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
      inline static mutex_type* try_lock(mutex_type& mut, deadline time_point) {
        if (mut.try_read_lock(time_point))
          return std::addressof(mut);
        return nullptr;
      }
      inline static mutex_type* try_lock(mutex_type& mut, timeout time) {
        if (mut.try_read_lock(time))
          return std::addressof(mut);
        return nullptr;
      }
      inline static void        unlock(mutex_type& mut) noexcept {
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
      inline static mutex_type* try_lock(mutex_type& mut, timeout time) {
        if (mut.try_write_lock(time))
          return std::addressof(mut);
        return nullptr;
      }
      inline static mutex_type* try_lock(mutex_type& mut, deadline time_point) {
        if (mut.try_write_lock(time_point))
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
      inline static mutex_type* try_lock(mutex_type& mut, deadline time_point) {
        if (mut.try_upgrade_lock(time_point))
          return std::addressof(mut);
        return nullptr;
      }
      inline static mutex_type* try_lock(mutex_type& mut, timeout time) {
        if (mut.try_upgrade_lock(time))
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




  
  inline constexpr static impl::read_access_t    read_access{};
  inline constexpr static impl::write_access_t   write_access{};
  inline constexpr static impl::upgrade_access_t upgrade_access{};

  inline constexpr static impl::non_blocking_t   do_not_block{};
  
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
        : mut_(Access<Mtx>::try_lock(mut, deadline_)){ }
    VK_nodiscard lock(mutex_type& mut, impl::access_tag_t, deadline deadline_) noexcept
        : mut_(Access<Mtx>::try_lock(mut, deadline_)){ }
    VK_nodiscard lock(mutex_type& mut, timeout timeout_) noexcept
        : mut_(Access<Mtx>::try_lock(mut, timeout_)){ }
    VK_nodiscard lock(mutex_type& mut, impl::access_tag_t, timeout timeout_) noexcept
        : mut_(Access<Mtx>::try_lock(mut, timeout_)){ }



    lock(const lock&) = delete;
    lock(lock&& other) noexcept : mut_(other.release()){}

    lock& operator=(const lock&) = delete;
    lock& operator=(lock&& other) noexcept {
      this->~lock();
      new(this) lock(std::move(other));
      return *this;
    }


    ~lock() {
      this->private_unlock();
    }

    explicit operator bool() const noexcept {
      return bool(mut_);
    }


    void unlock() noexcept {
      private_unlock();
      mut_ = nullptr;
    }

    mutex_type* take() noexcept {
      private_unlock();
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

    inline void private_unlock() noexcept {
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


  template <mutex_c Mtx>
  class generic_lock{
    using mutex_type = Mtx;

    /*VK_nodiscard generic_lock(mutex_type& mut) noexcept {
      do_write_lock(mut);
    }*/

    VK_nodiscard generic_lock(mutex_type& mut, impl::read_access_t) noexcept : is_write_(false){
      do_read_lock(mut);
    }
    VK_nodiscard generic_lock(mutex_type& mut, impl::write_access_t) noexcept {
      do_write_lock(mut);
    }
    VK_nodiscard generic_lock(mutex_type& mut, impl::upgrade_access_t) noexcept : is_write_(false){
      do_write_lock(mut);
    }

    VK_nodiscard generic_lock(mutex_type& mut, impl::read_access_t, impl::non_blocking_t) noexcept : is_write_(false){
      do_try_read_lock(mut);
    }
    VK_nodiscard generic_lock(mutex_type& mut, impl::write_access_t, impl::non_blocking_t) noexcept {
      do_try_write_lock(mut);
    }
    VK_nodiscard generic_lock(mutex_type& mut, impl::upgrade_access_t, impl::non_blocking_t) noexcept : is_write_(false){
      do_try_write_lock(mut);
    }

    VK_nodiscard generic_lock(mutex_type& mut, impl::read_access_t, deadline deadline_) noexcept : is_write_(false){
      do_try_read_lock(mut, deadline_);
    }
    VK_nodiscard generic_lock(mutex_type& mut, impl::write_access_t, deadline deadline_) noexcept {
      do_try_write_lock(mut, deadline_);
    }
    VK_nodiscard generic_lock(mutex_type& mut, impl::upgrade_access_t, deadline deadline_) noexcept : is_write_(false){
      do_try_write_lock(mut, deadline_);
    }

    VK_nodiscard generic_lock(mutex_type& mut, impl::read_access_t, timeout timeout_) noexcept : is_write_(false){
      do_try_read_lock(mut, timeout_);
    }
    VK_nodiscard generic_lock(mutex_type& mut, impl::write_access_t, timeout timeout_) noexcept {
      do_try_write_lock(mut, timeout_);
    }
    VK_nodiscard generic_lock(mutex_type& mut, impl::upgrade_access_t, timeout timeout_) noexcept : is_write_(false){
      do_try_write_lock(mut, timeout_);
    }

    /*VK_nodiscard generic_lock(generic_lock&& other, impl::read_access_t) noexcept : is_write_(other.is_write_){
      if ( other.mut_ ) {
        do_read_lock(*std::exchange(other.mut_, nullptr));
      }
    }
    VK_nodiscard generic_lock(generic_lock&& other, impl::write_access_t) noexcept : is_write_(other.is_write_){
      if ( other.mut_ ) {
        do_write_lock(*std::exchange(other.mut_, nullptr));
      }
    }
    */

    VK_nodiscard generic_lock(generic_lock&& other) noexcept
        : mut_(std::exchange(other.mut_, nullptr)),
          is_write_(other.is_write_){}


    generic_lock(const generic_lock&) = delete;

    ~generic_lock() {
      do_unlock();
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

    void swap(generic_lock& other) noexcept {
      std::swap(mut_, other.mut_);
      std::swap(is_write_, other.is_write_);
    }

  private:

    void do_write_lock(mutex_type& mutex) noexcept {
      if ( is_write_ ) {
        mutex.write_lock();
      }
      else {
        mutex.upgrade_lock();
        is_write_ = true;
      }
      mut_ = std::addressof(mutex);
    }
    void do_read_lock(mutex_type& mutex) noexcept {
      if ( is_write_ )
        return;
      mutex.read_lock();
      mut_ = std::addressof(mutex);
    }

    void do_try_write_lock(mutex_type& mutex) noexcept {
      if ( is_write_ ) {
        if ( !mutex.try_write_lock() )
          return;
      }
      else {
        if ( !mutex.try_upgrade_lock() )
          return;
        is_write_ = true;
      }
      mut_ = std::addressof(mutex);
    }
    void do_try_read_lock(mutex_type& mutex) noexcept {
      if ( !is_write_ ) {
        if ( !mutex.try_read_lock() )
          return;
      }
      mut_ = std::addressof(mutex);
    }

    void do_try_write_lock(mutex_type& mutex, timeout t) noexcept {
      if ( is_write_ ) {
        if ( !mutex.try_write_lock(t) )
          return;
      }
      else {
        if ( !mutex.try_upgrade_lock(t) )
          return;
        is_write_ = true;
      }
      mut_ = std::addressof(mutex);
    }
    void do_try_read_lock(mutex_type& mutex, timeout t) noexcept {
      if ( !is_write_ ) {
        if ( !mutex.try_read_lock(t) )
          return;
      }
      mut_ = std::addressof(mutex);
    }

    void do_try_write_lock(mutex_type& mutex, deadline d) noexcept {
      if ( is_write_ ) {
        if ( !mutex.try_write_lock(d) )
          return;
      }
      else {
        if ( !mutex.try_upgrade_lock(d) )
          return;
        is_write_ = true;
      }
      mut_ = std::addressof(mutex);
    }
    void do_try_read_lock(mutex_type& mutex, deadline d) noexcept {
      if ( !is_write_ ) {
        if ( !mutex.try_read_lock(d) )
          return;
      }
      mut_ = std::addressof(mutex);
    }

    void do_unlock() noexcept {
      if ( mut_ ) {
        if ( is_write_ ) {
          mut_->write_unlock();
        }
        else {
          mut_->read_unlock();
        }
      }
    }


    mutex_type* mut_      = nullptr;
    bool        is_write_ = true;
  };



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
    VK_nodiscard bool try_read_lock(timeout time) noexcept {
      return sem_.try_acquire(time);
    }
    VK_nodiscard bool try_read_lock(deadline time_point) noexcept {
      return sem_.try_acquire(time_point);
    }



    void              upgrade_lock() noexcept {}
    VK_nodiscard bool try_upgrade_lock() noexcept {
      return true;
    }
    VK_nodiscard bool try_upgrade_lock(timeout time) noexcept {
      return true;
    }
    VK_nodiscard bool try_upgrade_lock(deadline time_point) noexcept {
      return true;
    }



    void              write_lock() noexcept {
      sem_.acquire();
    }
    VK_nodiscard bool try_write_lock() noexcept {
      return sem_.try_acquire();
    }
    VK_nodiscard bool try_write_lock(timeout time) noexcept {
      return sem_.try_acquire(time);
    }
    VK_nodiscard bool try_write_lock(deadline time_point) noexcept {
      return sem_.try_acquire(time_point);
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
    VK_nodiscard bool try_read_lock(deadline time_point) noexcept {
      lock outer(read_mutex);

      if (++reader_count == 1 && !write_mutex.try_write_lock(time_point)) {
        --reader_count;
        return false;
      }

      return true;
    }
    VK_nodiscard bool try_read_lock(timeout time) noexcept {
      lock outer(read_mutex);

      if (++reader_count == 1 && !write_mutex.try_write_lock(time)) {
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
    VK_nodiscard bool try_upgrade_lock(timeout time) noexcept {
      lock outer(read_mutex);

      if (--reader_count != 0)
        return write_mutex.try_write_lock(time);

      return true;
    }
    VK_nodiscard bool try_upgrade_lock(deadline time_point) noexcept {
      lock outer(read_mutex);

      if (--reader_count != 0)
        return write_mutex.try_write_lock(time_point);

      return true;
    }


    void              write_lock() noexcept {
      write_mutex.write_lock();
    }
    VK_nodiscard bool try_write_lock() noexcept {
      return write_mutex.try_write_lock();
    }
    VK_nodiscard bool try_write_lock(timeout time) noexcept {
      return write_mutex.try_write_lock(time);
    }
    VK_nodiscard bool try_write_lock(deadline time_point) noexcept {
      return write_mutex.try_write_lock(time_point);
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

  class noop_mutex{
  public:

    //constexpr noop_mutex() = default;
    //noop_mutex(const noop_mutex&) = delete;

    //noop_mutex& operator=(const noop_mutex&) = delete;


    VK_forceinline void     read_lock()            noexcept { }
    VK_forceinline bool try_read_lock()            noexcept { return true; }
    VK_forceinline bool try_read_lock(timeout)     noexcept { return true; }
    VK_forceinline bool try_read_lock(deadline)    noexcept { return true; }

    VK_forceinline void     upgrade_lock()         noexcept { }
    VK_forceinline bool try_upgrade_lock()         noexcept { return true; }
    VK_forceinline bool try_upgrade_lock(timeout)  noexcept { return true; }
    VK_forceinline bool try_upgrade_lock(deadline) noexcept { return true; }

    VK_forceinline void     write_lock()           noexcept { }
    VK_forceinline bool try_write_lock()           noexcept { return true; }
    VK_forceinline bool try_write_lock(timeout)    noexcept { return true; }
    VK_forceinline bool try_write_lock(deadline)   noexcept { return true; }


    VK_forceinline void  read_unlock() noexcept { }
    VK_forceinline void write_unlock() noexcept { }
  };


  static_assert(mutex_c<mutex>);
  static_assert(mutex_c<shared_mutex>);
  static_assert(mutex_c<noop_mutex>);

}

#define VK_scope_lock(mutex, ...) valkyrie::lock scope_LOCK_{mutex, ##__VA_ARGS__}
#define VK_if_lock(mutex, access, ...) if (valkyrie::lock if_scope_LOCK_{mutex, access, VK_if(VK_pack_is_empty(__VA_ARGS__)(valkyrie::do_not_block)VK_else(__VA_ARGS__))})

#endif//VALKYRIE_ASYNC_MUTEX_HPP
