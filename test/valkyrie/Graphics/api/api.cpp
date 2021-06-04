//
// Created by maxwe on 2021-05-29.
//

#include "../../../../src/valkyrie/graphics/api/api.hpp"


#include <valkyrie/status/result.hpp>


#include <algorithm>


struct render_graph{
  VkSubpassDependency2 dep;
  VkSubpassDescription2 des;
  VkSubpassContents cont;
  VkSubpassBeginInfo beg;
  VkSubpassEndInfo end;
};









#pragma region helper structs
struct physical_device_features{
  VkPhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructure{
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR,
      .pNext = nullptr
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
struct physical_device_properties{
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
struct memory_properties{
  VkPhysicalDeviceMemoryBudgetPropertiesEXT budgetProperties{
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_BUDGET_PROPERTIES_EXT,
      .pNext = nullptr
  };
  VkPhysicalDeviceMemoryProperties2 properties{
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2,
      .pNext = &budgetProperties
  };
};
struct surface_capabilities{
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
struct surface_info{
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
#pragma endregion


namespace api = valkyrie::graphics::api;
namespace vlk = valkyrie;

struct module_descriptor{
  vlk::string_view name;
  vlk::version     version;
};


VkApplicationInfo getAppInfo(const module_descriptor& app, const module_descriptor& engine) noexcept {
  return {
    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .pNext = nullptr,
    .pApplicationName = app.name.c_str(),
    .applicationVersion = static_cast<uint32_t>(app.version),
    .pEngineName   = engine.name.c_str(),
    .engineVersion = static_cast<uint32_t>(engine.version),
    .apiVersion = VK_API_VERSION_1_2
  };
}



LRESULT WINAPI windowProc(_In_ HWND hWnd, _In_ UINT Msg, _In_ WPARAM wParam, _In_ LPARAM lParam) {
  /*switch ( Msg ) {
    case WM_DESTROY:
      windowShouldClose.store(true);
      break;
    case WM_SIZE: {
      auto& ext = *reinterpret_cast<uint16_t(*)[2]>(&lParam);
      windowExtents.store(VkExtent2D{ .width = ext[0], .height = ext[1] });
      std::cout << "Extents{ .width = " << ext[0] << ", .height = " << ext[1] << " }" << std::endl;
      break;
    }
    default:
      break;
  }*/
  return DefWindowProc(hWnd, Msg, wParam, lParam);
}

using cstring = const char*;
using valkyrie::extent2d, valkyrie::position2d, valkyrie::box2d, valkyrie::version;

void registerWindowClass(cstring className) noexcept {
  HINSTANCE hinst = GetModuleHandle(nullptr);

  WNDCLASS wndClass{
      .lpfnWndProc   = windowProc,
      .hInstance     = hinst,
      .lpszClassName = className
  };
  auto registrationAtom = RegisterClass(&wndClass);

  if (!registrationAtom)
    panic(valkyrie::sys::win32::get_last_error());
}
std::pair<HWND, HMONITOR> makeWindow(cstring className, cstring windowName, const box2d<int32_t>& box) noexcept {
  std::pair<HWND, HMONITOR> result;
  result.first = CreateWindow(
      className,
      windowName,
      WS_OVERLAPPEDWINDOW,
      box.offsets.x,
      box.offsets.y,
      box.extents.width,
      box.extents.height,
      nullptr,
      nullptr,
      GetModuleHandle(nullptr),
      nullptr
  );
  result.second = MonitorFromWindow(
      result.first,
      MONITOR_DEFAULTTONEAREST
  );
  return result;
}

int main() {

  using valkyrie::u32, valkyrie::i32;

  const cstring className = "Sample Window Class";
  const cstring windowName = "Sample Vulkan Window";
  HWND window;
  HMONITOR monitor;

  registerWindowClass(className);
  std::tie(window, monitor) = makeWindow(className, windowName, box2d<int>{ .offsets = { 0, 0 }, .extents = { 2560, 1440 } });

  api::initialize();


  module_descriptor app{ "Application Name", version(0, 0, 1) };
  module_descriptor engine{ "Engine Name", version(1, 1, 4) };

  auto appInfo = getAppInfo(app, engine);

  VkValidationFeatureEnableEXT enableFeatures[]{
      VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT
  };

  const cstring enabledLayers[]{
      "VK_LAYER_KHRONOS_validation"
  };
  const cstring enabledExtensions[]{
      VK_KHR_SURFACE_EXTENSION_NAME,
      VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
      VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME,
      VK_KHR_GET_DISPLAY_PROPERTIES_2_EXTENSION_NAME,
      VK_KHR_DISPLAY_EXTENSION_NAME,
      VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
      VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME,
      VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME
  };
  const cstring enabledDeviceExtensions[]{
      VK_EXT_BLEND_OPERATION_ADVANCED_EXTENSION_NAME,
      VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME,
      VK_EXT_FULL_SCREEN_EXCLUSIVE_EXTENSION_NAME,
      VK_EXT_INDEX_TYPE_UINT8_EXTENSION_NAME,
      VK_EXT_INLINE_UNIFORM_BLOCK_EXTENSION_NAME,
      VK_EXT_MEMORY_BUDGET_EXTENSION_NAME,
      VK_EXT_PIPELINE_CREATION_FEEDBACK_EXTENSION_NAME,
      VK_EXT_TEXEL_BUFFER_ALIGNMENT_EXTENSION_NAME,
      VK_EXT_TRANSFORM_FEEDBACK_EXTENSION_NAME,
      VK_EXT_SHADER_SUBGROUP_BALLOT_EXTENSION_NAME,
      VK_EXT_SHADER_SUBGROUP_VOTE_EXTENSION_NAME,
      VK_EXT_VALIDATION_CACHE_EXTENSION_NAME,
      VK_EXT_VERTEX_ATTRIBUTE_DIVISOR_EXTENSION_NAME,
      VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
      VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
      VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME,
      VK_KHR_RAY_QUERY_EXTENSION_NAME,
      VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
      VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME,
      VK_KHR_PIPELINE_EXECUTABLE_PROPERTIES_EXTENSION_NAME,
      VK_KHR_SHADER_CLOCK_EXTENSION_NAME,
      VK_KHR_SHADER_ATOMIC_INT64_EXTENSION_NAME,
      VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME,
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
      VK_KHR_SWAPCHAIN_MUTABLE_FORMAT_EXTENSION_NAME,
      VK_NV_MESH_SHADER_EXTENSION_NAME,
      VK_NV_COOPERATIVE_MATRIX_EXTENSION_NAME,
      VK_NV_DEVICE_GENERATED_COMMANDS_EXTENSION_NAME,
      VK_NV_ACQUIRE_WINRT_DISPLAY_EXTENSION_NAME,
      VK_NV_COMPUTE_SHADER_DERIVATIVES_EXTENSION_NAME,
      VK_NV_DEVICE_DIAGNOSTIC_CHECKPOINTS_EXTENSION_NAME,
      VK_NV_DEVICE_DIAGNOSTICS_CONFIG_EXTENSION_NAME,
      VK_NV_SHADER_SM_BUILTINS_EXTENSION_NAME
  };

  VkValidationFeaturesEXT validationFeatures{
      .sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT,
      .pNext = nullptr,
      .enabledValidationFeatureCount = 1,
      .pEnabledValidationFeatures = enableFeatures,
      .disabledValidationFeatureCount = 0,
      .pDisabledValidationFeatures = nullptr
  };

  VkInstanceCreateInfo instanceCreateInfo{
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pNext = &validationFeatures,
      .flags = 0,
      .pApplicationInfo = &appInfo,
      .enabledLayerCount = uint32_t(std::size(enabledLayers)),
      .ppEnabledLayerNames = enabledLayers,
      .enabledExtensionCount = uint32_t(std::size(enabledExtensions)),
      .ppEnabledExtensionNames = enabledExtensions
  };

  VkWin32SurfaceCreateInfoKHR surfaceCreateInfo{
      .sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
      .pNext     = nullptr,
      .flags     = 0,
      .hinstance = GetModuleHandle(nullptr),
      .hwnd      = window
  };

  VkResult initResult = api::initialize_instance(instanceCreateInfo);

  auto [ windowSurface, winCreateResult ] = api::create_surface(surfaceCreateInfo);

  api::physical_device physicalDevice;

  {
    uint32_t physicalDeviceCount = api::count_physical_devices().value;
    auto* pPhysicalDevices = new api::physical_device[physicalDeviceCount];
    std::span physicalDevices{ pPhysicalDevices, physicalDeviceCount };
    api::enumerate_physical_devices(physicalDevices);

    VkPhysicalDeviceProperties2 props{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
        .pNext = nullptr
    };

    for ( auto dev : physicalDevices ) {
      dev.get_properties(props);
      if ( props.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ) {
        physicalDevice = dev;
        break;
      }
    }

    delete[] pPhysicalDevices;
  }



  struct queue_family{
    VkQueue*     pQueues;
    u32          queueCount;
    VkQueueFlags flags;
    VkPipelineStageFlags2KHR checkpointStages;
    VkCommandPool permanentPool;
    // VkCommandPool transientPool;
    // VkCommandPool highPriorityPool;
    // VkCommandPool normalPriorityPool;
    // VkCommandPool lowPriorityPool;
  };
  queue_family* pQueueFamilies;
  u32           queueFamilyCount;

  {
    VkQueue* pQueues;
    VkQueueFamilyProperties2* pQueueFamilyProperties;
    VkQueueFamilyCheckpointProperties2NV* pCheckpointProps;
    u32      totalQueueCount = 0;

    queueFamilyCount = physicalDevice.count_queue_families();
    pQueueFamilies = new queue_family[queueFamilyCount];
    pQueueFamilyProperties = new VkQueueFamilyProperties2 [queueFamilyCount];
    pCheckpointProps = new VkQueueFamilyCheckpointProperties2NV[queueFamilyCount];
    std::span queueFamilyProperties(pQueueFamilyProperties, queueFamilyCount);
    for (u32 i = 0; i < queueFamilyCount; ++i ) {
      pQueueFamilyProperties[i].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
      pQueueFamilyProperties[i].pNext = pCheckpointProps + i;
      pCheckpointProps[i].sType = VK_STRUCTURE_TYPE_CHECKPOINT_DATA_2_NV;
      pCheckpointProps[i].pNext = nullptr;
    }
    physicalDevice.enumerate_queue_families(queueFamilyProperties);

    for (u32 i = 0; i < queueFamilyCount; ++i ) {
      totalQueueCount += pQueueFamilyProperties[i].queueFamilyProperties.queueCount;
      pQueueFamilies[i].queueCount = pQueueFamilyProperties[i].queueFamilyProperties.queueCount;
      pQueueFamilies[i].flags      = pQueueFamilyProperties[i].queueFamilyProperties.queueFlags;
      pQueueFamilies[i].checkpointStages = pCheckpointProps[i].checkpointExecutionStageMask;
    }

    pQueues = new VkQueue[totalQueueCount];

    u32 currentOffset = 0;
    for (u32 i = 0; i < queueFamilyCount; ++i ) {
      pQueueFamilies[i].pQueues = pQueues + currentOffset;
      currentOffset += pQueueFamilies[i].queueCount;
    }

    delete [] pCheckpointProps;
    delete [] pQueueFamilyProperties;
  }

  std::span<queue_family> queueFamilies{ pQueueFamilies, queueFamilyCount };


  api::device device;

  {
    physical_device_features physicalDeviceFeatures{};

    physicalDevice.get_features(physicalDeviceFeatures.vulkan10);

    auto* pQueueCreateInfos = new VkDeviceQueueCreateInfo[queueFamilyCount];

    for (u32 i = 0; i < queueFamilyCount; ++i ) {
      auto& createInfo = pQueueCreateInfos[i];
      createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      createInfo.pNext = nullptr;
      createInfo.flags = 0;
      createInfo.queueCount = queueFamilies[i].queueCount;
      createInfo.queueFamilyIndex = i;
      float* pLocalPriorities = new float[createInfo.queueCount];
      float priorityBase = float(createInfo.queueCount);
      for ( u32 j = 0; j < createInfo.queueCount; ++j )
        pLocalPriorities[j] = float(createInfo.queueCount - j) / priorityBase;
      createInfo.pQueuePriorities = pLocalPriorities;
    }

    VkDeviceCreateInfo deviceCreateInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &physicalDeviceFeatures.vulkan10,
        .flags = 0,
        .queueCreateInfoCount = queueFamilyCount,
        .pQueueCreateInfos = pQueueCreateInfos,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = std::size(enabledDeviceExtensions),
        .ppEnabledExtensionNames = enabledDeviceExtensions,
        .pEnabledFeatures = nullptr
    };

    auto [ dev, devResult ] = physicalDevice.create_device(deviceCreateInfo);

    device = dev;

    if ( devResult == VK_SUCCESS ) {
      for ( u32 i = 0; i < queueFamilyCount; ++i ) {
        for ( u32 j = 0; j < queueFamilies[i].queueCount; ++j ) {
          queueFamilies[i].pQueues[j] = dev.get_queue(i, j);
        }
      }
    }


    for (const auto& createInfo : std::span{ pQueueCreateInfos, queueFamilyCount })
      delete[] createInfo.pQueuePriorities;
    delete [] pQueueCreateInfos;
  }








  ShowWindow(window, SW_SHOW);

  namespace win32 = valkyrie::sys::win32;

  MSG msg;
  BOOL bRet;

  while( (bRet = GetMessage( &msg, NULL, 0, 0 )) ) {
    if (bRet == -1) {
      panic(win32::get_last_error());
      // handle the error and possibly exit
    } else {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    if (windowShouldClose)
      break;
  }



  device.destroy();
  api::destroy_surface(windowSurface);

  delete [] pQueueFamilies[0].pQueues;
  delete [] pQueueFamilies;
}