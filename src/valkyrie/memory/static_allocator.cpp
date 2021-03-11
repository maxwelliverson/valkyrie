//
// Created by maxwe on 2021-03-05.
//

#include <valkyrie/memory/error.hpp>
#include <valkyrie/memory/memory_arena.hpp>

using namespace valkyrie;

void* static_allocator::allocate_node(u64 size, u64 alignment)
{
  auto mem = stack_.allocate(end_, size, alignment);
  //if (!mem) FOONATHAN_THROW(out_of_fixed_memory(info(), size));
  return mem;
}

allocator_info static_allocator::info() const noexcept
{
return {"valkyrie::static_allocator", this};
}


template class valkyrie::allocator_traits<static_allocator>;


memory_block static_block_allocator::allocate_block()
{
  if (cur_ + block_size_ > end_)
    //FOONATHAN_THROW(out_of_fixed_memory(info(), block_size_));
    return { };
  auto mem = cur_;
  cur_ += block_size_;
  return {mem, block_size_};
}

void static_block_allocator::deallocate_block(memory_block block) noexcept
{
detail::
debug_check_pointer([&] { return static_cast<char*>(block.memory) + block.size == cur_; },
info(), block.memory);
cur_ -= block_size_;
}

allocator_info static_block_allocator::info() const noexcept
{
return {"valkyrie::static_block_allocator", this};
}