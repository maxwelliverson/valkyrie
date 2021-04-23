//
// Created by maxwe on 2021-04-19.
//

#ifndef VALKYRIE_UTILITY_REF_COUNTED_HPP
#define VALKYRIE_UTILITY_REF_COUNTED_HPP

#include <valkyrie/async/atomic.hpp>


namespace valkyrie{

  class ref_counted{
  public:
    virtual ~ref_counted();

    ref_counted();
    ref_counted(const ref_counted&);
    ref_counted& operator=(const ref_counted&);

    /// Increases reference count by one.
    void ref() const noexcept {
      ref_count_.fetch_add(1, std::memory_order_relaxed);
    }

    /// Decreases reference count by one and calls `request_deletion`
    /// when it drops to zero.
    void deref() const noexcept;

    /// Queries whether there is exactly one reference.
    bool unique() const noexcept {
      return rc_ == 1;
    }

    size_t get_reference_count() const noexcept {
      return rc_;
    }


  private:
    mutable atomic<u32> ref_count_;
  };

  /// @relates ref_counted
  inline void intrusive_ptr_add_ref(const ref_counted* p) {
    p->ref();
  }

/// @relates ref_counted
  inline void intrusive_ptr_release(const ref_counted* p) {
    p->deref();
  }


}

#endif  //VALKYRIE_UTILITY_REF_COUNTED_HPP
