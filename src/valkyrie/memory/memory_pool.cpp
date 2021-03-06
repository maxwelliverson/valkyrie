//
// Created by maxwe on 2021-03-05.
//

void detail::memory_pool_leak_handler::operator()(std::ptrdiff_t amount)
{
  get_leak_handler()({FOONATHAN_MEMORY_LOG_PREFIX "::memory_pool", this}, amount);
}

#if FOONATHAN_MEMORY_EXTERN_TEMPLATE
template class valkyrie::memory_pool<node_pool>;
template class valkyrie::memory_pool<array_pool>;
template class valkyrie::memory_pool<small_node_pool>;

template class valkyrie::allocator_traits<memory_pool<node_pool>>;
template class valkyrie::allocator_traits<memory_pool<array_pool>>;
template class valkyrie::allocator_traits<memory_pool<small_node_pool>>;

template class valkyrie::composable_allocator_traits<memory_pool<node_pool>>;
template class valkyrie::composable_allocator_traits<memory_pool<array_pool>>;
template class valkyrie::composable_allocator_traits<memory_pool<small_node_pool>>;
#endif