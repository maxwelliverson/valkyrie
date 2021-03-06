//
// Created by maxwe on 2021-03-05.
//

void detail::memory_pool_collection_leak_handler::operator()(std::ptrdiff_t amount)
{
  get_leak_handler()({FOONATHAN_MEMORY_LOG_PREFIX "::memory_pool_collection", this}, amount);
}

#if FOONATHAN_MEMORY_EXTERN_TEMPLATE
template class valkyrie::memory_pool_collection<node_pool, identity_buckets>;
template class valkyrie::memory_pool_collection<array_pool, identity_buckets>;
template class valkyrie::memory_pool_collection<small_node_pool, identity_buckets>;

template class valkyrie::memory_pool_collection<node_pool, log2_buckets>;
template class valkyrie::memory_pool_collection<array_pool, log2_buckets>;
template class valkyrie::memory_pool_collection<small_node_pool, log2_buckets>;

template class valkyrie::allocator_traits<memory_pool_collection<node_pool,
                                                                          identity_buckets>>;
template class valkyrie::allocator_traits<memory_pool_collection<array_pool,
                                                                          identity_buckets>>;
template class valkyrie::allocator_traits<memory_pool_collection<small_node_pool,
                                                                          identity_buckets>>;

template class valkyrie::allocator_traits<memory_pool_collection<node_pool, log2_buckets>>;
template class valkyrie::allocator_traits<memory_pool_collection<array_pool,
                                                                          log2_buckets>>;
template class valkyrie::allocator_traits<memory_pool_collection<small_node_pool,
                                                                          log2_buckets>>;

template class valkyrie::
    composable_allocator_traits<memory_pool_collection<node_pool, identity_buckets>>;
template class valkyrie::
    composable_allocator_traits<memory_pool_collection<array_pool, identity_buckets>>;
template class valkyrie::
    composable_allocator_traits<memory_pool_collection<small_node_pool, identity_buckets>>;

template class valkyrie::composable_allocator_traits<memory_pool_collection<node_pool,
                                                                                     log2_buckets>>;
template class valkyrie::composable_allocator_traits<memory_pool_collection<array_pool,
                                                                                     log2_buckets>>;
template class valkyrie::
    composable_allocator_traits<memory_pool_collection<small_node_pool, log2_buckets>>;
#endif