//
// Created by Maxwell on 2020-11-12.
//

#ifndef VALKYRIE_GRAPHICS_API_VULKAN_HPP
#define VALKYRIE_GRAPHICS_API_VULKAN_HPP

#include <valkyrie/Core/Error/Maybe.hpp>
#include <valkyrie/Graphics/API/Enums.hpp>
#include <valkyrie/Core/Utility/Shapes.hpp>
#include <valkyrie/Core/Utility/Interval.hpp>

#include <concepts>
#include <tuple>

namespace valkyrie::Graphics::API{

  using native_handle_t = void*;
  using api_handle_t    = const void*;

  namespace Internal{
    class QueueFamily;
  }

  enum class StatusCode : u32{
    Success = 0
  };

  class VulkanStatusDomain : public Core::StatusDomain{
    inline static StatusCode enumValue(const Core::StatusCode<void>& status) noexcept {
      //VK_assert();
    }
  public:
    using value_type = StatusCode;

    constexpr VulkanStatusDomain() noexcept : Core::StatusDomain(Core::Uuid("8ad355cf-d68b-4712-beb6-7e0938c9f835")){}


    string_ref name() const noexcept override{
      return VK_string("Vulkan API");
    }

    Core::Code doCode(const Core::StatusCode<void> &status) const noexcept override {}
    Core::StringRef doMessage(const Core::StatusCode<void> &status) const noexcept override {}
    bool doFailure(const Core::StatusCode<void> &status) const noexcept override {}
    Core::Severity doSeverity(const Core::StatusCode<void> &status) const noexcept override {}
    bool doEquivalent(const Core::StatusCode<void> &statusA, const Core::StatusCode<void> &statusB) const noexcept override {}

    inline static const VulkanStatusDomain& get() noexcept {
      constexpr static VulkanStatusDomain domainInstance{};
      return domainInstance;
    }
  };

  using Status = Core::StatusCode<VulkanStatusDomain>;
  using Error  = Core::ErrorCode<VulkanStatusDomain>;
  template <typename T>
  using Result = Core::Maybe<T, VulkanStatusDomain>;

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
  struct Viewport : Core::Box2D<f32>{
    Core::Interval<f32> depth;
  };
  struct StencilOpState{
    StencilOp failOp;
    StencilOp passOp;
    StencilOp depthFailOp;
    CompareOp compareOp;
    u32       compareMask;
    u32       writeMask;
    u32       reference;
  };
  struct ShadingRatePalette{
    u32                            entryCount;
    const ShadingRatePaletteEntry* pEntries;
  };
  struct SampleLocation : Core::Position2D<f32>{};
  struct CoarseSampleLocation{
    Core::Position2D<u32> pixel;
    u32 sample;
  };
  struct CoarseSampleOrderCustom {
    ShadingRatePaletteEntry     shadingRate;
    u32                         sampleCount;
    u32                         sampleLocationCount;
    const CoarseSampleLocation* pSampleLocations;
  };
}



#endif//VALKYRIE_GRAPHICS_API_VULKAN_HPP
