//
// Created by maxwe on 2021-03-05.
//

#include <valkyrie/memory/debugging.hpp>
#include <valkyrie/memory/error.hpp>

#include <valkyrie/async/atomic.hpp>

#include <cstdio>

#define NOMINMAX
#include <windows.h>
#include <libloaderapi.h>
#include <filesystem>

using namespace valkyrie;




namespace {

  inline std::u8string get_program_name() noexcept {
    char exeName[MAX_PATH];
    GetModuleFileName(nullptr, exeName, MAX_PATH);
    std::filesystem::path path(exeName);
    return path.filename().u8string();
  }

  std::u8string       default_log_prefix_ = get_program_name();
  atomic<utf8_string> log_prefix_         = default_log_prefix_.c_str();
  atomic<utf8_string> owned_log_prefix_   = nullptr;
}


void valkyrie::set_log_prefix(string_view new_prefix, bool copy) noexcept {

  utf8_string last_prefix      = nullptr;
  const utf8* new_string       = nullptr;
  utf8*       new_owned_string = nullptr;

  if ( copy ) {
    new_owned_string = new utf8 [new_prefix.size() + 1];
    std::memcpy(new_owned_string, new_prefix.data(), new_prefix.size());
    new_owned_string[new_prefix.size()] = u8'\0';
    new_string = new_owned_string;
  } else {
    new_string = new_prefix.data();
  }

  while ( log_prefix_.compare_exchange_weak(last_prefix, new_string) );
  if    ( owned_log_prefix_.compare_exchange_strong(last_prefix, new_owned_string) )
    delete last_prefix;
}
string_view valkyrie::get_log_prefix() noexcept {
  return { log_prefix_.load() };
}


namespace
{
  void default_leak_handler(const allocator_info& info, i64 amount) noexcept
{
  if (amount > 0)
    std::fprintf(stderr, "[%s] Allocator %s (at %p) leaked %zu bytes.\n",
                 (cstring)log_prefix_.load(), info.name, info.allocator,
                 u64(amount));
  else
    std::fprintf(stderr,
                 "[%s] Allocator %s (at %p) has deallocated %zu bytes more than "
                 "ever allocated "
                 "(it's amazing you're able to see this message!).\n",
                 (cstring)log_prefix_.load(), info.name, info.allocator,
                 u64(-amount));
}

std::atomic<leak_handler> leak_h(default_leak_handler);
} // namespace

leak_handler valkyrie::set_leak_handler(leak_handler h)
{
  return leak_h.exchange(h ? h : default_leak_handler);
}

leak_handler valkyrie::get_leak_handler()
{
  return leak_h;
}

namespace
{
  void default_invalid_ptr_handler(const allocator_info& info, const void* ptr) noexcept
{
  std::fprintf(stderr,
               "[%s] Deallocation function of allocator %s (at %p) received invalid "
               "pointer %p\n",
               (cstring)log_prefix_.load(), info.name, info.allocator, ptr);
  std::abort();
}

std::atomic<invalid_pointer_handler> invalid_ptr_h(default_invalid_ptr_handler);
} // namespace

invalid_pointer_handler valkyrie::set_invalid_pointer_handler(invalid_pointer_handler h)
{
  return invalid_ptr_h.exchange(h ? h : default_invalid_ptr_handler);
}

invalid_pointer_handler valkyrie::get_invalid_pointer_handler()
{
  return invalid_ptr_h;
}

namespace
{
  void default_buffer_overflow_handler(const void* memory, u64 node_size,
                                       const void* ptr) noexcept
{
  std::fprintf(stderr,
               "[%s] Buffer overflow at address %p detected, corresponding memory "
               "block %p has only size %zu.",
               (cstring)log_prefix_.load(), ptr, memory, node_size);
  std::abort();
}

std::atomic<buffer_overflow_handler> buffer_overflow_h(default_buffer_overflow_handler);
} // namespace

buffer_overflow_handler valkyrie::set_buffer_overflow_handler(buffer_overflow_handler h)
{
  return buffer_overflow_h.exchange(h ? h : default_buffer_overflow_handler);
}

buffer_overflow_handler valkyrie::get_buffer_overflow_handler()
{
  return buffer_overflow_h;
}