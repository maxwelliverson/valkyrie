//
// Created by maxwe on 2021-03-05.
//

#include <valkyrie/primitives.hpp>
#include <valkyrie/memory/debugging.hpp>
#include <valkyrie/async/atomic.hpp>
#include <valkyrie/memory/error.hpp>

using namespace valkyrie;


namespace
{
  void default_out_of_memory_handler(const allocator_info& info, u64 amount) noexcept
{
  std::fprintf(stderr,
               "[%s] Allocator %s (at %p) ran out of memory trying to allocate %zu bytes.\n",
               get_log_prefix().c_str(), info.name, info.allocator, amount);
}

std::atomic<out_of_memory::handler> out_of_memory_h(default_out_of_memory_handler);
} // namespace

out_of_memory::handler out_of_memory::set_handler(out_of_memory::handler h)
{
  return out_of_memory_h.exchange(h ? h : default_out_of_memory_handler);
}

out_of_memory::handler out_of_memory::get_handler()
{
  return out_of_memory_h;
}

out_of_memory::out_of_memory(const allocator_info& info, u64 amount)
    : info_(info), amount_(amount)
{
  out_of_memory_h.load()(info, amount);
}

const char* out_of_memory::what() const noexcept
{
return "low-level allocator is out of memory";
}

const char* out_of_fixed_memory::what() const noexcept
{
return "fixed size allocator is out of memory";
}

namespace
{
  void default_bad_alloc_size_handler(const allocator_info& info, u64 passed,
                                      u64 supported) noexcept
{
#if FOONATHAN_HOSTED_IMPLEMENTATION
  std::fprintf(stderr,
                     "[%s] Allocator %s (at %p) received invalid size/alignment %zu, "
                     "max supported is %zu",
                     FOONATHAN_MEMORY_LOG_PREFIX, info.name, info.allocator, passed, supported);
#endif
}

std::atomic<bad_allocation_size::handler> bad_alloc_size_h(default_bad_alloc_size_handler);
} // namespace

bad_allocation_size::handler bad_allocation_size::set_handler(bad_allocation_size::handler h)
{
  return bad_alloc_size_h.exchange(h ? h : default_bad_alloc_size_handler);
}

bad_allocation_size::handler bad_allocation_size::get_handler()
{
  return bad_alloc_size_h;
}

bad_allocation_size::bad_allocation_size(const allocator_info& info, u64 passed,
                                         u64 supported)
    : info_(info), passed_(passed), supported_(supported)
{
  bad_alloc_size_h.load()(info_, passed_, supported_);
}

const char* bad_allocation_size::what() const noexcept
{
return "allocation node size exceeds supported maximum of allocator";
}

const char* bad_node_size::what() const noexcept
{
return "allocation node size exceeds supported maximum of allocator";
}

const char* bad_array_size::what() const noexcept
{
return "allocation array size exceeds supported maximum of allocator";
}

const char* bad_alignment::what() const noexcept
{
return "allocation alignment exceeds supported maximum of allocator";
}