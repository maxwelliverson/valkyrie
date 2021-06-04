//
// Created by Maxwell on 2020-11-13.
//

#ifndef VALKYRIE_COMMAND_POOL_HPP
#define VALKYRIE_COMMAND_POOL_HPP

#include <valkyrie/async/agent.hpp>
#include <valkyrie/graphics/api/vulkan.hpp>

namespace valkyrie::graphics::api{

  class command_buffer;

  enum class cmd_msg_kind{
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

  /*class QueueFamily{
  public:

  };
  class queue {
    u32          familyIndex;
    u32          queueIndex;
    Flags::Queue flags;
  };


  struct viewport{
    float x;
    float y;
    float width;
    float height;
    interval<float> depth;
  };
  struct color{
    float a, r, g, b;
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

    Result<CommandBuffer> allocateCommandBuffer() noexcept;
    void                  freeCommandBuffer(const CommandBuffer&) noexcept;
    status allocateCommandBuffers(std::span<CommandBuffer> commandBuffers) noexcept;
  };

  class command_buffer {
    class Impl;
    Impl* pImpl;
  public:
    command_buffer(command_pool & pool) noexcept;


    status begin() noexcept;
    status end() noexcept;
    status reset() noexcept;


    void execute(const command_buffer& buffer) noexcept;
    void execute(std::span<command_buffer> buffers) noexcept;
    void begin_render_pass() noexcept;
    void next_subpass() noexcept;
    void end_render_pass() noexcept;

    void pipeline_barrier() noexcept;
    void set_device_mask() noexcept;
    void begin_conditional_rendering() noexcept;
    void end_conditional_rendering() noexcept;

    void preprocess_generated_commands() noexcept;
    void execute_generated_commands() noexcept;

    void set_event() noexcept;
    void reset_event() noexcept;
    void wait_events() noexcept;

    void push_constants() noexcept;
    void push_descriptor_set() noexcept;
    void push_descriptor_set_with_template() noexcept;

    void bind_pipeline() noexcept;
    void bind_descriptor_sets() noexcept;
    void bind_index_buffer() noexcept;
    void bind_vertex_buffers() noexcept;
    void bind_shading_rate_image() noexcept;
    void bind_transform_feedback_buffers() noexcept;
    void bind_pipeline_shader_group() noexcept;

    void set_viewport(u32 firstViewport, std::span<viewport> viewports) noexcept;
    void set_scissor(u32 firstScissor,   std::span<Rect2D> scissors) noexcept;
    void set_line_width(float lineWidth) noexcept;
    void set_depth_bias(float constantFactor, float clamp, float slopeFactor) noexcept;
    void set_blend_constants(std::span<float, 4> constants) noexcept;
    void set_depth_bounds(interval<float> bounds) noexcept;
    void set_stencil_compare_mask(StencilFaceFlags faceMask, u32 compareMask) noexcept;
    void set_stencil_write_mask(StencilFaceFlags faceMask, u32 writeMask) noexcept;
    void set_stencil_reference(StencilFaceFlags faceMask, u32 reference) noexcept;
    void set_viewport_w_scaling(u32 firstViewport, std::span<Position2D<float>> viewportWScalings) noexcept;
    void set_discard_rectangle(u32 firstDiscardRectangle, std::span<Rect2D> discardRectangles) noexcept;
    void set_sample_locations() noexcept;
    void set_viewport_shading_rate_palette() noexcept;
    void set_coarse_sample_order() noexcept;
    void set_exclusive_scissor() noexcept;
    void set_fragment_shading_rate() noexcept;
    void set_fragment_shading_rate_enum() noexcept;
    void set_line_stipple(u32 factor, u16 pattern) noexcept;
    void set_cull_mode(CullModeFlags cullMode) noexcept;
    void set_front_face() noexcept;
    void set_primitive_topology(PrimitiveTopology topology) noexcept;
    void set_viewport_with_count() noexcept;
    void set_scissor_with_count() noexcept;
    void set_depth_test_enable() noexcept;
    void set_depth_write_enable() noexcept;
    void set_depth_compare_op() noexcept;
    void set_depth_bounds_test_enable() noexcept;
    void set_stencil_test_enable() noexcept;
    void set_stencil_op() noexcept;

    void set_ray_tracing_pipeline_stack_size(u32 stackSize) noexcept;


    void insert_debug_utils_label(string_view name) noexcept;
    void insert_debug_utils_label(string_view name, color labelColor) noexcept;
    void begin_debug_utils_label(string_view name) noexcept;
    void begin_debug_utils_label(string_view name, color labelColor) noexcept;
    void end_debug_utils_label() noexcept;

    void set_checkpoint(void* pValue) noexcept;

    void set_performance_marker() noexcept;
    void set_performance_stream_marker() noexcept;
    void set_performance_override() noexcept;


    void draw(u32 vertexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance) noexcept;
    void draw_indexed() noexcept;
    void draw_indirect() noexcept;
    void draw_indirect_count() noexcept;
    void draw_indexed_indirect() noexcept;
    void draw_indexed_indirect_count() noexcept;
    void draw_indirect_byte_count() noexcept;
    void draw_mesh_tasks() noexcept;
    void draw_mesh_tasks_indirect() noexcept;
    void draw_mesh_tasks_indirect_count() noexcept;


    void dispatch(Extent3D<u32> extents) noexcept;
    void dispatch_indirect() noexcept;
    void dispatch_base() noexcept;

    void trace_rays() noexcept;
    void trace_rays_indirect() noexcept;


    void blit_image() noexcept;
    void copy_image() noexcept;
    void resolve_image() noexcept;
    void clear_color_image() noexcept;
    void clear_depth_stencil_image() noexcept;

    void copy_buffer() noexcept;
    void update_buffer() noexcept;
    void fill_buffer() noexcept;

    void copy_buffer_to_image() noexcept;
    void copy_image_to_buffer() noexcept;
    void clear_attachments() noexcept;

    void build_acceleration_structures() noexcept;
    void build_acceleration_structures_indirect() noexcept;
    void write_acceleration_structures_properties() noexcept;
    void copy_acceleration_structure() noexcept;
    void copy_acceleration_structure_to_memory() noexcept;
    void copy_memory_to_acceleration_structure() noexcept;


    void write_timestamp() noexcept;

    void begin_transform_feedback() noexcept;
    void end_transform_feedback() noexcept;
    void begin_query_indexed() noexcept;
    void end_query_indexed() noexcept;
    void begin_query() noexcept;
    void end_query() noexcept;

    void reset_query_pool() noexcept;
    void copy_query_pool_results() noexcept;
    void begin_recording_graphics_pipeline() noexcept;
    void end_recording_graphics_pipeline() noexcept;
  };*/


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
