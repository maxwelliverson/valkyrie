//
// Created by maxwe on 2021-03-05.
//

#include <valkyrie/memory/memory_arena.hpp>

using namespace valkyrie;
using namespace valkyrie::detail;

void memory_block_stack::push(allocated_mb block) noexcept
{
VK_assert(is_aligned(block.memory, max_alignment));
auto next = ::new (block.memory) node(head_, block.size - implementation_offset());
head_     = next;
}

memory_block_stack::allocated_mb memory_block_stack::pop() noexcept
{
VK_assert(head_);
auto to_pop = head_;
head_       = head_->prev;
return {to_pop, to_pop->usable_size + implementation_offset()};
}

void memory_block_stack::steal_top(memory_block_stack& other) noexcept
{
VK_assert(other.head_);
auto to_steal = other.head_;
other.head_   = other.head_->prev;

to_steal->prev = head_;
head_          = to_steal;
}

bool memory_block_stack::owns(const void* ptr) const noexcept
{
auto address = static_cast<const char*>(ptr);
for (auto cur = head_; cur; cur = cur->prev)
{
auto mem = static_cast<char*>(static_cast<void*>(cur));
if (address >= mem && address < mem + cur->usable_size)
return true;
}
return false;
}

u64 memory_block_stack::size() const noexcept
{
u64 res = 0u;
for (auto cur = head_; cur; cur = cur->prev)
++res;
return res;
}


template class valkyrie::memory_arena<static_block_allocator, true>;
template class valkyrie::memory_arena<static_block_allocator, false>;
template class valkyrie::memory_arena<virtual_block_allocator, true>;
template class valkyrie::memory_arena<virtual_block_allocator, false>;

template class valkyrie::growing_block_allocator<>;
template class valkyrie::memory_arena<growing_block_allocator<>, true>;
template class valkyrie::memory_arena<growing_block_allocator<>, false>;

template class valkyrie::fixed_block_allocator<>;
template class valkyrie::memory_arena<fixed_block_allocator<>, true>;
template class valkyrie::memory_arena<fixed_block_allocator<>, false>;
