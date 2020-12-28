//
// Created by Maxwell on 2020-11-15.
//

#ifndef VALKYRIE_FUNCTION_POINTERS_HPP
#define VALKYRIE_FUNCTION_POINTERS_HPP

#define VK_ENABLE_BETA_EXTENSIONS
#define VK_NO_PROTOTYPES
#define VK_USE_PLATFORM_WIN32_KHR
#define WIN32_MEAN_AND_LEAN
#define NOMINMAX
#include <vulkan/vulkan.h>

#include <cassert>

template <typename F>
class VulkanFunction;

template <typename Ret, typename ...Args>
class VulkanFunction<Ret(VKAPI_PTR*)(Args...)>{
  using F = Ret(*)(Args...);
  const F functionPtr;
public:
  VulkanFunction(F pFunction) noexcept : functionPtr(pFunction){}
  inline Ret operator()(Args... args) const noexcept {
    if constexpr (std::is_same_v<Ret, void>) {
      functionPtr(args...);
    } else {
      return functionPtr(args...);
    }
  }

  inline explicit operator bool() const noexcept {
    return functionPtr;
  }
};

class PolymorphicFunction{
  PFN_vkVoidFunction pFunction;
public:
  PolymorphicFunction(PFN_vkVoidFunction pFunction) noexcept : pFunction(pFunction){}

  template <typename Ret, typename ...Args>
  inline explicit operator VulkanFunction<Ret(VKAPI_PTR*)(Args...)>() const noexcept {
    return reinterpret_cast<Ret(VKAPI_PTR*)(Args...)>(pFunction);
  }
};

class FreeFunctionLoader{
  PFN_vkGetInstanceProcAddr getProcAddr;
public:
  FreeFunctionLoader(/*PFN_vkGetInstanceProcAddr getProcAddr*/);/* : getProcAddr(getProcAddr){}*/

  PolymorphicFunction load(const char* pFunc) const noexcept {
    return getProcAddr(nullptr, pFunc);
  }

  PFN_vkGetInstanceProcAddr getLoaderFunction() const noexcept {
    return getProcAddr;
  }
};
class InstanceFunctionLoader{
  VkInstance instance_;
  PFN_vkGetInstanceProcAddr getProcAddr;
public:
  InstanceFunctionLoader(VkInstance instance_, PFN_vkGetInstanceProcAddr getProcAddr) noexcept : instance_(instance_), getProcAddr(getProcAddr){}

  PolymorphicFunction load(const char* pFunc) const noexcept {
    return getProcAddr(instance_, pFunc);
  }
};
class DeviceFunctionLoader{
  VkDevice device_;
  PFN_vkGetDeviceProcAddr getProcAddr;
public:
  DeviceFunctionLoader(VkDevice device_, PFN_vkGetDeviceProcAddr getProcAddr) noexcept : device_(device_), getProcAddr(getProcAddr){}

  PolymorphicFunction load(const char* pFunc) const noexcept {
    return getProcAddr(device_, pFunc);
  }
};


#define VK_function(func_, field_name_) VulkanFunction<PFN_vk##func_> field_name_{loader.load("vk"#func_)}


struct FreeFunctions{

  static FreeFunctions* pInstance;

  FreeFunctionLoader loader{};

  VK_function(CreateInstance, createInstance);
  VK_function(EnumerateInstanceExtensionProperties, enumerateInstanceExtensions);
  VK_function(EnumerateInstanceLayerProperties, enumerateLayers);
  VK_function(EnumerateInstanceVersion, getInstanceVersion);

  FreeFunctions() noexcept{
    assert(createInstance);
    assert(enumerateLayers);
    assert(enumerateInstanceExtensions);
    assert(getInstanceVersion);

    pInstance = this;
  }
};
struct PhysicalDeviceFunctions{
  InstanceFunctionLoader loader;

  VK_function(CreateDevice, createDevice);
  VK_function(EnumerateDeviceExtensionProperties, enumerateDeviceExtensionProperties);


  //VK_function(GetPhysicalDeviceFeatures, getPhysicalDeviceFeatures);
  //VK_function(GetPhysicalDeviceProperties, getPhysicalDeviceProperties);
  //VK_function(GetPhysicalDeviceFormatProperties, getPhysicalDeviceFormatProperties);
  //VK_function(GetPhysicalDeviceFormatProperties, getPhysicalDeviceFormatProperties);
  //VK_function(GetPhysicalDeviceSparseImageFormatProperties, getPhysicalDeviceSparseImageFormatProperties);
  //VK_function(GetPhysicalDeviceImageFormatProperties, getPhysicalDeviceImageFormatProperties);
  //VK_function(GetPhysicalDeviceQueueFamilyProperties, getPhysicalDeviceQueueFamilyProperties);
  //VK_function(GetPhysicalDeviceMemoryProperties, getPhysicalDeviceMemoryProperties);
  //VK_function(GetPhysicalDeviceSurfaceCapabilitiesKHR, getSurfaceCapabilities);
  //VK_function(GetPhysicalDeviceSurfaceFormatsKHR, getSurfaceFormats);
  //VK_function(GetPhysicalDeviceSurfacePresentModesKHR, getSurfacePresentModes);
  //VK_function(GetDisplayModePropertiesKHR, getDisplayModeProperties);
  //VK_function(GetPhysicalDeviceDisplayPropertiesKHR, getPhysicalDeviceDisplayProperties);
  //VK_function(GetPhysicalDeviceDisplayPlanePropertiesKHR, getPhysicalDeviceDisplayPlaneProperties);
  //VK_function(GetDisplayPlaneCapabilitiesKHR, getDisplayPlaneCapabilities);


  VK_function(GetPhysicalDeviceFeatures2, getFeatures);
  VK_function(GetPhysicalDeviceProperties2, getProperties);
  VK_function(GetPhysicalDeviceFormatProperties2, getFormatProperties);
  VK_function(GetPhysicalDeviceImageFormatProperties2, getImageFormatProperties);
  VK_function(GetPhysicalDeviceSparseImageFormatProperties2, getSparseImageFormatProperties);
  VK_function(GetPhysicalDeviceQueueFamilyProperties2, getQueueFamilyProperties);

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


  VK_function(CreateDisplayModeKHR, createDisplayMode);
  VK_function(GetDisplayModeProperties2KHR, getDisplayModeProperties);
  VK_function(GetPhysicalDeviceDisplayProperties2KHR, getDisplayProperties);
  VK_function(GetPhysicalDeviceDisplayPlaneProperties2KHR, getDisplayPlaneProperties);
  VK_function(GetDisplayPlaneCapabilities2KHR, getDisplayPlaneCapabilities);
  VK_function(GetDisplayPlaneSupportedDisplaysKHR, getDisplayPlaneSupportedDisplays);
  VK_function(EnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR, enumerateQueueFamilyPerformanceQueryCounters);
  VK_function(GetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR, getQueueFamilyPerformanceQueryPasses);




  VK_function(ReleaseDisplayEXT, releaseDisplay);

  VK_function(GetPhysicalDeviceMultisamplePropertiesEXT, getMultisampleProperties);
  VK_function(GetPhysicalDeviceCalibrateableTimeDomainsEXT, getCalibrateableTimeDomains);
  VK_function(GetPhysicalDeviceToolPropertiesEXT, getToolProperties);
  VK_function(GetPhysicalDeviceCooperativeMatrixPropertiesNV, getCooperativeMatrixProperties);
  VK_function(GetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV, getSupportedFramebufferMixedSamplesCombinations);

  PhysicalDeviceFunctions(InstanceFunctionLoader loader) noexcept : loader(loader){}
};
struct InstanceFunctions{
  InstanceFunctionLoader loader;

  VK_function(DestroyInstance, destroyInstance);
  VK_function(EnumeratePhysicalDevices, enumeratePhysicalDevices);
  VK_function(EnumeratePhysicalDeviceGroups, enumeratePhysicalDeviceGroups);


  VK_function(CreateWin32SurfaceKHR, createWin32Surface);
  VK_function(DestroySurfaceKHR, destroySurface);


  // INSTANCE

  VK_function(CreateDisplayPlaneSurfaceKHR, createDisplayPlaneSurface);



  VK_function(CreateDebugUtilsMessengerEXT, createDebugUtilsMessenger);
  VK_function(DestroyDebugUtilsMessengerEXT, destroyDebugUtilsMessenger);
  VK_function(SubmitDebugUtilsMessageEXT, submitDebugUtilsMessage);

  VK_function(CreateHeadlessSurfaceEXT, createHeadlessSurface);

  PhysicalDeviceFunctions physicalDeviceFunctions{loader};

  InstanceFunctions(InstanceFunctionLoader loader) noexcept : loader(loader){}
};

struct CommandBufferFunctions{
  DeviceFunctionLoader loader;

  VK_function(BeginCommandBuffer, begin);
  VK_function(EndCommandBuffer, end);
  VK_function(ResetCommandBuffer, reset);


  VK_function(CmdSetViewport, cmdSetViewport);
  VK_function(CmdSetScissor, cmdSetScissor);
  VK_function(CmdSetLineWidth, cmdSetLineWidth);
  VK_function(CmdSetDepthBias, cmdSetDepthBias);
  VK_function(CmdSetBlendConstants, cmdSetBlendConstants);
  VK_function(CmdSetDepthBounds, cmdSetDepthBounds);
  VK_function(CmdSetStencilCompareMask, cmdSetStencilCompareMask);
  VK_function(CmdSetStencilWriteMask, cmdSetStencilWriteMask);
  VK_function(CmdSetStencilReference, cmdSetStencilReference);
  VK_function(CmdSetCullModeEXT, cmdSetCullMode);
  VK_function(CmdSetFrontFaceEXT, cmdSetFrontFace);
  VK_function(CmdSetPrimitiveTopologyEXT, cmdSetPrimitiveTopology);
  VK_function(CmdSetViewportWithCountEXT, cmdSetViewportWithCount);
  VK_function(CmdSetScissorWithCountEXT, cmdSetScissorWithCount);
  VK_function(CmdSetDepthTestEnableEXT, cmdSetDepthTestEnable);
  VK_function(CmdSetDepthWriteEnableEXT, cmdSetDepthWriteEnable);
  VK_function(CmdSetDepthCompareOpEXT, cmdSetDepthCompareOp);
  VK_function(CmdSetDepthBoundsTestEnableEXT, cmdSetDepthBoundsTestEnable);
  VK_function(CmdSetStencilTestEnableEXT, cmdSetStencilTestEnable);
  VK_function(CmdSetStencilOpEXT, cmdSetStencilOp);
  VK_function(CmdSetViewportWScalingNV, cmdSetViewportWScaling);
  VK_function(CmdSetDiscardRectangleEXT, cmdSetDiscardRectangle);
  VK_function(CmdSetViewportShadingRatePaletteNV, cmdSetViewportShadingRatePalette);
  VK_function(CmdSetCoarseSampleOrderNV, cmdSetCoarseSampleOrder);
  VK_function(CmdSetSampleLocationsEXT, cmdSetSampleLocations);
  VK_function(CmdSetExclusiveScissorNV, cmdSetExclusiveScissor);
  VK_function(CmdSetCheckpointNV, cmdSetCheckpoint);
  VK_function(CmdSetPerformanceMarkerINTEL, cmdSetPerformanceMarker);
  VK_function(CmdSetPerformanceStreamMarkerINTEL, cmdSetPerformanceStreamMarker);
  VK_function(CmdSetPerformanceOverrideINTEL, cmdSetPerformanceOverride);
  VK_function(CmdSetLineStippleEXT, cmdSetLineStipple);
  VK_function(CmdSetDeviceMask, cmdSetDeviceMask);
  VK_function(CmdSetEvent, cmdSetEvent);
  VK_function(CmdSetRayTracingPipelineStackSizeKHR, cmdSetRayTracingPipelineStackSizeKHR);


  VK_function(CmdBindPipeline, cmdBindPipeline);
  VK_function(CmdBindDescriptorSets, cmdBindDescriptorSets);
  VK_function(CmdBindIndexBuffer, cmdBindIndexBuffer);
  VK_function(CmdBindVertexBuffers, cmdBindVertexBuffers);
  VK_function(CmdBindVertexBuffers2EXT, cmdBindVertexBuffers2);
  VK_function(CmdBindShadingRateImageNV, cmdBindShadingRateImage);
  VK_function(CmdBindTransformFeedbackBuffersEXT, cmdBindTransformFeedbackBuffers);
  VK_function(CmdBindPipelineShaderGroupNV, cmdBindPipelineShaderGroup);


  VK_function(CmdDraw, cmdDraw);
  VK_function(CmdDrawIndexed, cmdDrawIndexed);
  VK_function(CmdDrawIndirect, cmdDrawIndirect);
  VK_function(CmdDrawIndirectCount, cmdDrawIndirectCount);
  VK_function(CmdDrawIndexedIndirect, cmdDrawIndexedIndirect);
  VK_function(CmdDrawIndexedIndirectCount, cmdDrawIndexedIndirectCount);
  VK_function(CmdDrawIndirectByteCountEXT, cmdDrawIndirectByteCount);
  VK_function(CmdDrawMeshTasksNV, cmdDrawMeshTasks);
  VK_function(CmdDrawMeshTasksIndirectNV, cmdDrawMeshTasksIndirect);
  VK_function(CmdDrawMeshTasksIndirectCountNV, cmdDrawMeshTasksIndirectCount);
  VK_function(CmdTraceRaysKHR, cmdTraceRaysKHR);
  VK_function(CmdTraceRaysIndirectKHR, cmdTraceRaysIndirectKHR);


  VK_function(CmdCopyBuffer, cmdCopyBuffer);
  VK_function(CmdCopyBuffer2KHR, cmdCopyBuffer2);
  VK_function(CmdCopyImage, cmdCopyImage);
  VK_function(CmdCopyImage2KHR, cmdCopyImage2);
  VK_function(CmdCopyBufferToImage, cmdCopyBufferToImage);
  VK_function(CmdCopyBufferToImage2KHR, cmdCopyBufferToImage2);
  VK_function(CmdCopyImageToBuffer, cmdCopyImageToBuffer);
  VK_function(CmdCopyImageToBuffer2KHR, cmdCopyImageToBuffer2);
  VK_function(CmdCopyAccelerationStructureNV, cmdCopyAccelerationStructureNV);
  VK_function(CmdCopyQueryPoolResults, cmdCopyQueryPoolResults);


  VK_function(CmdBeginQuery, cmdBeginQuery);
  VK_function(CmdBeginRenderPass, cmdBeginRenderPass);
  VK_function(CmdBeginRenderPass2, cmdBeginRenderPass2);
  VK_function(CmdBeginTransformFeedbackEXT, cmdBeginTransformFeedback);
  VK_function(CmdBeginQueryIndexedEXT, cmdBeginQueryIndexed);
  VK_function(CmdBeginConditionalRenderingEXT, cmdBeginConditionalRendering);
  VK_function(CmdBeginDebugUtilsLabelEXT, cmdBeginDebugUtilsLabel);


  VK_function(CmdEndRenderPass, cmdEndRenderPass);
  VK_function(CmdEndRenderPass2, cmdEndRenderPass2);
  VK_function(CmdEndQuery, cmdEndQuery);
  VK_function(CmdEndTransformFeedbackEXT, cmdEndTransformFeedback);
  VK_function(CmdEndQueryIndexedEXT, cmdEndQueryIndexed);
  VK_function(CmdEndConditionalRenderingEXT, cmdEndConditionalRendering);
  VK_function(CmdEndDebugUtilsLabelEXT, cmdEndDebugUtilsLabel);


  VK_function(CmdDispatch, cmdDispatch);
  VK_function(CmdDispatchIndirect, cmdDispatchIndirect);
  VK_function(CmdDispatchBase, cmdDispatchBase);


  VK_function(CmdBlitImage, cmdBlitImage);
  VK_function(CmdBlitImage2KHR, cmdBlitImage2);


  VK_function(CmdClearColorImage, cmdClearColorImage);
  VK_function(CmdClearDepthStencilImage, cmdClearDepthStencilImage);
  VK_function(CmdClearAttachments, cmdClearAttachments);


  VK_function(CmdResolveImage, cmdResolveImage);
  VK_function(CmdResolveImage2KHR, cmdResolveImage2);


  VK_function(CmdResetQueryPool, cmdResetQueryPool);
  VK_function(CmdResetEvent, cmdResetEvent);


  VK_function(CmdPushConstants, cmdPushConstants);
  VK_function(CmdPushDescriptorSetKHR, cmdPushDescriptorSet);
  VK_function(CmdPushDescriptorSetWithTemplateKHR, cmdPushDescriptorSetWithTemplate);


  VK_function(CmdBuildAccelerationStructuresKHR,           cmdBuildAccelerationStructuresKHR);
  VK_function(CmdBuildAccelerationStructuresIndirectKHR,   cmdBuildAccelerationStructuresIndirectKHR);
  VK_function(CmdWriteAccelerationStructuresPropertiesKHR, cmdWriteAccelerationStructuresPropertiesKHR);
  VK_function(CmdCopyAccelerationStructureKHR,             cmdCopyAccelerationStructureKHR);
  VK_function(CmdCopyAccelerationStructureToMemoryKHR,     cmdCopyAccelerationStructureToMemoryKHR);
  VK_function(CmdCopyMemoryToAccelerationStructureKHR,     cmdCopyMemoryToAccelerationStructureKHR);


  VK_function(CmdNextSubpass, cmdNextSubpass);
  VK_function(CmdNextSubpass2, cmdNextSubpass2);


  VK_function(CmdPreprocessGeneratedCommandsNV, cmdPreprocessGeneratedCommands);
  VK_function(CmdExecuteGeneratedCommandsNV, cmdExecuteGeneratedCommands);
  VK_function(CmdExecuteCommands, cmdExecuteCommands);

  VK_function(CmdPipelineBarrier, cmdPipelineBarrier);
  VK_function(CmdWaitEvents, cmdWaitEvents);


  VK_function(CmdWriteTimestamp, cmdWriteTimestamp);
  VK_function(CmdInsertDebugUtilsLabelEXT, cmdInsertDebugUtilsLabel);


  VK_function(CmdUpdateBuffer, cmdUpdateBuffer);
  VK_function(CmdFillBuffer, cmdFillBuffer);


  CommandBufferFunctions(DeviceFunctionLoader loader) noexcept : loader(loader){}
};
struct DescriptorFunctions{
  DeviceFunctionLoader loader;

  VK_function(CreateDescriptorSetLayout, createDescriptorSetLayout);
  VK_function(DestroyDescriptorSetLayout, destroyDescriptorSetLayout);
  VK_function(GetDescriptorSetLayoutSupport, getDescriptorSetLayoutSupport);

  VK_function(AllocateDescriptorSets, allocateDescriptorSets);
  VK_function(FreeDescriptorSets, freeDescriptorSets);
  VK_function(UpdateDescriptorSets, updateDescriptorSets);
  VK_function(UpdateDescriptorSetWithTemplate, updateDescriptorSetWithTemplate);

  VK_function(CreateDescriptorPool, createDescriptorPool);
  VK_function(DestroyDescriptorPool, destroyDescriptorPool);
  VK_function(ResetDescriptorPool, resetDescriptorPool);

  VK_function(CreateDescriptorUpdateTemplate, createDescriptorUpdateTemplate);
  VK_function(DestroyDescriptorUpdateTemplate, destroyDescriptorUpdateTemplate);

  DescriptorFunctions(DeviceFunctionLoader loader) noexcept : loader(loader){}
};
struct PipelineFunctions{
  DeviceFunctionLoader loader;

  VK_function(CreateGraphicsPipelines, createGraphicsPipeline);
  VK_function(CreateComputePipelines, createComputePipeline);
  VK_function(CreateRayTracingPipelinesNV, createRayTracingPipelineNV);
  VK_function(CreateRayTracingPipelinesKHR, createRayTracingPipelineKHR);
  VK_function(DestroyPipeline, destroyPipeline);

  VK_function(CreatePipelineCache, createPipelineCache);
  VK_function(DestroyPipelineCache, destroyPipelineCache);
  VK_function(GetPipelineCacheData, getPipelineCacheData);
  VK_function(MergePipelineCaches, mergePipelineCaches);

  VK_function(CreatePipelineLayout, createPipelineLayout);
  VK_function(DestroyPipelineLayout, destroyPipelineLayout);


  VK_function(GetPipelineExecutablePropertiesKHR, getPipelineExecutableProperties);
  VK_function(GetPipelineExecutableStatisticsKHR, getPipelineExecutableStatistics);
  VK_function(GetPipelineExecutableInternalRepresentationsKHR, getPipelineExecutableInternalRepresentations);

  VK_function(GetRayTracingShaderGroupHandlesKHR, getRayTracingShaderGroupHandlesKHR);
  VK_function(GetRayTracingCaptureReplayShaderGroupHandlesKHR, getRayTracingCaptureReplayShaderGroupHandlesKHR);



  PipelineFunctions(DeviceFunctionLoader loader) noexcept : loader(loader){}
};
struct AccelerationStructureFunctions{
  DeviceFunctionLoader loader;

  /*VK_function(CreateAccelerationStructureKHR,  createKHR);
  VK_function(DestroyAccelerationStructureKHR, destroyKHR);
  VK_function(GetAccelerationStructureMemoryRequirementsKHR, getMemoryRequirementsKHR);
  VK_function(BindAccelerationStructureMemoryKHR, bindMemoryKHR);
  VK_function(BuildAccelerationStructuresKHR, buildKHR);
  VK_function(CopyAccelerationStructureKHR, copyKHR);
  VK_function(CopyAccelerationStructureToMemoryKHR, copyToMemoryKHR);
  VK_function(CopyMemoryToAccelerationStructureKHR, copyFromMemoryKHR);
  VK_function(WriteAccelerationStructuresPropertiesKHR, writePropertiesKHR);
  VK_function(GetAccelerationStructureDeviceAddressKHR, getDeviceAddressKHR);
  VK_function(GetDeviceAccelerationStructureCompatibilityKHR, getDeviceCompatibilityKHR);


  VK_function(CreateAccelerationStructureNV, createNV);
  VK_function(DestroyAccelerationStructureNV, destroyNV);
  VK_function(GetAccelerationStructureMemoryRequirementsNV, getMemoryRequirementsNV);
  VK_function(BindAccelerationStructureMemoryNV, bindMemoryNV);
  VK_function(GetAccelerationStructureHandleNV, getHandleNV);
*/

  VK_function(CreateAccelerationStructureKHR,                 createKHR);
  VK_function(DestroyAccelerationStructureKHR,                destroyKHR);
  VK_function(BuildAccelerationStructuresKHR,                 buildKHR);
  VK_function(CopyAccelerationStructureKHR,                   copyKHR);
  VK_function(CopyAccelerationStructureToMemoryKHR,           copyToMemoryKHR);
  VK_function(CopyMemoryToAccelerationStructureKHR,           copyFromMemoryKHR);
  VK_function(WriteAccelerationStructuresPropertiesKHR,       writePropertiesKHR);
  VK_function(GetAccelerationStructureDeviceAddressKHR,       getDeviceAddressKHR);
  VK_function(GetDeviceAccelerationStructureCompatibilityKHR, getDeviceCompatibilityKHR);
  VK_function(GetAccelerationStructureBuildSizesKHR,          getBuildSizesKHR);



  AccelerationStructureFunctions(DeviceFunctionLoader loader) noexcept : loader(loader){}
};
struct QueueFunctions{
  DeviceFunctionLoader loader;

  VK_function(QueueSubmit, queueSubmit);
  VK_function(QueueWaitIdle, queueWaitIdle);
  VK_function(QueueBindSparse, queueBindSparse);

  VK_function(QueuePresentKHR, queuePresent);

  VK_function(QueueBeginDebugUtilsLabelEXT, queueBeginDebugUtilsLabel);
  VK_function(QueueEndDebugUtilsLabelEXT, queueEndDebugUtilsLabel);
  VK_function(QueueInsertDebugUtilsLabelEXT, queueInsertDebugUtilsLabel);

  VK_function(GetQueueCheckpointDataNV, getQueueCheckpointData);
  VK_function(QueueSetPerformanceConfigurationINTEL, queueSetPerformanceConfiguration);

  QueueFunctions(DeviceFunctionLoader loader) noexcept : loader(loader){}
};
struct ImageFunctions{
  DeviceFunctionLoader loader;

  VK_function(CreateImage, createImage);
  VK_function(DestroyImage, destroyImage);
  VK_function(BindImageMemory, bindImageMemory);
  VK_function(BindImageMemory2, bindImageMemory2);
  VK_function(GetImageMemoryRequirements, getImageMemoryRequirements);
  VK_function(GetImageMemoryRequirements2, getImageMemoryRequirements2);
  VK_function(GetImageSparseMemoryRequirements, getImageSparseMemoryRequirements);
  VK_function(GetImageSparseMemoryRequirements2, getImageSparseMemoryRequirements2);
  VK_function(GetImageSubresourceLayout, getImageSubresourceLayout);

  VK_function(CreateImageView, createImageView);
  VK_function(DestroyImageView, destroyImageView);

  VK_function(GetImageViewHandleNVX, getImageViewHandle);
  VK_function(GetImageViewAddressNVX, getImageViewAddress);

  VK_function(GetImageDrmFormatModifierPropertiesEXT, getImageDrmFormatModifierProperties);

  ImageFunctions(DeviceFunctionLoader loader) noexcept : loader(loader){}
};
struct BufferFunctions{
  DeviceFunctionLoader loader;

  VK_function(CreateBuffer, createBuffer);
  VK_function(DestroyBuffer, destroyBuffer);

  VK_function(BindBufferMemory, bindBufferMemory);
  VK_function(BindBufferMemory2, bindBufferMemory2);
  VK_function(GetBufferMemoryRequirements, getBufferMemoryRequirements);
  VK_function(GetBufferMemoryRequirements2, getBufferMemoryRequirements2);

  VK_function(GetBufferDeviceAddress, getBufferDeviceAddress);
  VK_function(GetBufferOpaqueCaptureAddress, getBufferOpaqueCaptureAddress);

  VK_function(CreateBufferView, createBufferView);
  VK_function(DestroyBufferView, destroyBufferView);


  VK_function(GetBufferDeviceAddressEXT, getBufferDeviceAddressEXT);

  BufferFunctions(DeviceFunctionLoader loader) noexcept : loader(loader){}
};
struct PresentationFunctions{
  DeviceFunctionLoader loader;









  VK_function(AcquireFullScreenExclusiveModeEXT, acquireFullScreenExclusiveMode);
  VK_function(ReleaseFullScreenExclusiveModeEXT, releaseFullScreenExclusiveMode);
  VK_function(GetDeviceGroupSurfacePresentModes2EXT, getDeviceGroupSurfacePresentModes2);

  VK_function(CreateSwapchainKHR, createSwapchain);
  VK_function(DestroySwapchainKHR, destroySwapchain);
  VK_function(GetSwapchainImagesKHR, getSwapchainImages);
  VK_function(AcquireNextImageKHR, acquireNextImage);

  VK_function(GetDeviceGroupPresentCapabilitiesKHR, getDeviceGroupPresentCapabilities);
  VK_function(GetDeviceGroupSurfacePresentModesKHR, getDeviceGroupSurfacePresentModes);
  VK_function(AcquireNextImage2KHR, acquireNextImage2);

  VK_function(CreateSharedSwapchainsKHR, createSharedSwapchains);


  VK_function(GetSwapchainStatusKHR, getSwapchainStatus);

  VK_function(DisplayPowerControlEXT, displayPowerControl);
  VK_function(RegisterDeviceEventEXT, registerDeviceEvent);
  VK_function(RegisterDisplayEventEXT, registerDisplayEvent);
  VK_function(GetSwapchainCounterEXT, getSwapchainCounter);

  VK_function(SetHdrMetadataEXT, setHdrMetadata);
  VK_function(SetLocalDimmingAMD, setLocalDimming);

  PresentationFunctions(DeviceFunctionLoader loader) noexcept : loader(loader){}
};

struct DeviceFunctions{
  DeviceFunctionLoader loader;

  VK_function(DeviceWaitIdle, waitIdle);
  VK_function(GetDeviceQueue, getQueue);
  VK_function(GetDeviceQueue2, getQueue2);
  VK_function(GetDeviceMemoryOpaqueCaptureAddress, getMemoryOpaqueCaptureAddress);
  VK_function(DestroyDevice, destroy);



  VK_function(AllocateMemory, allocateMemory);
  VK_function(FreeMemory, freeMemory);
  VK_function(MapMemory, mapMemory);
  VK_function(UnmapMemory, unmapMemory);
  VK_function(FlushMappedMemoryRanges, flushMappedMemory);
  VK_function(InvalidateMappedMemoryRanges, invalidateMappedMemory);
  VK_function(GetDeviceMemoryCommitment, getMemoryCommitment);
  VK_function(GetDeviceMemoryOpaqueCaptureAddress, getOpaqueCaptureAddress);






  VK_function(CreateEvent, createEvent);
  VK_function(DestroyEvent, destroyEvent);
  VK_function(GetEventStatus, getEventStatus);
  VK_function(SetEvent, setEvent);
  VK_function(ResetEvent, resetEvent);


  VK_function(CreateQueryPool, createQueryPool);
  VK_function(DestroyQueryPool, destroyQueryPool);
  VK_function(GetQueryPoolResults, getQueryPoolResults);
  VK_function(ResetQueryPool, resetQueryPool);

  VK_function(CreateShaderModule, createShaderModule);
  VK_function(DestroyShaderModule, destroyShaderModule);

  VK_function(CreateSampler, createSampler);
  VK_function(DestroySampler, destroySampler);


  VK_function(CreateFramebuffer, createFramebuffer);
  VK_function(DestroyFramebuffer, destroyFramebuffer);


  VK_function(CreateRenderPass, createRenderPass);
  VK_function(CreateRenderPass2, createRenderPass2);
  VK_function(DestroyRenderPass, destroyRenderPass);
  VK_function(GetRenderAreaGranularity, getRenderAreaGranularity);


  VK_function(CreateCommandPool, createCommandPool);
  VK_function(DestroyCommandPool, destroyCommandPool);
  VK_function(ResetCommandPool, resetCommandPool);
  VK_function(AllocateCommandBuffers, allocateCommandBuffers);
  VK_function(FreeCommandBuffers, freeCommandBuffers);
  VK_function(TrimCommandPool, trimCommandPool);

  VK_function(CreateSamplerYcbcrConversion, createSamplerYcbcrConversion);
  VK_function(DestroySamplerYcbcrConversion, destroySamplerYcbcrConversion);



  VK_function(GetMemoryWin32HandleKHR, getMemoryWin32Handle);
  VK_function(GetMemoryWin32HandlePropertiesKHR, getMemoryWin32HandleProperties);
  VK_function(GetMemoryFdKHR, getMemoryFd);
  VK_function(GetMemoryFdPropertiesKHR, getMemoryFdProperties);
  VK_function(GetMemoryWin32HandleNV, getMemoryWin32HandleNV);

  VK_function(GetDeviceGroupPeerMemoryFeaturesKHR, getDeviceGroupPeerMemoryFeatures);
  VK_function(GetMemoryHostPointerPropertiesEXT, getMemoryHostPointerProperties);
  VK_function(GetGeneratedCommandsMemoryRequirementsNV, getGeneratedCommandsMemoryRequirements);


  VK_function(CreateSemaphore, createSemaphore);
  VK_function(DestroySemaphore, destroySemaphore);
  VK_function(GetSemaphoreCounterValue, getSemaphoreCounterValue);
  VK_function(WaitSemaphores, waitSemaphores);
  VK_function(SignalSemaphore, signalSemaphore);
  VK_function(ImportSemaphoreWin32HandleKHR, importSemaphoreWin32Handle);
  VK_function(GetSemaphoreWin32HandleKHR, getSemaphoreWin32Handle);


  VK_function(CreateFence, createFence);
  VK_function(DestroyFence, destroyFence);
  VK_function(ResetFences, resetFence);
  VK_function(GetFenceStatus, getFenceStatus);
  VK_function(WaitForFences, waitForFences);
  VK_function(ImportFenceWin32HandleKHR, importFenceWin32Handle);
  VK_function(GetFenceWin32HandleKHR, getFenceWin32Handle);
  VK_function(ImportFenceFdKHR, importFenceFd);
  VK_function(GetFenceFdKHR, getFenceFd);






  VK_function(AcquireProfilingLockKHR, acquireProfilingLock);
  VK_function(ReleaseProfilingLockKHR, releaseProfilingLock);


  VK_function(SetDebugUtilsObjectNameEXT, setDebugUtilsObjectName);
  VK_function(SetDebugUtilsObjectTagEXT, setDebugUtilsObjectTag);


  VK_function(CreateValidationCacheEXT, createValidationCache);
  VK_function(DestroyValidationCacheEXT, destroyValidationCache);
  VK_function(MergeValidationCachesEXT, mergeValidationCaches);
  VK_function(GetValidationCacheDataEXT, getValidationCacheData);



  VK_function(GetCalibratedTimestampsEXT, getCalibratedTimestamps);



  VK_function(InitializePerformanceApiINTEL, initializePerformanceApi);
  VK_function(UninitializePerformanceApiINTEL, uninitializePerformanceApi);
  VK_function(AcquirePerformanceConfigurationINTEL, acquirePerformanceConfiguration);
  VK_function(ReleasePerformanceConfigurationINTEL, releasePerformanceConfiguration);

  VK_function(GetPerformanceParameterINTEL, getPerformanceParameter);


  VK_function(CreateIndirectCommandsLayoutNV, createIndirectCommandsLayout);
  VK_function(DestroyIndirectCommandsLayoutNV, destroyIndirectCommandsLayout);

  VK_function(CreatePrivateDataSlotEXT, createPrivateDataSlot);
  VK_function(DestroyPrivateDataSlotEXT, destroyPrivateDataSlot);
  VK_function(SetPrivateDataEXT, setPrivateData);
  VK_function(GetPrivateDataEXT, getPrivateData);


  VK_function(CreateDeferredOperationKHR, createDeferredOperation);
  VK_function(DestroyDeferredOperationKHR, destroyDeferredOperation);
  VK_function(GetDeferredOperationMaxConcurrencyKHR, getDeferredOperationMaxConcurrency);
  VK_function(GetDeferredOperationResultKHR, getDeferredOperationResult);
  VK_function(DeferredOperationJoinKHR, deferredOperationJoin);
  VK_function(CompileDeferredNV, compileDeferredNV);


  CommandBufferFunctions commandBuffer;
  DescriptorFunctions descriptor;
  PipelineFunctions pipeline;
  QueueFunctions queue;
  ImageFunctions image;
  BufferFunctions buffer;
  AccelerationStructureFunctions accelerationStructure;
  PresentationFunctions presentation;




  DeviceFunctions(DeviceFunctionLoader loader) noexcept
      : loader(loader),
        commandBuffer(loader),
        descriptor(loader),
        pipeline(loader),
        queue(loader),
        image(loader),
        buffer(loader),
        accelerationStructure(loader),
        presentation(loader){}
};

#endif//VALKYRIE_FUNCTION_POINTERS_HPP
