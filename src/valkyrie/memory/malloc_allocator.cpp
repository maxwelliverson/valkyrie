//
// Created by maxwe on 2021-03-05.
//

#include <valkyrie/memory/allocators/malloc_allocator.hpp>
#include <valkyrie/memory/error.hpp>

using namespace valkyrie;


allocator_info detail::malloc_allocator_impl::info() noexcept
{
return {"valkyrie::malloc_allocator", nullptr};
}


template class detail::lowlevel_allocator<detail::malloc_allocator_impl>;
VK_extern_trait_instantiation(allocator, malloc_allocator);
