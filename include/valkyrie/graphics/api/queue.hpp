//
// Created by Maxwell on 2020-11-09.
//

#ifndef VALKYRIE_GRAPHICS_API_QUEUE_HPP
#define VALKYRIE_GRAPHICS_API_QUEUE_HPP

#include <valkyrie/adt/array_ref.hpp>
#include <valkyrie/adt/interval.hpp>
#include <valkyrie/graphics/api/vulkan.hpp>
#include <valkyrie/utility/bitflag.hpp>


namespace valkyrie::graphics::api{

  enum class Priority{
    Low,
    Default,
    High,
    Realtime
  };


  class queue : public VulkanObject{
  protected:
    device * pDevice;
  };
  class graphics_queue : public queue {};
  class compute_queue : public queue {};
  class transfer_queue : public queue {};
  class sparse_binding_queue : public queue {};


  class DeviceQueueGlobalPriority : public InputStruct{
  public:
    enum Level{
      Low = 128,
      Medium = 256,
      High = 512,
      Realtime = 1024
    };
  private:
    Level level;
  public:
    constexpr DeviceQueueGlobalPriority(Level level) noexcept : InputStruct((StructureType)1000174000), level(level){}

    constexpr DeviceQueueGlobalPriority& setPriority(Level level) noexcept {
      this->level = level;
      return *this;
    }
  };
  class DeviceQueueCreateInfo : public InputStruct{
    const u32 flags = 0; // Protected memory...
    u32 queueFamilyIndex;
    u32 queueCount = 0;
    const float* pQueuePriorities = nullptr;
  public:
    constexpr DeviceQueueCreateInfo(u32 queueFamily) noexcept
        : InputStruct((StructureType)2),
          queueFamilyIndex(queueFamily){}
    constexpr DeviceQueueCreateInfo& setPriorities(span<float> priorities) noexcept {
      queueCount = priorities.size();
      pQueuePriorities = priorities.data();
      return *this;
    }
  };


}

#endif//VALKYRIE_GRAPHICS_API_QUEUE_HPP
