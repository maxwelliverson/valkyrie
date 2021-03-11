//
// Created by maxwe on 2021-03-05.
//

#ifndef VALKYRIE_FREE_LIST_UTILS_HPP
#define VALKYRIE_FREE_LIST_UTILS_HPP

#include <valkyrie/preprocessor.hpp>
#include <valkyrie/primitives.hpp>

#include <cstring>

namespace valkyrie{
  namespace detail
  {
    //=== storage ===///
    // reads stored integer value
    inline u64 get_int(void *address) noexcept
  {
    VK_assert(address);
    u64 res;
    std::memcpy(&res, address, sizeof(u64));
    return res;
  }

  // sets stored integer value
  inline void set_int(void *address, u64 i) noexcept
{
  VK_assert(address);
  std::memcpy(address, &i, sizeof(u64));
}

// pointer to integer
inline u64 to_int(char *ptr) noexcept
{
return reinterpret_cast<u64>(ptr);
}

// integer to pointer
inline char *from_int(u64 i) noexcept
{
return reinterpret_cast<char *>(i);
}

//=== intrusive linked list ===//
// reads a stored pointer value
inline char *list_get_next(void *address) noexcept
{
return from_int(get_int(address));
}

// stores a pointer value
inline void list_set_next(void *address, char *ptr) noexcept
{
set_int(address, to_int(ptr));
}

//=== intrusive xor linked list ===//
// returns the other pointer given one pointer
inline char *xor_list_get_other(void *address, char *prev_or_next) noexcept
{
return from_int(get_int(address) ^ to_int(prev_or_next));
}

// sets the next and previous pointer (order actually does not matter)
inline void xor_list_set(void *address, char *prev, char *next) noexcept
{
set_int(address, to_int(prev) ^ to_int(next));
}

// changes other pointer given one pointer
inline void xor_list_change(void *address, char *old_ptr, char *new_ptr) noexcept
{
VK_assert(address);
auto other = xor_list_get_other(address, old_ptr);
xor_list_set(address, other, new_ptr);
}

// advances a pointer pair forward/backward
inline void xor_list_iter_next(char *&cur, char *&prev) noexcept
{
auto next = xor_list_get_other(cur, prev);
prev = cur;
cur = next;
}

// links new node between prev and next
inline void xor_list_insert(char *new_node, char *prev, char *next) noexcept
{
xor_list_set(new_node, prev, next);
xor_list_change(prev, next, new_node); // change prev's next to new_node
xor_list_change(next, prev, new_node); // change next's prev to new_node
}

//=== sorted list utils ===//
// if std::less/std::greater not available compare integer representation and hope it works
inline bool less(void *a, void *b) noexcept
{
  return std::less<void*>()(a, b);
}

inline bool less_equal(void *a, void *b) noexcept
{
return a == b || less(a, b);
}

inline bool greater(void *a, void *b) noexcept
{
  return std::greater<void*>()(a, b);
}

inline bool greater_equal(void *a, void *b) noexcept
{
return a == b || greater(a, b);
}
} // namespace detail
}

#endif//VALKYRIE_FREE_LIST_UTILS_HPP
