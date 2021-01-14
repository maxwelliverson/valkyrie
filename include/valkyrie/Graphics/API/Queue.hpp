//
// Created by Maxwell on 2020-11-09.
//

#ifndef VALKYRIE_QUEUE_HPP
#define VALKYRIE_QUEUE_HPP

#include <valkyrie/Graphics/API/Vulkan.hpp>
#include <valkyrie/Core/ADT/ArrayRef.hpp.hpp>
#include <valkyrie/Core/Utility/BitFlags.hpp>
#include <valkyrie/Core/Utility/Interval.hpp>


namespace valkyrie::Graphics::API{

  enum class Priority{
    Low,
    Default,
    High,
    Realtime
  };


  class Queue : public VulkanObject{
  protected:
    Device* pDevice;
  };
  class GraphicsQueue : public Queue{};
  class ComputeQueue : public Queue{};
  class TransferQueue : public Queue{};
  class SparseBindingQueue : public Queue{};


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
    constexpr DeviceQueueCreateInfo& setPriorities(Core::Span<float> priorities) noexcept {
      queueCount = priorities.size();
      pQueuePriorities = priorities.data();
      return *this;
    }
  };


}

#endif//VALKYRIE_QUEUE_HPP
