//
// Created by maxwe on 2021-05-05.
//

#ifndef VALKYRIE_MEMORY_MEMORY_RESOURCE_HPP
#define VALKYRIE_MEMORY_MEMORY_RESOURCE_HPP

#include <valkyrie/traits.hpp>

namespace valkyrie{
  class memory_resource{

  public:
    inline void* allocate_node(u64 size, u64 align) {
      return this->do_allocate_node(size, align);
    }
    inline void* allocate_array(u64 count, u64 size, u64 align) {
      return this->do_allocate_array(count, size, align);
    }
    inline void  deallocate_node(void* p, u64 size, u64 align) noexcept {
      this->do_deallocate_node(p, size, align);
    }
    inline void  deallocate_array(void* p, u64 count, u64 size, u64 align) noexcept {
      this->do_deallocate_array(p, count, size, align);
    }

    virtual ~memory_resource() = default;
  private:
  protected:
    virtual void* do_allocate_node(u64 size, u64 align) = 0;
    virtual void* do_allocate_array(u64 count, u64 size, u64 align) = 0;
    virtual void  do_deallocate_node(void* p, u64 size, u64 align) noexcept = 0;
    virtual void  do_deallocate_array(void* p, u64 count, u64 size, u64 align) = 0;
  };
}

#endif  //VALKYRIE_MEMORY_MEMORY_RESOURCE_HPP
