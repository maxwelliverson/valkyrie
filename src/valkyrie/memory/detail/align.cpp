//
// Created by maxwe on 2021-03-05.
//

bool valkyrie::detail::is_aligned(void* ptr, std::size_t alignment) noexcept
{
FOONATHAN_MEMORY_ASSERT(is_valid_alignment(alignment));
auto address = reinterpret_cast<std::uintptr_t>(ptr);
return address % alignment == 0u;
}

std::size_t valkyrie::detail::alignment_for(std::size_t size) noexcept
{
return size >= max_alignment ? max_alignment : (std::size_t(1) << ilog2(size));
}