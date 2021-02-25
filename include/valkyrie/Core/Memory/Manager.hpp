//
// Created by Maxwell on 2021-02-22.
//

#ifndef VALKYRIE_CORE_MEMORY_MANAGER_HPP
#define VALKYRIE_CORE_MEMORY_MANAGER_HPP

#include <valkyrie/Core/Config.hpp>
#include <valkyrie/Core/Types.hpp>

#include <memory>
#include <memory_resource>
#include <optional>
#include <deque>
#include <bit>
#include <vector>
#include <ranges>
#include <algorithm>

namespace valkyrie::Core{
  class PageAllocator{
    u64 pageSize; // Virtual page size, need not correspond with the physical page size. Eg. on x64 Windows, this would default to 64KB, despite the physical page size being 4KB.

    class PageDescriptor{
      union{
        u32 nextAvailableIndex;
        struct {
          u32   consecutivePageCount;
          u32   nextPageIndex;
          u32   prevPageIndex;
          void* pageAddress;


        };
      };
    };


    void* addressSpaceInitial;
    u64   maxPageCount;
    u64   maxAllocationSize;
    u64   addressSpaceSize; // addressSpaceSize == (maxPageCount * pageSize)



  public:

    PageAllocator(u64 pageSize, u64 addressSpaceSize);
    ~PageAllocator();


    void*  allocate(u64 bytes);
    //void*  allocate(u64 bytes, u64 alignment);

    void deallocate(void* address, u64 bytes);

  };
}

#endif//VALKYRIE_CORE_MEMORY_MANAGER_HPP
