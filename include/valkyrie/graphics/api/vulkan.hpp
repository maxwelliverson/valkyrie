//
// Created by Maxwell on 2020-11-12.
//

#ifndef VALKYRIE_GRAPHICS_API_VULKAN_HPP
#define VALKYRIE_GRAPHICS_API_VULKAN_HPP

#include <valkyrie/graphics/api/enums.hpp>
#include <valkyrie/utility/interval.hpp>
#include <valkyrie/utility/shapes.hpp>
#include <valkyrie/status/maybe.hpp>

#include <concepts>
#include <tuple>

namespace valkyrie::graphics::api{

  using native_handle_t = void*;
  using api_handle_t    = const void*;

  namespace Internal{
    class QueueFamily;
  }

  enum class status_code : u32{
    Success = 0
  };

  class VulkanStatusDomain : public status_domain{
    inline static status_code enumValue(const status_code<void>& status) noexcept {
      //VK_assert();
    }
  public:
    using value_type = status_code;

    constexpr VulkanStatusDomain() noexcept : status_domain(uuid("8ad355cf-d68b-4712-beb6-7e0938c9f835")){}


    string_ref name() const noexcept override{
      return VK_string("Vulkan api");
    }

    code doCode(const status_code<void> &status) const noexcept override {}
    string_ref doMessage(const status_code<void> &status) const noexcept override {}
    bool doFailure(const status_code<void> &status) const noexcept override {}
    severity doSeverity(const status_code<void> &status) const noexcept override {}
    bool doEquivalent(const status_code<void> &statusA, const status_code<void> &statusB) const noexcept override {}

    inline static const VulkanStatusDomain& get() noexcept {
      constexpr static VulkanStatusDomain domainInstance{};
      return domainInstance;
    }
  };

  using Status = status_code<VulkanStatusDomain>;
  using Error  = error_code<VulkanStatusDomain>;
  template <typename T>
  using Result = maybe<T, VulkanStatusDomain>;

  class instance;
  class physical_device;
  class device;
  class queue;
  class semaphore;
  class command_buffer;
  class Fence;
  class device_memory;
  class buffer;
  class image;
  class event;
  class QueryPool;
  class BufferView;
  class ImageView;
  class shader_module;
  class PipelineCache;
  class PipelineLayout;
  class render_pass;
  class pipeline;
  class DescriptorSetLayout;
  class Sampler;
  class DescriptorPool;
  class DescriptorSet;
  class Framebuffer;
  class command_pool;
  class SamplerYcbcrConversion;
  class DescriptorUpdateTemplate;
  class surface;
  class Swapchain;
  class display;
  class DisplayMode;
  class DebugReportCallback;
  class DebugUtilsMessenger;
  class acceleration_structure;
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
  struct Viewport : Box2D<f32>{
    interval<f32> depth;
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
  struct SampleLocation : Position2D<f32>{};
  struct CoarseSampleLocation{
    Position2D<u32> pixel;
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
