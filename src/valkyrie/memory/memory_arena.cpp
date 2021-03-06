//
// Created by maxwe on 2021-03-05.
//

void memory_block_stack::push(allocated_mb block) noexcept
{
FOONATHAN_MEMORY_ASSERT(is_aligned(block.memory, max_alignment));
auto next = ::new (block.memory) node(head_, block.size - implementation_offset());
head_     = next;
}

memory_block_stack::allocated_mb memory_block_stack::pop() noexcept
{
FOONATHAN_MEMORY_ASSERT(head_);
auto to_pop = head_;
head_       = head_->prev;
return {to_pop, to_pop->usable_size + implementation_offset()};
}

void memory_block_stack::steal_top(memory_block_stack& other) noexcept
{
FOONATHAN_MEMORY_ASSERT(other.head_);
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

std::size_t memory_block_stack::size() const noexcept
{
std::size_t res = 0u;
for (auto cur = head_; cur; cur = cur->prev)
++res;
return res;
}

#if FOONATHAN_MEMORY_EXTERN_TEMPLATE
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
#endif