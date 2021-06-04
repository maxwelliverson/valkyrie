//
// Created by maxwe on 2021-05-14.
//

#include <valkyrie/adt/small_vector.hpp>
#include <valkyrie/status/status_code.hpp>

using namespace valkyrie;

namespace {
  struct struct_16bytes {
    alignas(16) void * x;
  };
  struct struct_32bytes {
    alignas(32) void * x;
  };
}

static_assert(sizeof(small_array<void *, 0>) == sizeof(unsigned) * 2 + sizeof(void *),
              "wasted space in small_array size 0");
static_assert(alignof(small_array<struct_16bytes, 0>) >= alignof(struct_16bytes),
              "wrong alignment for 16-byte aligned T");
static_assert(alignof(small_array<struct_32bytes, 0>) >= alignof(struct_32bytes),
              "wrong alignment for 32-byte aligned T");
static_assert(sizeof(small_array<struct_16bytes, 0>) >= alignof(struct_16bytes),
              "missing padding for 16-byte aligned T");
static_assert(sizeof(small_array<struct_32bytes, 0>) >= alignof(struct_32bytes),
              "missing padding for 32-byte aligned T");
static_assert(sizeof(small_array<void *, 1>) == sizeof(unsigned) * 2 + sizeof(void *) * 2,
              "wasted space in small_array size 1");

static_assert(sizeof(small_array<char, 0>) == sizeof(void *) * 2 + sizeof(void *),
              "1 byte elements have word-sized type for size and capacity");

/// Report that minimum_size doesn't fit into this vector's size type. Throws
/// std::length_error or calls report_fatal_error.
VK_noreturn static void report_size_overflow(u64 minimum_size, u64 maximum_size) VK_throws {
#ifdef VK_exceptions_enabled
  std::string Reason = "valkyrie::small_vector unable to grow. Requested capacity (" +
                       std::to_string(minimum_size) +
                       ") is larger than maximum value for size type (" +
                       std::to_string(maximum_size) + ")";
  throw std::length_error(Reason);
#else
  std::u8string reason = VK_string("valkyrie::small_vector unable to grow. Requested capacity (") +

  panic();
#endif
}

/// Report that this vector is already at maximum capacity. Throws
/// std::length_error or calls report_fatal_error.
VK_noreturn static void report_at_maximum_capacity(u64 maximum_size) VK_throws {
  std::string Reason =
      "valkyrie::small_vector capacity unable to grow. Already at maximum size " +
      std::to_string(maximum_size);
#ifdef VK_exceptions_enabled
  throw std::length_error(Reason);
#else
  report_fatal_error(Reason);
#endif
}


template <class Size_T>
u64 impl::small_array_base<Size_T>::get_new_capacity(u64 minimum_size, u64 old_capacity) noexcept {
  constexpr u64 maximum_size = std::numeric_limits<Size_T>::max();

  // Ensure we can fit the new capacity.
  // This is only going to be applicable when the capacity is 32 bit.
  if (minimum_size > maximum_size) VK_unlikely
    report_size_overflow(minimum_size, maximum_size);

  // Ensure we can meet the guarantee of space for at least one more element.
  // The above check alone will not catch the case where grow is called with a
  // default minimum_size of 0, but the current capacity cannot be increased.
  // This is only going to be applicable when the capacity is 32 bit.
  if (old_capacity == maximum_size) VK_unlikely
    report_at_maximum_capacity(maximum_size);

  // In theory 2*capacity can overflow if the capacity is 64 bit, but the
  // original capacity would never be large enough for this to be a problem.
  const u64 new_capacity = 2 * old_capacity + 1; // Always grow.
  return std::min(std::max(new_capacity, minimum_size), maximum_size);
}




template class impl::small_array_base<valkyrie::u32>;
template class impl::small_array_base<valkyrie::u64>;