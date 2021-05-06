//
// Created by maxwe on 2021-03-05.
//

#include "free_list_utils.hpp"
#include <valkyrie/traits.hpp>
#include <valkyrie/memory/detail/free_list_array.hpp>

using namespace valkyrie;
using namespace valkyrie::detail;

u64 log2_access_policy::index_from_size(u64 size) noexcept
{
VK_assert(size);
return ilog2_ceil(size);
}

u64 log2_access_policy::size_from_index(u64 index) noexcept
{
return u64(1) << index;
}