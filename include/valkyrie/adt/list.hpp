//
// Created by Maxwell on 2021-01-12.
//

#ifndef VALKYRIE_ADT_LIST_HPP
#define VALKYRIE_ADT_LIST_HPP

#include <valkyrie/memory/default_allocator.hpp>
#include <valkyrie/memory/std_allocator.hpp>

#include <plf_list.h>

namespace valkyrie{

  template <typename T, raw_allocator Alloc = default_allocator>
  using list = plf::list<T, std_allocator<T, Alloc>>;
}

#endif //VALKYRIE_ADT_LIST_HPP
