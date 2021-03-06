//
// Created by maxwe on 2021-03-05.
//

std::size_t log2_access_policy::index_from_size(std::size_t size) noexcept
{
FOONATHAN_MEMORY_ASSERT_MSG(size, "size must not be zero");
return ilog2_ceil(size);
}

std::size_t log2_access_policy::size_from_index(std::size_t index) noexcept
{
return std::size_t(1) << index;
}