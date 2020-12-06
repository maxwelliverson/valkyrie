//
// Created by Maxwell on 2020-11-12.
//

#ifndef VALKYRIE_DEVICE_MEMORY_HPP
#define VALKYRIE_DEVICE_MEMORY_HPP

#include <valkyrie/Graphics/API/Vulkan.hpp>

namespace valkyrie::Graphics::API{
  class DeviceMemory : public VulkanType{};
  class MemoryHeap{};

  enum class MemoryProperty{
    DeviceLocal = 0x1,
    HostVisible = 0x2,
    HostCoherent = 0x4,
    HostCached   = 0x8,
    LazilyAllocated = 0x10,
    Protected    = 0x20,
    DeviceCoherent = 0x40,
    DeviceUncached = 0x80
  };
  enum class HeapKind{
    Unknown = 0x0,
    DeviceLocal = 0x1,
    Communal = 0x6,
    CommunalCaching = 0xA,
    FastCommunal = 0xE,
    Global = 0x7,
    GlobalCaching = 0xB,
    Universal = 0xF,
    Lazy = 0x11,
    Protected = 0x20,
    LocalProtected = 0x21
  };

}

#endif//VALKYRIE_DEVICE_MEMORY_HPP
