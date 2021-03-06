//
// Created by maxwe on 2021-03-05.
//

void* static_allocator::allocate_node(std::size_t size, std::size_t alignment)
{
  auto mem = stack_.allocate(end_, size, alignment);
  if (!mem)
    FOONATHAN_THROW(out_of_fixed_memory(info(), size));
  return mem;
}

allocator_info static_allocator::info() const noexcept
{
return {FOONATHAN_MEMORY_LOG_PREFIX "::static_allocator", this};
}

#if FOONATHAN_MEMORY_EXTERN_TEMPLATE
template class valkyrie::allocator_traits<static_allocator>;
#endif

memory_block static_block_allocator::allocate_block()
{
  if (cur_ + block_size_ > end_)
    FOONATHAN_THROW(out_of_fixed_memory(info(), block_size_));
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
return {FOONATHAN_MEMORY_LOG_PREFIX "::static_block_allocator", this};
}