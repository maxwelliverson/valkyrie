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

    struct alignas(16) cslist_node {
      cslist_node * next;
      byte          data[];
    };
    struct clist_node {};

    struct alignas(16) cslist_header{
      u64 alignment;
      u64 data;
    };

    class concurrent_forward_list{

      forward_ilist_header header;
      atomic<u64> list_length_;

    protected:

      void clear() noexcept;
      void push_front(cslist_node * node) noexcept;
      void pop_front(cslist_node * node) noexcept;
      void push_back(cslist_node * node) noexcept;
      void pop_back(cslist_node * node) noexcept;
      static void insert_node(cslist_node * after_node, cslist_node * node) noexcept;
      static void remove_node(cslist_node * after_node) noexcept;

      inline u64 size() const noexcept {
        return list_length_.load(std::memory_order_acquire);
      }
    };
    class concurrent_list {
    public:

    };
  }


  template <typename T, raw_allocator Alloc = default_allocator>
  class clist : impl::concurrent_list {
    
  };
  
  template <typename T, raw_allocator Alloc = default_allocator>
  class cslist : impl::concurrent_forward_list{




  };




}

#endif//VALKYRIE_ADT_CONCURRENT_LIST_HPP
