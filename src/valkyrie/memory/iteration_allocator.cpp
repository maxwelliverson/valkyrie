//
// Created by maxwe on 2021-03-05.
//

#include <valkyrie/memory/iteration_allocator.hpp>

using namespace valkyrie;


template class valkyrie::iteration_allocator<2>;
template class valkyrie::allocator_traits<iteration_allocator<2>>;
template class valkyrie::composable_allocator_traits<iteration_allocator<2>>;
