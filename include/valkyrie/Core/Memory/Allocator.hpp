//
// Created by Maxwell on 2020-11-09.
//

#ifndef VALKYRIE_ALLOCATOR_HPP
#define VALKYRIE_ALLOCATOR_HPP

#include <valkyrie/Core/Config.hpp>
#include <valkyrie/Core/Error/Maybe.hpp>
#include <valkyrie/Core/Memory/Address.hpp>

namespace valkyrie::Core{
  class MemoryResource;
  template <typename T>
  class MemoryAllocator;
  template <typename T>
  class MemoryAllocation;


  template <typename T>
  class MemoryPlaceholder;



  struct MapParameters{};
  struct ReserveParameters{};
  struct CommitParameters{};
  struct AllocateParameters{};

  struct ReallocateParameters{};

  struct UnmapParameters{};
  struct ReleaseParameters{};
  struct DecommitParameters{};
  struct DeallocateParameters{};
}

#endif//VALKYRIE_ALLOCATOR_HPP
