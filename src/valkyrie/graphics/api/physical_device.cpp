//
// Created by Maxwell on 2020-11-09.
//



/*#include <valkyrie/graphics/api/Instance.hpp>
#include <valkyrie/graphics/api/PhysicalDevice.hpp>

#include "Extensions.hpp"
#include "FunctionPointers.hpp"
#include "StatusWrappers.hpp"*/


//#include <boost/static_string.hpp>

#include <valkyrie/graphics/api/physical_device.hpp>

#include "internal_structures.hpp"


#define VK_alloca(type, count) (type*)alloca(count * sizeof(type))
#define VK_malloc(type, count) (type*)std::malloc(count * sizeof(type))
#define VK_free(val) std::free(val)

#define VK_safe_call(...) if ((result = __VA_ARGS__)) VK_throw()


#define VK_try vulkan_status result; do
#define VK_catch while(false); VK_catch_label: if (result.value())
#define VK_handle(Val_) if (result.value() == (Val_))
#define VK_throw(...) VK_if(VK_not(VK_pack_is_empty(__VA_ARGS__))((result = __VA_ARGS__;))) goto VK_catch_label
#define VK_rethrow while(false); VK_catch_label: return std::move(result)
#define VK_return return std::move(result)


namespace valkyrie::graphics::api{
  namespace internal{
    namespace {
      inline bool supports(const physical_device_impl &impl, DeviceExtensionID id) noexcept {
        return impl.extensions.contains(id);
      }
      void lazyLoadQueueFamiliesDefault(physical_device_impl &impl) noexcept {
        u32 queueFamilySize;
        static_vector<VkQueueFamilyProperties2, 8> queueFamiliesTmp;
        impl.pInterface->getQueueFamilyProperties(impl.handle, &queueFamilySize, nullptr);
        queueFamiliesTmp.resize(queueFamilySize);
        for (u32 i = 0; i < queueFamilySize; ++i) {
          queueFamiliesTmp[i].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
          queueFamiliesTmp[i].pNext = nullptr;
        }
        impl.pInterface->getQueueFamilyProperties(impl.handle, &queueFamilySize, queueFamiliesTmp.data());
        for (u32 i = 0; i < queueFamilySize; ++i)
          impl.queueFamilies.emplace_back(queueFamiliesTmp[i].queueFamilyProperties);
      }
      void lazyLoadQueueFamiliesWithCheckpoints(physical_device_impl &impl) noexcept {
        u32 queueFamilySize;
        static_vector<VkQueueFamilyCheckpointPropertiesNV, 8> checkpointPropertiesTmp;
        static_vector<VkQueueFamilyProperties2, 8> queueFamiliesTmp;
        impl.pInterface->getQueueFamilyProperties(impl.handle, &queueFamilySize, nullptr);
        queueFamiliesTmp.resize(queueFamilySize);
        checkpointPropertiesTmp.resize(queueFamilySize);
        for (u32 i = 0; i < queueFamilySize; ++i) {
          checkpointPropertiesTmp[i].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_CHECKPOINT_PROPERTIES_NV;
          checkpointPropertiesTmp[i].pNext = nullptr;
          queueFamiliesTmp[i].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
          queueFamiliesTmp[i].pNext = &checkpointPropertiesTmp[i];
        }
        impl.pInterface->getQueueFamilyProperties(impl.handle, &queueFamilySize, queueFamiliesTmp.data());
        for (u32 i = 0; i < queueFamilySize; ++i)
          impl.queueFamilies.emplace_back(queueFamiliesTmp[i].queueFamilyProperties, checkpointPropertiesTmp[i]);
      }
      vulkan_status lazyLoadPerformanceCounters(physical_device_impl &impl) noexcept {
        VkPerformanceCounterKHR *counters;
        VkPerformanceCounterDescriptionKHR *descriptions;
        i32 i = 0;

        VK_try {
          for (; i < impl.queueFamilies.size(); ++i) {
            auto &family = impl.queueFamilies[i];
            u32 counterCount = 0;
            VK_safe_call(impl.pInterface->enumerateQueueFamilyPerformanceQueryCounters(impl.handle, i, &counterCount, nullptr, nullptr));
            counters = VK_malloc(VkPerformanceCounterKHR, counterCount);
            descriptions = VK_malloc(VkPerformanceCounterDescriptionKHR, counterCount);
            VK_safe_call(impl.pInterface->enumerateQueueFamilyPerformanceQueryCounters(impl.handle, i, &counterCount, counters, descriptions));
            for (u32 j = 0; i < counterCount; ++j)
              family.performanceCounters.emplace_back(counters[j], descriptions[j]);
            VK_free(counters);
            VK_free(descriptions);
          }
        }
        VK_catch {
          VK_free(counters);
          VK_free(descriptions);
          for (; i >= 0; --i)
            impl.queueFamilies[i].performanceCounters.clear();
        }
        VK_return;
      }
      vulkan_status lazyLoadExtensions(physical_device_impl &impl) noexcept {
        VK_try {
          if (!impl.extensionsLoaded) {
            vector<VkExtensionProperties> deviceExtensions;
            u32 extSize;
            VK_safe_call(make_status_code(impl.pInterface->enumerateDeviceExtensionProperties(impl.handle, nullptr, &extSize, nullptr)));
            deviceExtensions.resize(extSize);
            VK_safe_call(make_status_code(impl.pInterface->enumerateDeviceExtensionProperties(impl.handle, nullptr, &extSize, deviceExtensions.data())));

            impl.extensions.insert(deviceExtensions);


            for (auto &&layer : impl.pInstance->layers) {
              deviceExtensions.clear();
              VK_safe_call(make_status_code(impl.pInterface->enumerateDeviceExtensionProperties(impl.handle, layer.name.data(), &extSize, nullptr)));
              deviceExtensions.resize(extSize);
              VK_safe_call(make_status_code(impl.pInterface->enumerateDeviceExtensionProperties(impl.handle, layer.name.data(), &extSize, deviceExtensions.data())));
              impl.extensions.insert(deviceExtensions);
            }
            impl.extensionsLoaded = true;
          }
        }
        VK_rethrow;
      }
      void lazyLoadFeatures(physical_device_impl &impl) noexcept {
        if (!impl.featuresLoaded) {
          impl.pInterface->getFeatures(impl.handle, &impl.availableFeatures.vulkan10);
          impl.featuresLoaded = true;
        }
      }
      void lazyLoadProperties(physical_device_impl &impl) noexcept {
        if (!impl.propertiesLoaded) {
          impl.pInterface->getProperties(impl.handle, &impl.properties.vulkan10);
          impl.propertiesLoaded = true;
        }
      }
      vulkan_status lazyLoadQueueFamilies(physical_device_impl &impl) noexcept {
        VK_try {
          if (!impl.queueFamiliesLoaded) {
            VK_safe_call(lazyLoadExtensions(impl));
            if (supports(impl, DeviceExtensionID::NV_device_diagnostic_checkpoints))
              lazyLoadQueueFamiliesWithCheckpoints(impl);
            else
              lazyLoadQueueFamiliesDefault(impl);
            if (supports(impl, DeviceExtensionID::KHR_performance_query)) {
              VK_safe_call(lazyLoadPerformanceCounters(impl));
            }
            impl.queueFamiliesLoaded = true;
          }
        }
        VK_rethrow;
      }
      vulkan_status eagerLoad(physical_device_impl &impl) noexcept {
        VK_try {
          VK_safe_call(lazyLoadExtensions(impl));
          lazyLoadFeatures(impl);
          lazyLoadProperties(impl);
          VK_safe_call(lazyLoadQueueFamilies(impl));
        }
        VK_rethrow;
      }

      PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR cap;
      PFN_vkGetPhysicalDeviceSurfaceFormats2KHR form;
      PFN_vkGetPhysicalDeviceSurfacePresentModes2EXT present;
      PFN_vkGetPhysicalDevicePresentRectanglesKHR rect;
      PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR win32;
      PFN_vkGetPhysicalDeviceSurfaceSupportKHR surfaceSupport;

      PFN_vkGetPhysicalDeviceMultisamplePropertiesEXT multi;
    }
  }

  class physical_device::Impl : public internal::physical_device_impl{};





}




/*using namespace valkyrie::graphics::api;
using valkyrie::u32;
namespace Core = valkyrie;
namespace {

  using VulkanString = boost::static_string<VK_MAX_DESCRIPTION_SIZE>;

  enum class QueueFlagBits{
    Graphics = 0x1,
    Compute  = 0x2,
    Transfer = 0x4,
    SparseBinding = 0x8,
    Protected = 0x10
  };

  using QueueFlags = bitflag<QueueFlagBits>;
  using PipelineStageFlags = bitflag<VkPipelineStageFlagBits>;

  enum class PerformanceCounterFlagBits{
    PerformanceImpacting = 0x00000001,
    ConcurrentlyImpacted = 0x00000002,
  };
  using PerformanceCounterFlags = bitflag<PerformanceCounterFlagBits>;


  struct PerformanceCounter{

    inline constexpr static PerformanceCounterFlags PerformanceImpacting{PerformanceCounterFlagBits::PerformanceImpacting};
    inline constexpr static PerformanceCounterFlags ConcurrentlyImpacted{PerformanceCounterFlagBits::ConcurrentlyImpacted};

    VkPerformanceCounterKHR handle;
    PerformanceCounterFlags flags;
    VulkanString name;
    VulkanString category;
    VulkanString description;

    PerformanceCounter(VkPerformanceCounterKHR counter, VkPerformanceCounterDescriptionKHR desc) noexcept
        : handle(counter),
          flags(desc.flags),
          name(desc.name),
          category(desc.category),
          description(desc.description){}
  };

  struct QueueFamily{
    QueueFlags             queueFlags;
    u32                    queueCount;
    u32                    timestampValidBits;
    Extent3D<MinU32> imageTransferGranularity;
    PipelineStageFlags     checkpointExecutionStageMask;
    small_vector<PerformanceCounter> performanceCounters{};

    explicit QueueFamily(const VkQueueFamilyProperties& props) noexcept
        : queueFlags(props.queueFlags),
          queueCount(props.queueCount),
          timestampValidBits(props.timestampValidBits),
          imageTransferGranularity{ props.minImageTransferGranularity.width,
                                    props.minImageTransferGranularity.height,
                                    props.minImageTransferGranularity.depth },
          checkpointExecutionStageMask{}{}
    QueueFamily(const VkQueueFamilyProperties& props, const VkQueueFamilyCheckpointPropertiesNV& checkpointProps) noexcept
        : queueFlags(props.queueFlags),
          queueCount(props.queueCount),
          timestampValidBits(props.timestampValidBits),
          imageTransferGranularity{ props.minImageTransferGranularity.width,
                                    props.minImageTransferGranularity.height,
                                    props.minImageTransferGranularity.depth },
          checkpointExecutionStageMask(checkpointProps.checkpointExecutionStageMask){}
  };

  struct PhysicalDeviceFeatures{
    VkPhysicalDevicePortabilitySubsetFeaturesKHR portabilitySubset{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR,
        .pNext = nullptr
    };
    VkPhysicalDeviceRayTracingFeaturesKHR rayTracing{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_FEATURES_KHR,
        .pNext = &portabilitySubset
    };
    VkPhysicalDeviceCornerSampledImageFeaturesNV cornerSampledImage{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CORNER_SAMPLED_IMAGE_FEATURES_NV,
        .pNext = &rayTracing
    };
    VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT textureCompressionAstchdr{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXTURE_COMPRESSION_ASTC_HDR_FEATURES_EXT,
        .pNext = &cornerSampledImage
    };
    VkPhysicalDeviceASTCDecodeFeaturesEXT astcDecode{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ASTC_DECODE_FEATURES_EXT,
        .pNext = &textureCompressionAstchdr
    };
    VkPhysicalDeviceConditionalRenderingFeaturesEXT conditionalRendering{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CONDITIONAL_RENDERING_FEATURES_EXT,
        .pNext = &astcDecode
    };
    VkPhysicalDeviceDepthClipEnableFeaturesEXT depthClipEnable{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_ENABLE_FEATURES_EXT,
        .pNext = &conditionalRendering
    };
    VkPhysicalDeviceInlineUniformBlockFeaturesEXT inlineUniformBlock{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES_EXT,
        .pNext = &depthClipEnable
    };
    VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT blendOperationAdvanced{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_FEATURES_EXT,
        .pNext = &inlineUniformBlock
    };
    VkPhysicalDeviceShaderSMBuiltinsFeaturesNV shaderSmBuiltins{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SM_BUILTINS_FEATURES_NV,
        .pNext = &blendOperationAdvanced
    };
    VkPhysicalDeviceShadingRateImageFeaturesNV shadingRateImage{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADING_RATE_IMAGE_FEATURES_NV,
        .pNext = &shaderSmBuiltins
    };
    VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV representativeFragmentTest{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_REPRESENTATIVE_FRAGMENT_TEST_FEATURES_NV,
        .pNext = &shadingRateImage
    };
    VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT vertexAttributeDivisor{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_FEATURES_EXT,
        .pNext = &representativeFragmentTest
    };
    VkPhysicalDeviceMeshShaderFeaturesNV meshShader{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_NV,
        .pNext = &vertexAttributeDivisor
    };
    VkPhysicalDeviceFragmentShaderBarycentricFeaturesNV fragmentShaderBarycentric{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_BARYCENTRIC_FEATURES_NV,
        .pNext = &meshShader
    };
    VkPhysicalDeviceShaderImageFootprintFeaturesNV shaderImageFootprint{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_IMAGE_FOOTPRINT_FEATURES_NV,
        .pNext = &fragmentShaderBarycentric
    };
    VkPhysicalDeviceExclusiveScissorFeaturesNV exclusiveScissor{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXCLUSIVE_SCISSOR_FEATURES_NV,
        .pNext = &shaderImageFootprint
    };
    VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL shaderIntegerFunctions2{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_FUNCTIONS_2_FEATURES_INTEL,
        .pNext = &exclusiveScissor
    };
    VkPhysicalDevice4444FormatsFeaturesEXT formats4444Ext{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_4444_FORMATS_FEATURES_EXT,
        .pNext = &shaderIntegerFunctions2
    };
    VkPhysicalDeviceRobustness2FeaturesEXT robustness2{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT,
        .pNext = &formats4444Ext
    };
    VkPhysicalDeviceImageRobustnessFeaturesEXT imageRobustness{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_ROBUSTNESS_FEATURES_EXT,
        .pNext = &robustness2
    };
    VkPhysicalDeviceDiagnosticsConfigFeaturesNV diagnosticsConfig{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DIAGNOSTICS_CONFIG_FEATURES_NV,
        .pNext = &imageRobustness
    };
    VkPhysicalDevicePrivateDataFeaturesEXT privateData{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIVATE_DATA_FEATURES_EXT,
        .pNext = &diagnosticsConfig
    };
    VkPhysicalDeviceCustomBorderColorFeaturesEXT customBorderColor{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_BORDER_COLOR_FEATURES_EXT,
        .pNext = &privateData
    };
    VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT texelBufferAlignment{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_FEATURES_EXT,
        .pNext = &customBorderColor
    };
    VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV deviceDeviceGeneratedCommands{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_GENERATED_COMMANDS_FEATURES_NV,
        .pNext = &texelBufferAlignment
    };
    VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT shaderDemoteToHelperInvocation{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DEMOTE_TO_HELPER_INVOCATION_FEATURES_EXT,
        .pNext = &deviceDeviceGeneratedCommands
    };
    VkPhysicalDeviceExtendedDynamicStateFeaturesEXT extendedDynamicState{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT,
        .pNext = &shaderDemoteToHelperInvocation
    };
    VkPhysicalDeviceIndexTypeUint8FeaturesEXT indexTypeUint8{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INDEX_TYPE_UINT8_FEATURES_EXT,
        .pNext = &extendedDynamicState
    };
    VkPhysicalDeviceShaderAtomicFloatFeaturesEXT shaderAtomicFloat{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_FEATURES_EXT,
        .pNext = &indexTypeUint8
    };
    VkPhysicalDeviceYcbcrImageArraysFeaturesEXT ycbcrImageArrays{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_IMAGE_ARRAYS_FEATURES_EXT,
        .pNext = &shaderAtomicFloat
    };
    VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT fragmentShaderInterlock{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_INTERLOCK_FEATURES_EXT,
        .pNext = &ycbcrImageArrays
    };
    VkPhysicalDeviceCoverageReductionModeFeaturesNV coverageReductionMode{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COVERAGE_REDUCTION_MODE_FEATURES_NV,
        .pNext = &fragmentShaderInterlock
    };
    VkPhysicalDeviceCooperativeMatrixFeaturesNV cooperativeMatrix{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_FEATURES_NV,
        .pNext = &coverageReductionMode
    };
    VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV dedicatedAllocationImageAliasing{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEDICATED_ALLOCATION_IMAGE_ALIASING_FEATURES_NV,
        .pNext = &cooperativeMatrix
    };
    VkPhysicalDeviceSubgroupSizeControlFeaturesEXT subgroupSizeControl{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_FEATURES_EXT,
        .pNext = &dedicatedAllocationImageAliasing
    };
    VkPhysicalDeviceFragmentDensityMap2FeaturesEXT fragmentDensityMap2{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_2_FEATURES_EXT,
        .pNext = &subgroupSizeControl
    };
    VkPhysicalDeviceFragmentDensityMapFeaturesEXT fragmentDensityMap{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_FEATURES_EXT,
        .pNext = &fragmentDensityMap2
    };
    VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT pipelineCreationCacheControl{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_CREATION_CACHE_CONTROL_FEATURES_EXT,
        .pNext = &fragmentDensityMap
    };
    VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR pipelineExecutableProperties{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_EXECUTABLE_PROPERTIES_FEATURES_KHR,
        .pNext = &pipelineCreationCacheControl
    };
    VkPhysicalDeviceLineRasterizationFeaturesEXT lineRasterization{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_FEATURES_EXT,
        .pNext = &pipelineExecutableProperties
    };
    VkPhysicalDeviceMemoryPriorityFeaturesEXT memoryPriority{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PRIORITY_FEATURES_EXT,
        .pNext = &lineRasterization
    };
    VkPhysicalDeviceTransformFeedbackFeaturesEXT transformFeedback{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_FEATURES_EXT,
        .pNext = &memoryPriority
    };
    VkPhysicalDevicePerformanceQueryFeaturesKHR performanceQuery{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_QUERY_FEATURES_KHR,
        .pNext = &transformFeedback
    };
    VkPhysicalDeviceShaderClockFeaturesKHR shaderClock{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CLOCK_FEATURES_KHR,
        .pNext = &performanceQuery
    };
    VkPhysicalDeviceVulkan12Features vulkan12{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
        .pNext = &shaderClock
    };
    VkPhysicalDeviceVulkan11Features vulkan11{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES,
        .pNext = &vulkan12
    };
    VkPhysicalDeviceFeatures2 vulkan10{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
        .pNext = &vulkan11
    };
  };
  struct PhysicalDeviceProperties{
    VkPhysicalDevicePortabilitySubsetPropertiesKHR portabilitySubset{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_PROPERTIES_KHR,
        .pNext = nullptr
    };
    VkPhysicalDeviceGroupProperties group{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES,
        .pNext = &portabilitySubset
    };
    VkPhysicalDevicePushDescriptorPropertiesKHR pushDescriptor{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PUSH_DESCRIPTOR_PROPERTIES_KHR,
        .pNext = &group
    };
    VkPhysicalDevicePerformanceQueryPropertiesKHR performanceQuery{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_QUERY_PROPERTIES_KHR,
        .pNext = &pushDescriptor
    };
    VkPhysicalDeviceTransformFeedbackPropertiesEXT transformFeedback{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_PROPERTIES_EXT,
        .pNext = &performanceQuery
    };
    VkPhysicalDeviceMultiviewPerViewAttributesPropertiesNVX multiviewPerViewAttributes{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PER_VIEW_ATTRIBUTES_PROPERTIES_NVX,
        .pNext = &transformFeedback
    };
    VkPhysicalDeviceDiscardRectanglePropertiesEXT discardRectangle{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DISCARD_RECTANGLE_PROPERTIES_EXT,
        .pNext = &multiviewPerViewAttributes
    };
    VkPhysicalDeviceConservativeRasterizationPropertiesEXT conservativeRasterization{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CONSERVATIVE_RASTERIZATION_PROPERTIES_EXT,
        .pNext = &discardRectangle
    };
    VkPhysicalDeviceInlineUniformBlockPropertiesEXT inlineUniformBlock{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_PROPERTIES_EXT,
        .pNext = &conservativeRasterization
    };
    VkPhysicalDeviceSampleLocationsPropertiesEXT sampleLocations{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLE_LOCATIONS_PROPERTIES_EXT,
        .pNext = &inlineUniformBlock
    };
    VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT blendOperationAdvanced{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_PROPERTIES_EXT,
        .pNext = &sampleLocations
    };
    VkPhysicalDeviceShaderSMBuiltinsPropertiesNV shaderSmBuiltins{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SM_BUILTINS_PROPERTIES_NV,
        .pNext = &blendOperationAdvanced
    };
    VkPhysicalDeviceShadingRateImagePropertiesNV shadingRateImage{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADING_RATE_IMAGE_PROPERTIES_NV,
        .pNext = &shaderSmBuiltins
    };
    VkPhysicalDeviceRayTracingPropertiesKHR rayTracingBETA{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PROPERTIES_KHR,
        .pNext = &shadingRateImage
    };
    VkPhysicalDeviceRayTracingPropertiesNV rayTracing{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PROPERTIES_NV,
        .pNext = &rayTracingBETA
    };
    VkPhysicalDeviceExternalMemoryHostPropertiesEXT externalMemoryHost{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_HOST_PROPERTIES_EXT,
        .pNext = &rayTracing
    };
    VkPhysicalDeviceShaderCoreProperties2AMD shaderCore2{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CORE_PROPERTIES_2_AMD,
        .pNext = &externalMemoryHost
    };
    VkPhysicalDeviceShaderCorePropertiesAMD shaderCore{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CORE_PROPERTIES_AMD,
        .pNext = &shaderCore2
    };
    VkPhysicalDeviceVertexAttributeDivisorPropertiesEXT vertexAttributeDivisor{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_PROPERTIES_EXT,
        .pNext = &shaderCore
    };
    VkPhysicalDeviceMeshShaderPropertiesNV meshShader{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_PROPERTIES_NV,
        .pNext = &vertexAttributeDivisor
    };
    VkPhysicalDeviceDeviceGeneratedCommandsPropertiesNV deviceGeneratedCommands{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_GENERATED_COMMANDS_PROPERTIES_NV,
        .pNext = &meshShader
    };
    VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT texelBufferAlignment{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_PROPERTIES_EXT,
        .pNext = &deviceGeneratedCommands
    };
    VkPhysicalDeviceRobustness2PropertiesEXT robustness2{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_PROPERTIES_EXT,
        .pNext = &texelBufferAlignment
    };
    VkPhysicalDeviceCustomBorderColorPropertiesEXT customBorderColor{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_BORDER_COLOR_PROPERTIES_EXT,
        .pNext = &robustness2
    };
    VkPhysicalDeviceCooperativeMatrixPropertiesNV cooperativeMatrix{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_PROPERTIES_NV,
        .pNext = &customBorderColor
    };
    VkPhysicalDeviceMemoryBudgetPropertiesEXT memoryBudget{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_BUDGET_PROPERTIES_EXT,
        .pNext = &cooperativeMatrix
    };
    VkPhysicalDeviceSubgroupSizeControlPropertiesEXT subgroupSizeControl{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_PROPERTIES_EXT,
        .pNext = &memoryBudget
    };
    VkPhysicalDeviceFragmentDensityMap2PropertiesEXT fragmentDensityMap2{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_2_PROPERTIES_EXT,
        .pNext = &subgroupSizeControl
    };
    VkPhysicalDeviceFragmentDensityMapPropertiesEXT fragmentDensityMap{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_PROPERTIES_EXT,
        .pNext = &fragmentDensityMap2
    };
    VkPhysicalDevicePCIBusInfoPropertiesEXT pciBusInfo{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PCI_BUS_INFO_PROPERTIES_EXT,
        .pNext = &fragmentDensityMap
    };
    VkPhysicalDeviceLineRasterizationPropertiesEXT lineRasterization{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_PROPERTIES_EXT,
        .pNext = &pciBusInfo
    };
    VkPhysicalDeviceMemoryProperties2       memory{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2,
        .pNext = &lineRasterization
    };
    VkPhysicalDeviceVulkan12Properties vulkan12{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES,
        .pNext = &memory
    };
    VkPhysicalDeviceVulkan11Properties vulkan11{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES,
        .pNext = &vulkan12
    };
    VkPhysicalDeviceProperties2 vulkan10{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
        .pNext = &vulkan11
    };
  };
  struct MemoryProperties{
    VkPhysicalDeviceMemoryBudgetPropertiesEXT budgetProperties{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_BUDGET_PROPERTIES_EXT,
        .pNext = nullptr
    };
    VkPhysicalDeviceMemoryProperties2 properties{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2,
        .pNext = &budgetProperties
    };
  };

  struct ExtensionInfo{
    std::string             name;
    DeviceExtensionID       id;
    valkyrie::Version specVersion;
    bool                    isEnabled;


    ExtensionInfo(const VkExtensionProperties& props) noexcept
        : name(props.extensionName, std::strlen(props.extensionName)),
          id(getDeviceExtensionID(name)),
          specVersion(props.specVersion),
          isEnabled(false){}

    friend bool operator==(const ExtensionInfo& A, DeviceExtensionID extensionId) noexcept {
      return A.id == extensionId;
    }
    friend auto operator<=>(const ExtensionInfo& A, DeviceExtensionID extensionId) noexcept {
      return A.id <=> extensionId;
    }
    friend bool operator==(const ExtensionInfo& A, const ExtensionInfo& B) noexcept {
      return A.id == B.id;
    }
    friend auto operator<=>(const ExtensionInfo& A, const ExtensionInfo& B) noexcept {
      return A.id <=> B.id;
    }
  };
}


class PhysicalDevice::Impl{
  VkPhysicalDevice handle;
  Instance instance;
  const PhysicalDeviceFunctions* pInterface;


  mutable PhysicalDeviceFeatures availableFeatures;
  PhysicalDeviceFeatures enabledFeatures;
  mutable PhysicalDeviceProperties properties;
  mutable MemoryProperties memoryProperties;

  mutable flat_set<ExtensionInfo> extensions;

  flat_set<std::string, DynamicArray<std::string>> availableExtensions;
  flat_set<string_view>                             enabledExtensions;

  mutable flat_map<VkFormat, VkFormatProperties2> formatProperties;
  mutable flat_map<u32, QueueFamily> queueFamilies;
  mutable flat_map<VkSampleCountFlagBits, VkMultisamplePropertiesEXT> multisampleProperties;
  mutable static_vector<VkTimeDomainEXT, 4> timeDomains;
  mutable DynamicArray<VkCooperativeMatrixPropertiesNV> cooperativeMatrixProperties;

  mutable bool extensionsLoaded = false;
  mutable bool featuresLoaded = false;
  mutable bool propertiesLoaded = false;
  mutable bool queueFamiliesLoaded = false;


  void lazyLoadQueueFamiliesDefault() noexcept {
    u32 queueFamilySize;
    static_vector<VkQueueFamilyProperties2, 8> queueFamiliesTmp;
    pInterface->getQueueFamilyProperties(handle, &queueFamilySize, nullptr);
    queueFamiliesTmp.resize(queueFamilySize);
    for (u32 i = 0; i < queueFamilySize; ++i) {
      queueFamiliesTmp[i].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
      queueFamiliesTmp[i].pNext = nullptr;
    }
    pInterface->getQueueFamilyProperties(handle, &queueFamilySize, queueFamiliesTmp.data());
    for (u32 i = 0; i < queueFamilySize; ++i)
      queueFamilies.emplace(i, queueFamiliesTmp[i].queueFamilyProperties);
  }
  void lazyLoadQueueFamiliesWithCheckpoints() noexcept {
    u32 queueFamilySize;
    static_vector<VkQueueFamilyCheckpointPropertiesNV, 8> checkpointPropertiesTmp;
    static_vector<VkQueueFamilyProperties2, 8> queueFamiliesTmp;
    pInterface->getQueueFamilyProperties(handle, &queueFamilySize, nullptr);
    queueFamiliesTmp.resize(queueFamilySize);
    checkpointPropertiesTmp.resize(queueFamilySize);
    for (u32 i = 0; i < queueFamilySize; ++i) {
      checkpointPropertiesTmp[i].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_CHECKPOINT_PROPERTIES_NV;
      checkpointPropertiesTmp[i].pNext = nullptr;
      queueFamiliesTmp[i].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
      queueFamiliesTmp[i].pNext = &checkpointPropertiesTmp[i];
    }
    pInterface->getQueueFamilyProperties(handle, &queueFamilySize, queueFamiliesTmp.data());
    for (u32 i = 0; i < queueFamilySize; ++i)
      queueFamilies.emplace(i, queueFamiliesTmp[i].queueFamilyProperties, checkpointPropertiesTmp[i]);
  }
  vulkan_status lazyLoadPerformanceCounters() noexcept {
    VkPerformanceCounterKHR* counters;
    VkPerformanceCounterDescriptionKHR* descriptions;
    i32 i = 0;

    VK_try{
      for (; i < queueFamilies.size(); ++i) {
        auto& family = queueFamilies[i];
        u32 counterCount = 0;
        VK_safe_call(pInterface->enumerateQueueFamilyPerformanceQueryCounters(handle, i, &counterCount, nullptr, nullptr));
        counters = VK_malloc(VkPerformanceCounterKHR, counterCount);
        descriptions = VK_malloc(VkPerformanceCounterDescriptionKHR, counterCount);
        VK_safe_call(pInterface->enumerateQueueFamilyPerformanceQueryCounters(handle, i, &counterCount, counters, descriptions));
        for (u32 j = 0; i < counterCount; ++j)
          family.performanceCounters.emplace_back(counters[j], descriptions[j]);
        VK_free(counters);
        VK_free(descriptions);
      }
    }
    VK_catch{
      VK_free(counters);
      VK_free(descriptions);
      for (;i >= 0; --i)
        queueFamilies[i].performanceCounters.clear();
    }
    VK_return;
  }

  vulkan_status lazyLoadExtensions() noexcept {
    VK_try {
      if (!extensionsLoaded) {
        DynamicArray<VkExtensionProperties> deviceExtensions;
        u32 extSize;
        VK_safe_call(pInterface->enumerateDeviceExtensionProperties(handle, nullptr, &extSize, nullptr));
        deviceExtensions.resize(extSize);
        VK_safe_call(pInterface->enumerateDeviceExtensionProperties(handle, nullptr, &extSize, deviceExtensions.data()));

        extensions.insert(deviceExtensions);



        for (auto&& layer : instance.enabledLayers()) {
          deviceExtensions.clear();
          VK_safe_call(pInterface->enumerateDeviceExtensionProperties(handle, layer.c_str(), &extSize, nullptr));
          deviceExtensions.resize(extSize);
          VK_safe_call(pInterface->enumerateDeviceExtensionProperties(handle, layer.c_str(), &extSize, deviceExtensions.data()));
          extensions.insert(deviceExtensions);
        }
        extensionsLoaded = true;
      }
    }
    VK_rethrow;
  }
  void         lazyLoadFeatures() noexcept {
    if (!featuresLoaded) {
      pInterface->getFeatures(handle, &availableFeatures.vulkan10);
      featuresLoaded = true;
    }
  }
  void         lazyLoadProperties() noexcept {
    if (!propertiesLoaded) {
      pInterface->getProperties(handle, &properties.vulkan10);
      propertiesLoaded = true;
    }

  }
  vulkan_status lazyLoadQueueFamilies() noexcept {
    VK_try{
      if (!queueFamiliesLoaded) {
        VK_safe_call(lazyLoadExtensions());
        if (supports(DeviceExtensionID::NV_device_diagnostic_checkpoints))
          lazyLoadQueueFamiliesWithCheckpoints();
        else
          lazyLoadQueueFamiliesDefault();
        if (supports(DeviceExtensionID::KHR_performance_query)) {
          VK_safe_call(lazyLoadPerformanceCounters());
        }
        queueFamiliesLoaded = true;
      }
    }
    VK_rethrow;
  }
  vulkan_status eagerLoad() noexcept {
    VK_try {
      VK_safe_call(lazyLoadExtensions());
                   lazyLoadFeatures();
                   lazyLoadProperties();
      VK_safe_call(lazyLoadQueueFamilies());
    }
    VK_rethrow;
  }

public:

  Impl(Instance inst, VkPhysicalDevice physicalDevice) noexcept
      : handle(), instance(std::move(inst)){}



  bool supports(DeviceExtensionID id) const noexcept {
    return extensions.contains(id);
  }


  const VkFormatProperties2& getFormatProperties(VkFormat format) noexcept {
    auto&& propIter = formatProperties.find(format);
    if (propIter == formatProperties.end()) {
      propIter = formatProperties.emplace(format);
      pInterface->getFormatProperties(handle, format, &propIter->value);
    }
    return propIter->value;
  }




  VkDevice makeDevice(std::span<const VkDeviceQueueCreateInfo> queueInfo) noexcept {
    DynamicArray<const char*> deviceExtensions;
    for (auto&& ext : extensions) {
      if (ext.isEnabled)
        deviceExtensions.push_back(ext.name.c_str());
    }

    uint32_t                           queueCreateInfoCount;
    const VkDeviceQueueCreateInfo*     pQueueCreateInfos;
    uint32_t                           enabledLayerCount;
    const char* const*                 ppEnabledLayerNames;
    uint32_t                           enabledExtensionCount;
    const char* const*                 ppEnabledExtensionNames;
    const VkPhysicalDeviceFeatures*    pEnabledFeatures;

    VkDeviceCreateInfo createInfo{
      .sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .pNext                   = &enabledFeatures.vulkan10,
      .queueCreateInfoCount    = (u32)queueInfo.size(),
      .pQueueCreateInfos       = queueInfo.data(),
      .enabledLayerCount       = 0,
      .ppEnabledLayerNames     = nullptr,
      .enabledExtensionCount   = (u32)deviceExtensions.size(),
      .ppEnabledExtensionNames = deviceExtensions.data(),
      .pEnabledFeatures        = nullptr
    };

    VkDevice deviceHandle;
    VkResult result;

    result = pInterface->createDevice(handle, &createInfo, nullptr, &deviceHandle);
  }
};*/



/*VK_function(GetPhysicalDeviceFormatProperties2,      getFormatProperties);
  VK_function(GetPhysicalDeviceImageFormatProperties2, getImageFormatProperties);
  VK_function(GetPhysicalDeviceSparseImageFormatProperties2, getSparseImageFormatProperties);

  VK_function(GetPhysicalDeviceQueueFamilyProperties2, getQueueFamilyProperties);
  VK_function(EnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR, enumerateQueueFamilyPerformanceQueryCounters);
  VK_function(GetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR, getQueueFamilyPerformanceQueryPasses);

  VK_function(GetPhysicalDeviceMemoryProperties2, getMemoryProperties);
  VK_function(GetPhysicalDeviceExternalBufferProperties, getExternalBufferProperties);
  VK_function(GetPhysicalDeviceExternalFenceProperties, getExternalFenceProperties);
  VK_function(GetPhysicalDeviceExternalSemaphoreProperties, getExternalSemaphoreProperties);
  VK_function(GetPhysicalDeviceExternalImageFormatPropertiesNV, getExternalImageFormatProperties);

  VK_function(GetPhysicalDeviceWin32PresentationSupportKHR, getWin32PresentationSupport);
  VK_function(GetPhysicalDeviceSurfaceSupportKHR, getSurfaceSupport);

  VK_function(GetPhysicalDeviceSurfaceCapabilities2KHR, getSurfaceCapabilities);
  VK_function(GetPhysicalDeviceSurfaceFormats2KHR, getSurfaceFormats);
  VK_function(GetPhysicalDeviceSurfacePresentModes2EXT, getSurfacePresentModes);
  VK_function(GetPhysicalDevicePresentRectanglesKHR, getPresentRectangles);


  VK_function(ReleaseDisplayEXT, releaseDisplay);
  VK_function(CreateDisplayModeKHR, createDisplayMode);
  VK_function(GetDisplayModeProperties2KHR, getDisplayModeProperties);
  VK_function(GetPhysicalDeviceDisplayProperties2KHR, getDisplayProperties);
  VK_function(GetPhysicalDeviceDisplayPlaneProperties2KHR, getDisplayPlaneProperties);
  VK_function(GetDisplayPlaneCapabilities2KHR, getDisplayPlaneCapabilities);
  VK_function(GetDisplayPlaneSupportedDisplaysKHR, getDisplayPlaneSupportedDisplays);







  VK_function(GetPhysicalDeviceMultisamplePropertiesEXT, getMultisampleProperties);
  VK_function(GetPhysicalDeviceCalibrateableTimeDomainsEXT, getCalibrateableTimeDomains);
  VK_function(GetPhysicalDeviceToolPropertiesEXT, getToolProperties);
  VK_function(GetPhysicalDeviceCooperativeMatrixPropertiesNV, getCooperativeMatrixProperties);
  VK_function(GetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV, getSupportedFramebufferMixedSamplesCombinations);*/