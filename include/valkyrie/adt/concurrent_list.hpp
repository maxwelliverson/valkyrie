//
// Created by maxwe on 2021-03-28.
//

#ifndef VALKYRIE_ADT_CONCURRENT_LIST_HPP
#define VALKYRIE_ADT_CONCURRENT_LIST_HPP

#include <valkyrie/async/atomic.hpp>
#include <valkyrie/adt/intrusive_list.hpp>
#include <valkyrie/memory/default_allocator.hpp>

namespace valkyrie{




  namespace impl{

    class cslist_node_base;
    class clist_node_base;

    class concurrent_forward_list{
      atomic<cslist_node_base*> head;
      atomic<cslist_node_base*> tail;
      atomic<u64>               length;
    };
    class concurrent_forward_ilist{};
    class concurrent_list {
      
    };
    class concurrent_ilist {};
    
  }


  template <typename T, raw_allocator Alloc = default_allocator>
  class clist : impl::concurrent_list {
    
  };
  
  template <typename T, raw_allocator Alloc = default_allocator>
  class cslist : impl::concurrent_forward_list{
    
  };




}

#endif//VALKYRIE_ADT_CONCURRENT_LIST_HPP
