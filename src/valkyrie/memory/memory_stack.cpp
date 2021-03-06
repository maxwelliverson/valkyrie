//
// Created by maxwe on 2021-03-05.
//

void detail::memory_stack_leak_handler::operator()(std::ptrdiff_t amount)
{
  get_leak_handler()({FOONATHAN_MEMORY_LOG_PREFIX "::memory_stack", this}, amount);
}

#if FOONATHAN_MEMORY_EXTERN_TEMPLATE
template class valkyrie::memory_stack<>;
template class valkyrie::memory_stack_raii_unwind<memory_stack<>>;
template class valkyrie::allocator_traits<memory_stack<>>;
template class valkyrie::composable_allocator_traits<memory_stack<>>;
#endif
