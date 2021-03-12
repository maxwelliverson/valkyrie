//
// Created by maxwe on 2021-03-05.
//

#include <valkyrie/memory/error.hpp>
#include <valkyrie/memory/heap_allocator.hpp>

#define NOMINMAX
#include <windows.h>
#include <heapapi.h>

using namespace valkyrie;

namespace
{
  HANDLE get_process_heap() noexcept
{
  static auto heap = GetProcessHeap();
  return heap;
}

u64 max_size() noexcept
{
return _HEAP_MAXREQ;
}
}

void* valkyrie::heap_alloc(u64 size) noexcept
{
return HeapAlloc(get_process_heap(), 0, size);
}

void valkyrie::heap_dealloc(void* ptr, u64) noexcept
{
HeapFree(get_process_heap(), 0, ptr);
}

allocator_info detail::heap_allocator_impl::info() noexcept
{
return {"valkyrie::heap_allocator", nullptr};
}

u64 detail::heap_allocator_impl::max_node_size() noexcept
{
return max_size();
}


template class detail::lowlevel_allocator<detail::heap_allocator_impl>;
template class valkyrie::allocator_traits<heap_allocator>;
