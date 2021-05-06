//
// Created by maxwe on 2021-03-05.
//

#include <valkyrie/memory/memory_pool_collection.hpp>

using namespace valkyrie;

void detail::memory_pool_collection_leak_handler::operator()(i64 amount)
{
  get_leak_handler()({"valkyrie::memory_pool_collection", this}, amount);
}


template class valkyrie::memory_pool_collection<node_pool, identity_buckets>;
template class valkyrie::memory_pool_collection<array_pool, identity_buckets>;
template class valkyrie::memory_pool_collection<small_node_pool, identity_buckets>;

template class valkyrie::memory_pool_collection<node_pool, log2_buckets>;
template class valkyrie::memory_pool_collection<array_pool, log2_buckets>;
template class valkyrie::memory_pool_collection<small_node_pool, log2_buckets>;

VK_trait_instantiation(allocator, memory_pool_collection<node_pool, identity_buckets>);
VK_trait_instantiation(allocator, memory_pool_collection<array_pool, identity_buckets>);
VK_trait_instantiation(allocator, memory_pool_collection<small_node_pool, identity_buckets>);

VK_trait_instantiation(allocator, memory_pool_collection<node_pool, log2_buckets>);
VK_trait_instantiation(allocator, memory_pool_collection<array_pool, log2_buckets>);
VK_trait_instantiation(allocator, memory_pool_collection<small_node_pool, log2_buckets>);

