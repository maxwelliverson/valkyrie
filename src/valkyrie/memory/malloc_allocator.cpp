//
// Created by maxwe on 2021-03-05.
//


allocator_info detail::malloc_allocator_impl::info() noexcept
{
return {FOONATHAN_MEMORY_LOG_PREFIX "::malloc_allocator", nullptr};
}

#if FOONATHAN_MEMORY_EXTERN_TEMPLATE
template class detail::lowlevel_allocator<detail::malloc_allocator_impl>;
template class valkyrie::allocator_traits<malloc_allocator>;
#endif