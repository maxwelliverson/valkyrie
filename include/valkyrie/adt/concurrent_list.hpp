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

    class concurrent_forward_list{

      forward_ilist_header header;

      using node_t = forward_ilist_node_base*;

      u64         native_length() const noexcept;

      inline void inc_length() noexcept {
        ++atomic<u64&>(header.length);
      }
      inline void dec_length() noexcept {
        --atomic<u64&>(header.length);
      }

      inline void add_length(u64 val) noexcept {
        atomic<u64&>(header.length).fetch_add(val);
      }
      inline void sub_length(u64 val) noexcept {
        atomic<u64&>(header.length).fetch_sub(val);
      }

      inline void reset_length(u64 len, u64 val = 0) noexcept {
        atomic<u64&>(header.length).compare_exchange_strong(len, val);
      }

      //static void   insert_node(node_t after_node, node_t node) noexcept;
      //static node_t remove_node(node_t after_node) noexcept;

    protected:

      void init() noexcept;

      node_t clear() noexcept;
      void   push_front(node_t node) noexcept;
      node_t pop_front() noexcept;


      VK_nodiscard inline u64 size() const noexcept {
        return atomic<const u64&>(header.length).load();
      }
    };
    class concurrent_list {
    public:

    };
  }


  template <typename T, allocator_c Alloc = default_allocator>
  class clist : impl::concurrent_list {
    
  };
  
  template <typename T, allocator_c Alloc = default_allocator>
  class cslist : impl::concurrent_forward_list{




  };




}

#endif//VALKYRIE_ADT_CONCURRENT_LIST_HPP
