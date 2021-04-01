//
// Created by maxwe on 2021-03-28.
//

#ifndef VALKYRIE_ADT_HAZARD_PTR_HPP
#define VALKYRIE_ADT_HAZARD_PTR_HPP


#include <valkyrie/async/atomic.hpp>
#include <valkyrie/adt/ptr.hpp>
#include <valkyrie/adt/pointer_int_pair.hpp>

namespace valkyrie{

  namespace impl{
    inline constexpr static u64 WriteLockAtomicPtr = static_cast<u64>(0x10);



    class atomic_ptr_storage{

      inline constexpr static u64 write_lock_value = static_cast<u64>(-1);

      inline static void* cast(u64 value) noexcept {
        return reinterpret_cast<void*>(value);
      }
      inline static u64   cast(const void* value) noexcept {
        return reinterpret_cast<u64>(value);
      }

    public:

      explicit atomic_ptr_storage(const void* ptr_) noexcept
          : pointer_value_(reinterpret_cast<u64>(ptr_)),
            current_value_(pointer_value_){}


      void*       do_write()                   noexcept {

        u64 expected;

        do {
          expected = pointer_value_;
          if ( current_value_.compare_exchange_strong(expected, write_lock_value) )
            break;
          if ( expected == write_lock_value )



        } while ( );

        return cast(pointer_value_);
      }
      void*       do_try_write()               noexcept { }
      void*       do_try_write(u64 timeout_ns) noexcept { }
      const void* do_read(const void* lock)                    noexcept {
        u64 expected = pointer_value_;
      }
      const void* do_try_read(const void* lock)                noexcept { }
      void*       do_try_write(const void* lock, u64 timeout_ns) noexcept { }



    private:
      const u64   pointer_value_;
      atomic<u64> current_value_;
    };


  }


  template <typename T>
  class atomic_ptr{

    using value_t = std::remove_cv_t<T>;

    inline static value_t* locked_ptr() noexcept {
      return reinterpret_cast<value_t*>(impl::WriteLockAtomicPtr);
    }


    struct storage{
      value_t*         real_ptr_;
      atomic<value_t*> ptr_;
    };

    class proxy{
      T*       ptr_;
      storage* storage_;
    public:

      // TODO: Finish implementation
      explicit proxy(storage* storage_) noexcept : ptr_(), storage_(storage_){
        value_t* expected = const_cast<value_t*>(storage_->real_ptr_);
        while (!this->storage_->ptr_.compare_exchange_strong(expected, locked_ptr())) {
          storage_->ptr_.wait(locked_ptr());
        }
      }

      ~proxy() {
        const bool result = storage_->ptr_.compare_exchange_strong();
      }

      T* operator->() const noexcept {
        return ptr_;
      }
    };

  public:



    proxy operator->() const noexcept {
      return proxy(storage_);
    }

  private:
    storage* storage_;
  };




  template <typename T>
  class hazard_ptr{
  public:



  private:
    T* p;
  };
}

#endif//VALKYRIE_ADT_HAZARD_PTR_HPP
