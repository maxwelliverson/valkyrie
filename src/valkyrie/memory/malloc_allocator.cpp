//
// Created by maxwe on 2021-03-05.
//

#include <valkyrie/memory/error.hpp>
#include <valkyrie/memory/malloc_allocator.hpp>

using namespace valkyrie;


allocator_info detail::malloc_allocator_impl::info() noexcept
{
return {"valkyrie::malloc_allocator", nullptr};
}


template class detail::lowlevel_allocator<detail::malloc_allocator_impl>;
template class valkyrie::allocator_traits<malloc_allocator>;
