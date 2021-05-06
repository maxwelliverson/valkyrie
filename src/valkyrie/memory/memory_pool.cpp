//
// Created by maxwe on 2021-03-05.
//

#include <valkyrie/memory/debugging.hpp>
#include <valkyrie/memory/memory_pool.hpp>

using namespace valkyrie;

void detail::memory_pool_leak_handler::operator()(i64 amount)
{
  get_leak_handler()({ "valkyrie::memory_pool", this }, amount);
}


template class valkyrie::memory_pool<node_pool>;
template class valkyrie::memory_pool<array_pool>;
template class valkyrie::memory_pool<small_node_pool>;

VK_trait_instantiation(allocator, memory_pool<node_pool>);
VK_trait_instantiation(allocator, memory_pool<array_pool>);
VK_trait_instantiation(allocator, memory_pool<small_node_pool>);