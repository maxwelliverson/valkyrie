//
// Created by Maxwell on 2020-11-13.
//

#ifndef VALKYRIE_COMMAND_POOL_HPP
#define VALKYRIE_COMMAND_POOL_HPP

#include <valkyrie/async/agent.hpp>
#include <valkyrie/graphics/api/vulkan.hpp>

namespace valkyrie::graphics::api{

  class command_buffer;

  struct CommandPoolMessageDomain{

    using status_domain = VulkanStatusDomain;



    struct Processor{
      virtual status destroyCommandPool()
    };
  };



  struct ICommandPoolMsgProcessor{
    virtual
  };


  enum class CommandPoolMsg{
    DestroyCommandPool,
    ResetCommandPool,
    TrimCommandPool,
    AllocateCommandBuffers,
    FreeCommandBuffers,


    BeginCommandBuffer,
    EndCommandBuffer,
    ResetCommandBuffer,

    // Control Flow
    ExecuteCommands,
    BeginRenderPass,
    BeginRenderPass2,
    NextSubpass,
    NextSubpass2,
    EndRenderPass,
    EndRenderPass2,

    PipelineBarrier,
    SetDeviceMask,
    BeginConditionalRenderingEXT,
    EndConditionalRenderingEXT,

    PreprocessGeneratedCommandsNV,
    ExecuteGeneratedCommandsNV,

    SetEvent,
    ResetEvent,
    WaitEvents,




    // State Modification
    PushConstants,
    PushDescriptorSetKHR,
    PushDescriptorSetWithTemplateKHR,
    BindPipeline,
    BindDescriptorSets,
    BindIndexBuffer,
    BindVertexBuffers,
    BindShadingRateImageNV,
    BindTransformFeedbackBuffersEXT,
    BindPipelineShaderGroupNV,

    SetViewport,
    SetScissor,
    SetLineWidth,
    SetDepthBias,
    SetBlendConstants,
    SetDepthBounds,
    SetStencilCompareMask,
    SetStencilWriteMask,
    SetStencilReference,
    SetViewportWScalingNV,
    SetDiscardRectangleEXT,
    SetSampleLocationsEXT,
    SetViewportShadingRatePaletteNV,
    SetCoarseSampleOrderNV,
    SetExclusiveScissorNV,
    SetFragmentShadingRateKHR,
    SetFragmentShadingRateEnumNV,
    SetLineStippleEXT,
    SetCullModeEXT,
    SetFrontFaceEXT,
    SetPrimitiveTopologyEXT,
    SetViewportWithCountEXT,
    SetScissorWithCountEXT,
    BindVertexBuffers2EXT,
    SetDepthTestEnableEXT,
    SetDepthWriteEnableEXT,
    SetDepthCompareOpEXT,
    SetDepthBoundsTestEnableEXT,
    SetStencilTestEnableEXT,
    SetStencilOpEXT,

    SetRayTracingPipelineStackSizeKHR,




    // Development Utilities
    InsertDebugUtilsLabelEXT,
    BeginDebugUtilsLabelEXT,
    EndDebugUtilsLabelEXT,
    SetCheckpointNV,
    SetPerformanceMarkerINTEL,
    SetPerformanceStreamMarkerINTEL,
    SetPerformanceOverrideINTEL,



    // Draw Commands
    Draw,
    DrawIndexed,
    DrawIndirect,
    DrawIndirectCount,
    DrawIndexedIndirect,
    DrawIndexedIndirectCount,
    DrawIndirectByteCountEXT,
    DrawMeshTasksNV,
    DrawMeshTasksIndirectNV,
    DrawMeshTasksIndirectCountNV,

    Dispatch,
    DispatchIndirect,
    DispatchBase,

    TraceRaysKHR,
    TraceRaysIndirectKHR,


    // Memory Commands
    //  |
    //  |-> Images
    BlitImage,
    BlitImage2KHR,
    CopyImage,
    CopyImage2KHR,
    ResolveImage,
    ResolveImage2KHR,
    ClearColorImage,
    ClearDepthStencilImage,

    CopyBuffer,
    CopyBuffer2KHR,
    UpdateBuffer,
    FillBuffer,

    CopyBufferToImage,
    CopyBufferToImage2KHR,
    CopyImageToBuffer,
    CopyImageToBuffer2KHR,
    ClearAttachments,

    BuildAccelerationStructuresKHR,
    BuildAccelerationStructuresIndirectKHR,
    WriteAccelerationStructuresPropertiesKHR,
    CopyAccelerationStructureKHR,
    CopyAccelerationStructureNV,
    CopyAccelerationStructureToMemoryKHR,
    CopyMemoryToAccelerationStructureKHR,


    // Queries

    WriteTimestamp,

    BeginTransformFeedbackEXT,
    EndTransformFeedbackEXT,
    BeginQueryIndexedEXT,
    EndQueryIndexedEXT,
    BeginQuery,
    EndQuery,

    ResetQueryPool,
    CopyQueryPoolResults,
    BeginRecordingGraphicsPipeline,
    EndRecordingGraphicsPipeline
  };

  class QueueFamily{
  public:

  };
  class queue {
    u32          familyIndex;
    u32          queueIndex;
    Flags::Queue flags;
  };

  class command_pool {
    class Impl;
    Impl* pImpl;



    AgentMailbox urgentMailbox;
    AgentMailbox normalMailbox;
    AgentMailbox lazyMailbox;



  public:
    command_pool(const device & device);

    Result<command_buffer> makePrimaryCommandBuffer() noexcept;
    Result<command_buffer> makeSecondaryCommandBuffer() noexcept;

    void destroy() noexcept;
    void trim() noexcept;
    status reset() noexcept;

    /*Result<CommandBuffer> allocateCommandBuffer() noexcept;
    void                  freeCommandBuffer(const CommandBuffer&) noexcept;
    status allocateCommandBuffers(std::span<CommandBuffer> commandBuffers) noexcept;*/
  };

  class command_buffer {
    class Impl;
    Impl* pImpl;
  public:
    command_buffer(command_pool & pool) noexcept;


    status begin() noexcept;
  };





  /* Threading
   *
   *

    The instance parameter in                 vkDestroyInstance
    The device parameter in                   vkDestroyDevice
    The event parameter in                    vkDestroyEvent
    The queryPool parameter in                vkDestroyQueryPool
    The buffer parameter in                   vkDestroyBuffer
    The bufferView parameter in               vkDestroyBufferView
    The image parameter in                    vkDestroyImage
    The imageView parameter in                vkDestroyImageView
    The shaderModule parameter in             vkDestroyShaderModule
    The pipelineCache parameter in            vkDestroyPipelineCache
    The pipeline parameter in                 vkDestroyPipeline
    The pipelineLayout parameter in           vkDestroyPipelineLayout
    The sampler parameter in                  vkDestroySampler
    The descriptorSetLayout parameter in      vkDestroyDescriptorSetLayout
    The descriptorPool parameter in           vkDestroyDescriptorPool
    The framebuffer parameter in              vkDestroyFramebuffer
    The renderPass parameter in               vkDestroyRenderPass
    The ycbcrConversion parameter in          vkDestroySamplerYcbcrConversion
    The descriptorUpdateTemplate parameter in vkDestroyDescriptorUpdateTemplate
    The semaphore parameter in                vkDestroySemaphore
    The surface parameter in                  vkDestroySurfaceKHR
    The fence parameter in                    vkDestroyFence
    The swapchain parameter in                vkDestroySwapchainKHR
    The descriptorUpdateTemplate parameter in vkDestroyDescriptorUpdateTemplateKHR
    The indirectCommandsLayout parameter in   vkDestroyIndirectCommandsLayoutNV
    The privateDataSlot parameter in          vkDestroyPrivateDataSlotEXT
    The accelerationStructure parameter in    vkDestroyAccelerationStructureKHR
    The accelerationStructure parameter in    vkDestroyAccelerationStructureNV
    The messenger parameter in                vkDestroyDebugUtilsMessengerEXT
    The validationCache parameter in          vkDestroyValidationCacheEXT
    The ycbcrConversion parameter in          vkDestroySamplerYcbcrConversionKHR
    The operation parameter in                vkDestroyDeferredOperationKHR
    The callback parameter in                 vkDestroyDebugReportCallbackEXT


    The queue parameter in vkQueuePresentKHR
    The queue parameter in vkQueueSubmit
    The queue parameter in vkQueueBindSparse
    The queue parameter in vkQueueWaitIdle


    The fence parameter in vkQueueSubmit
    The fence parameter in vkQueueBindSparse
    The fence parameter in vkAcquireNextImageKHR

    The memory parameter in vkFreeMemory
    The memory parameter in vkMapMemory
    The memory parameter in vkUnmapMemory


    The buffer parameter in vkBindBufferMemory

    The image parameter in  vkBindImageMemory

    The event parameter in vkSetEvent
    The event parameter in vkResetEvent




    The object member of the pTagInfo parameter in vkDebugMarkerSetObjectTagEXT
    The object member of the pNameInfo parameter in vkDebugMarkerSetObjectNameEXT
    The objectHandle member of the pNameInfo parameter in vkSetDebugUtilsObjectNameEXT
    The objectHandle member of the pTagInfo parameter in vkSetDebugUtilsObjectTagEXT



    The dstCache parameter in vkMergePipelineCaches

    The descriptorPool parameter in vkResetDescriptorPool
    The descriptorPool member of the pAllocateInfo parameter in vkAllocateDescriptorSets
    The descriptorPool parameter in vkFreeDescriptorSets
    The descriptorSet parameter in vkUpdateDescriptorSetWithTemplate

    The surface member of the pCreateInfo parameter in vkCreateSwapchainKHR
    The surface parameter in vkGetDeviceGroupSurfacePresentModesKHR
    The surface parameter in vkGetPhysicalDevicePresentRectanglesKHR

    The oldSwapchain member of the pCreateInfo parameter in vkCreateSwapchainKHR
    The swapchain parameter in vkAcquireNextImageKHR
    The semaphore parameter in vkAcquireNextImageKHR



    The display parameter in vkCreateDisplayModeKHR
    The mode parameter in vkGetDisplayPlaneCapabilitiesKHR


    The swapchain parameter in vkGetSwapchainStatusKHR
    The swapchain parameter in vkGetRefreshCycleDurationGOOGLE
    The swapchain parameter in vkGetPastPresentationTimingGOOGLE

    The dstCache parameter in vkMergeValidationCachesEXT

    The configuration parameter in vkReleasePerformanceConfigurationINTEL

    The buffer member of each element of the pBufferBinds member of each element of the pBindInfo parameter in vkQueueBindSparse
    The image member of each element of the pImageOpaqueBinds member of each element of the pBindInfo parameter in vkQueueBindSparse
    The image member of each element of the pImageBinds member of each element of the pBindInfo parameter in vkQueueBindSparse
    Each element of the pFences parameter in vkResetFences
    Each element of the pDescriptorSets parameter in vkFreeDescriptorSets
    The dstSet member of each element of the pDescriptorWrites parameter in vkUpdateDescriptorSets
    The dstSet member of each element of the pDescriptorCopies parameter in vkUpdateDescriptorSets
    Each element of the pCommandBuffers parameter in vkFreeCommandBuffers
    Each element of the pWaitSemaphores member of the pPresentInfo parameter in vkQueuePresentKHR
    Each element of the pSwapchains member of the pPresentInfo parameter in vkQueuePresentKHR
    The surface member of each element of the pCreateInfos parameter in vkCreateSharedSwapchainsKHR
    The oldSwapchain member of each element of the pCreateInfos parameter in vkCreateSharedSwapchainsKHR



    All VkPhysicalDevice objects enumerated from instance in vkDestroyInstance
    All VkQueue objects received from device in vkDestroyDevice
    All VkQueue objects created from device in vkDeviceWaitIdle
    Any VkDescriptorSet objects allocated from descriptorPool in vkResetDescriptorPool



   * */
}

#endif//VALKYRIE_COMMAND_POOL_HPP
