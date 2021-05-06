//
// Created by Maxwell on 2020-11-12.
//

#ifndef VALKYRIE_GRAPHICS_API_VULKAN_HPP
#define VALKYRIE_GRAPHICS_API_VULKAN_HPP

#include <valkyrie/adt/interval.hpp>
#include <valkyrie/graphics/api/enums.hpp>
#include <valkyrie/status/result.hpp>
#include <valkyrie/utility/shapes.hpp>

#include <concepts>
#include <tuple>

namespace valkyrie::graphics::api{

  using native_handle_t = void*;
  using api_handle_t    = const void*;

  namespace Internal{
    class QueueFamily;
  }

  enum class vulkan_status_code : u32{
    Success = 0
  };

  class vulkan_status_domain final : public status_domain{
    inline static vulkan_status_code enumValue(const status_code<void>& status) noexcept {
      return static_cast<const status_code<vulkan_status_domain>&>(status).value();
      //VK_assert();
    }
  public:
    using value_type = vulkan_status_code;

    constexpr vulkan_status_domain() noexcept : status_domain(uuid("8ad355cf-d68b-4712-beb6-7e0938c9f835")){}


    string_ref name() const noexcept override{
      return VK_string("Vulkan api");
    }

    code do_generic_code(const status_code<void> &status) const noexcept override {}
    string_ref do_message(const status_code<void> &status) const noexcept override {}
    bool do_failure(const status_code<void> &status) const noexcept override {}
    severity do_severity(const status_code<void> &status) const noexcept override {}
    bool do_equivalent(const status_code<void> &statusA, const status_code<void> &statusB) const noexcept override {}

    inline static const vulkan_status_domain& get() noexcept {
      constexpr static vulkan_status_domain domainInstance{};
      return domainInstance;
    }
  };

  using status = status_code<vulkan_status_domain>;
  using error  = error_code<vulkan_status_domain>;
  template <typename T, typename NoValuePolicy = void>
  using basic_result = valkyrie::basic_result<T, vulkan_status_domain, NoValuePolicy>;
  template <typename T>
  using result = basic_result<T>;


  class instance;
  class physical_device;
  class device;
  class queue;
  class semaphore;
  class command_buffer;
  class fence;
  class device_memory;
  class buffer;
  class image;
  class event;
  class query_pool;
  class buffer_view;
  class image_view;
  class shader_module;
  class pipeline_cache;
  class pipeline_layout;
  class render_pass;
  class pipeline;
  class descriptor_set_layout;
  class sampler;
  class descriptor_pool;
  class descriptor_set;
  class framebuffer;
  class command_pool;
  class sampler_ycbcr_conversion;
  class descriptor_update_template;
  class surface;
  class swapchain;
  class display;
  class display_mode;
  class debug_report_callback;
  class debug_utils_messenger;
  class acceleration_structure;
  class validation_cache;
  class performance_configuration;
  class deferred_operation;
  class indirect_commands_layout;
  class private_data_slot;

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
