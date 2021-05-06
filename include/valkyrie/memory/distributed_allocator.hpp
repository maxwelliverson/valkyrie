//
// Created by maxwe on 2021-04-06.
//

#ifndef VALKYRIE_MEMORY_DISTRIBUTED_ALLOCATOR_HPP
#define VALKYRIE_MEMORY_DISTRIBUTED_ALLOCATOR_HPP

#include <valkyrie/traits.hpp>
#include "default_allocator.hpp"

#include <valkyrie/agent/channel.hpp>

namespace valkyrie{

  namespace impl{
    enum class allocator_msg_kind{
      free,
      alloc
    };

    struct allocator_msg{
      allocator_msg_kind kind;
      void*              address;
      u64                length;
    };
  }


  class distributed_allocator{
  public:






    void* allocate_node(u64 size, u64 align) noexcept;
    void* allocate_array(u64 count, u64 size, u64 align) noexcept;
    void deallocate_node(void* p, u64 size, u64 align) noexcept;
    void deallocate_array(void* p, u64 count, u64 size, u64 align) noexcept;
  };



}

#endif//VALKYRIE_MEMORY_DISTRIBUTED_ALLOCATOR_HPP
