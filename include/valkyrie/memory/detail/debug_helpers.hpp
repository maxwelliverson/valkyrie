//
// Created by maxwe on 2021-03-05.
//

#ifndef VALKYRIE_MEMORY_DETAIL_DEBUG_HELPERS_HPP
#define VALKYRIE_MEMORY_DETAIL_DEBUG_HELPERS_HPP

#include <valkyrie/primitives.hpp>
#include <valkyrie/traits.hpp>
#include <valkyrie/async/atomic.hpp>

#define PP_VK_impl_INLINE_LINKAGE_FN(...) { __VA_ARGS__ }
#define PP_VK_impl_EXTERNAL_LINKAGE_FN(...) ;

#define VK_inline_linkage_fn PP_VK_impl_INLINE_LINKAGE_FN
#define VK_external_linkage_fn PP_VK_impl_EXTERNAL_LINKAGE_FN
#define VK_debug_memory(...) VK_if(VK_not(VK_debug_build)( inline static )) __VA_ARGS__ VK_if(VK_debug_build(VK_external_linkage_fn)VK_else(VK_inline_linkage_fn))

namespace valkyrie{
  enum class debug_magic : unsigned char;
  struct allocator_info;

  namespace detail {

    using debug_fill_enabled = meta::boolean_constant<VK_debug_build>;
    constexpr u64 debug_fence_size = debug_fill_enabled::value;

    VK_debug_memory(void debug_fill(void *, u64, debug_magic) noexcept)()

    VK_debug_memory(void* debug_is_filled(void*, u64, debug_magic) noexcept)( return nullptr; )

    VK_debug_memory(void *debug_fill_new(void *memory, u64, u64) noexcept)( return memory; )

    VK_debug_memory(void *debug_fill_free(void *memory, u64, u64) noexcept )( return static_cast<char *>(memory); )

    VK_debug_memory(void debug_fill_internal(void *, u64, bool) noexcept)()

    void debug_handle_invalid_ptr(const allocator_info &info, void *ptr);

    // validates given ptr by evaluating the Functor
    // if the Functor returns false, calls the debug_leak_checker
    // note: ptr is just used as the information passed to the invalid ptr handler
    template<class Functor>
    void debug_check_pointer(Functor condition, const allocator_info &info, void *ptr) {
      VK_if(VK_debug_build(
          if (!condition()) debug_handle_invalid_ptr(info, ptr);
      ) VK_else(
            (void)ptr;
            (void)condition;
            (void)info;
      ))
    }

    // validates ptr by using a more expensive double-dealloc check
    template<class Functor>
    void debug_check_double_dealloc(Functor condition, const allocator_info &info,
                                    void *ptr) {
      VK_if(VK_debug_build(
                if (!condition()) debug_handle_invalid_ptr(info, ptr);
            ) VK_else(
                (void)ptr;
                (void)condition;
                (void)info;
            ))
    }

    void debug_handle_memory_leak(const allocator_info &info, i64 amount);

    // does no leak checking, null overhead
    template<class Handler>
    class no_leak_checker {
    public:
      no_leak_checker() noexcept = default;
      no_leak_checker(no_leak_checker &&) noexcept = default;
      //~no_leak_checker() noexcept = default;

      no_leak_checker &operator=(no_leak_checker &&) noexcept = default;

      void on_allocate(u64) noexcept {}
      void on_deallocate(u64) noexcept {}
    };

    // does leak checking per-object
    // leak is detected upon destructor
    template<class Handler>
    class object_leak_checker : Handler {
    public:
      object_leak_checker() noexcept : allocated_(0) {}

      object_leak_checker(object_leak_checker &&other) noexcept
          : allocated_(other.allocated_) {
        other.allocated_ = 0;
      }

      ~object_leak_checker() /*noexcept*/ {
        if (allocated_ != 0)
          this->operator()(allocated_);
      }

      object_leak_checker &operator=(object_leak_checker &&other) noexcept {
        allocated_ = other.allocated_;
        other.allocated_ = 0;
        return *this;
      }

      void on_allocate(u64 size) noexcept {
        allocated_ += i64(size);
      }

      void on_deallocate(u64 size) noexcept {
        allocated_ -= i64(size);
      }

    private:
      i64 allocated_;
    };

    // does leak checking on a global basis
    // call macro VALKYRIE_GLOBAL_LEAK_CHECKER(handler, var_name) in the header
    // when last counter gets destroyed, leak is detected
    template<class Handler>
    class global_leak_checker_impl {
    public:
      struct counter : Handler {
        counter() {
          ++no_counter_objects_;
        }

        ~counter() {
          --no_counter_objects_;
          if (no_counter_objects_ == 0u && allocated_ != 0u)
            this->operator()(allocated_);
        }
      };

      global_leak_checker_impl() noexcept = default;
      global_leak_checker_impl(global_leak_checker_impl &&) noexcept {}
      ~global_leak_checker_impl() noexcept = default;

      global_leak_checker_impl &operator=(global_leak_checker_impl &&) noexcept {
        return *this;
      }

      void on_allocate(u64 size) noexcept {
        allocated_ += i64(size);
      }

      void on_deallocate(u64 size) noexcept {
        allocated_ -= i64(size);
      }

    private:
      static atomic<u64> no_counter_objects_;
      static atomic<i64> allocated_;
    };

    template<class Handler>
    atomic<u64> global_leak_checker_impl<Handler>::no_counter_objects_(0u);

    template<class Handler>
    atomic<i64> global_leak_checker_impl<Handler>::allocated_(0);

#if defined(DEBUG_GLOBAL_LEAK_CHECKING)
    template<class Handler>
    using global_leak_checker = global_leak_checker_impl<Handler>;

#define VALKYRIE_GLOBAL_LEAK_CHECKER(handler, var_name)                                    \
    static valkyrie::detail::global_leak_checker<handler>::counter var_name;
#else
    template<class Handler>
    using global_leak_checker = no_leak_checker<int>;// only one instantiation

#define VALKYRIE_GLOBAL_LEAK_CHECKER(handler, var_name)
#endif


#if VK_debug_build
    template<class Handler>
    using default_leak_checker = object_leak_checker<Handler>;
#else
    template<class Handler>
    using default_leak_checker = no_leak_checker<Handler>;
#endif
  }
}

#endif//VALKYRIE_MEMORY_DETAIL_DEBUG_HELPERS_HPP
