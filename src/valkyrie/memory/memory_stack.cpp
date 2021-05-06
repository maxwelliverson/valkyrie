//
// Created by maxwe on 2021-03-05.
//

#include <valkyrie/memory/debugging.hpp>
#include <valkyrie/memory/memory_stack.hpp>

using namespace valkyrie;

void detail::memory_stack_leak_handler::operator()(i64 amount)
{
  get_leak_handler()({"valkyrie::memory_stack", this}, amount);
}


template class valkyrie::memory_stack<>;
template class valkyrie::memory_stack_raii_unwind<memory_stack<>>;
VK_trait_instantiation(allocator, memory_stack<>);

