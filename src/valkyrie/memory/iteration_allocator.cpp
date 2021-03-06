//
// Created by maxwe on 2021-03-05.
//

#if FOONATHAN_MEMORY_EXTERN_TEMPLATE
template class valkyrie::iteration_allocator<2>;
template class valkyrie::allocator_traits<iteration_allocator<2>>;
template class valkyrie::composable_allocator_traits<iteration_allocator<2>>;
#endif