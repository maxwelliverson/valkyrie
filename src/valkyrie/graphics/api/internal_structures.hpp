//
// Created by Maxwell on 2020-11-28.
//

#ifndef VALKYRIE_INTERNAL_STRUCTURES_HPP
#define VALKYRIE_INTERNAL_STRUCTURES_HPP


#include <valkyrie/adt/flat_map.hpp>
#include <valkyrie/adt/interval.hpp>
#include <valkyrie/adt/static_vector.hpp>
#include <valkyrie/utility/version.hpp>

#include <valkyrie/graphics/api/vulkan.hpp>

#include "extensions.hpp"
#include "function_pointers.hpp"
#include "status_wrappers.hpp"

#include <windows.h>



#include <atomic>

#include <boost/static_string.hpp>



namespace valkyrie::graphics::api::Internal{
  
  using InstanceAPI       = const InstanceFunctions*;
  using PhysicalDeviceAPI = const PhysicalDeviceFunctions*;
  using DeviceAPI         = const DeviceFunctions*;
  using QueueAPI          = const QueueFunctions*;
  using CommandBufferAPI  = const CommandBufferFunctions*;

  using InternalAllocator = const VkAllocationCallbacks*;

  using PhysicalDeviceNameString = boost::static_string<VK_MAX_PHYSICAL_DEVICE_NAME_SIZE>;
  using NameString               = boost::static_string<VK_MAX_EXTENSION_NAME_SIZE>;
  using DescriptionString        = boost::static_string<VK_MAX_DESCRIPTION_SIZE>;

  struct InstanceImpl;
  struct ApplicationImpl;
  struct DeviceImpl;
  struct QueueImpl;
  struct CommandBufferImpl;




  struct InstanceExtension{
    std::string             name;
    InstanceExtensionID     id;
    valkyrie::version specVersion;
    bool                    isEnabled;


    InstanceExtension(const VkExtensionProperties& props) noexcept
        : name(props.extensionName, std::strlen(props.extensionName)),
          id(getInstanceExtensionID(name)),
          specVersion(props.specVersion),
          isEnabled(false){}

    friend bool operator==(const InstanceExtension& A, InstanceExtensionID extensionId) noexcept {
      return A.id == extensionId;
    }
    friend auto operator<=>(const InstanceExtension& A, InstanceExtensionID extensionId) noexcept {
      return A.id <=> extensionId;
    }
    friend bool operator==(const InstanceExtension& A, const InstanceExtension& B) noexcept {
      return A.id == B.id;
    }
    friend auto operator<=>(const InstanceExtension& A, const InstanceExtension& B) noexcept {
      return A.id <=> B.id;
    }
  };
  struct DeviceExtension{
    std::string             name;
    DeviceExtensionID       id;
    valkyrie::version specVersion;
    bool                    isEnabled;


    DeviceExtension(const VkExtensionProperties& props) noexcept
        : name(props.extensionName, std::strlen(props.extensionName)),
          id(getDeviceExtensionID(name)),
          specVersion(props.specVersion),
          isEnabled(false){}

    friend bool operator==(const DeviceExtension& A, DeviceExtensionID extensionId) noexcept {
      return A.id == extensionId;
    }
    friend auto operator<=>(const DeviceExtension& A, DeviceExtensionID extensionId) noexcept {
      return A.id <=> extensionId;
    }
    friend bool operator==(const DeviceExtension& A, const DeviceExtension& B) noexcept {
      return A.id == B.id;
    }
    friend auto operator<=>(const DeviceExtension& A, const DeviceExtension& B) noexcept {
      return A.id <=> B.id;
    }
  };
  struct Layer{
    NameString        name;
    DescriptionString description;
    LayerID           id;
    version specVersion;
    version implementationVersion;
    bool              isEnabled;
  };

  struct PhysicalDeviceFeatures{
    VkPhysicalDevicePortabilitySubsetFeaturesKHR portabilitySubset{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR,
        .pNext = nullptr
    };
    VkPhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructure{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR,
        .pNext = &portabilitySubset
    };
    VkPhysicalDeviceRayQueryFeaturesKHR rayQuery{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR,
        .pNext = &accelerationStructure
    };
    VkPhysicalDeviceRayTracingPipelineFeaturesKHR rayTracingPipeline{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR,
        .pNext = &rayQuery
    };
    VkPhysicalDeviceCornerSampledImageFeaturesNV cornerSampledImage{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CORNER_SAMPLED_IMAGE_FEATURES_NV,
        .pNext = &rayTracingPipeline
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


    ////

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

    // TODO: Add features from the remaining structs.

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
    /*VkPhysicalDevicePortabilitySubsetPropertiesKHR portabilitySubset{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_PROPERTIES_KHR,
        .pNext = nullptr
    };*/
    /*VkPhysicalDeviceGroupProperties group{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES,
        .pNext =
    };*/
    VkPhysicalDevicePushDescriptorPropertiesKHR pushDescriptor{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PUSH_DESCRIPTOR_PROPERTIES_KHR,
        .pNext = nullptr
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
    VkPhysicalDeviceAccelerationStructurePropertiesKHR accelerationStructure{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR,
        .pNext = &shadingRateImage
    };
    VkPhysicalDeviceRayTracingPipelinePropertiesKHR rayTracingPipeline{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR,
        .pNext = &accelerationStructure
    };
    VkPhysicalDeviceRayTracingPropertiesNV rayTracingNV{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PROPERTIES_NV,
        .pNext = &rayTracingPipeline
    };
    VkPhysicalDeviceExternalMemoryHostPropertiesEXT externalMemoryHost{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_HOST_PROPERTIES_EXT,
        .pNext = &rayTracingNV
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
    VkPhysicalDeviceSubgroupSizeControlPropertiesEXT subgroupSizeControl{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_PROPERTIES_EXT,
        .pNext = &cooperativeMatrix
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
    VkPhysicalDeviceVulkan12Properties vulkan12{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES,
        .pNext = &lineRasterization
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
  struct SurfaceCapabilities{
    VkSharedPresentSurfaceCapabilitiesKHR sharedPresent{
        .sType = VK_STRUCTURE_TYPE_SHARED_PRESENT_SURFACE_CAPABILITIES_KHR,
        .pNext = nullptr
    };
    VkSurfaceCapabilitiesFullScreenExclusiveEXT fullScreen{
        .sType = VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_FULL_SCREEN_EXCLUSIVE_EXT,
        .pNext = &sharedPresent
    };
    VkSurfaceCapabilities2KHR capabilities{
        .sType = VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_KHR,
        .pNext = &fullScreen};
  };
  struct SurfaceInfo{
    VkSurfaceFullScreenExclusiveWin32InfoEXT fullscreenWin32Info{
      .sType = VK_STRUCTURE_TYPE_SURFACE_FULL_SCREEN_EXCLUSIVE_WIN32_INFO_EXT,
      .pNext = nullptr
    };
    VkSurfaceFullScreenExclusiveInfoEXT fullscreenInfo{
      .sType = VK_STRUCTURE_TYPE_SURFACE_FULL_SCREEN_EXCLUSIVE_INFO_EXT,
      .pNext = &fullscreenWin32Info
    };
    VkPhysicalDeviceSurfaceInfo2KHR info{
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR,
      .pNext = &fullscreenInfo
    };
  };

  struct PerformanceCounter{

    VkPerformanceCounterKHR handle;
    PerformanceCounterFlags flags;
    NameString              name;
    DescriptionString       category;
    DescriptionString       description;

    PerformanceCounter(VkPerformanceCounterKHR counter, VkPerformanceCounterDescriptionKHR desc) noexcept
        : handle(counter),
          flags(desc.flags),
          name(desc.name),
          category(desc.category),
          description(desc.description){}
  };

  struct QueueFamily{
    QueueFlags                   queueFlags;
    u32                          queueCount;
    u32                          timestampValidBits;
    Extent3D<MinU32> imageTransferGranularity;
    PipelineStageFlags           checkpointExecutionStageMask;
    //small_vector<PerformanceCounter> performanceCounters{};

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

  

  struct Application{
    std::u8string name;
    std::u8string engineName;

  };

  struct SurfaceImpl{
    VkSurfaceKHR  handle;
    InstanceImpl* pInstance;

    InternalAllocator allocator;

    std::atomic_uint32_t refCount;

    HWND      nativeWindow;
    HMONITOR  nativeMonitor;
    HINSTANCE nativeProcess;

    system_status lastResult;

    Extent2D<> dims;
  };

  void destroySurfaceRef(Internal::SurfaceImpl* pImpl) noexcept;

  struct PhysicalDeviceImpl{
    VkPhysicalDevice  handle;
    InstanceImpl*     pInstance;
    PhysicalDeviceAPI pInterface;

    InternalAllocator internalAllocator;


    mutable PhysicalDeviceFeatures availableFeatures;
    PhysicalDeviceFeatures enabledFeatures;
    mutable PhysicalDeviceProperties properties;
    mutable MemoryProperties memoryProperties;

    mutable flat_set<DeviceExtension> extensions;

    mutable flat_map<VkFormat, VkFormatProperties2> formatProperties;
    mutable static_vector<QueueFamily, 8> queueFamilies;
    mutable flat_map<FlagBits::SampleCount, Extent2D<MaxU32>> sampleLocationMap;
    mutable static_vector<VkTimeDomainEXT, 4> timeDomains;
    mutable DynamicArray<VkCooperativeMatrixPropertiesNV> cooperativeMatrixProperties;

    mutable bool extensionsLoaded = false;
    mutable bool featuresLoaded = false;
    mutable bool propertiesLoaded = false;
    mutable bool queueFamiliesLoaded = false;
  };

  struct InstanceImpl{
    VkInstance handle;
    FreeFunctions freeFunctions;
    InstanceAPI instanceFunctions;

    VkAllocationCallbacks allocationCallbacks = VkAllocationCallbacks{
      .pUserData             = nullptr,
      .pfnAllocation         = [](void*, size_t size, size_t alignment, VkSystemAllocationScope){ return _aligned_malloc(size, alignment); },
      .pfnReallocation       = [](void*, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope){ return _aligned_realloc(pOriginal, size, alignment); },
      .pfnFree               = [](void*, void* pMemory){ return _aligned_free(pMemory); },
      .pfnInternalAllocation = nullptr,
      .pfnInternalFree       = nullptr
    };

    mutable std::atomic_uint32_t refCount;

    std::u8string applicationName;
    std::u8string engineName;


    mutable flat_set<Layer>             layers;
    mutable flat_set<InstanceExtension> extensions;

    small_vector<PhysicalDeviceImpl, 4> physicalDevices;
    version version;
  };

  void destroyInstanceRef(Internal::InstanceImpl* pImpl) noexcept;

  struct QueueImpl{

    /*inline constexpr static QueueFlags Graphics      { QueueBits::Graphics      };
    inline constexpr static QueueFlags Compute       { QueueBits::Compute       };
    inline constexpr static QueueFlags Transfer      { QueueBits::Transfer      };
    inline constexpr static QueueFlags SparseBinding { QueueBits::SparseBinding };
    inline constexpr static QueueFlags Protected     { QueueBits::Protected     };*/

    VkQueue handle;
    PhysicalDeviceImpl* pPhysicalDevice;
    QueueAPI api;
    u32      familyIndex;
    float    priority;
  };

  struct DeviceImpl{
    VkDevice            handle;
    PhysicalDeviceImpl* pPhysicalDevice;

    InternalAllocator internalAllocator;

    mutable std::atomic_uint32_t refCount;


  };

  void destroyDeviceRef(Internal::InstanceImpl* pImpl) noexcept;

  struct CommandPoolImpl{
    VkCommandPool    handle;
    DeviceImpl*      pDevice;
    CommandPoolFlags flags;
    u32              queueFamilyIndex;
  };

  struct CommandBufferImpl{
    VkCommandBuffer   handle;
    DeviceImpl*       pDevice;
    CommandPoolImpl*  pCommandPool;

    InternalAllocator internalAllocator;
  };

  struct SwapchainImpl{
    VkSwapchainKHR handle;
    VkSwapchainCreateInfoKHR createInfo;
    SwapchainFlags   flags;
    SurfaceImpl*     surface;
    uint32_t         minImageCount;
    Format           imageFormat;
    ColorSpace       imageColorSpace;
    Extent2D<> imageExtent;
    uint32_t         imageArrayLayers;
    ImageUsageFlags  imageUsage;
    SharingMode      imageSharingMode;
    uint32_t         queueFamilyIndexCount;
    const uint32_t*  pQueueFamilyIndices;
    FlagBits::SurfaceTransform preTransform;
    FlagBits::CompositeAlpha   compositeAlpha;
    PresentMode          presentMode;
    bool                 clipped;
    VkSwapchainKHR       oldSwapchain;
  };
}








#endif//VALKYRIE_INTERNAL_STRUCTURES_HPP
