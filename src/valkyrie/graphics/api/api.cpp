//
// Created by maxwe on 2021-04-23.
//

#include "api.hpp"



struct FreeFunctions{

  static FreeFunctions* pInstance;

  FreeFunctionLoader loader{  };

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

#define VK_cmd_function(name_, ...) VK_function(Cmd##name_, cmd##name_)

  VK_function(BeginCommandBuffer, begin);
  VK_function(EndCommandBuffer, end);
  VK_function(ResetCommandBuffer, reset);


  /*  =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*[ Vulkan Commands ]*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
   *
   *              Command                                 | Command Buffer Level | Render Pass Scope |         Queue Types         | Pipeline Type
   * */


  // Control Flow
  VK_cmd_function(ExecuteCommands,                          Primary,               Both,               Graphics | Compute | Transfer);
  VK_cmd_function(BeginRenderPass,                          Primary,               Outside,            Graphics,                      Graphics);
  VK_cmd_function(BeginRenderPass2,                         Primary,               Outside,            Graphics,                      Graphics);
  VK_cmd_function(NextSubpass,                              Primary,               Inside,             Graphics,                      Graphics);
  VK_cmd_function(NextSubpass2,                             Primary,               Inside,             Graphics,                      Graphics);
  VK_cmd_function(EndRenderPass,                            Primary,               Inside,             Graphics,                      Graphics);
  VK_cmd_function(EndRenderPass2,                           Primary,               Inside,             Graphics,                      Graphics);


  VK_cmd_function(PipelineBarrier,                          Both,                  Both,               Graphics | Compute | Transfer);
  VK_cmd_function(SetDeviceMask,                            Both,                  Both,               Graphics | Compute | Transfer);
  VK_cmd_function(BeginConditionalRenderingEXT,             Both,                  Both,               Graphics | Compute);
  VK_cmd_function(EndConditionalRenderingEXT,               Both,                  Both,               Graphics | Compute);

  VK_cmd_function(PreprocessGeneratedCommandsNV,            Both,                  Outside,            Graphics | Compute);
  VK_cmd_function(ExecuteGeneratedCommandsNV,               Both,                  Inside,             Graphics | Compute);

  VK_cmd_function(SetEvent,                                 Both,                  Outside,            Graphics | Compute);
  VK_cmd_function(ResetEvent,                               Both,                  Outside,            Graphics | Compute);
  VK_cmd_function(WaitEvents,                               Both,                  Both,               Graphics | Compute);




  // State Modification
  VK_cmd_function(PushConstants,                            Both,                  Both,               Graphics | Compute);
  VK_cmd_function(PushDescriptorSetKHR,                     Both,                  Both,               Graphics | Compute);
  VK_cmd_function(PushDescriptorSetWithTemplateKHR,         Both,                  Both,               Graphics | Compute);
  VK_cmd_function(BindPipeline,                             Both,                  Both,               Graphics | Compute);
  VK_cmd_function(BindDescriptorSets,                       Both,                  Both,               Graphics | Compute);
  VK_cmd_function(BindIndexBuffer,                          Both,                  Both,               Graphics);
  VK_cmd_function(BindVertexBuffers,                        Both,                  Both,               Graphics);
  VK_cmd_function(BindShadingRateImageNV,                   Both,                  Both,               Graphics);
  VK_cmd_function(BindTransformFeedbackBuffersEXT,          Both,                  Both,               Graphics);
  VK_cmd_function(BindPipelineShaderGroupNV,                Both,                  Both,               Graphics | Compute);

  VK_cmd_function(SetViewport,                              Both,                  Both,               Graphics);
  VK_cmd_function(SetScissor,                               Both,                  Both,               Graphics);
  VK_cmd_function(SetLineWidth,                             Both,                  Both,               Graphics);
  VK_cmd_function(SetDepthBias,                             Both,                  Both,               Graphics);
  VK_cmd_function(SetBlendConstants,                        Both,                  Both,               Graphics);
  VK_cmd_function(SetDepthBounds,                           Both,                  Both,               Graphics);
  VK_cmd_function(SetStencilCompareMask,                    Both,                  Both,               Graphics);
  VK_cmd_function(SetStencilWriteMask,                      Both,                  Both,               Graphics);
  VK_cmd_function(SetStencilReference,                      Both,                  Both,               Graphics);
  VK_cmd_function(SetViewportWScalingNV,                    Both,                  Both,               Graphics);
  VK_cmd_function(SetDiscardRectangleEXT,                   Both,                  Both,               Graphics);
  VK_cmd_function(SetSampleLocationsEXT,                    Both,                  Both,               Graphics);
  VK_cmd_function(SetViewportShadingRatePaletteNV,          Both,                  Both,               Graphics);
  VK_cmd_function(SetCoarseSampleOrderNV,                   Both,                  Both,               Graphics);
  VK_cmd_function(SetExclusiveScissorNV,                    Both,                  Both,               Graphics);
  VK_cmd_function(SetFragmentShadingRateKHR,                Both,                  Both,               Graphics);
  VK_cmd_function(SetFragmentShadingRateEnumNV,             Both,                  Both,               Graphics);
  VK_cmd_function(SetLineStippleEXT,                        Both,                  Both,               Graphics);
  VK_cmd_function(SetCullModeEXT,                           Both,                  Both,               Graphics);
  VK_cmd_function(SetFrontFaceEXT,                          Both,                  Both,               Graphics);
  VK_cmd_function(SetPrimitiveTopologyEXT,                  Both,                  Both,               Graphics);
  VK_cmd_function(SetViewportWithCountEXT,                  Both,                  Both,               Graphics);
  VK_cmd_function(SetScissorWithCountEXT,                   Both,                  Both,               Graphics);
  VK_cmd_function(BindVertexBuffers2EXT,                    Both,                  Both,               Graphics);
  VK_cmd_function(SetDepthTestEnableEXT,                    Both,                  Both,               Graphics);
  VK_cmd_function(SetDepthWriteEnableEXT,                   Both,                  Both,               Graphics);
  VK_cmd_function(SetDepthCompareOpEXT,                     Both,                  Both,               Graphics);
  VK_cmd_function(SetDepthBoundsTestEnableEXT,              Both,                  Both,               Graphics);
  VK_cmd_function(SetStencilTestEnableEXT,                  Both,                  Both,               Graphics);
  VK_cmd_function(SetStencilOpEXT,                          Both,                  Both,               Graphics);

  VK_cmd_function(SetRayTracingPipelineStackSizeKHR,        Both,                  Outside,            Compute);




  // Development Utilities
  VK_cmd_function(InsertDebugUtilsLabelEXT,                 Both,                  Both,               Graphics | Compute);
  VK_cmd_function(BeginDebugUtilsLabelEXT,                  Both,                  Both,               Graphics | Compute | Transfer);
  VK_cmd_function(EndDebugUtilsLabelEXT,                    Both,                  Both,               Graphics | Compute | Transfer);
  VK_cmd_function(SetCheckpointNV,                          Both,                  Both,               Graphics | Compute | Transfer);
  VK_cmd_function(SetPerformanceMarkerINTEL,                Both,                  Both,               Graphics | Compute | Transfer);
  VK_cmd_function(SetPerformanceStreamMarkerINTEL,          Both,                  Both,               Graphics | Compute | Transfer);
  VK_cmd_function(SetPerformanceOverrideINTEL,              Both,                  Both,               Graphics | Compute | Transfer);



  // Draw Commands
  VK_cmd_function(Draw,                                     Both,                  Inside,             Graphics,                      Graphics);
  VK_cmd_function(DrawIndexed,                              Both,                  Inside,             Graphics,                      Graphics);
  VK_cmd_function(DrawIndirect,                             Both,                  Inside,             Graphics,                      Graphics);
  VK_cmd_function(DrawIndirectCount,                        Both,                  Inside,             Graphics,                      Graphics);
  VK_cmd_function(DrawIndexedIndirect,                      Both,                  Inside,             Graphics,                      Graphics);
  VK_cmd_function(DrawIndexedIndirectCount,                 Both,                  Inside,             Graphics,                      Graphics);
  VK_cmd_function(DrawIndirectByteCountEXT,                 Both,                  Inside,             Graphics,                      Graphics);
  VK_cmd_function(DrawMeshTasksNV,                          Both,                  Inside,             Graphics,                      Graphics);
  VK_cmd_function(DrawMeshTasksIndirectNV,                  Both,                  Inside,             Graphics,                      Graphics);
  VK_cmd_function(DrawMeshTasksIndirectCountNV,             Both,                  Inside,             Graphics,                      Graphics);

  VK_cmd_function(Dispatch,                                 Both,                  Outside,            Compute,                       Compute);
  VK_cmd_function(DispatchIndirect,                         Both,                  Outside,            Compute,                       Compute);
  VK_cmd_function(DispatchBase,                             Both,                  Outside,            Compute,                       Compute);

  VK_cmd_function(TraceRaysKHR,                             Both,                  Outside,            Compute,                       RayTracing);
  VK_cmd_function(TraceRaysIndirectKHR,                     Both,                  Outside,            Compute,                       RayTracing);


  // Memory Commands
  //  |
  //  |-> Images
  VK_cmd_function(BlitImage,                                Both,                  Outside,            Graphics,                      Transfer);
  VK_cmd_function(BlitImage2KHR,                            Both,                  Outside,            Graphics,                      Transfer);
  VK_cmd_function(CopyImage,                                Both,                  Outside,            Graphics | Compute | Transfer, Transfer);
  VK_cmd_function(CopyImage2KHR,                            Both,                  Outside,            Graphics | Compute | Transfer, Transfer);
  VK_cmd_function(ResolveImage,                             Both,                  Outside,            Graphics,                      Transfer);
  VK_cmd_function(ResolveImage2KHR,                         Both,                  Outside,            Graphics,                      Transfer);
  VK_cmd_function(ClearColorImage,                          Both,                  Outside,            Graphics | Compute,            Transfer);
  VK_cmd_function(ClearDepthStencilImage,                   Both,                  Outside,            Graphics,                      Transfer);

  VK_cmd_function(CopyBuffer,                               Both,                  Outside,            Graphics | Compute | Transfer, Transfer);
  VK_cmd_function(CopyBuffer2KHR,                           Both,                  Outside,            Graphics | Compute | Transfer, Transfer);
  VK_cmd_function(UpdateBuffer,                             Both,                  Outside,            Graphics | Compute | Transfer, Transfer);
  VK_cmd_function(FillBuffer,                               Both,                  Outside,            Graphics | Compute | Transfer, Transfer);

  VK_cmd_function(CopyBufferToImage,                        Both,                  Outside,            Graphics | Compute | Transfer, Transfer);
  VK_cmd_function(CopyBufferToImage2KHR,                    Both,                  Outside,            Graphics | Compute | Transfer, Transfer);
  VK_cmd_function(CopyImageToBuffer,                        Both,                  Outside,            Graphics | Compute | Transfer, Transfer);
  VK_cmd_function(CopyImageToBuffer2KHR,                    Both,                  Outside,            Graphics | Compute | Transfer, Transfer);
  VK_cmd_function(ClearAttachments,                         Both,                  Inside,             Graphics,                      Graphics);

  VK_cmd_function(BuildAccelerationStructuresKHR,           Both,                  Outside,            Compute);
  VK_cmd_function(BuildAccelerationStructuresIndirectKHR,   Both,                  Outside,            Compute);
  VK_cmd_function(WriteAccelerationStructuresPropertiesKHR, Both,                  Outside,            Compute);
  VK_cmd_function(CopyAccelerationStructureKHR,             Both,                  Outside,            Compute);
  VK_cmd_function(CopyAccelerationStructureNV,              Both,                  Outside,            Compute);
  VK_cmd_function(CopyAccelerationStructureToMemoryKHR,     Both,                  Outside,            Compute);
  VK_cmd_function(CopyMemoryToAccelerationStructureKHR,     Both,                  Outside,            Compute);


  // Queries

  VK_cmd_function(WriteTimestamp,                           Both,                  Both,               Graphics | Compute | Transfer, Transfer);

  VK_cmd_function(BeginTransformFeedbackEXT,                Both,                  Inside,             Graphics);
  VK_cmd_function(EndTransformFeedbackEXT,                  Both,                  Inside,             Graphics);
  VK_cmd_function(BeginQueryIndexedEXT,                     Both,                  Both,               Graphics | Compute);
  VK_cmd_function(EndQueryIndexedEXT,                       Both,                  Both,               Graphics | Compute);
  VK_cmd_function(BeginQuery,                               Both,                  Both,               Graphics | Compute);
  VK_cmd_function(EndQuery,                                 Both,                  Both,               Graphics | Compute);

  VK_cmd_function(ResetQueryPool,                           Both,                  Outside,            Graphics | Compute);
  VK_cmd_function(CopyQueryPoolResults,                     Both,                  Outside,            Graphics | Compute,            Transfer);


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
  //VK_function(SetLocalDimmingAMD, setLocalDimming);

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