//
// Created by Maxwell on 2021-02-22.
//

#include <valkyrie/Core/Memory/Manager.hpp>


#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <memoryapi.h>


namespace valk = valkyrie::Core;


valk::PageAllocator::PageAllocator(u64 pageSize, u64 addressSpaceSize)
    : pageSize(pageSize),
      addressSpaceInitial(nullptr),
      maxPageCount(addressSpaceSize / pageSize),
      maxAllocationSize(),
      addressSpaceSize(addressSpaceSize) {

  MEM_ADDRESS_REQUIREMENTS addrReq{
      .LowestStartingAddress = nullptr,
      .HighestEndingAddress  = nullptr,
      .Alignment             = addressSpaceSize
  };
  MEM_EXTENDED_PARAMETER extParam{
      .Type    = MemExtendedParameterAddressRequirements,
      .Pointer = &addrReq
  };

  void* result = VirtualAlloc2(GetCurrentProcess(),
                               nullptr,
                               addressSpaceSize,
                               MEM_RESERVE | MEM_RESERVE_PLACEHOLDER,
                               PAGE_NOACCESS,
                               &extParam,
                               1);



}

valk::PageAllocator::~PageAllocator() {}
void* valk::PageAllocator::allocate(u64 bytes){}
void  valk::PageAllocator::deallocate(void *address, u64 bytes) {}