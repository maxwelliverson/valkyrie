//
// Created by maxwe on 2021-03-05.
//

#include <valkyrie/memory/error.hpp>
#include <valkyrie/memory/new_allocator.hpp>

using namespace valkyrie;


allocator_info detail::new_allocator_impl::info() noexcept
{
return { "valkyrie::new_allocator", nullptr };
}

void* detail::new_allocator_impl::allocate(u64 size, size_t) noexcept
{
void* memory = nullptr;
while (true)
{
memory = ::operator new(size, std::nothrow);
if (memory)
break;

auto handler = std::get_new_handler();
if (handler)
{
#if FOONATHAN_HAS_EXCEPTION_SUPPORT
try
            {
                handler();
            }
            catch (...)
            {
                return nullptr;
            }
#else
handler();
#endif
}
else
{
return nullptr;
}
}
return memory;
}

void detail::new_allocator_impl::deallocate(void* ptr, u64, size_t) noexcept
{
::operator delete(ptr);
}

u64 detail::new_allocator_impl::max_node_size() noexcept
{
  return std::allocator_traits<std::allocator<char>>::max_size({});
}


template class detail::lowlevel_allocator<detail::new_allocator_impl>;
template class valkyrie::allocator_traits<new_allocator>;
