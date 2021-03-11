//
// Created by maxwe on 2021-03-05.
//

#include <valkyrie/memory/detail/debug_helpers.hpp>
#include <valkyrie/memory/memory_arena.hpp>
#include <valkyrie/memory/error.hpp>



void valkyrie::detail::virtual_memory_allocator_leak_handler::operator()(i64 amount)
{
  detail::debug_handle_memory_leak({"valkyrie::virtual_memory_allocator",
                                   nullptr},
                                   amount);
}

#if defined(_WIN32)
#include <windows.h>

using namespace valkyrie;

namespace
{
    u64 get_page_size() noexcept
    {
        static_assert(sizeof(u64) >= sizeof(DWORD), "possible loss of data");

        SYSTEM_INFO info;
        GetSystemInfo(&info);
        return u64(info.dwPageSize);
    }
} // namespace

const u64 valkyrie::virtual_memory_page_size = get_page_size();

void* valkyrie::virtual_memory_reserve(u64 no_pages) noexcept
{
    auto pages =
#if (_MSC_VER <= 1900)
        VirtualAlloc(nullptr, no_pages * virtual_memory_page_size, MEM_RESERVE, PAGE_READWRITE);
#else
        VirtualAllocFromApp(nullptr, no_pages * virtual_memory_page_size, MEM_RESERVE,
                            PAGE_READWRITE);
#endif
    return pages;
}

void valkyrie::virtual_memory_release(void* pages, u64) noexcept
{
    auto result = VirtualFree(pages, 0u, MEM_RELEASE);
    VK_assert_msg(result, "cannot release pages");
}

void* valkyrie::virtual_memory_commit(void* memory, u64 no_pages) noexcept
{
    auto region =
#if (_MSC_VER <= 1900)
        VirtualAlloc(memory, no_pages * virtual_memory_page_size, MEM_COMMIT, PAGE_READWRITE);
#else
        VirtualAllocFromApp(memory, no_pages * virtual_memory_page_size, MEM_COMMIT,
                            PAGE_READWRITE);
#endif
    if (!region)
        return nullptr;
    VK_assert(region == memory);
    return region;
}

void valkyrie::virtual_memory_decommit(void* memory, u64 no_pages) noexcept
{
    auto result = VirtualFree(memory, no_pages * virtual_memory_page_size, MEM_DECOMMIT);
    VK_assert_msg(result, "cannot decommit memory");
}
#elif defined(__unix__) || defined(__APPLE__) || defined(__VXWORKS__)                              \
    || defined(__QNXNTO__) // POSIX systems
#include <sys/mman.h>
#include <unistd.h>

#if defined(PAGESIZE)
const u64 valkyrie::virtual_memory_page_size = PAGESIZE;
#elif defined(PAGE_SIZE)
const u64 valkyrie::virtual_memory_page_size = PAGE_SIZE;
#else
const u64 valkyrie::virtual_memory_page_size = sysconf(_SC_PAGESIZE);
#endif

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

void* valkyrie::virtual_memory_reserve(u64 no_pages) noexcept
{
    auto pages = mmap(nullptr, no_pages * virtual_memory_page_size, PROT_NONE,
                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    return pages == MAP_FAILED ? nullptr : pages;
}

void valkyrie::virtual_memory_release(void* pages, u64 no_pages) noexcept
{
    auto result = munmap(pages, no_pages * virtual_memory_page_size);
    FOONATHAN_MEMORY_ASSERT_MSG(result == 0, "cannot release pages");
    (void)result;
}

void* valkyrie::virtual_memory_commit(void* memory, u64 no_pages) noexcept
{
    auto size   = no_pages * virtual_memory_page_size;
    auto result = mprotect(memory, size, PROT_WRITE | PROT_READ);
    if (result != 0u)
        return nullptr;

// advise that the memory will be needed
#if defined(MADV_WILLNEED)
    madvise(memory, size, MADV_WILLNEED);
#elif defined(POSIX_MADV_WILLNEED)
    posix_madvise(memory, size, POSIX_MADV_WILLNEED);
#endif

    return memory;
}

void valkyrie::virtual_memory_decommit(void* memory, u64 no_pages) noexcept
{
    auto size = no_pages * virtual_memory_page_size;
// advise that the memory won't be needed anymore
#if defined(MADV_FREE)
    madvise(memory, size, MADV_FREE);
#elif defined(MADV_DONTNEED)
    madvise(memory, size, MADV_DONTNEED);
#elif defined(POSIX_MADV_DONTNEED)
    posix_madvise(memory, size, POSIX_MADV_DONTNEED);
#endif

    auto result = mprotect(memory, size, PROT_NONE);
    FOONATHAN_MEMORY_ASSERT_MSG(result == 0, "cannot decommit memory");
    (void)result;
}
#else
#warning "virtual memory functions not available on your platform, define your own"
#endif

namespace
{
  u64 calc_no_pages(u64 size) noexcept
{
  auto div  = size / virtual_memory_page_size;
  auto rest = size % virtual_memory_page_size;

  return div + (rest != 0u) + (detail::debug_fence_size ? 2u : 1u);
}
} // namespace

void* virtual_memory_allocator::allocate_node(u64 size, u64)
{
  auto no_pages = calc_no_pages(size);
  auto pages    = virtual_memory_reserve(no_pages);
  /*if (!pages || !virtual_memory_commit(pages, no_pages))
    FOONATHAN_THROW(
        out_of_memory({"valkyrie::virtual_memory_allocator", nullptr},
                      no_pages * virtual_memory_page_size));*/

  if (!pages || !virtual_memory_commit(pages, no_pages))
    return nullptr;

  on_allocate(size);
  return detail::debug_fill_new(pages, size, virtual_memory_page_size);
}

void virtual_memory_allocator::deallocate_node(void* node, u64 size, u64) noexcept
{
auto pages = detail::debug_fill_free(node, size, virtual_memory_page_size);

on_deallocate(size);

auto no_pages = calc_no_pages(size);
virtual_memory_decommit(pages, no_pages);
virtual_memory_release(pages, no_pages);
}

u64 virtual_memory_allocator::max_node_size() const noexcept
{
return u64(-1);
}

u64 virtual_memory_allocator::max_alignment() const noexcept
{
return virtual_memory_page_size;
}


template class valkyrie::allocator_traits<virtual_memory_allocator>;


virtual_block_allocator::virtual_block_allocator(u64 block_size, u64 no_blocks)
    : block_size_(block_size)
{
  VK_assert(block_size % virtual_memory_page_size == 0u);
  VK_assert(no_blocks > 0);
  auto total_size = block_size_ * no_blocks;
  auto no_pages   = total_size / virtual_memory_page_size;

  cur_ = static_cast<char*>(virtual_memory_reserve(no_pages));
  if (!cur_)
    panic(u8"Out of memory!!");
  /*if (!cur_)
    FOONATHAN_THROW(out_of_memory(info(), total_size));*/
  end_ = cur_ + total_size;
}

virtual_block_allocator::~virtual_block_allocator() noexcept
{
virtual_memory_release(cur_, (end_ - cur_) / virtual_memory_page_size);
}

memory_block virtual_block_allocator::allocate_block()
{
  if (u64(end_ - cur_) < block_size_)
    //FOONATHAN_THROW(out_of_fixed_memory(info(), block_size_));
    return {};
  auto mem = virtual_memory_commit(cur_, block_size_ / virtual_memory_page_size);
  if (!mem)
    //FOONATHAN_THROW(out_of_fixed_memory(info(), block_size_));
    return {};
  cur_ += block_size_;
  return {mem, block_size_};
}

void virtual_block_allocator::deallocate_block(memory_block block) noexcept
{
detail::
debug_check_pointer([&] { return static_cast<char*>(block.memory) == cur_ - block_size_; },
info(), block.memory);
cur_ -= block_size_;
virtual_memory_decommit(cur_, block_size_);
}

allocator_info virtual_block_allocator::info() noexcept
{
return {"valkyrie::virtual_block_allocator", this};
}