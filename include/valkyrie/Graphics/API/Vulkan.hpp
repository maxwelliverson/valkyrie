//
// Created by Maxwell on 2020-11-12.
//

#ifndef VALKYRIE_GRAPHICS_API_VULKAN_HPP
#define VALKYRIE_GRAPHICS_API_VULKAN_HPP

#include <valkyrie/Core/Types.hpp>
#include <valkyrie/Graphics/API/Enums.hpp>
#include <valkyrie/Core/Utility/Shapes.hpp>

#include <concepts>
#include <tuple>

namespace valkyrie::Graphics::API{

  using native_handle_t = void*;
  using api_handle_t    = const void*;

  namespace Internal{
    class QueueFamily;
  }

  class Instance;
  class PhysicalDevice;
  class Device;
  class Queue;
  class Semaphore;
  class CommandBuffer;
  class Fence;
  class DeviceMemory;
  class Buffer;
  class Image;
  class Event;
  class QueryPool;
  class BufferView;
  class ImageView;
  class ShaderModule;
  class PipelineCache;
  class PipelineLayout;
  class RenderPass;
  class Pipeline;
  class DescriptorSetLayout;
  class Sampler;
  class DescriptorPool;
  class DescriptorSet;
  class Framebuffer;
  class CommandPool;
  class SamplerYcbcrConversion;
  class DescriptorUpdateTemplate;
  class Surface;
  class Swapchain;
  class Display;
  class DisplayMode;
  class DebugReportCallback;
  class DebugUtilsMessenger;
  class AccelerationStructure;
  class ValidationCache;
  class PerformanceConfiguration;
  class DeferredOperation;
  class IndirectCommandsLayout;
  class PrivateDataSlot;

  class VulkanType{};

  class VulkanObject : public VulkanType{};
  class VulkanResource : public VulkanType{};

  
  class VulkanStruct{
    StructureType sType;
  protected:
    constexpr VulkanStruct(StructureType sType) noexcept : sType(sType){}
  };
  class InputStruct : public VulkanStruct{
    const InputStruct* pNext = nullptr;
  protected:
    constexpr InputStruct(StructureType structureType) noexcept : VulkanStruct(structureType){}
  public:
    constexpr void link(const InputStruct& next) noexcept {
      pNext = &next;
    }
  };
  class OutputStruct : public VulkanStruct{
    OutputStruct* pNext = nullptr;
  protected:
    constexpr OutputStruct(StructureType structureType) noexcept : VulkanStruct(structureType){}
  public:
    constexpr void link(OutputStruct& next) noexcept {
      pNext = &next;
    }
  };


  struct ComponentMapping{
    ComponentSwizzle r = ComponentSwizzle::Identity;
    ComponentSwizzle g = ComponentSwizzle::Identity;
    ComponentSwizzle b = ComponentSwizzle::Identity;
    ComponentSwizzle a = ComponentSwizzle::Identity;
  };
}



#endif//VALKYRIE_GRAPHICS_API_VULKAN_HPP
