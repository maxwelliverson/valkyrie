//
// Created by maxwe on 2021-04-23.
//

#include "api.hpp"


#include <valkyrie/async/atomic.hpp>

using namespace valkyrie;
using namespace valkyrie::graphics::api;

namespace api = graphics::api;


static_assert(VK_offsetof(viewport, x)      == VK_offsetof(VkViewport, x));
static_assert(VK_offsetof(viewport, y)      == VK_offsetof(VkViewport, y));
//static_assert(VK_offsetof(viewport, width)  == VK_offsetof(VkViewport, width));
//static_assert(VK_offsetof(viewport, height) == VK_offsetof(VkViewport, height));
static_assert(offset_of<&viewport::depth>()   == offset_of<&VkViewport::minDepth>());
static_assert(sizeof(viewport) == sizeof(VkViewport));
static_assert(alignof(viewport) == alignof(VkViewport));

#define load_fn(fn) (PFN_vk##fn)(pfn_get_instance_proc_addr(handle, "vk" #fn))

namespace {

  inline constexpr char lib_name[] = "vulkan-1";

  PFN_vkGetInstanceProcAddr                  pfn_get_instance_proc_addr;
  PFN_vkCreateInstance                       pfn_create_instance;
  PFN_vkEnumerateInstanceVersion             pfn_instance_version;
  PFN_vkEnumerateInstanceExtensionProperties pfn_enumerate_instance_extensions;
  PFN_vkEnumerateInstanceLayerProperties     pfn_enumerate_layers;



  struct instance_t{

    VkResult initialize(const VkInstanceCreateInfo& createInfo, const VkAllocationCallbacks* callbacks) noexcept {
      VkResult result = pfn_create_instance(&createInfo, callbacks, &handle);
      if ( result == VK_SUCCESS ) {
        allocationCallbacks = callbacks;

#define load_fn(fn) (PFN_vk##fn)(pfn_get_instance_proc_addr(handle, "vk" #fn))

        pfn_destroy = load_fn(DestroyInstance);
        pfn_enumerate_physical_devices = load_fn(EnumeratePhysicalDevices);
        pfn_enumerate_physical_device_groups = load_fn(EnumeratePhysicalDeviceGroups);

        pfn_create_win32_surface = load_fn(CreateWin32SurfaceKHR);
        pfn_create_display_plane_surface = load_fn(CreateDisplayPlaneSurfaceKHR);
        pfn_create_headless_surface = load_fn(CreateHeadlessSurfaceEXT);
        pfn_destroy_surface = load_fn(DestroySurfaceKHR);

        pfn_create_debug_utils_messenger = load_fn(CreateDebugUtilsMessengerEXT);
        pfn_destroy_debug_utils_messenger = load_fn(DestroyDebugUtilsMessengerEXT);
        pfn_submit_debug_utils_message = load_fn(SubmitDebugUtilsMessageEXT);



        // [ physical_device ]

        pfn_create_device = load_fn(CreateDevice);
        pfn_enumerate_device_extension_properties = load_fn(EnumerateDeviceExtensionProperties);

        pfn_get_features = load_fn(GetPhysicalDeviceFeatures2);
        pfn_get_properties = load_fn(GetPhysicalDeviceProperties2);
        pfn_get_format_properties = load_fn(GetPhysicalDeviceFormatProperties2);
        pfn_get_image_format_properties = load_fn(GetPhysicalDeviceImageFormatProperties2);
        pfn_get_sparse_image_format_properties = load_fn(GetPhysicalDeviceSparseImageFormatProperties2);
        pfn_get_queue_family_properties = load_fn(GetPhysicalDeviceQueueFamilyProperties2);

        pfn_get_memory_properties = load_fn(GetPhysicalDeviceMemoryProperties2);
        pfn_get_external_buffer_properties = load_fn(GetPhysicalDeviceExternalBufferProperties);
        pfn_get_external_fence_properties = load_fn(GetPhysicalDeviceExternalFenceProperties);
        pfn_get_external_semaphore_properties = load_fn(GetPhysicalDeviceExternalSemaphoreProperties);
        pfn_get_external_image_format_properties = load_fn(GetPhysicalDeviceExternalImageFormatPropertiesNV);

        pfn_get_win32_presentation_support = load_fn(GetPhysicalDeviceWin32PresentationSupportKHR);
        pfn_get_surface_support = load_fn(GetPhysicalDeviceSurfaceSupportKHR);

        pfn_get_surface_capabilities = load_fn(GetPhysicalDeviceSurfaceCapabilities2KHR);
        pfn_get_surface_formats = load_fn(GetPhysicalDeviceSurfaceFormats2KHR);
        pfn_get_surface_present_modes = load_fn(GetPhysicalDeviceSurfacePresentModes2EXT);
        pfn_get_present_rectangles = load_fn(GetPhysicalDevicePresentRectanglesKHR);


        pfn_create_display_mode = load_fn(CreateDisplayModeKHR);
        pfn_get_display_mode_properties = load_fn(GetDisplayModeProperties2KHR);
        pfn_get_display_properties = load_fn(GetPhysicalDeviceDisplayProperties2KHR);
        pfn_get_display_plane_properties = load_fn(GetPhysicalDeviceDisplayPlaneProperties2KHR);
        pfn_get_display_plane_capabilities = load_fn(GetDisplayPlaneCapabilities2KHR);
        pfn_get_display_plane_supported_displays = load_fn(GetDisplayPlaneSupportedDisplaysKHR);
        pfn_enumerate_queue_family_performance_query_counters = load_fn(EnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR);
        pfn_get_queue_family_performance_query_passes = load_fn(GetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR);


        pfn_release_display = load_fn(ReleaseDisplayEXT);

        pfn_get_multisample_properties = load_fn(GetPhysicalDeviceMultisamplePropertiesEXT);
        pfn_get_calibrateable_time_domains = load_fn(GetPhysicalDeviceCalibrateableTimeDomainsEXT);
        pfn_get_tool_properties = load_fn(GetPhysicalDeviceToolPropertiesEXT);
        pfn_get_cooperative_matrix_properties = load_fn(GetPhysicalDeviceCooperativeMatrixPropertiesNV);
        pfn_get_supported_framebuffer_mixed_samples_combinations = load_fn(GetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV);

#undef load_fn
      }
      return result;
    }


    void         destroy() const noexcept {
      pfn_destroy(handle, allocationCallbacks);
    }

    outcome<u32> count_physical_devices() const noexcept {
      outcome<u32> outcome;
      outcome.result = pfn_enumerate_physical_devices(handle, &outcome.value, nullptr);
      return outcome;
    }
    VkResult     enumerate_physical_devices(u32 count, VkPhysicalDevice* devices) const noexcept {
      return pfn_enumerate_physical_devices(handle, &count, devices);
    }
    outcome<u32> count_physical_device_groups() const noexcept {
      outcome<u32> outcome;
      outcome.result = pfn_enumerate_physical_device_groups(handle, &outcome.value, nullptr);
      return outcome;
    }
    VkResult     enumerate_physical_device_groups(u32 count, VkPhysicalDeviceGroupProperties* deviceGroups) const noexcept {
      return pfn_enumerate_physical_device_groups(handle, &count, deviceGroups);
    }

    outcome<VkSurfaceKHR> create_surface(const VkWin32SurfaceCreateInfoKHR& createInfo, const VkAllocationCallbacks* allocCallbacks = nullptr) const noexcept {
      outcome<VkSurfaceKHR> outcome;
      outcome.result = pfn_create_win32_surface(handle, &createInfo, allocCallbacks, &outcome.value);
      return outcome;
    }
    outcome<VkSurfaceKHR> create_headless_surface(const VkHeadlessSurfaceCreateInfoEXT& createInfo, const VkAllocationCallbacks* allocCallbacks = nullptr) const noexcept {
      outcome<VkSurfaceKHR> outcome;
      outcome.result = pfn_create_headless_surface(handle, &createInfo, allocCallbacks, &outcome.value);
      return outcome;
    }
    outcome<VkSurfaceKHR> create_display_plane_surface(const VkDisplaySurfaceCreateInfoKHR& createInfo, const VkAllocationCallbacks* allocCallbacks = nullptr) const noexcept {
      outcome<VkSurfaceKHR> outcome;
      outcome.result = pfn_create_display_plane_surface(handle, &createInfo, allocCallbacks, &outcome.value);
      return outcome;
    }
    void                  destroy_surface(VkSurfaceKHR surface, const VkAllocationCallbacks* allocCallbacks = nullptr) const noexcept {
      pfn_destroy_surface(handle, surface, allocCallbacks);
    }

    outcome<VkDebugUtilsMessengerEXT> create_debug_messenger(const VkDebugUtilsMessengerCreateInfoEXT& createInfo, const VkAllocationCallbacks* allocCallbacks = nullptr) const noexcept {
      outcome<VkDebugUtilsMessengerEXT> outcome;
      outcome.result = pfn_create_debug_utils_messenger(handle, &createInfo, allocCallbacks, &outcome.value);
      return outcome;
    }
    void                              submit_debug_message(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT& data) const noexcept {
      pfn_submit_debug_utils_message(handle, severity, type, &data);
    }
    void                              destroy_debug_messenger(VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* allocCallbacks = nullptr) const noexcept {
      pfn_destroy_debug_utils_messenger(handle, messenger, allocCallbacks);
    }



    VkInstance                          handle;
    const VkAllocationCallbacks*        allocationCallbacks;

    PFN_vkDestroyInstance               pfn_destroy;
    PFN_vkEnumeratePhysicalDevices      pfn_enumerate_physical_devices;
    PFN_vkEnumeratePhysicalDeviceGroups pfn_enumerate_physical_device_groups;

    PFN_vkCreateWin32SurfaceKHR         pfn_create_win32_surface;
    PFN_vkCreateDisplayPlaneSurfaceKHR  pfn_create_display_plane_surface;
    PFN_vkCreateHeadlessSurfaceEXT      pfn_create_headless_surface;
    PFN_vkDestroySurfaceKHR             pfn_destroy_surface;

    PFN_vkCreateDebugUtilsMessengerEXT  pfn_create_debug_utils_messenger;
    PFN_vkDestroyDebugUtilsMessengerEXT pfn_destroy_debug_utils_messenger;
    PFN_vkSubmitDebugUtilsMessageEXT    pfn_submit_debug_utils_message;


    PFN_vkGetDeviceProcAddr             pfn_get_device_proc_addr;



    // [ physical_device ]

  public:

    PFN_vkCreateDevice pfn_create_device;
    PFN_vkEnumerateDeviceExtensionProperties pfn_enumerate_device_extension_properties;

    PFN_vkGetPhysicalDeviceFeatures2 pfn_get_features;
    PFN_vkGetPhysicalDeviceProperties2 pfn_get_properties;
    PFN_vkGetPhysicalDeviceFormatProperties2 pfn_get_format_properties;
    PFN_vkGetPhysicalDeviceImageFormatProperties2 pfn_get_image_format_properties;
    PFN_vkGetPhysicalDeviceSparseImageFormatProperties2 pfn_get_sparse_image_format_properties;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties2 pfn_get_queue_family_properties;

    PFN_vkGetPhysicalDeviceMemoryProperties2 pfn_get_memory_properties;
    PFN_vkGetPhysicalDeviceExternalBufferProperties pfn_get_external_buffer_properties;
    PFN_vkGetPhysicalDeviceExternalFenceProperties pfn_get_external_fence_properties;
    PFN_vkGetPhysicalDeviceExternalSemaphoreProperties pfn_get_external_semaphore_properties;
    PFN_vkGetPhysicalDeviceExternalImageFormatPropertiesNV pfn_get_external_image_format_properties;

    PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR pfn_get_win32_presentation_support;
    PFN_vkGetPhysicalDeviceSurfaceSupportKHR           pfn_get_surface_support;

    PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR     pfn_get_surface_capabilities;
    PFN_vkGetPhysicalDeviceSurfaceFormats2KHR          pfn_get_surface_formats;
    PFN_vkGetPhysicalDeviceSurfacePresentModes2EXT     pfn_get_surface_present_modes;
    PFN_vkGetPhysicalDevicePresentRectanglesKHR        pfn_get_present_rectangles;


    PFN_vkCreateDisplayModeKHR pfn_create_display_mode;
    PFN_vkGetDisplayModeProperties2KHR pfn_get_display_mode_properties;
    PFN_vkGetPhysicalDeviceDisplayProperties2KHR pfn_get_display_properties;
    PFN_vkGetPhysicalDeviceDisplayPlaneProperties2KHR pfn_get_display_plane_properties;
    PFN_vkGetDisplayPlaneCapabilities2KHR pfn_get_display_plane_capabilities;
    PFN_vkGetDisplayPlaneSupportedDisplaysKHR pfn_get_display_plane_supported_displays;
    PFN_vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR pfn_enumerate_queue_family_performance_query_counters;
    PFN_vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR pfn_get_queue_family_performance_query_passes;


    PFN_vkGetWinrtDisplayNV pfn_get_winrt_display;
    PFN_vkAcquireWinrtDisplayNV pfn_acquire_winrt_display;
    PFN_vkReleaseDisplayEXT pfn_release_display;

    PFN_vkGetPhysicalDeviceMultisamplePropertiesEXT pfn_get_multisample_properties;
    PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT pfn_get_calibrateable_time_domains;
    PFN_vkGetPhysicalDeviceToolPropertiesEXT pfn_get_tool_properties;
    PFN_vkGetPhysicalDeviceCooperativeMatrixPropertiesNV pfn_get_cooperative_matrix_properties;
    PFN_vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV pfn_get_supported_framebuffer_mixed_samples_combinations;
  };




  inline instance_t instance{};

  DWORD init_library() noexcept {
    HMODULE lib_handle = LoadLibrary(lib_name);
    if ( !lib_handle )
      return GetLastError();
    HMODULE tmp;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_PIN, lib_name, &tmp)) {
      return GetLastError();
    }
    pfn_get_instance_proc_addr = (PFN_vkGetInstanceProcAddr)GetProcAddress(lib_handle, "vkGetInstanceProcAddr");
    if (!pfn_get_instance_proc_addr)
      return GetLastError();

    pfn_create_instance = (PFN_vkCreateInstance)pfn_get_instance_proc_addr(nullptr, "vkCreateInstance");
    pfn_enumerate_instance_extensions = (PFN_vkEnumerateInstanceExtensionProperties)pfn_get_instance_proc_addr(nullptr, "vkEnumerateInstanceExtensionProperties");
    pfn_enumerate_layers = (PFN_vkEnumerateInstanceLayerProperties)pfn_get_instance_proc_addr(nullptr, "vkEnumerateInstanceLayerProperties");
    pfn_instance_version = (PFN_vkEnumerateInstanceVersion)pfn_get_instance_proc_addr(nullptr, "vkEnumerateInstanceVersion");

    VK_assert( pfn_create_instance != nullptr );
    VK_assert( pfn_enumerate_instance_extensions != nullptr );
    VK_assert( pfn_enumerate_layers != nullptr );
    VK_assert( pfn_instance_version != nullptr );

    return NO_ERROR;
  }

  void destroy_instance() noexcept {
    instance.destroy();
  }
}

#define load_fn(fn) (PFN_vk##fn)(getDeviceProcAddr(handle, "vk" #fn))

struct api::impl::command_buffer_api{

  void initialize(PFN_vkGetDeviceProcAddr getDeviceProcAddr, VkDevice handle) noexcept {
    pfn_beginCommandBuffer = load_fn(BeginCommandBuffer);
    pfn_endCommandBuffer = load_fn(EndCommandBuffer);
    pfn_resetCommandBuffer = load_fn(ResetCommandBuffer);

    // Control Flow
    pfn_cmdExecuteCommands = load_fn(CmdExecuteCommands);
    pfn_cmdBeginRenderPass = load_fn(CmdBeginRenderPass);
    pfn_cmdBeginRenderPass2 = load_fn(CmdBeginRenderPass2);
    pfn_cmdNextSubpass = load_fn(CmdNextSubpass);
    pfn_cmdNextSubpass2 = load_fn(CmdNextSubpass2);
    pfn_cmdEndRenderPass = load_fn(CmdEndRenderPass);
    pfn_cmdEndRenderPass2 = load_fn(CmdEndRenderPass2);


    pfn_cmdPipelineBarrier = load_fn(CmdPipelineBarrier2KHR);
    pfn_cmdSetDeviceMask = load_fn(CmdSetDeviceMask);
    pfn_cmdBeginConditionalRenderingEXT = load_fn(CmdBeginConditionalRenderingEXT);
    pfn_cmdEndConditionalRenderingEXT = load_fn(CmdEndConditionalRenderingEXT);

    pfn_cmdPreprocessGeneratedCommandsNV = load_fn(CmdPreprocessGeneratedCommandsNV);
    pfn_cmdExecuteGeneratedCommandsNV = load_fn(CmdExecuteGeneratedCommandsNV);

    pfn_cmdSetEvent = load_fn(CmdSetEvent2KHR);
    pfn_cmdResetEvent = load_fn(CmdResetEvent2KHR);
    pfn_cmdWaitEvents = load_fn(CmdWaitEvents2KHR);


    // State Modification
    pfn_cmdPushConstants = load_fn(CmdPushConstants);
    pfn_cmdPushDescriptorSetKHR = load_fn(CmdPushDescriptorSetKHR);
    pfn_cmdPushDescriptorSetWithTemplateKHR = load_fn(CmdPushDescriptorSetWithTemplateKHR);
    pfn_cmdBindPipeline = load_fn(CmdBindPipeline);
    pfn_cmdBindDescriptorSets = load_fn(CmdBindDescriptorSets);
    pfn_cmdBindIndexBuffer = load_fn(CmdBindIndexBuffer);
    pfn_cmdBindVertexBuffers = load_fn(CmdBindVertexBuffers);
    pfn_cmdBindShadingRateImageNV = load_fn(CmdBindShadingRateImageNV);
    pfn_cmdBindTransformFeedbackBuffersEXT = load_fn(CmdBindTransformFeedbackBuffersEXT);
    pfn_cmdBindPipelineShaderGroupNV = load_fn(CmdBindPipelineShaderGroupNV);

    pfn_cmdSetViewport = load_fn(CmdSetViewport);
    pfn_cmdSetScissor = load_fn(CmdSetScissor);
    pfn_cmdSetLineWidth = load_fn(CmdSetLineWidth);
    pfn_cmdSetDepthBias = load_fn(CmdSetDepthBias);
    pfn_cmdSetBlendConstants = load_fn(CmdSetBlendConstants);
    pfn_cmdSetDepthBounds = load_fn(CmdSetDepthBounds);
    pfn_cmdSetStencilCompareMask = load_fn(CmdSetStencilCompareMask);
    pfn_cmdSetStencilWriteMask = load_fn(CmdSetStencilWriteMask);
    pfn_cmdSetStencilReference = load_fn(CmdSetStencilReference);
    pfn_cmdSetViewportWScalingNV = load_fn(CmdSetViewportWScalingNV);
    pfn_cmdSetDiscardRectangleEXT = load_fn(CmdSetDiscardRectangleEXT);
    pfn_cmdSetSampleLocationsEXT = load_fn(CmdSetSampleLocationsEXT);
    pfn_cmdSetViewportShadingRatePaletteNV = load_fn(CmdSetViewportShadingRatePaletteNV);
    pfn_cmdSetCoarseSampleOrderNV = load_fn(CmdSetCoarseSampleOrderNV);
    pfn_cmdSetExclusiveScissorNV = load_fn(CmdSetExclusiveScissorNV);
    pfn_cmdSetFragmentShadingRateKHR = load_fn(CmdSetFragmentShadingRateKHR);
    pfn_cmdSetFragmentShadingRateEnumNV = load_fn(CmdSetFragmentShadingRateEnumNV);
    pfn_cmdSetLineStippleEXT = load_fn(CmdSetLineStippleEXT);
    pfn_cmdSetCullModeEXT = load_fn(CmdSetCullModeEXT);
    pfn_cmdSetFrontFaceEXT = load_fn(CmdSetFrontFaceEXT);
    pfn_cmdSetPrimitiveTopologyEXT = load_fn(CmdSetPrimitiveTopologyEXT);
    pfn_cmdSetViewportWithCountEXT = load_fn(CmdSetViewportWithCountEXT);
    pfn_cmdSetScissorWithCountEXT = load_fn(CmdSetScissorWithCountEXT);
    pfn_cmdBindVertexBuffers2EXT = load_fn(CmdBindVertexBuffers2EXT);
    pfn_cmdSetDepthTestEnableEXT = load_fn(CmdSetDepthTestEnableEXT);
    pfn_cmdSetDepthWriteEnableEXT = load_fn(CmdSetDepthWriteEnableEXT);
    pfn_cmdSetDepthCompareOpEXT = load_fn(CmdSetDepthCompareOpEXT);
    pfn_cmdSetDepthBoundsTestEnableEXT = load_fn(CmdSetDepthBoundsTestEnableEXT);
    pfn_cmdSetStencilTestEnableEXT = load_fn(CmdSetStencilTestEnableEXT);
    pfn_cmdSetStencilOpEXT = load_fn(CmdSetStencilOpEXT);
    pfn_cmdSetRayTracingPipelineStackSizeKHR = load_fn(CmdSetRayTracingPipelineStackSizeKHR);
    pfn_cmdSetPatchControlPoints = load_fn(CmdSetPatchControlPointsEXT);
    pfn_cmdSetRasterizerDiscardEnable = load_fn(CmdSetRasterizerDiscardEnableEXT);
    pfn_cmdSetDepthBiasEnable = load_fn(CmdSetDepthBiasEnableEXT);
    pfn_cmdSetLogicOp = load_fn(CmdSetLogicOpEXT);
    pfn_cmdSetPrimitiveRestartEnable = load_fn(CmdSetPrimitiveRestartEnableEXT);
    pfn_cmdSetVertexInput = load_fn(CmdSetVertexInputEXT);
    pfn_cmdSetColorWriteEnable = load_fn(CmdSetColorWriteEnableEXT);


    // Development Utilities
    pfn_cmdInsertDebugUtilsLabelEXT = load_fn(CmdInsertDebugUtilsLabelEXT);
    pfn_cmdBeginDebugUtilsLabelEXT = load_fn(CmdBeginDebugUtilsLabelEXT);
    pfn_cmdEndDebugUtilsLabelEXT = load_fn(CmdEndDebugUtilsLabelEXT);
    pfn_cmdSetCheckpointNV = load_fn(CmdSetCheckpointNV);
    pfn_cmdSetPerformanceMarkerINTEL = load_fn(CmdSetPerformanceMarkerINTEL); // *
    pfn_cmdSetPerformanceStreamMarkerINTEL = load_fn(CmdSetPerformanceStreamMarkerINTEL); // *
    pfn_cmdSetPerformanceOverrideINTEL = load_fn(CmdSetPerformanceOverrideINTEL); // *


    // Draw Commands
    pfn_cmdDraw = load_fn(CmdDraw);
    pfn_cmdDrawIndexed = load_fn(CmdDrawIndexed);
    pfn_cmdDrawIndirect = load_fn(CmdDrawIndirect);
    pfn_cmdDrawIndirectCount = load_fn(CmdDrawIndirectCount);
    pfn_cmdDrawIndexedIndirect = load_fn(CmdDrawIndexedIndirect);
    pfn_cmdDrawIndexedIndirectCount = load_fn(CmdDrawIndexedIndirectCount);
    pfn_cmdDrawIndirectByteCountEXT = load_fn(CmdDrawIndirectByteCountEXT);
    pfn_cmdDrawMeshTasksNV = load_fn(CmdDrawMeshTasksNV);
    pfn_cmdDrawMeshTasksIndirectNV = load_fn(CmdDrawMeshTasksIndirectNV);
    pfn_cmdDrawMeshTasksIndirectCountNV = load_fn(CmdDrawMeshTasksIndirectCountNV);

    pfn_cmdDispatch = load_fn(CmdDispatch);
    pfn_cmdDispatchIndirect = load_fn(CmdDispatchIndirect);
    pfn_cmdDispatchBase = load_fn(CmdDispatchBase);

    pfn_cmdTraceRaysKHR = load_fn(CmdTraceRaysKHR);
    pfn_cmdTraceRaysIndirectKHR = load_fn(CmdTraceRaysIndirectKHR);

    pfn_cmdCuLaunchKernelNVX = load_fn(CmdCuLaunchKernelNVX);

    // Memory Commands
    //  |
    //  |-> Images
    pfn_cmdBlitImage = load_fn(CmdBlitImage);
    pfn_cmdBlitImage2KHR = load_fn(CmdBlitImage2KHR);
    pfn_cmdCopyImage = load_fn(CmdCopyImage);
    pfn_cmdCopyImage2KHR = load_fn(CmdCopyImage2KHR);
    pfn_cmdResolveImage = load_fn(CmdResolveImage);
    pfn_cmdResolveImage2KHR = load_fn(CmdResolveImage2KHR);
    pfn_cmdClearColorImage = load_fn(CmdClearColorImage);
    pfn_cmdClearDepthStencilImage = load_fn(CmdClearDepthStencilImage);

    pfn_cmdCopyBuffer = load_fn(CmdCopyBuffer);
    pfn_cmdCopyBuffer2KHR = load_fn(CmdCopyBuffer2KHR);
    pfn_cmdUpdateBuffer = load_fn(CmdUpdateBuffer);
    pfn_cmdFillBuffer = load_fn(CmdFillBuffer);

    pfn_cmdCopyBufferToImage = load_fn(CmdCopyBufferToImage);
    pfn_cmdCopyBufferToImage2KHR = load_fn(CmdCopyBufferToImage2KHR);
    pfn_cmdCopyImageToBuffer = load_fn(CmdCopyImageToBuffer);
    pfn_cmdCopyImageToBuffer2KHR = load_fn(CmdCopyImageToBuffer2KHR);
    pfn_cmdClearAttachments = load_fn(CmdClearAttachments);

    pfn_cmdBuildAccelerationStructuresKHR = load_fn(CmdBuildAccelerationStructuresKHR);
    pfn_cmdBuildAccelerationStructuresIndirectKHR = load_fn(CmdBuildAccelerationStructuresIndirectKHR);
    pfn_cmdWriteAccelerationStructuresPropertiesKHR = load_fn(CmdWriteAccelerationStructuresPropertiesKHR);
    pfn_cmdCopyAccelerationStructureKHR = load_fn(CmdCopyAccelerationStructureKHR);
    pfn_cmdCopyAccelerationStructureNV = load_fn(CmdCopyAccelerationStructureNV);
    pfn_cmdCopyAccelerationStructureToMemoryKHR = load_fn(CmdCopyAccelerationStructureToMemoryKHR);
    pfn_cmdCopyMemoryToAccelerationStructureKHR = load_fn(CmdCopyMemoryToAccelerationStructureKHR);

    // Queries

    pfn_cmdWriteTimestamp = load_fn(CmdWriteTimestamp2KHR);

    pfn_cmdBeginTransformFeedbackEXT = load_fn(CmdBeginTransformFeedbackEXT);
    pfn_cmdEndTransformFeedbackEXT = load_fn(CmdEndTransformFeedbackEXT);
    pfn_cmdBeginQueryIndexedEXT = load_fn(CmdBeginQueryIndexedEXT);
    pfn_cmdEndQueryIndexedEXT = load_fn(CmdEndQueryIndexedEXT);
    pfn_cmdBeginQuery = load_fn(CmdBeginQuery);
    pfn_cmdEndQuery = load_fn(CmdEndQuery);

    pfn_cmdResetQueryPool = load_fn(CmdResetQueryPool);
    pfn_cmdCopyQueryPoolResults = load_fn(CmdCopyQueryPoolResults);

    pfn_cmdWriteBufferMarker = load_fn(CmdWriteBufferMarker2AMD);
  }
  

  PFN_vkBeginCommandBuffer pfn_beginCommandBuffer;
  PFN_vkEndCommandBuffer pfn_endCommandBuffer;
  PFN_vkResetCommandBuffer pfn_resetCommandBuffer;

  // Control Flow
  PFN_vkCmdExecuteCommands pfn_cmdExecuteCommands;
  PFN_vkCmdBeginRenderPass pfn_cmdBeginRenderPass;
  PFN_vkCmdBeginRenderPass2 pfn_cmdBeginRenderPass2;
  PFN_vkCmdNextSubpass pfn_cmdNextSubpass;
  PFN_vkCmdNextSubpass2 pfn_cmdNextSubpass2;
  PFN_vkCmdEndRenderPass pfn_cmdEndRenderPass;
  PFN_vkCmdEndRenderPass2 pfn_cmdEndRenderPass2;


  PFN_vkCmdPipelineBarrier2KHR pfn_cmdPipelineBarrier;
  PFN_vkCmdSetDeviceMask pfn_cmdSetDeviceMask;
  PFN_vkCmdBeginConditionalRenderingEXT pfn_cmdBeginConditionalRenderingEXT;
  PFN_vkCmdEndConditionalRenderingEXT pfn_cmdEndConditionalRenderingEXT;

  PFN_vkCmdPreprocessGeneratedCommandsNV pfn_cmdPreprocessGeneratedCommandsNV;
  PFN_vkCmdExecuteGeneratedCommandsNV pfn_cmdExecuteGeneratedCommandsNV;

  PFN_vkCmdSetEvent2KHR pfn_cmdSetEvent;
  PFN_vkCmdResetEvent2KHR pfn_cmdResetEvent;
  PFN_vkCmdWaitEvents2KHR pfn_cmdWaitEvents;


  // State Modification
  PFN_vkCmdPushConstants pfn_cmdPushConstants;
  PFN_vkCmdPushDescriptorSetKHR pfn_cmdPushDescriptorSetKHR;
  PFN_vkCmdPushDescriptorSetWithTemplateKHR pfn_cmdPushDescriptorSetWithTemplateKHR;
  PFN_vkCmdBindPipeline pfn_cmdBindPipeline;
  PFN_vkCmdBindDescriptorSets pfn_cmdBindDescriptorSets;
  PFN_vkCmdBindIndexBuffer pfn_cmdBindIndexBuffer;
  PFN_vkCmdBindVertexBuffers pfn_cmdBindVertexBuffers;
  PFN_vkCmdBindShadingRateImageNV pfn_cmdBindShadingRateImageNV;
  PFN_vkCmdBindTransformFeedbackBuffersEXT pfn_cmdBindTransformFeedbackBuffersEXT;
  PFN_vkCmdBindPipelineShaderGroupNV pfn_cmdBindPipelineShaderGroupNV;

  PFN_vkCmdSetViewport pfn_cmdSetViewport;
  PFN_vkCmdSetScissor pfn_cmdSetScissor;
  PFN_vkCmdSetLineWidth pfn_cmdSetLineWidth;
  PFN_vkCmdSetDepthBias pfn_cmdSetDepthBias;
  PFN_vkCmdSetBlendConstants pfn_cmdSetBlendConstants;
  PFN_vkCmdSetDepthBounds pfn_cmdSetDepthBounds;
  PFN_vkCmdSetStencilCompareMask pfn_cmdSetStencilCompareMask;
  PFN_vkCmdSetStencilWriteMask pfn_cmdSetStencilWriteMask;
  PFN_vkCmdSetStencilReference pfn_cmdSetStencilReference;
  PFN_vkCmdSetViewportWScalingNV pfn_cmdSetViewportWScalingNV;
  PFN_vkCmdSetDiscardRectangleEXT pfn_cmdSetDiscardRectangleEXT;
  PFN_vkCmdSetSampleLocationsEXT pfn_cmdSetSampleLocationsEXT;
  PFN_vkCmdSetViewportShadingRatePaletteNV pfn_cmdSetViewportShadingRatePaletteNV;
  PFN_vkCmdSetCoarseSampleOrderNV pfn_cmdSetCoarseSampleOrderNV;
  PFN_vkCmdSetExclusiveScissorNV pfn_cmdSetExclusiveScissorNV;
  PFN_vkCmdSetFragmentShadingRateKHR pfn_cmdSetFragmentShadingRateKHR;
  PFN_vkCmdSetFragmentShadingRateEnumNV pfn_cmdSetFragmentShadingRateEnumNV;
  PFN_vkCmdSetLineStippleEXT pfn_cmdSetLineStippleEXT;
  PFN_vkCmdSetCullModeEXT pfn_cmdSetCullModeEXT;
  PFN_vkCmdSetFrontFaceEXT pfn_cmdSetFrontFaceEXT;
  PFN_vkCmdSetPrimitiveTopologyEXT pfn_cmdSetPrimitiveTopologyEXT;
  PFN_vkCmdSetViewportWithCountEXT pfn_cmdSetViewportWithCountEXT;
  PFN_vkCmdSetScissorWithCountEXT pfn_cmdSetScissorWithCountEXT;
  PFN_vkCmdBindVertexBuffers2EXT pfn_cmdBindVertexBuffers2EXT;
  PFN_vkCmdSetDepthTestEnableEXT pfn_cmdSetDepthTestEnableEXT;
  PFN_vkCmdSetDepthWriteEnableEXT pfn_cmdSetDepthWriteEnableEXT;
  PFN_vkCmdSetDepthCompareOpEXT pfn_cmdSetDepthCompareOpEXT;
  PFN_vkCmdSetDepthBoundsTestEnableEXT pfn_cmdSetDepthBoundsTestEnableEXT;
  PFN_vkCmdSetStencilTestEnableEXT pfn_cmdSetStencilTestEnableEXT;
  PFN_vkCmdSetStencilOpEXT pfn_cmdSetStencilOpEXT;
  PFN_vkCmdSetRayTracingPipelineStackSizeKHR pfn_cmdSetRayTracingPipelineStackSizeKHR;
  PFN_vkCmdSetPatchControlPointsEXT pfn_cmdSetPatchControlPoints;
  PFN_vkCmdSetRasterizerDiscardEnableEXT pfn_cmdSetRasterizerDiscardEnable;
  PFN_vkCmdSetDepthBiasEnableEXT pfn_cmdSetDepthBiasEnable;
  PFN_vkCmdSetLogicOpEXT pfn_cmdSetLogicOp;
  PFN_vkCmdSetPrimitiveRestartEnableEXT pfn_cmdSetPrimitiveRestartEnable;
  PFN_vkCmdSetVertexInputEXT pfn_cmdSetVertexInput;
  PFN_vkCmdSetColorWriteEnableEXT pfn_cmdSetColorWriteEnable;


  // Development Utilities
  PFN_vkCmdInsertDebugUtilsLabelEXT pfn_cmdInsertDebugUtilsLabelEXT;
  PFN_vkCmdBeginDebugUtilsLabelEXT pfn_cmdBeginDebugUtilsLabelEXT;
  PFN_vkCmdEndDebugUtilsLabelEXT pfn_cmdEndDebugUtilsLabelEXT;
  PFN_vkCmdSetCheckpointNV pfn_cmdSetCheckpointNV;
  PFN_vkCmdSetPerformanceMarkerINTEL pfn_cmdSetPerformanceMarkerINTEL; // *
  PFN_vkCmdSetPerformanceStreamMarkerINTEL pfn_cmdSetPerformanceStreamMarkerINTEL; // *
  PFN_vkCmdSetPerformanceOverrideINTEL pfn_cmdSetPerformanceOverrideINTEL; // *


  // Draw Commands
  PFN_vkCmdDraw pfn_cmdDraw;
  PFN_vkCmdDrawIndexed pfn_cmdDrawIndexed;
  PFN_vkCmdDrawIndirect pfn_cmdDrawIndirect;
  PFN_vkCmdDrawIndirectCount pfn_cmdDrawIndirectCount;
  PFN_vkCmdDrawIndexedIndirect pfn_cmdDrawIndexedIndirect;
  PFN_vkCmdDrawIndexedIndirectCount pfn_cmdDrawIndexedIndirectCount;
  PFN_vkCmdDrawIndirectByteCountEXT pfn_cmdDrawIndirectByteCountEXT;
  PFN_vkCmdDrawMeshTasksNV pfn_cmdDrawMeshTasksNV;
  PFN_vkCmdDrawMeshTasksIndirectNV pfn_cmdDrawMeshTasksIndirectNV;
  PFN_vkCmdDrawMeshTasksIndirectCountNV pfn_cmdDrawMeshTasksIndirectCountNV;

  PFN_vkCmdDispatch pfn_cmdDispatch;
  PFN_vkCmdDispatchIndirect pfn_cmdDispatchIndirect;
  PFN_vkCmdDispatchBase pfn_cmdDispatchBase;

  PFN_vkCmdTraceRaysKHR pfn_cmdTraceRaysKHR;
  PFN_vkCmdTraceRaysIndirectKHR pfn_cmdTraceRaysIndirectKHR;

  PFN_vkCmdCuLaunchKernelNVX pfn_cmdCuLaunchKernelNVX;

  // Memory Commands
  //  |
  //  |-> Images
  PFN_vkCmdBlitImage pfn_cmdBlitImage;
  PFN_vkCmdBlitImage2KHR pfn_cmdBlitImage2KHR;
  PFN_vkCmdCopyImage pfn_cmdCopyImage;
  PFN_vkCmdCopyImage2KHR pfn_cmdCopyImage2KHR;
  PFN_vkCmdResolveImage pfn_cmdResolveImage;
  PFN_vkCmdResolveImage2KHR pfn_cmdResolveImage2KHR;
  PFN_vkCmdClearColorImage pfn_cmdClearColorImage;
  PFN_vkCmdClearDepthStencilImage pfn_cmdClearDepthStencilImage;

  PFN_vkCmdCopyBuffer pfn_cmdCopyBuffer;
  PFN_vkCmdCopyBuffer2KHR pfn_cmdCopyBuffer2KHR;
  PFN_vkCmdUpdateBuffer pfn_cmdUpdateBuffer;
  PFN_vkCmdFillBuffer pfn_cmdFillBuffer;

  PFN_vkCmdCopyBufferToImage pfn_cmdCopyBufferToImage;
  PFN_vkCmdCopyBufferToImage2KHR pfn_cmdCopyBufferToImage2KHR;
  PFN_vkCmdCopyImageToBuffer pfn_cmdCopyImageToBuffer;
  PFN_vkCmdCopyImageToBuffer2KHR pfn_cmdCopyImageToBuffer2KHR;
  PFN_vkCmdClearAttachments pfn_cmdClearAttachments;

  PFN_vkCmdBuildAccelerationStructuresKHR pfn_cmdBuildAccelerationStructuresKHR;
  PFN_vkCmdBuildAccelerationStructuresIndirectKHR pfn_cmdBuildAccelerationStructuresIndirectKHR;
  PFN_vkCmdWriteAccelerationStructuresPropertiesKHR pfn_cmdWriteAccelerationStructuresPropertiesKHR;
  PFN_vkCmdCopyAccelerationStructureKHR pfn_cmdCopyAccelerationStructureKHR;
  PFN_vkCmdCopyAccelerationStructureNV pfn_cmdCopyAccelerationStructureNV;
  PFN_vkCmdCopyAccelerationStructureToMemoryKHR pfn_cmdCopyAccelerationStructureToMemoryKHR;
  PFN_vkCmdCopyMemoryToAccelerationStructureKHR pfn_cmdCopyMemoryToAccelerationStructureKHR;

  // Queries

  PFN_vkCmdWriteTimestamp2KHR pfn_cmdWriteTimestamp;

  PFN_vkCmdBeginTransformFeedbackEXT pfn_cmdBeginTransformFeedbackEXT;
  PFN_vkCmdEndTransformFeedbackEXT pfn_cmdEndTransformFeedbackEXT;
  PFN_vkCmdBeginQueryIndexedEXT pfn_cmdBeginQueryIndexedEXT;
  PFN_vkCmdEndQueryIndexedEXT pfn_cmdEndQueryIndexedEXT;
  PFN_vkCmdBeginQuery pfn_cmdBeginQuery;
  PFN_vkCmdEndQuery pfn_cmdEndQuery;

  PFN_vkCmdResetQueryPool pfn_cmdResetQueryPool;
  PFN_vkCmdCopyQueryPoolResults pfn_cmdCopyQueryPoolResults;

  PFN_vkCmdWriteBufferMarker2AMD pfn_cmdWriteBufferMarker;
};
struct api::impl::queue_api{
  
  void initialize(PFN_vkGetDeviceProcAddr getDeviceProcAddr, VkDevice handle) noexcept {
    pfn_queueSubmit = load_fn(QueueSubmit2KHR);
    pfn_queueWaitIdle = load_fn(QueueWaitIdle);
    pfn_queueBindSparse = load_fn(QueueBindSparse);

    pfn_queuePresent = load_fn(QueuePresentKHR);

    pfn_queueBeginDebugUtilsLabel = load_fn(QueueBeginDebugUtilsLabelEXT);
    pfn_queueEndDebugUtilsLabel = load_fn(QueueEndDebugUtilsLabelEXT);
    pfn_queueInsertDebugUtilsLabel = load_fn(QueueInsertDebugUtilsLabelEXT);

    pfn_getQueueCheckpointData = load_fn(GetQueueCheckpointData2NV);
    pfn_queueSetPerformanceConfiguration = load_fn(QueueSetPerformanceConfigurationINTEL);
  }
  
  PFN_vkQueueSubmit2KHR pfn_queueSubmit;
  PFN_vkQueueWaitIdle pfn_queueWaitIdle;
  PFN_vkQueueBindSparse pfn_queueBindSparse;

  PFN_vkQueuePresentKHR pfn_queuePresent;

  PFN_vkQueueBeginDebugUtilsLabelEXT pfn_queueBeginDebugUtilsLabel;
  PFN_vkQueueEndDebugUtilsLabelEXT pfn_queueEndDebugUtilsLabel;
  PFN_vkQueueInsertDebugUtilsLabelEXT pfn_queueInsertDebugUtilsLabel;

  PFN_vkGetQueueCheckpointData2NV pfn_getQueueCheckpointData;
  PFN_vkQueueSetPerformanceConfigurationINTEL pfn_queueSetPerformanceConfiguration;
};
struct api::impl::device_api{

  VkDevice                     handle = nullptr;
  api::physical_device         physicalDevice = {};
  const VkAllocationCallbacks* allocationCallbacks = nullptr;
  atomic<u32>                  refCount = 0;


  device_api(VkResult& result, api::physical_device physDev, const VkDeviceCreateInfo& createInfo, const VkAllocationCallbacks* allocCallbacks) noexcept {
    const auto getDeviceProcAddr = (PFN_vkGetDeviceProcAddr)pfn_get_instance_proc_addr(instance.handle, "vkGetDeviceProcAddr");

    if ( !getDeviceProcAddr ) {
      result = VK_ERROR_INITIALIZATION_FAILED;
      return;
    }

    result = instance.pfn_create_device(physDev.handle, &createInfo, allocCallbacks, &handle);

    if ( result == VK_SUCCESS && handle != VK_NULL_HANDLE) {

      physicalDevice      = physDev;
      allocationCallbacks = allocCallbacks;
      ++refCount;

      
      

      pfn_destroy   = load_fn(DestroyDevice);
      pfn_waitIdle  = load_fn(DeviceWaitIdle);
      pfn_getQueue  = load_fn(GetDeviceQueue);
      pfn_getQueue2 = load_fn(GetDeviceQueue2);



      pfn_allocateMemory = load_fn(AllocateMemory);
      pfn_freeMemory = load_fn(FreeMemory);
      pfn_mapMemory = load_fn(MapMemory);
      pfn_unmapMemory = load_fn(UnmapMemory);
      pfn_flushMappedMemory = load_fn(FlushMappedMemoryRanges);
      pfn_invalidateMappedMemory = load_fn(InvalidateMappedMemoryRanges);
      pfn_getMemoryCommitment = load_fn(GetDeviceMemoryCommitment);
      pfn_getMemoryOpaqueCaptureAddress = load_fn(GetDeviceMemoryOpaqueCaptureAddress);




      pfn_createEvent = load_fn(CreateEvent);
      pfn_destroyEvent = load_fn(DestroyEvent);
      pfn_getEventStatus = load_fn(GetEventStatus);
      pfn_setEvent = load_fn(SetEvent);
      pfn_resetEvent = load_fn(ResetEvent);


      pfn_createQueryPool = load_fn(CreateQueryPool);
      pfn_destroyQueryPool = load_fn(DestroyQueryPool);
      pfn_getQueryPoolResults = load_fn(GetQueryPoolResults);
      pfn_resetQueryPool = load_fn(ResetQueryPool);

      pfn_createShaderModule = load_fn(CreateShaderModule);
      pfn_destroyShaderModule = load_fn(DestroyShaderModule);

      pfn_createSampler = load_fn(CreateSampler);
      pfn_destroySampler = load_fn(DestroySampler);


      pfn_createFramebuffer = load_fn(CreateFramebuffer);
      pfn_destroyFramebuffer = load_fn(DestroyFramebuffer);


      pfn_createRenderPass = load_fn(CreateRenderPass);
      pfn_createRenderPass2 = load_fn(CreateRenderPass2);
      pfn_destroyRenderPass = load_fn(DestroyRenderPass);
      pfn_getRenderAreaGranularity = load_fn(GetRenderAreaGranularity);


      pfn_createCommandPool = load_fn(CreateCommandPool);
      pfn_destroyCommandPool = load_fn(DestroyCommandPool);
      pfn_resetCommandPool = load_fn(ResetCommandPool);
      pfn_allocateCommandBuffers = load_fn(AllocateCommandBuffers);
      pfn_freeCommandBuffers = load_fn(FreeCommandBuffers);
      pfn_trimCommandPool = load_fn(TrimCommandPool);

      pfn_createSamplerYcbcrConversion = load_fn(CreateSamplerYcbcrConversion);
      pfn_destroySamplerYcbcrConversion = load_fn(DestroySamplerYcbcrConversion);



      pfn_getMemoryWin32Handle = load_fn(GetMemoryWin32HandleKHR);
      pfn_getMemoryWin32HandleProperties = load_fn(GetMemoryWin32HandlePropertiesKHR);
      pfn_getMemoryFd = load_fn(GetMemoryFdKHR);
      pfn_getMemoryFdProperties = load_fn(GetMemoryFdPropertiesKHR);
      pfn_getMemoryWin32HandleNV = load_fn(GetMemoryWin32HandleNV);

      pfn_getDeviceGroupPeerMemoryFeatures = load_fn(GetDeviceGroupPeerMemoryFeaturesKHR);
      pfn_getMemoryHostPointerProperties = load_fn(GetMemoryHostPointerPropertiesEXT);
      pfn_getGeneratedCommandsMemoryRequirements = load_fn(GetGeneratedCommandsMemoryRequirementsNV);


      pfn_createSemaphore = load_fn(CreateSemaphore);
      pfn_destroySemaphore = load_fn(DestroySemaphore);
      pfn_getSemaphoreCounterValue = load_fn(GetSemaphoreCounterValue);
      pfn_waitSemaphores = load_fn(WaitSemaphores);
      pfn_signalSemaphore = load_fn(SignalSemaphore);
      pfn_importSemaphoreWin32Handle = load_fn(ImportSemaphoreWin32HandleKHR);
      pfn_getSemaphoreWin32Handle = load_fn(GetSemaphoreWin32HandleKHR);


      pfn_createFence = load_fn(CreateFence);
      pfn_destroyFence = load_fn(DestroyFence);
      pfn_resetFence = load_fn(ResetFences);
      pfn_getFenceStatus = load_fn(GetFenceStatus);
      pfn_waitForFences = load_fn(WaitForFences);
      pfn_importFenceWin32Handle = load_fn(ImportFenceWin32HandleKHR);
      pfn_getFenceWin32Handle = load_fn(GetFenceWin32HandleKHR);
      pfn_importFenceFd = load_fn(ImportFenceFdKHR);
      pfn_getFenceFd = load_fn(GetFenceFdKHR);






      pfn_acquireProfilingLock = load_fn(AcquireProfilingLockKHR);
      pfn_releaseProfilingLock = load_fn(ReleaseProfilingLockKHR);


      pfn_setDebugUtilsObjectName = load_fn(SetDebugUtilsObjectNameEXT);
      pfn_setDebugUtilsObjectTag = load_fn(SetDebugUtilsObjectTagEXT);


      pfn_createValidationCache = load_fn(CreateValidationCacheEXT);
      pfn_destroyValidationCache = load_fn(DestroyValidationCacheEXT);
      pfn_mergeValidationCaches = load_fn(MergeValidationCachesEXT);
      pfn_getValidationCacheData = load_fn(GetValidationCacheDataEXT);



      pfn_getCalibratedTimestamps = load_fn(GetCalibratedTimestampsEXT);



      pfn_initializePerformanceApi = load_fn(InitializePerformanceApiINTEL);
      pfn_uninitializePerformanceApi = load_fn(UninitializePerformanceApiINTEL);
      pfn_acquirePerformanceConfiguration = load_fn(AcquirePerformanceConfigurationINTEL);
      pfn_releasePerformanceConfiguration = load_fn(ReleasePerformanceConfigurationINTEL);

      pfn_getPerformanceParameter = load_fn(GetPerformanceParameterINTEL);


      pfn_createIndirectCommandsLayout = load_fn(CreateIndirectCommandsLayoutNV);
      pfn_destroyIndirectCommandsLayout = load_fn(DestroyIndirectCommandsLayoutNV);

      pfn_createPrivateDataSlot = load_fn(CreatePrivateDataSlotEXT);
      pfn_destroyPrivateDataSlot = load_fn(DestroyPrivateDataSlotEXT);
      pfn_setPrivateData = load_fn(SetPrivateDataEXT);
      pfn_getPrivateData = load_fn(GetPrivateDataEXT);


      pfn_createDeferredOperation = load_fn(CreateDeferredOperationKHR);
      pfn_destroyDeferredOperation = load_fn(DestroyDeferredOperationKHR);
      pfn_getDeferredOperationMaxConcurrency = load_fn(GetDeferredOperationMaxConcurrencyKHR);
      pfn_getDeferredOperationResult = load_fn(GetDeferredOperationResultKHR);
      pfn_deferredOperationJoin = load_fn(DeferredOperationJoinKHR);
      pfn_compileDeferredNV = load_fn(CompileDeferredNV);



      pfn_acquireFullScreenExclusiveMode = load_fn(AcquireFullScreenExclusiveModeEXT);
      pfn_releaseFullScreenExclusiveMode = load_fn(ReleaseFullScreenExclusiveModeEXT);
      pfn_getDeviceGroupSurfacePresentModes2 = load_fn(GetDeviceGroupSurfacePresentModes2EXT);

      pfn_createSwapchain = load_fn(CreateSwapchainKHR);
      pfn_destroySwapchain = load_fn(DestroySwapchainKHR);
      pfn_getSwapchainImages = load_fn(GetSwapchainImagesKHR);
      pfn_acquireNextImage = load_fn(AcquireNextImageKHR);

      pfn_getDeviceGroupPresentCapabilities = load_fn(GetDeviceGroupPresentCapabilitiesKHR);
      pfn_getDeviceGroupSurfacePresentModes = load_fn(GetDeviceGroupSurfacePresentModesKHR);
      pfn_acquireNextImage2 = load_fn(AcquireNextImage2KHR);

      pfn_createSharedSwapchains = load_fn(CreateSharedSwapchainsKHR);


      pfn_getSwapchainStatus = load_fn(GetSwapchainStatusKHR);

      pfn_displayPowerControl = load_fn(DisplayPowerControlEXT);
      pfn_registerDeviceEvent = load_fn(RegisterDeviceEventEXT);
      pfn_registerDisplayEvent = load_fn(RegisterDisplayEventEXT);
      pfn_getSwapchainCounter = load_fn(GetSwapchainCounterEXT);

      pfn_setHdrMetadata = load_fn(SetHdrMetadataEXT);
      pfn_setLocalDimming = load_fn(SetLocalDimmingAMD);





      pfn_createBuffer = load_fn(CreateBuffer);
      pfn_destroyBuffer = load_fn(DestroyBuffer);

      pfn_bindBufferMemory = load_fn(BindBufferMemory);
      pfn_bindBufferMemory2 = load_fn(BindBufferMemory2);
      pfn_getBufferMemoryRequirements = load_fn(GetBufferMemoryRequirements);
      pfn_getBufferMemoryRequirements2 = load_fn(GetBufferMemoryRequirements2);

      pfn_getBufferDeviceAddress = load_fn(GetBufferDeviceAddress);
      pfn_getBufferOpaqueCaptureAddress = load_fn(GetBufferOpaqueCaptureAddress);

      pfn_createBufferView = load_fn(CreateBufferView);
      pfn_destroyBufferView = load_fn(DestroyBufferView);


      pfn_getBufferDeviceAddressEXT = load_fn(GetBufferDeviceAddressEXT);



      pfn_createImage = load_fn(CreateImage);
      pfn_destroyImage = load_fn(DestroyImage);
      pfn_bindImageMemory = load_fn(BindImageMemory);
      pfn_bindImageMemory2 = load_fn(BindImageMemory2);
      pfn_getImageMemoryRequirements = load_fn(GetImageMemoryRequirements);
      pfn_getImageMemoryRequirements2 = load_fn(GetImageMemoryRequirements2);
      pfn_getImageSparseMemoryRequirements = load_fn(GetImageSparseMemoryRequirements);
      pfn_getImageSparseMemoryRequirements2 = load_fn(GetImageSparseMemoryRequirements2);
      pfn_getImageSubresourceLayout = load_fn(GetImageSubresourceLayout);

      pfn_createImageView = load_fn(CreateImageView);
      pfn_destroyImageView = load_fn(DestroyImageView);

      pfn_getImageViewHandle = load_fn(GetImageViewHandleNVX);
      pfn_getImageViewAddress = load_fn(GetImageViewAddressNVX);

      pfn_getImageDrmFormatModifierProperties = load_fn(GetImageDrmFormatModifierPropertiesEXT);



      pfn_createAccelerationStructure = load_fn(CreateAccelerationStructureKHR);
      pfn_destroyAccelerationStructure = load_fn(DestroyAccelerationStructureKHR);
      pfn_buildAccelerationStructure = load_fn(BuildAccelerationStructuresKHR);
      pfn_copyAccelerationStructure = load_fn(CopyAccelerationStructureKHR);
      pfn_copyAccelerationStructureToMemory = load_fn(CopyAccelerationStructureToMemoryKHR);
      pfn_copyMemoryToAccelerationStructure = load_fn(CopyMemoryToAccelerationStructureKHR);
      pfn_writeAccelerationStructureProperties = load_fn(WriteAccelerationStructuresPropertiesKHR);
      pfn_getAccelerationStructureDeviceAddress = load_fn(GetAccelerationStructureDeviceAddressKHR);
      pfn_getDeviceAccelerationStructureCompatibility = load_fn(GetDeviceAccelerationStructureCompatibilityKHR);
      pfn_getAccelerationStructureBuildSizes = load_fn(GetAccelerationStructureBuildSizesKHR);



      pfn_createGraphicsPipeline = load_fn(CreateGraphicsPipelines);
      pfn_createComputePipeline = load_fn(CreateComputePipelines);
      pfn_createRayTracingPipelineNV = load_fn(CreateRayTracingPipelinesNV);
      pfn_createRayTracingPipelineKHR = load_fn(CreateRayTracingPipelinesKHR);
      pfn_destroyPipeline = load_fn(DestroyPipeline);

      pfn_createPipelineCache = load_fn(CreatePipelineCache);
      pfn_destroyPipelineCache = load_fn(DestroyPipelineCache);
      pfn_getPipelineCacheData = load_fn(GetPipelineCacheData);
      pfn_mergePipelineCaches = load_fn(MergePipelineCaches);

      pfn_createPipelineLayout = load_fn(CreatePipelineLayout);
      pfn_destroyPipelineLayout = load_fn(DestroyPipelineLayout);


      pfn_getPipelineExecutableProperties = load_fn(GetPipelineExecutablePropertiesKHR);
      pfn_getPipelineExecutableStatistics = load_fn(GetPipelineExecutableStatisticsKHR);
      pfn_getPipelineExecutableInternalRepresentations = load_fn(GetPipelineExecutableInternalRepresentationsKHR);

      pfn_getRayTracingShaderGroupHandlesKHR = load_fn(GetRayTracingShaderGroupHandlesKHR);
      pfn_getRayTracingCaptureReplayShaderGroupHandlesKHR = load_fn(GetRayTracingCaptureReplayShaderGroupHandlesKHR);


      pfn_createDescriptorSetLayout = load_fn(CreateDescriptorSetLayout);
      pfn_destroyDescriptorSetLayout = load_fn(DestroyDescriptorSetLayout);
      pfn_getDescriptorSetLayoutSupport = load_fn(GetDescriptorSetLayoutSupport);

      pfn_allocateDescriptorSets = load_fn(AllocateDescriptorSets);
      pfn_freeDescriptorSets = load_fn(FreeDescriptorSets);
      pfn_updateDescriptorSets = load_fn(UpdateDescriptorSets);
      pfn_updateDescriptorSetWithTemplate = load_fn(UpdateDescriptorSetWithTemplate);

      pfn_createDescriptorPool = load_fn(CreateDescriptorPool);
      pfn_destroyDescriptorPool = load_fn(DestroyDescriptorPool);
      pfn_resetDescriptorPool = load_fn(ResetDescriptorPool);

      pfn_createDescriptorUpdateTemplate = load_fn(CreateDescriptorUpdateTemplate);
      pfn_destroyDescriptorUpdateTemplate = load_fn(DestroyDescriptorUpdateTemplate);
      
      
      this->queue.initialize(getDeviceProcAddr, handle);
      this->command_buffer.initialize(getDeviceProcAddr, handle);
    }
  }


  queue_api          queue;
  command_buffer_api command_buffer;


  PFN_vkDeviceWaitIdle pfn_waitIdle;
  PFN_vkGetDeviceQueue pfn_getQueue;
  PFN_vkGetDeviceQueue2 pfn_getQueue2;

  PFN_vkDestroyDevice pfn_destroy;



  PFN_vkAllocateMemory pfn_allocateMemory;
  PFN_vkFreeMemory pfn_freeMemory;
  PFN_vkMapMemory pfn_mapMemory;
  PFN_vkUnmapMemory pfn_unmapMemory;
  PFN_vkFlushMappedMemoryRanges pfn_flushMappedMemory;
  PFN_vkInvalidateMappedMemoryRanges pfn_invalidateMappedMemory;
  PFN_vkGetDeviceMemoryCommitment pfn_getMemoryCommitment;
  PFN_vkGetDeviceMemoryOpaqueCaptureAddress pfn_getMemoryOpaqueCaptureAddress;



  PFN_vkCreateEvent pfn_createEvent;
  PFN_vkDestroyEvent pfn_destroyEvent;
  PFN_vkGetEventStatus pfn_getEventStatus;
  PFN_vkSetEvent pfn_setEvent;
  PFN_vkResetEvent pfn_resetEvent;


  PFN_vkCreateQueryPool pfn_createQueryPool;
  PFN_vkDestroyQueryPool pfn_destroyQueryPool;
  PFN_vkGetQueryPoolResults pfn_getQueryPoolResults;
  PFN_vkResetQueryPool pfn_resetQueryPool;

  PFN_vkCreateShaderModule pfn_createShaderModule;
  PFN_vkDestroyShaderModule pfn_destroyShaderModule;

  PFN_vkCreateSampler pfn_createSampler;
  PFN_vkDestroySampler pfn_destroySampler;


  PFN_vkCreateFramebuffer pfn_createFramebuffer;
  PFN_vkDestroyFramebuffer pfn_destroyFramebuffer;


  PFN_vkCreateRenderPass pfn_createRenderPass;
  PFN_vkCreateRenderPass2 pfn_createRenderPass2;
  PFN_vkDestroyRenderPass pfn_destroyRenderPass;
  PFN_vkGetRenderAreaGranularity pfn_getRenderAreaGranularity;


  PFN_vkCreateCommandPool pfn_createCommandPool;
  PFN_vkDestroyCommandPool pfn_destroyCommandPool;
  PFN_vkResetCommandPool pfn_resetCommandPool;
  PFN_vkAllocateCommandBuffers pfn_allocateCommandBuffers;
  PFN_vkFreeCommandBuffers pfn_freeCommandBuffers;
  PFN_vkTrimCommandPool pfn_trimCommandPool;

  PFN_vkCreateSamplerYcbcrConversion pfn_createSamplerYcbcrConversion;
  PFN_vkDestroySamplerYcbcrConversion pfn_destroySamplerYcbcrConversion;



  PFN_vkGetMemoryWin32HandleKHR pfn_getMemoryWin32Handle;
  PFN_vkGetMemoryWin32HandlePropertiesKHR pfn_getMemoryWin32HandleProperties;
  PFN_vkGetMemoryFdKHR pfn_getMemoryFd;
  PFN_vkGetMemoryFdPropertiesKHR pfn_getMemoryFdProperties;
  PFN_vkGetMemoryWin32HandleNV pfn_getMemoryWin32HandleNV;

  PFN_vkGetDeviceGroupPeerMemoryFeaturesKHR pfn_getDeviceGroupPeerMemoryFeatures;
  PFN_vkGetMemoryHostPointerPropertiesEXT pfn_getMemoryHostPointerProperties;
  PFN_vkGetGeneratedCommandsMemoryRequirementsNV pfn_getGeneratedCommandsMemoryRequirements;


  PFN_vkCreateSemaphore pfn_createSemaphore;
  PFN_vkDestroySemaphore pfn_destroySemaphore;
  PFN_vkGetSemaphoreCounterValue pfn_getSemaphoreCounterValue;
  PFN_vkWaitSemaphores pfn_waitSemaphores;
  PFN_vkSignalSemaphore pfn_signalSemaphore;
  PFN_vkImportSemaphoreWin32HandleKHR pfn_importSemaphoreWin32Handle;
  PFN_vkGetSemaphoreWin32HandleKHR pfn_getSemaphoreWin32Handle;
  PFN_vkImportSemaphoreFdKHR pfn_importSemaphoreFd;
  PFN_vkGetSemaphoreFdKHR pfn_getSemaphoreFd;


  PFN_vkCreateFence pfn_createFence;
  PFN_vkDestroyFence pfn_destroyFence;
  PFN_vkResetFences pfn_resetFence;
  PFN_vkGetFenceStatus pfn_getFenceStatus;
  PFN_vkWaitForFences pfn_waitForFences;
  PFN_vkImportFenceWin32HandleKHR pfn_importFenceWin32Handle;
  PFN_vkGetFenceWin32HandleKHR pfn_getFenceWin32Handle;
  PFN_vkImportFenceFdKHR pfn_importFenceFd;
  PFN_vkGetFenceFdKHR pfn_getFenceFd;






  PFN_vkAcquireProfilingLockKHR pfn_acquireProfilingLock;
  PFN_vkReleaseProfilingLockKHR pfn_releaseProfilingLock;


  PFN_vkSetDebugUtilsObjectNameEXT pfn_setDebugUtilsObjectName;
  PFN_vkSetDebugUtilsObjectTagEXT pfn_setDebugUtilsObjectTag;


  PFN_vkCreateValidationCacheEXT pfn_createValidationCache;
  PFN_vkDestroyValidationCacheEXT pfn_destroyValidationCache;
  PFN_vkMergeValidationCachesEXT pfn_mergeValidationCaches;
  PFN_vkGetValidationCacheDataEXT pfn_getValidationCacheData;



  PFN_vkGetCalibratedTimestampsEXT pfn_getCalibratedTimestamps;



  PFN_vkInitializePerformanceApiINTEL pfn_initializePerformanceApi;
  PFN_vkUninitializePerformanceApiINTEL pfn_uninitializePerformanceApi;
  PFN_vkAcquirePerformanceConfigurationINTEL pfn_acquirePerformanceConfiguration;
  PFN_vkReleasePerformanceConfigurationINTEL pfn_releasePerformanceConfiguration;

  PFN_vkGetPerformanceParameterINTEL pfn_getPerformanceParameter;


  PFN_vkCreateIndirectCommandsLayoutNV pfn_createIndirectCommandsLayout;
  PFN_vkDestroyIndirectCommandsLayoutNV pfn_destroyIndirectCommandsLayout;

  PFN_vkCreatePrivateDataSlotEXT pfn_createPrivateDataSlot;
  PFN_vkDestroyPrivateDataSlotEXT pfn_destroyPrivateDataSlot;
  PFN_vkSetPrivateDataEXT pfn_setPrivateData;
  PFN_vkGetPrivateDataEXT pfn_getPrivateData;


  PFN_vkCreateDeferredOperationKHR pfn_createDeferredOperation;
  PFN_vkDestroyDeferredOperationKHR pfn_destroyDeferredOperation;
  PFN_vkGetDeferredOperationMaxConcurrencyKHR pfn_getDeferredOperationMaxConcurrency;
  PFN_vkGetDeferredOperationResultKHR pfn_getDeferredOperationResult;
  PFN_vkDeferredOperationJoinKHR pfn_deferredOperationJoin;
  PFN_vkCompileDeferredNV pfn_compileDeferredNV;



  PFN_vkAcquireFullScreenExclusiveModeEXT pfn_acquireFullScreenExclusiveMode;
  PFN_vkReleaseFullScreenExclusiveModeEXT pfn_releaseFullScreenExclusiveMode;
  PFN_vkGetDeviceGroupSurfacePresentModes2EXT pfn_getDeviceGroupSurfacePresentModes2;

  PFN_vkCreateSwapchainKHR pfn_createSwapchain;
  PFN_vkDestroySwapchainKHR pfn_destroySwapchain;
  PFN_vkGetSwapchainImagesKHR pfn_getSwapchainImages;
  PFN_vkAcquireNextImageKHR pfn_acquireNextImage;

  PFN_vkGetDeviceGroupPresentCapabilitiesKHR pfn_getDeviceGroupPresentCapabilities;
  PFN_vkGetDeviceGroupSurfacePresentModesKHR pfn_getDeviceGroupSurfacePresentModes;
  PFN_vkAcquireNextImage2KHR pfn_acquireNextImage2;

  PFN_vkCreateSharedSwapchainsKHR pfn_createSharedSwapchains;


  PFN_vkGetSwapchainStatusKHR pfn_getSwapchainStatus;

  PFN_vkDisplayPowerControlEXT pfn_displayPowerControl;
  PFN_vkRegisterDeviceEventEXT pfn_registerDeviceEvent;
  PFN_vkRegisterDisplayEventEXT pfn_registerDisplayEvent;
  PFN_vkGetSwapchainCounterEXT pfn_getSwapchainCounter;

  PFN_vkSetHdrMetadataEXT pfn_setHdrMetadata;
  PFN_vkSetLocalDimmingAMD pfn_setLocalDimming;





  PFN_vkCreateBuffer pfn_createBuffer;
  PFN_vkDestroyBuffer pfn_destroyBuffer;

  PFN_vkBindBufferMemory pfn_bindBufferMemory;
  PFN_vkBindBufferMemory2 pfn_bindBufferMemory2;
  PFN_vkGetBufferMemoryRequirements pfn_getBufferMemoryRequirements;
  PFN_vkGetBufferMemoryRequirements2 pfn_getBufferMemoryRequirements2;

  PFN_vkGetBufferDeviceAddress pfn_getBufferDeviceAddress;
  PFN_vkGetBufferOpaqueCaptureAddress pfn_getBufferOpaqueCaptureAddress;

  PFN_vkCreateBufferView pfn_createBufferView;
  PFN_vkDestroyBufferView pfn_destroyBufferView;


  PFN_vkGetBufferDeviceAddressEXT pfn_getBufferDeviceAddressEXT;



  PFN_vkCreateImage pfn_createImage;
  PFN_vkDestroyImage pfn_destroyImage;
  PFN_vkBindImageMemory pfn_bindImageMemory;
  PFN_vkBindImageMemory2 pfn_bindImageMemory2;
  PFN_vkGetImageMemoryRequirements pfn_getImageMemoryRequirements;
  PFN_vkGetImageMemoryRequirements2 pfn_getImageMemoryRequirements2;
  PFN_vkGetImageSparseMemoryRequirements pfn_getImageSparseMemoryRequirements;
  PFN_vkGetImageSparseMemoryRequirements2 pfn_getImageSparseMemoryRequirements2;
  PFN_vkGetImageSubresourceLayout pfn_getImageSubresourceLayout;

  PFN_vkCreateImageView pfn_createImageView;
  PFN_vkDestroyImageView pfn_destroyImageView;

  PFN_vkGetImageViewHandleNVX pfn_getImageViewHandle;
  PFN_vkGetImageViewAddressNVX pfn_getImageViewAddress;

  PFN_vkGetImageDrmFormatModifierPropertiesEXT pfn_getImageDrmFormatModifierProperties;



  PFN_vkCreateAccelerationStructureKHR pfn_createAccelerationStructure;
  PFN_vkDestroyAccelerationStructureKHR pfn_destroyAccelerationStructure;
  PFN_vkBuildAccelerationStructuresKHR pfn_buildAccelerationStructure;
  PFN_vkCopyAccelerationStructureKHR pfn_copyAccelerationStructure;
  PFN_vkCopyAccelerationStructureToMemoryKHR pfn_copyAccelerationStructureToMemory;
  PFN_vkCopyMemoryToAccelerationStructureKHR pfn_copyMemoryToAccelerationStructure;
  PFN_vkWriteAccelerationStructuresPropertiesKHR pfn_writeAccelerationStructureProperties;
  PFN_vkGetAccelerationStructureDeviceAddressKHR pfn_getAccelerationStructureDeviceAddress;
  PFN_vkGetDeviceAccelerationStructureCompatibilityKHR pfn_getDeviceAccelerationStructureCompatibility;
  PFN_vkGetAccelerationStructureBuildSizesKHR pfn_getAccelerationStructureBuildSizes;



  PFN_vkCreateGraphicsPipelines pfn_createGraphicsPipeline;
  PFN_vkCreateComputePipelines pfn_createComputePipeline;
  PFN_vkCreateRayTracingPipelinesNV pfn_createRayTracingPipelineNV;
  PFN_vkCreateRayTracingPipelinesKHR pfn_createRayTracingPipelineKHR;
  PFN_vkDestroyPipeline pfn_destroyPipeline;

  PFN_vkCreatePipelineCache pfn_createPipelineCache;
  PFN_vkDestroyPipelineCache pfn_destroyPipelineCache;
  PFN_vkGetPipelineCacheData pfn_getPipelineCacheData;
  PFN_vkMergePipelineCaches pfn_mergePipelineCaches;

  PFN_vkCreatePipelineLayout pfn_createPipelineLayout;
  PFN_vkDestroyPipelineLayout pfn_destroyPipelineLayout;


  PFN_vkGetPipelineExecutablePropertiesKHR pfn_getPipelineExecutableProperties;
  PFN_vkGetPipelineExecutableStatisticsKHR pfn_getPipelineExecutableStatistics;
  PFN_vkGetPipelineExecutableInternalRepresentationsKHR pfn_getPipelineExecutableInternalRepresentations;

  PFN_vkGetRayTracingShaderGroupHandlesKHR pfn_getRayTracingShaderGroupHandlesKHR;
  PFN_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR pfn_getRayTracingCaptureReplayShaderGroupHandlesKHR;


  PFN_vkCreateDescriptorSetLayout pfn_createDescriptorSetLayout;
  PFN_vkDestroyDescriptorSetLayout pfn_destroyDescriptorSetLayout;
  PFN_vkGetDescriptorSetLayoutSupport pfn_getDescriptorSetLayoutSupport;

  PFN_vkAllocateDescriptorSets pfn_allocateDescriptorSets;
  PFN_vkFreeDescriptorSets pfn_freeDescriptorSets;
  PFN_vkUpdateDescriptorSets pfn_updateDescriptorSets;
  PFN_vkUpdateDescriptorSetWithTemplate pfn_updateDescriptorSetWithTemplate;

  PFN_vkCreateDescriptorPool pfn_createDescriptorPool;
  PFN_vkDestroyDescriptorPool pfn_destroyDescriptorPool;
  PFN_vkResetDescriptorPool pfn_resetDescriptorPool;

  PFN_vkCreateDescriptorUpdateTemplate pfn_createDescriptorUpdateTemplate;
  PFN_vkDestroyDescriptorUpdateTemplate pfn_destroyDescriptorUpdateTemplate;
};

bool                api::initialize() noexcept {
  DWORD result = init_library();
  VK_assert( result == NO_ERROR );
  return result == NO_ERROR;
}

VkResult            api::initialize_instance(const VkInstanceCreateInfo& createInfo, const VkAllocationCallbacks* callbacks) noexcept {
  VkResult result = instance.initialize(createInfo, callbacks);
  if ( result == VK_SUCCESS )
    std::atexit(destroy_instance);
  return result;
}
outcome<version>    api::instance_version() noexcept {
  outcome<version> outcome;
  outcome.result = pfn_instance_version((u32*)((void*)&outcome.value));
  return outcome;
}
outcome<u32>        api::count_instance_extensions(const char* layer) noexcept {
  outcome<u32> outcome;
  outcome.result = pfn_enumerate_instance_extensions(layer, &outcome.value, nullptr);
  return outcome;
}
outcome<u32>        api::count_layers() noexcept {
  outcome<u32> outcome;
  outcome.result = pfn_enumerate_layers(&outcome.value, nullptr);
  return outcome;
}
VkResult            api::enumerate_instance_extensions(const char* layer, std::span<VkExtensionProperties> extensionProperties) noexcept {
  u32 len = extensionProperties.size();
  return pfn_enumerate_instance_extensions(layer, &len, extensionProperties.data());
}
VkResult            api::enumerate_layers(std::span<VkLayerProperties> layerProperties) noexcept {
  u32 len = layerProperties.size();
  return pfn_enumerate_layers(&len, layerProperties.data());
}



outcome<u32>        api::count_physical_devices() noexcept {
  return instance.count_physical_devices();
}
outcome<u32>        api::count_physical_device_groups() noexcept {
  return instance.count_physical_device_groups();
}
VkResult            api::enumerate_physical_devices(std::span<physical_device> devices) noexcept {
  return instance.enumerate_physical_devices(devices.size(), (VkPhysicalDevice*)devices.data());
}
VkResult            api::enumerate_physical_device_groups(std::span<VkPhysicalDeviceGroupProperties> deviceGroups) noexcept {
  return instance.enumerate_physical_device_groups(deviceGroups.size(), deviceGroups.data());
}

outcome<VkSurfaceKHR> api::create_surface(const VkWin32SurfaceCreateInfoKHR& createInfo, const VkAllocationCallbacks* allocCallbacks) noexcept {
  return instance.create_surface(createInfo, allocCallbacks);
}
outcome<VkSurfaceKHR> api::create_surface(const VkHeadlessSurfaceCreateInfoEXT& createInfo, const VkAllocationCallbacks* allocCallbacks) noexcept {
  return instance.create_headless_surface(createInfo, allocCallbacks);
}
outcome<VkSurfaceKHR> api::create_surface(const VkDisplaySurfaceCreateInfoKHR& createInfo, const VkAllocationCallbacks* allocCallbacks) noexcept {
  return instance.create_display_plane_surface(createInfo, allocCallbacks);
}
void                  api::destroy_surface(VkSurfaceKHR surface, const VkAllocationCallbacks* allocCallbacks) noexcept {
  instance.destroy_surface(surface, allocCallbacks);
}

outcome<VkDebugUtilsMessengerEXT> api::create_debug_messenger(const VkDebugUtilsMessengerCreateInfoEXT& createInfo, const VkAllocationCallbacks* allocCallbacks) noexcept {
  return instance.create_debug_messenger(createInfo, allocCallbacks);
}
void                              api::submit_debug_message(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT& data) noexcept {
  instance.submit_debug_message(severity, type, data);
}
void                              api::destroy_debug_messenger(VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* allocCallbacks) noexcept {
  instance.destroy_debug_messenger(messenger, allocCallbacks);
}



// physical_device

outcome<device> api::physical_device::create_device(const VkDeviceCreateInfo& createInfo, const VkAllocationCallbacks* allocCallbacks) const noexcept {
  outcome<device> outcome;
  outcome.value.api = new api::impl::device_api(outcome.result, *this, createInfo, allocCallbacks);
  return outcome;
}

outcome<u32> api::physical_device::count_device_extensions() const noexcept {
  outcome<u32> outcome;
  outcome.result = instance.pfn_enumerate_device_extension_properties(handle, nullptr, &outcome.value, nullptr);
  return outcome;
}
VkResult     api::physical_device::enumerate_device_extensions(std::span<VkExtensionProperties> extensions) const noexcept {
  u32 len = extensions.size();
  return instance.pfn_enumerate_device_extension_properties(handle, nullptr, &len, extensions.data());
}

void api::physical_device::get_features(VkPhysicalDeviceFeatures2& features) const noexcept {
  instance.pfn_get_features(handle, &features);
}
void api::physical_device::get_properties(VkPhysicalDeviceProperties2& properties) const noexcept {
  instance.pfn_get_properties(handle, &properties);
}
void api::physical_device::get_format_properties(VkFormat format, VkFormatProperties2& properties) const noexcept {
  instance.pfn_get_format_properties(handle, format, &properties);
}
VkResult api::physical_device::get_image_format_properties(const VkPhysicalDeviceImageFormatInfo2& info, VkImageFormatProperties2& properties) const noexcept {
  return instance.pfn_get_image_format_properties(handle, &info, &properties);
}

u32  api::physical_device::count_sparse_image_formats(const VkPhysicalDeviceSparseImageFormatInfo2& info) const noexcept {
  u32 length = 0;
  instance.pfn_get_sparse_image_format_properties(handle, &info, &length, nullptr);
  return length;
}
void api::physical_device::enumerate_sparse_image_formats(const VkPhysicalDeviceSparseImageFormatInfo2& info, std::span<VkSparseImageFormatProperties2> properties) const noexcept {
  u32 len = properties.size();
  instance.pfn_get_sparse_image_format_properties(handle, &info, &len, properties.data());
}
u32  api::physical_device::count_queue_families() const noexcept {
  u32 len = 0;
  instance.pfn_get_queue_family_properties(handle, &len, nullptr);
  return len;
}
void api::physical_device::enumerate_queue_families(std::span<VkQueueFamilyProperties2> properties) const noexcept {
  u32 len = properties.size();
  instance.pfn_get_queue_family_properties(handle, &len, properties.data());
}


void api::physical_device::get_memory_properties(VkPhysicalDeviceMemoryProperties2& properties) const noexcept {
  instance.pfn_get_memory_properties(handle, &properties);
}
void api::physical_device::get_external_buffer_properties(const VkPhysicalDeviceExternalBufferInfo& info, VkExternalBufferProperties& properties) const noexcept {
  instance.pfn_get_external_buffer_properties(handle, &info, &properties);
}
void api::physical_device::get_external_fence_properties(const VkPhysicalDeviceExternalFenceInfo& info, VkExternalFenceProperties& properties) const noexcept {
  instance.pfn_get_external_fence_properties(handle, &info, &properties);
}
void api::physical_device::get_external_semaphore_properties(const VkPhysicalDeviceExternalSemaphoreInfo& info, VkExternalSemaphoreProperties& properties) const noexcept {
  instance.pfn_get_external_semaphore_properties(handle, &info, &properties);
}

bool api::physical_device::get_win32_presentation_support(u32 queueFamily) const noexcept {
  return instance.pfn_get_win32_presentation_support(handle, queueFamily);
}
outcome<bool> api::physical_device::get_surface_support(u32 queueFamily, VkSurfaceKHR surface) const noexcept {
  outcome<bool> outcome;
  VkBool32 Bool;
  outcome.result = instance.pfn_get_surface_support(handle, queueFamily, surface, &Bool);
  outcome.value = Bool;
  return outcome;
}

VkResult api::physical_device::get_surface_capabilities(const VkPhysicalDeviceSurfaceInfo2KHR& info, VkSurfaceCapabilities2KHR& capabilities) const noexcept {
  return instance.pfn_get_surface_capabilities(handle, &info, &capabilities);
}
outcome<u32> api::physical_device::count_surface_formats(const VkPhysicalDeviceSurfaceInfo2KHR& info) const noexcept {
  outcome<u32> outcome;
  outcome.result = instance.pfn_get_surface_formats(handle, &info, &outcome.value, nullptr);
  return outcome;
}
VkResult api::physical_device::enumerate_surface_formats(const VkPhysicalDeviceSurfaceInfo2KHR& info, std::span<VkSurfaceFormat2KHR> formats) const noexcept {
  u32 len = formats.size();
  return instance.pfn_get_surface_formats(handle, &info, &len, formats.data());
}
outcome<u32> api::physical_device::count_present_modes(const VkPhysicalDeviceSurfaceInfo2KHR& info) const noexcept {
  outcome<u32> outcome;
  outcome.result = instance.pfn_get_surface_present_modes(handle, &info, &outcome.value, nullptr);
  return outcome;
}
VkResult api::physical_device::enumerate_present_modes(const VkPhysicalDeviceSurfaceInfo2KHR& info, std::span<VkPresentModeKHR> presentModes) const noexcept {
  u32 len = presentModes.size();
  return instance.pfn_get_surface_present_modes(handle, &info, &len, presentModes.data());
}
outcome<u32> api::physical_device::count_present_rectangles(VkSurfaceKHR surface) const noexcept{
  outcome<u32> outcome;
  outcome.result = instance.pfn_get_present_rectangles(handle, surface, &outcome.value, nullptr);
  return outcome;
}
VkResult api::physical_device::enumerate_present_rectangles(VkSurfaceKHR surface, std::span<VkRect2D> presentRectangles) const noexcept {
  u32 len = presentRectangles.size();
  return instance.pfn_get_present_rectangles(handle, surface, &len, presentRectangles.data());
}

outcome<VkDisplayModeKHR> api::physical_device::create_display_mode(VkDisplayKHR display, const VkDisplayModeCreateInfoKHR& info, const VkAllocationCallbacks* allocCallbacks) const noexcept {
  outcome<VkDisplayModeKHR> outcome;
  outcome.result = instance.pfn_create_display_mode(handle, display, &info, allocCallbacks, &outcome.value);
  return outcome;
}

outcome<u32> api::physical_device::count_displays() const noexcept {
  outcome<u32> outcome;
  outcome.result = instance.pfn_get_display_properties(handle, &outcome.value, nullptr);
  return outcome;
}
outcome<u32> api::physical_device::count_display_modes(VkDisplayKHR display) const noexcept {
  outcome<u32> outcome;
  outcome.result = instance.pfn_get_display_mode_properties(handle, display, &outcome.value, nullptr);
  return outcome;
}
outcome<u32> api::physical_device::count_display_planes() const noexcept {
  outcome<u32> outcome;
  outcome.result = instance.pfn_get_display_plane_properties(handle, &outcome.value, nullptr);
  return outcome;
}
outcome<u32> api::physical_device::count_supported_displays(u32 displayPlaneIndex) const noexcept {
  outcome<u32> outcome;
  outcome.result = instance.pfn_get_display_plane_supported_displays(handle, displayPlaneIndex, &outcome.value, nullptr);
  return outcome;
}

VkResult     api::physical_device::enumerate_displays(std::span<VkDisplayProperties2KHR> displays) const noexcept {
  u32 len = displays.size();
  return instance.pfn_get_display_properties(handle, &len, displays.data());
}
VkResult     api::physical_device::enumerate_display_modes(VkDisplayKHR display, std::span<VkDisplayModeProperties2KHR> displayModes) const noexcept {
  u32 len = displayModes.size();
  return instance.pfn_get_display_mode_properties(handle, display, &len, displayModes.data());
}
VkResult     api::physical_device::enumerate_display_planes(std::span<VkDisplayPlaneProperties2KHR> displayPlanes) const noexcept {
  u32 len = displayPlanes.size();
  return instance.pfn_get_display_plane_properties(handle, &len, displayPlanes.data());
}
VkResult     api::physical_device::enumerate_supported_displays(u32 displayPlaneIndex, std::span<VkDisplayKHR> displays) const noexcept {
  u32 len = displays.size();
  return instance.pfn_get_display_plane_supported_displays(handle, displayPlaneIndex, &len, displays.data());
}

VkResult     api::physical_device::get_display_plane_capabilities(const VkDisplayPlaneInfo2KHR& info, VkDisplayPlaneCapabilities2KHR& capabilities) const noexcept {
  return instance.pfn_get_display_plane_capabilities(handle, &info, &capabilities);
}


outcome<u32> api::physical_device::count_queue_family_performance_query_counters(u32 queueFamily) const noexcept {
  outcome<u32> outcome;
  outcome.result = instance.pfn_enumerate_queue_family_performance_query_counters(handle, queueFamily, &outcome.value, nullptr, nullptr);
  return outcome;
}
VkResult     api::physical_device::enumerate_queue_family_performance_query_counters(u32 queueFamily, std::span<VkPerformanceCounterKHR> counters, std::span<VkPerformanceCounterDescriptionKHR> descriptions) const noexcept {
  u32 len = counters.size();
  if ( len == 0 ) {
    len = descriptions.size();
  }
  return instance.pfn_enumerate_queue_family_performance_query_counters(handle, queueFamily, &len, counters.data(), descriptions.data());
}

u32          api::physical_device::get_queue_family_performance_query_passes(const VkQueryPoolPerformanceCreateInfoKHR& info) const noexcept {
  u32 length;
  instance.pfn_get_queue_family_performance_query_passes(handle, &info, &length);
  return length;
}

outcome<VkDisplayKHR> api::physical_device::get_winrt_display(u32 index) const noexcept {
  outcome<VkDisplayKHR> outcome;
  outcome.result = instance.pfn_get_winrt_display(handle, index, &outcome.value);
  return outcome;
}
VkResult     api::physical_device::acquire_winrt_display(VkDisplayKHR display) const noexcept {
  return instance.pfn_acquire_winrt_display(handle, display);
}
VkResult     api::physical_device::release_display(VkDisplayKHR display) const noexcept {
  return instance.pfn_release_display(handle, display);
}

void         api::physical_device::get_multisample_properties(VkSampleCountFlagBits sampleCount, VkMultisamplePropertiesEXT& properties) const noexcept {
  instance.pfn_get_multisample_properties(handle, sampleCount, &properties);
}


outcome<u32> api::physical_device::count_calibrateable_time_domains() const noexcept {
  outcome<u32> outcome;
  outcome.result = instance.pfn_get_calibrateable_time_domains(handle, &outcome.value, nullptr);
  return outcome;
}
outcome<u32> api::physical_device::count_cooperative_matrices() const noexcept {
  outcome<u32> outcome;
  outcome.result = instance.pfn_get_cooperative_matrix_properties(handle, &outcome.value, nullptr);
  return outcome;
}
outcome<u32> api::physical_device::count_supported_framebuffer_mixed_samples_combinations() const noexcept {
  outcome<u32> outcome;
  outcome.result = instance.pfn_get_supported_framebuffer_mixed_samples_combinations(handle, &outcome.value, nullptr);
  return outcome;
}

VkResult     api::physical_device::enumerate_calibrateable_time_domains(std::span<VkTimeDomainEXT> domains) const noexcept {
  u32 len = domains.size();
  return instance.pfn_get_calibrateable_time_domains(handle, &len, domains.data());
}
VkResult     api::physical_device::enumerate_cooperative_matrices(std::span<VkCooperativeMatrixPropertiesNV> matrices) const noexcept {
  u32 len = matrices.size();
  return instance.pfn_get_cooperative_matrix_properties(handle, &len, matrices.data());
}
VkResult     api::physical_device::enumerate_supported_framebuffer_mixed_samples_combinations(std::span<VkFramebufferMixedSamplesCombinationNV> combos) const noexcept {
  u32 len = combos.size();
  return instance.pfn_get_supported_framebuffer_mixed_samples_combinations(handle, &len, combos.data());
}


// device

VkQueue  device::get_queue(u32 queueFamily, u32 queueIndex) const noexcept {
  VkQueue queue;
  api->pfn_getQueue(api->handle, queueFamily, queueIndex, &queue);
  return queue;
}
VkQueue  device::get_queue(const VkDeviceQueueInfo2 &info) const noexcept {
  VkQueue queue;
  api->pfn_getQueue2(api->handle, &info, &queue);
  return queue;
}

VkResult device::wait_idle(VkQueue queue) const noexcept {
  return api->queue.pfn_queueWaitIdle(queue);
}
VkResult device::submit(VkQueue queue, std::span<const VkSubmitInfo2KHR> submissions, VkFence fence) const noexcept {
  return api->queue.pfn_queueSubmit(queue, submissions.size(), submissions.data(), fence);
}
VkResult device::present(VkQueue queue, const VkPresentInfoKHR &info) const noexcept {
  return api->queue.pfn_queuePresent(queue, &info);
}
VkResult device::bind_sparse(VkQueue queue, std::span<const VkBindSparseInfo> bindings, VkFence fence) const noexcept {
  return api->queue.pfn_queueBindSparse(queue, bindings.size(), bindings.data(), fence);
}

void     device::begin_debug_label(VkQueue queue, const VkDebugUtilsLabelEXT &label) const noexcept {
  api->queue.pfn_queueBeginDebugUtilsLabel(queue, &label);
}
void     device::insert_debug_label(VkQueue queue, const VkDebugUtilsLabelEXT &label) const noexcept {
  api->queue.pfn_queueInsertDebugUtilsLabel(queue, &label);
}
void     device::end_debug_label(VkQueue queue) const noexcept {
  api->queue.pfn_queueEndDebugUtilsLabel(queue);
}

u32      device::count_checkpoint_data(VkQueue queue) const noexcept {
  u32 len;
  api->queue.pfn_getQueueCheckpointData(queue, &len, nullptr);
  return len;
}
void     device::enumerate_checkpoint_data(VkQueue queue, std::span<VkCheckpointData2NV> checkpoints) const noexcept {
  u32 len = checkpoints.size();
  api->queue.pfn_getQueueCheckpointData(queue, &len, checkpoints.data());
}

VkResult device::set_performance_configuration(VkQueue queue, VkPerformanceConfigurationINTEL performanceConfig) const noexcept {
  return api->queue.pfn_queueSetPerformanceConfiguration(queue, performanceConfig);
}

// WEEEEEEEEEEEEEEEEEEE



void device::destroy() const noexcept {
  api->pfn_destroy(api->handle, api->allocationCallbacks);
  delete api;
}
VkResult device::wait_idle() const noexcept {
  return api->pfn_waitIdle(api->handle);
}



outcome<VkDeviceMemory> device::allocate_memory(const VkMemoryAllocateInfo &info, const VkAllocationCallbacks *allocCallbacks) const noexcept {
  outcome<VkDeviceMemory> outcome;
  outcome.result = api->pfn_allocateMemory(api->handle, &info, allocCallbacks, &outcome.value);
  return outcome;
}
void           device::free_memory(VkDeviceMemory memory, const VkAllocationCallbacks *allocCallbacks) const noexcept {
  api->pfn_freeMemory(api->handle, memory, allocCallbacks);
}
outcome<void*> device::map_memory(VkDeviceMemory memory, u64 offset, u64 size, VkMemoryMapFlags flags) const noexcept {
  outcome<void*> outcome;
  outcome.result = api->pfn_mapMemory(api->handle, memory, offset, size, flags, &outcome.value);
  return outcome;
}
void           device::unmap_memory(VkDeviceMemory memory) const noexcept {
  api->pfn_unmapMemory(api->handle, memory);
}
VkResult       device::flush_mapped_memory_ranges(std::span<const VkMappedMemoryRange> mappedRanges) const noexcept {
  return api->pfn_flushMappedMemory(api->handle, mappedRanges.size(), mappedRanges.data());
}
VkResult       device::invalidate_mapped_memory_ranges(std::span<const VkMappedMemoryRange> mappedRanges) const noexcept {
  return api->pfn_invalidateMappedMemory(api->handle, mappedRanges.size(), mappedRanges.data());
}
u64            device::get_device_memory_commitment(VkDeviceMemory memory) const noexcept {
  u64 val;
  api->pfn_getMemoryCommitment(api->handle, memory, &val);
  return val;
}
u64            device::get_opaque_capture_address(const VkDeviceMemoryOpaqueCaptureAddressInfo &info) const noexcept {
  return api->pfn_getMemoryOpaqueCaptureAddress(api->handle, &info);
}



outcome<VkEvent> device::create_event(const VkEventCreateInfo &info, const VkAllocationCallbacks *allocCallbacks) const noexcept {
  outcome<VkEvent> outcome;
  outcome.result = api->pfn_createEvent(api->handle, &info, allocCallbacks, &outcome.value);
  return outcome;
}
void     device::destroy_event(VkEvent event, const VkAllocationCallbacks *allocCallbacks) const noexcept {
  api->pfn_destroyEvent(api->handle, event, allocCallbacks);
}
VkResult device::get_event_status(VkEvent event) const noexcept {
  return api->pfn_getEventStatus(api->handle, event);
}
VkResult device::set_event(VkEvent event) const noexcept {
  return api->pfn_setEvent(api->handle, event);
}
VkResult device::reset_event(VkEvent event) const noexcept {
  return api->pfn_resetEvent(api->handle, event);
}


outcome<VkQueryPool> device::create_query_pool(const VkQueryPoolCreateInfo &info, const VkAllocationCallbacks *allocCallbacks) const noexcept {
  outcome<VkQueryPool> outcome;
  outcome.result = api->pfn_createQueryPool(api->handle, &info, allocCallbacks, &outcome.value);
  return outcome;
}
void device::destroy_query_pool(VkQueryPool queryPool, const VkAllocationCallbacks *allocCallbacks) const noexcept {
  api->pfn_destroyQueryPool(api->handle, queryPool, allocCallbacks);
}
VkResult device::get_query_pool_results(VkQueryPool queryPool, u32 firstQuery, u32 queryCount, std::span<byte> data, u64 stride, VkQueryResultFlags flags) const noexcept {
  return api->pfn_getQueryPoolResults(api->handle, queryPool, firstQuery, queryCount, data.size(), data.data(), stride, flags);
}
void device::reset_query_pool(VkQueryPool queryPool, u32 firstQuery, u32 queryCount) {
  api->pfn_resetQueryPool(api->handle, queryPool, firstQuery, queryCount);
}


outcome<VkShaderModule> device::create_shader_module(const VkShaderModuleCreateInfo &info, const VkAllocationCallbacks * allocCallbacks) const noexcept {
  outcome<VkShaderModule> outcome;
  outcome.result = api->pfn_createShaderModule(api->handle, &info, allocCallbacks, &outcome.value);
  return outcome;
}
void device::destroy_shader_module(VkShaderModule shaderModule, const VkAllocationCallbacks *callbacks) const noexcept {
  api->pfn_destroyShaderModule(api->handle, shaderModule, callbacks);
}


outcome<VkSampler> device::create_sampler(const VkSamplerCreateInfo &info, const VkAllocationCallbacks *callbacks) const noexcept {
  outcome<VkSampler> outcome;
  outcome.result = api->pfn_createSampler(api->handle, &info, callbacks, &outcome.value);
  return outcome;
}
void device::destroy_sampler(VkSampler sampler, const VkAllocationCallbacks *callbacks) const noexcept {
  api->pfn_destroySampler(api->handle, sampler, callbacks);
}
outcome<VkSamplerYcbcrConversion> device::create_sampler_ycbcr_conversion(const VkSamplerYcbcrConversionCreateInfo &createInfo, const VkAllocationCallbacks *callbacks) const noexcept {
  outcome<VkSamplerYcbcrConversion> outcome;
  outcome.result = api->pfn_createSamplerYcbcrConversion(api->handle, &createInfo, callbacks, &outcome.value);
  return outcome;
}
void device::destroy_sampler_ycbcr_conversion(VkSamplerYcbcrConversion samplerYcbcrConversion, const VkAllocationCallbacks *callbacks) const noexcept {
  api->pfn_destroySamplerYcbcrConversion(api->handle, samplerYcbcrConversion, callbacks);
}



outcome<void *> device::get_memory_win32_handle(const VkMemoryGetWin32HandleInfoKHR &info) const noexcept {
  outcome<void *> outcome;
  outcome.result = api->pfn_getMemoryWin32Handle(api->handle, &info, &outcome.value);
  return outcome;
}
VkResult device::get_memory_win32_handle_properties(VkExternalMemoryHandleTypeFlagBits handleType, void *handle, VkMemoryWin32HandlePropertiesKHR &properties) const noexcept {
  return api->pfn_getMemoryWin32HandleProperties(api->handle, handleType, handle, &properties);
}
outcome<int> device::get_memory_fd(const VkMemoryGetFdInfoKHR &info) const noexcept {
  outcome<int> outcome;
  outcome.result = api->pfn_getMemoryFd(api->handle, &info, &outcome.value);
  return outcome;
}
VkResult device::get_memory_fd_properties(VkExternalMemoryHandleTypeFlagBits fdType, int fd, VkMemoryFdPropertiesKHR &properties) const noexcept {
  return api->pfn_getMemoryFdProperties(api->handle, fdType, fd, &properties);
}
VkPeerMemoryFeatureFlags device::get_device_group_peer_memory_features(u32 heapIndex, u32 localDeviceIndex, u32 remoteDeviceIndex) const noexcept {
  VkPeerMemoryFeatureFlags flags;
  api->pfn_getDeviceGroupPeerMemoryFeatures(api->handle, heapIndex, localDeviceIndex, remoteDeviceIndex, &flags);
  return flags;
}
VkResult device::get_memory_host_pointer_properties(VkExternalMemoryHandleTypeFlagBits handleType, const void *hostPointer, VkMemoryHostPointerPropertiesEXT &memoryHostPointerProperties) const noexcept {
  return api->pfn_getMemoryHostPointerProperties(api->handle, handleType, hostPointer, &memoryHostPointerProperties);
}
void device::get_generated_commands_memory_requirements(const VkGeneratedCommandsMemoryRequirementsInfoNV &info, VkMemoryRequirements2 &req) const noexcept {
  api->pfn_getGeneratedCommandsMemoryRequirements(api->handle, &info, &req);
}



outcome<VkSemaphore> device::create_semaphore(const VkSemaphoreCreateInfo &createInfo, const VkAllocationCallbacks *callbacks) const noexcept {
  outcome<VkSemaphore> outcome;
  outcome.result = api->pfn_createSemaphore(api->handle, &createInfo, callbacks, &outcome.value);
  return outcome;
}
void device::destroy_semaphore(VkSemaphore semaphore, const VkAllocationCallbacks *callbacks) const noexcept {
  api->pfn_destroySemaphore(api->handle, semaphore, callbacks);
}
outcome<u64> device::get_semaphore_counter_value(VkSemaphore semaphore) const noexcept {
  outcome<u64> outcome;
  outcome.result = api->pfn_getSemaphoreCounterValue(api->handle, semaphore, &outcome.value);
  return outcome;
}
VkResult device::wait_semaphores(const VkSemaphoreWaitInfo &info, u64 timeout_ns) const noexcept {
  return api->pfn_waitSemaphores(api->handle, &info, timeout_ns);
}
VkResult device::signal_semaphore(const VkSemaphoreSignalInfo &info) const noexcept {
  return api->pfn_signalSemaphore(api->handle, &info);
}
VkResult device::import_semaphore_win32_handle(const VkImportSemaphoreWin32HandleInfoKHR &info) const noexcept {
  return api->pfn_importSemaphoreWin32Handle(api->handle, &info);
}
outcome<void *> device::get_semaphore_win32_handle(const VkSemaphoreGetWin32HandleInfoKHR &info) const noexcept {
  outcome<void *> outcome;
  outcome.result = api->pfn_getSemaphoreWin32Handle(api->handle, &info, &outcome.value);
  return outcome;
}
VkResult device::import_semaphore_fd(const VkImportSemaphoreFdInfoKHR &info) const noexcept {
  return api->pfn_importSemaphoreFd(api->handle, &info);
}
outcome<int> device::get_semaphore_fd(const VkSemaphoreGetFdInfoKHR &info) const noexcept {
  outcome<int> outcome;
  outcome.result = api->pfn_getSemaphoreFd(api->handle, &info, &outcome.value);
  return outcome;
}



outcome<VkFence> device::create_fence(const VkFenceCreateInfo &createInfo, const VkAllocationCallbacks *callbacks) const noexcept {
  outcome<VkFence> outcome;
  outcome.result = api->pfn_createFence(api->handle, &createInfo, callbacks, &outcome.value);
  return outcome;
}
void device::destroy_fence(VkFence fence, const VkAllocationCallbacks *callbacks) const noexcept {
  api->pfn_destroyFence(api->handle, fence, callbacks);
}
VkResult device::reset_fences(std::span<const VkFence> fences) const noexcept {
  return api->pfn_resetFence(api->handle, fences.size(), fences.data());
}
VkResult device::get_fence_status(VkFence fence) const noexcept {
  return api->pfn_getFenceStatus(api->handle, fence);
}
VkResult device::wait_for_fences(std::span<const VkFence> fences, bool32 waitAll, u64 timeout_ns) const noexcept {
  return api->pfn_waitForFences(api->handle, fences.size(), fences.data(), reinterpret_cast<VkBool32&>(waitAll), timeout_ns);
}
outcome<VkFence> device::register_device_event(const VkDeviceEventInfoEXT &info, const VkAllocationCallbacks *callbacks) const noexcept {
  outcome<VkFence> outcome;
  outcome.result = api->pfn_registerDeviceEvent(api->handle, &info, callbacks, &outcome.value);
  return outcome;
}
outcome<VkFence> device::register_display_event(VkDisplayKHR display, const VkDisplayEventInfoEXT &info, const VkAllocationCallbacks *callbacks) const noexcept {
  outcome<VkFence> outcome;
  outcome.result = api->pfn_registerDisplayEvent(api->handle, display, &info, callbacks, &outcome.value);
  return outcome;
}
VkResult device::import_fence_win32_handle(const VkImportFenceWin32HandleInfoKHR &info) const noexcept {
  return api->pfn_importFenceWin32Handle(api->handle, &info);
}
outcome<void *> device::get_fence_win32_handle(const VkFenceGetWin32HandleInfoKHR &info) const noexcept {
  outcome<void *> outcome;
  outcome.result = api->pfn_getFenceWin32Handle(api->handle, &info, &outcome.value);
  return outcome;
}
VkResult device::import_fence_fd(const VkImportFenceFdInfoKHR &info) const noexcept {
  return api->pfn_importFenceFd(api->handle, &info);
}
outcome<int> device::get_fence_fd(const VkFenceGetFdInfoKHR &info) const noexcept {
  outcome<int> outcome;
  outcome.result = api->pfn_getFenceFd(api->handle, &info, &outcome.value);
  return outcome;
}



VkResult device::set_object_debug_name(const VkDebugUtilsObjectNameInfoEXT &info) const noexcept {
  return api->pfn_setDebugUtilsObjectName(api->handle, &info);
}
VkResult device::set_object_debug_tag(const VkDebugUtilsObjectTagInfoEXT &info) const noexcept {
  return api->pfn_setDebugUtilsObjectTag(api->handle, &info);
}
outcome<VkValidationCacheEXT> device::create_validation_cache(const VkValidationCacheCreateInfoEXT &info, const VkAllocationCallbacks *callbacks) const noexcept {
  outcome<VkValidationCacheEXT> outcome;
  outcome.result = api->pfn_createValidationCache(api->handle, &info, callbacks, &outcome.value);
  return outcome;
}
void device::destroy_validation_cache(VkValidationCacheEXT validationCache, const VkAllocationCallbacks *callbacks) const noexcept {
  api->pfn_destroyValidationCache(api->handle, validationCache, callbacks);
}
VkResult device::merge_validation_caches(VkValidationCacheEXT dstCache, std::span<const VkValidationCacheEXT> srcCache) const noexcept {
  return api->pfn_mergeValidationCaches(api->handle, dstCache, srcCache.size(), srcCache.data());
}
outcome<u64> device::get_validation_cache_size(VkValidationCacheEXT validationCache) const noexcept {
  outcome<u64> outcome;
  outcome.result = api->pfn_getValidationCacheData(api->handle, validationCache, &outcome.value, nullptr);
  return outcome;
}
VkResult device::get_validation_cache_data(VkValidationCacheEXT validationCache, std::span<byte> dest) const noexcept {
  u64 length = dest.size();
  return api->pfn_getValidationCacheData(api->handle, validationCache, &length, dest.data());
}
VkResult device::acquire_profiling_lock(const VkAcquireProfilingLockInfoKHR &info) const noexcept {
  return api->pfn_acquireProfilingLock(api->handle, &info);
}
void device::release_profiling_lock() const noexcept {
  api->pfn_releaseProfilingLock(api->handle);
}
outcome<u64> device::get_calibrated_timestamps(std::span<const VkCalibratedTimestampInfoEXT> timeDomains, u64 *timestamps) const noexcept {
  outcome<u64> outcome;
  outcome.result = api->pfn_getCalibratedTimestamps(api->handle, timeDomains.size(), timeDomains.data(), timestamps, &outcome.value);
  return outcome;
}
VkResult device::initialize_performance_api(const VkInitializePerformanceApiInfoINTEL &info) const noexcept {
  return api->pfn_initializePerformanceApi(api->handle, &info);
}
void device::uninitialize_performance_api() const noexcept {
  api->pfn_uninitializePerformanceApi(api->handle);
}
outcome<VkPerformanceConfigurationINTEL> device::acquire_performance_configuration(const VkPerformanceConfigurationAcquireInfoINTEL &info) const noexcept {
  outcome<VkPerformanceConfigurationINTEL> outcome;
  outcome.result = api->pfn_acquirePerformanceConfiguration(api->handle, &info, &outcome.value);
  return outcome;
}
void device::release_performance_configuration(VkPerformanceConfigurationINTEL perfConfig) const noexcept {
  api->pfn_releasePerformanceConfiguration(api->handle, perfConfig);
}
outcome<VkPerformanceValueINTEL> device::get_performance_parameter(VkPerformanceParameterTypeINTEL type) const noexcept {
  outcome<VkPerformanceValueINTEL> outcome;
  outcome.result = api->pfn_getPerformanceParameter(api->handle, type, &outcome.value);
  return outcome;
}




outcome<VkIndirectCommandsLayoutNV> device::create_indirect_commands_layout(const VkIndirectCommandsLayoutCreateInfoNV &info, const VkAllocationCallbacks *callbacks) const noexcept {
  outcome<VkIndirectCommandsLayoutNV> outcome;
  outcome.result = api->pfn_createIndirectCommandsLayout(api->handle, &info, callbacks, &outcome.value);
  return outcome;
}
void device::destroy_indirect_commands_layout(VkIndirectCommandsLayoutNV indirectCommandsLayout, const VkAllocationCallbacks *callbacks) const noexcept {
  api->pfn_destroyIndirectCommandsLayout(api->handle, indirectCommandsLayout, callbacks);
}

outcome<VkPrivateDataSlotEXT> device::create_private_data_slot(const VkPrivateDataSlotCreateInfoEXT &info, const VkAllocationCallbacks *callbacks) const noexcept {
  outcome<VkPrivateDataSlotEXT> outcome;
  outcome.result = api->pfn_createPrivateDataSlot(api->handle, &info, callbacks, &outcome.value);
  return outcome;
}
void     device::destroy_private_data_slot(VkPrivateDataSlotEXT privateDataSlot, const VkAllocationCallbacks *callbacks) const noexcept {
  api->pfn_destroyPrivateDataSlot(api->handle, privateDataSlot, callbacks);
}
VkResult device::set_private_data(VkObjectType objectType, u64 objectHandle, VkPrivateDataSlotEXT privateDataSlot, u64 value) const noexcept {
  return api->pfn_setPrivateData(api->handle, objectType, objectHandle, privateDataSlot, value);
}
u64      device::get_private_data(VkObjectType objectType, u64 objectHandle, VkPrivateDataSlotEXT privateDataSlot) const noexcept {
  u64 value;
  api->pfn_getPrivateData(api->handle, objectType, objectHandle, privateDataSlot, &value);
  return value;
}

outcome<VkDeferredOperationKHR> device::create_deferred_operation(const VkAllocationCallbacks *callbacks) const noexcept {
  outcome<VkDeferredOperationKHR> outcome;
  outcome.result = api->pfn_createDeferredOperation(api->handle, callbacks, &outcome.value);
  return outcome;
}
void     device::destroy_deferred_operation(VkDeferredOperationKHR deferredOp, const VkAllocationCallbacks *callbacks) const noexcept {
  api->pfn_destroyDeferredOperation(api->handle, deferredOp, callbacks);
}
u32      device::get_deferred_operation_max_concurrency(VkDeferredOperationKHR deferredOp) const noexcept {
  return api->pfn_getDeferredOperationMaxConcurrency(api->handle, deferredOp);
}
VkResult device::get_deferred_operation_result(VkDeferredOperationKHR deferredOp) const noexcept {
  return api->pfn_getDeferredOperationResult(api->handle, deferredOp);
}
VkResult device::deferred_operation_join(VkDeferredOperationKHR deferredOp) const noexcept {
  return api->pfn_deferredOperationJoin(api->handle, deferredOp);
}

outcome<VkSwapchainKHR> device::create_swapchain(const VkSwapchainCreateInfoKHR &createInfo, const VkAllocationCallbacks *callbacks) const noexcept {
  outcome<VkSwapchainKHR> outcome;
  outcome.result = api->pfn_createSwapchain(api->handle, &createInfo, callbacks, &outcome.value);
  return outcome;
}
VkResult device::create_shared_swapchains(std::span<const VkSwapchainCreateInfoKHR> infos, VkSwapchainKHR* swapchains, const VkAllocationCallbacks *callbacks) const noexcept {
  return api->pfn_createSharedSwapchains(api->handle, infos.size(), infos.data(), callbacks, swapchains);
}
void device::destroy_swapchain(VkSwapchainKHR swapchain, const VkAllocationCallbacks *callbacks) const noexcept {
  api->pfn_destroySwapchain(api->handle, swapchain, callbacks);
}
outcome<u32> device::count_swapchain_images(VkSwapchainKHR swapchain) const noexcept {
  outcome<u32> outcome;
  outcome.result = api->pfn_getSwapchainImages(api->handle, swapchain, &outcome.value, nullptr);
  return outcome;
}
VkResult device::enumerate_swapchain_images(VkSwapchainKHR swapchain, std::span<VkImage> images) const noexcept {
  u32 length = images.size();
  return api->pfn_getSwapchainImages(api->handle, swapchain, &length, images.data());
}
outcome<u32> device::acquire_next_image(const VkAcquireNextImageInfoKHR &info) const noexcept {
  outcome<u32> outcome;
  outcome.result = api->pfn_acquireNextImage2(api->handle, &info, &outcome.value);
  return outcome;
}
VkResult device::get_swapchain_status(VkSwapchainKHR swapchain) const noexcept {
  return api->pfn_getSwapchainStatus(api->handle, swapchain);
}
outcome<u64> device::get_swapchain_counter(VkSwapchainKHR swapchain, VkSurfaceCounterFlagBitsEXT counter) const noexcept {
  outcome<u64> outcome;
  outcome.result = api->pfn_getSwapchainCounter(api->handle, swapchain, counter, &outcome.value);
  return outcome;
}
VkResult device::acquire_fullscreen_exclusive_mode(VkSwapchainKHR swapchain) const noexcept {
  return api->pfn_acquireFullScreenExclusiveMode(api->handle, swapchain);
}
VkResult device::release_fullscreen_exclusive_mode(VkSwapchainKHR swapchain) const noexcept {
  return api->pfn_releaseFullScreenExclusiveMode(api->handle, swapchain);
}

void device::set_hdr_metadata(u32 swapchainCount, const VkSwapchainKHR *swapchains, const VkHdrMetadataEXT *metadata) const noexcept {
  api->pfn_setHdrMetadata(api->handle, swapchainCount, swapchains, metadata);
}
void device::set_local_dimming(VkSwapchainKHR swapchain, bool32 enabled) const noexcept {
  api->pfn_setLocalDimming(api->handle, swapchain, reinterpret_cast<VkBool32&>(enabled));
}
outcome<VkDeviceGroupPresentModeFlagsKHR> device::get_device_group_surface_present_modes(VkSurfaceKHR surface) const noexcept {
  outcome<VkDeviceGroupPresentModeFlagsKHR> outcome;
  outcome.result = api->pfn_getDeviceGroupSurfacePresentModes(api->handle, surface, &outcome.value);
  return outcome;
}
outcome<VkDeviceGroupPresentModeFlagsKHR> device::get_device_group_surface_present_modes(const VkPhysicalDeviceSurfaceInfo2KHR &info) const noexcept {
  outcome<VkDeviceGroupPresentModeFlagsKHR> outcome;
  outcome.result = api->pfn_getDeviceGroupSurfacePresentModes2(api->handle, &info, &outcome.value);
  return outcome;
}
VkResult device::get_device_group_present_capabilities(VkDeviceGroupPresentCapabilitiesKHR &capabilities) const noexcept {
  return api->pfn_getDeviceGroupPresentCapabilities(api->handle, &capabilities);
}
VkResult device::display_power_control(VkDisplayKHR display, const VkDisplayPowerInfoEXT &info) const noexcept {
  return api->pfn_displayPowerControl(api->handle, display, &info);
}




outcome<VkBuffer> device::create_buffer(const VkBufferCreateInfo &info, const VkAllocationCallbacks *callbacks) const noexcept {
  outcome<VkBuffer> outcome;
  outcome.result = api->pfn_createBuffer(api->handle, &info, callbacks, &outcome.value);
  return outcome;
}
void device::destroy_buffer(VkBuffer buffer, const VkAllocationCallbacks *callbacks) const noexcept {
  api->pfn_destroyBuffer(api->handle, buffer, callbacks);
}
VkResult device::bind_buffer_memory(std::span<const VkBindBufferMemoryInfo> infos) const noexcept {
  return api->pfn_bindBufferMemory2(api->handle, infos.size(), infos.data());
}
void device::get_buffer_memory_requirements(const VkBufferMemoryRequirementsInfo2 &info, VkMemoryRequirements2 &requirements) const noexcept {
  api->pfn_getBufferMemoryRequirements2(api->handle, &info, &requirements);
}
u64 device::get_opaque_capture_address(const VkBufferDeviceAddressInfo &info) const noexcept {
  return api->pfn_getBufferOpaqueCaptureAddress(api->handle, &info);
}
VkDeviceAddress device::get_buffer_device_address(const VkBufferDeviceAddressInfo &info) const noexcept {
  return api->pfn_getBufferDeviceAddress(api->handle, &info);
}
u64 device::get_buffer_opaque_capture_address(const VkBufferDeviceAddressInfo &info) const noexcept {
  return api->pfn_getBufferOpaqueCaptureAddress(api->handle, &info);
}

outcome<VkBufferView> device::create_buffer_view(const VkBufferViewCreateInfo &createInfo, const VkAllocationCallbacks *callbacks) const noexcept {
  outcome<VkBufferView> outcome;
  outcome.result = api->pfn_createBufferView(api->handle, &createInfo, callbacks, &outcome.value);
  return outcome;
}
void device::destroy_buffer_view(VkBufferView bufferView, const VkAllocationCallbacks *callbacks) const noexcept {
  api->pfn_destroyBufferView(api->handle, bufferView, callbacks);
}

outcome<VkImage> device::create_image(const VkImageCreateInfo &info, const VkAllocationCallbacks *callbacks) const noexcept {
  outcome<VkImage> outcome;
  outcome.result = api->pfn_createImage(api->handle, &info, callbacks, &outcome.value);
  return outcome;
}
void device::destroy_image(VkImage image, const VkAllocationCallbacks *callbacks) const noexcept {
  api->pfn_destroyImage(api->handle, image, callbacks);
}
VkResult device::bind_image_memory(std::span<const VkBindImageMemoryInfo> infos) const noexcept {
  return api->pfn_bindImageMemory2(api->handle, infos.size(), infos.data());
}
void device::get_image_memory_requirements(const VkImageMemoryRequirementsInfo2 &info, VkMemoryRequirements2 &requirements) const noexcept {
  api->pfn_getImageMemoryRequirements2(api->handle, &info, &requirements);
}
u32 device::count_image_sparse_memory_requirements(const VkImageSparseMemoryRequirementsInfo2 &info) const noexcept {
  u32 length;
  api->pfn_getImageSparseMemoryRequirements2(api->handle, &info, &length, nullptr);
  return length;
}
void device::enumerate_image_sparse_memory_requirements(const VkImageSparseMemoryRequirementsInfo2 &info, std::span<VkSparseImageMemoryRequirements2> requirements) const noexcept {
  u32 length = requirements.size();
  api->pfn_getImageSparseMemoryRequirements2(api->handle, &info, &length, requirements.data());
}
VkSubresourceLayout device::get_image_subresource_layout(VkImage image, const VkImageSubresource &subresource) const noexcept {
  VkSubresourceLayout layout;
  api->pfn_getImageSubresourceLayout(api->handle, image, &subresource, &layout);
  return layout;
}
VkResult device::get_image_drm_format_modifier_properties(VkImage image, VkImageDrmFormatModifierPropertiesEXT &properties) const noexcept {
  return api->pfn_getImageDrmFormatModifierProperties(api->handle, image, &properties);
}

outcome<VkImageView> device::create_image_view(const VkImageViewCreateInfo &createInfo, const VkAllocationCallbacks *callbacks) const noexcept {
  outcome<VkImageView> outcome;
  outcome.result = api->pfn_createImageView(api->handle, &createInfo, callbacks, &outcome.value);
  return outcome;
}
void device::destroy_image_view(VkImageView imageView, const VkAllocationCallbacks *callbacks) const noexcept {
  api->pfn_destroyImageView(api->handle, imageView, callbacks);
}
u32  device::get_image_view_handle(const VkImageViewHandleInfoNVX &info) const noexcept {
  return api->pfn_getImageViewHandle(api->handle, &info);
}
VkResult device::get_image_view_address(VkImageView imageView, VkImageViewAddressPropertiesNVX &addressProperties) const noexcept {
  return api->pfn_getImageViewAddress(api->handle, imageView, &addressProperties);
}



outcome<VkAccelerationStructureKHR> device::create_acceleration_structure(const VkAccelerationStructureCreateInfoKHR &createInfo, const VkAllocationCallbacks *callbacks) const noexcept {
  outcome<VkAccelerationStructureKHR> outcome;
  outcome.result = api->pfn_createAccelerationStructure(api->handle, &createInfo, callbacks, &outcome.value);
  return outcome;
}
void     device::destroy_acceleration_structure(VkAccelerationStructureKHR accelerationStructure, const VkAllocationCallbacks *callbacks) const noexcept {
  api->pfn_destroyAccelerationStructure(api->handle, accelerationStructure, callbacks);
}
VkResult device::build_acceleration_structures(u32 infoCount, const VkAccelerationStructureBuildGeometryInfoKHR *pInfos, const VkAccelerationStructureBuildRangeInfoKHR *const *ppBuildRangeInfos) const noexcept {
  return api->pfn_buildAccelerationStructure(api->handle, nullptr, infoCount, pInfos, ppBuildRangeInfos);
}
VkResult device::copy_acceleration_structure(const VkCopyAccelerationStructureInfoKHR &info) const noexcept {
  return api->pfn_copyAccelerationStructure(api->handle, nullptr, &info);
}
VkResult device::copy_acceleration_structure_to_memory(const VkCopyAccelerationStructureToMemoryInfoKHR &info) const noexcept {
  return api->pfn_copyAccelerationStructureToMemory(api->handle, nullptr, &info);
}
VkResult device::copy_memory_to_acceleration_structure(const VkCopyMemoryToAccelerationStructureInfoKHR &info) const noexcept {
  return api->pfn_copyMemoryToAccelerationStructure(api->handle, nullptr, &info);
}
VkResult device::build_acceleration_structures(VkDeferredOperationKHR deferredOperation, u32 infoCount, const VkAccelerationStructureBuildGeometryInfoKHR *pInfos, const VkAccelerationStructureBuildRangeInfoKHR *const *ppBuildRangeInfos) const noexcept {
  return api->pfn_buildAccelerationStructure(api->handle, deferredOperation, infoCount, pInfos, ppBuildRangeInfos);
}
VkResult device::copy_acceleration_structure(VkDeferredOperationKHR deferredOperation, const VkCopyAccelerationStructureInfoKHR &info) const noexcept {
  return api->pfn_copyAccelerationStructure(api->handle, deferredOperation, &info);
}
VkResult device::copy_acceleration_structure_to_memory(VkDeferredOperationKHR deferredOperation, const VkCopyAccelerationStructureToMemoryInfoKHR &info) const noexcept {
  return api->pfn_copyAccelerationStructureToMemory(api->handle, deferredOperation, &info);
}
VkResult device::copy_memory_to_acceleration_structure(VkDeferredOperationKHR deferredOperation, const VkCopyMemoryToAccelerationStructureInfoKHR &info) const noexcept {
  return api->pfn_copyMemoryToAccelerationStructure(api->handle, deferredOperation, &info);
}
VkResult device::write_acceleration_structures_properties(std::span<const VkAccelerationStructureKHR> accelerationStructures, VkQueryType queryType, u64 dataSize, void *pData, u64 stride) const noexcept {
  return api->pfn_writeAccelerationStructureProperties(api->handle, accelerationStructures.size(), accelerationStructures.data(), queryType, dataSize, pData, stride);
}
VkDeviceAddress device::get_acceleration_structure_device_address(const VkAccelerationStructureDeviceAddressInfoKHR &info) const noexcept {
  return api->pfn_getAccelerationStructureDeviceAddress(api->handle, &info);
}
VkAccelerationStructureCompatibilityKHR device::get_device_acceleration_structure_compatibility(const VkAccelerationStructureVersionInfoKHR &info) const noexcept {
  VkAccelerationStructureCompatibilityKHR compat;
  api->pfn_getDeviceAccelerationStructureCompatibility(api->handle, &info, &compat);
  return compat;
}
void device::get_acceleration_structure_build_sizes(VkAccelerationStructureBuildTypeKHR buildType, const VkAccelerationStructureBuildGeometryInfoKHR &buildInfo, const uint32_t *maxPrimitiveCounts, VkAccelerationStructureBuildSizesInfoKHR &sizeInfo) const noexcept {
  api->pfn_getAccelerationStructureBuildSizes(api->handle, buildType, &buildInfo, maxPrimitiveCounts, &sizeInfo);
}



outcome<VkPipelineCache> device::create_pipeline_cache(const VkPipelineCacheCreateInfo &createInfo, const VkAllocationCallbacks *allocCallbacks) const noexcept {
  outcome<VkPipelineCache> outcome;
  outcome.result = api->pfn_createPipelineCache(api->handle, &createInfo, allocCallbacks, &outcome.value);
  return outcome;
}
void device::destroy_pipeline_cache(VkPipelineCache pipelineCache, const VkAllocationCallbacks *allocCallbacks) const noexcept {
  api->pfn_destroyPipelineCache(api->handle, pipelineCache, allocCallbacks);
}
outcome<u64> device::get_pipeline_cache_size(VkPipelineCache pipelineCache) const noexcept {
  outcome<u64> outcome;
  outcome.result = api->pfn_getPipelineCacheData(api->handle, pipelineCache, &outcome.value, nullptr);
  return outcome;
}
VkResult device::get_pipeline_cache_data(VkPipelineCache pipelineCache, std::span<byte> data) const noexcept {
  u64 length = data.size();
  return api->pfn_getPipelineCacheData(api->handle, pipelineCache, &length, data.data());
}
VkResult device::merge_pipeline_caches(VkPipelineCache dstCache, std::span<const VkPipelineCache> fromCaches) const noexcept {
  return api->pfn_mergePipelineCaches(api->handle, dstCache, fromCaches.size(), fromCaches.data());
}

outcome<VkPipelineLayout> device::create_pipeline_layout(const VkPipelineLayoutCreateInfo &createInfo, const VkAllocationCallbacks *allocCallbacks) const noexcept {
  outcome<VkPipelineLayout> outcome;
  outcome.result = api->pfn_createPipelineLayout(api->handle, &createInfo, allocCallbacks, &outcome.value);
  return outcome;
}
void device::destroy_pipeline_layout(VkPipelineLayout pipelineLayout, const VkAllocationCallbacks *allocCallbacks) const noexcept {
  api->pfn_destroyPipelineLayout(api->handle, pipelineLayout, allocCallbacks);
}

VkResult device::create_graphics_pipelines(VkPipelineCache pipelineCache, u32 pipelineCount, const VkGraphicsPipelineCreateInfo *createInfos, VkPipeline *pipelines, const VkAllocationCallbacks *allocCallbacks) const noexcept {
  return api->pfn_createGraphicsPipeline(api->handle, pipelineCache, pipelineCount, createInfos, allocCallbacks, pipelines);
}
VkResult device::create_compute_pipelines(VkPipelineCache pipelineCache, u32 pipelineCount, const VkComputePipelineCreateInfo *createInfos, VkPipeline *pipelines, const VkAllocationCallbacks *allocCallbacks) const noexcept {
  return api->pfn_createComputePipeline(api->handle, pipelineCache, pipelineCount, createInfos, allocCallbacks, pipelines);
}
VkResult device::create_ray_tracing_pipelines(VkDeferredOperationKHR deferredOp, VkPipelineCache pipelineCache, u32 pipelineCount, const VkRayTracingPipelineCreateInfoKHR *createInfos, VkPipeline *pipelines, const VkAllocationCallbacks *allocCallbacks) const noexcept {
  return api->pfn_createRayTracingPipelineKHR(api->handle, deferredOp, pipelineCache, pipelineCount, createInfos, allocCallbacks, pipelines);
}
void     device::destroy_pipeline(VkPipeline pipeline, const VkAllocationCallbacks *allocCallbacks) const noexcept {
  api->pfn_destroyPipeline(api->handle, pipeline, allocCallbacks);
}

VkResult device::get_ray_tracing_shader_group_handles(VkPipeline pipeline, u32 firstGroup, u32 groupCount, std::span<byte> data) const noexcept {
  return api->pfn_getRayTracingShaderGroupHandlesKHR(api->handle, pipeline, firstGroup, groupCount, data.size(), data.data());
}
VkResult device::get_ray_tracing_capture_replay_shader_group_handles(VkPipeline pipeline, u32 firstGroup, u32 groupCount, std::span<byte> data) const noexcept {
  return api->pfn_getRayTracingCaptureReplayShaderGroupHandlesKHR(api->handle, pipeline, firstGroup, groupCount, data.size(), data.data());
}
outcome<u32> device::count_pipeline_executable_properties(const VkPipelineInfoKHR &info) const noexcept {
  outcome<u32> outcome;
  outcome.result = api->pfn_getPipelineExecutableProperties(api->handle, &info, &outcome.value, nullptr);
  return outcome;
}
outcome<u32> device::count_pipeline_executable_statistics(const VkPipelineExecutableInfoKHR &info) const noexcept {
  outcome<u32> outcome;
  outcome.result = api->pfn_getPipelineExecutableStatistics(api->handle, &info, &outcome.value, nullptr);
  return outcome;
}
outcome<u32> device::count_pipeline_executable_internal_representations(const VkPipelineExecutableInfoKHR &info) const noexcept {
  outcome<u32> outcome;
  outcome.result = api->pfn_getPipelineExecutableInternalRepresentations(api->handle, &info, &outcome.value, nullptr);
  return outcome;
}
VkResult device::enumerate_pipeline_executable_properties(const VkPipelineInfoKHR &info, std::span<VkPipelineExecutablePropertiesKHR> properties) const noexcept {
  u32 length = properties.size();
  return api->pfn_getPipelineExecutableProperties(api->handle, &info, &length, properties.data());
}
VkResult device::enumerate_pipeline_executable_statistics(const VkPipelineExecutableInfoKHR &info, std::span<VkPipelineExecutableStatisticKHR> statistics) const noexcept {
  u32 length = statistics.size();
  return api->pfn_getPipelineExecutableStatistics(api->handle, &info, &length, statistics.data());
}
VkResult device::enumerate_pipeline_executable_internal_representations(const VkPipelineExecutableInfoKHR &info, std::span<VkPipelineExecutableInternalRepresentationKHR> internalRepresentations) const noexcept {
  u32 length = internalRepresentations.size();
  return api->pfn_getPipelineExecutableInternalRepresentations(api->handle, &info, &length, internalRepresentations.data());
}



outcome<VkDescriptorPool> device::create_descriptor_pool(const VkDescriptorPoolCreateInfo &createInfo, const VkAllocationCallbacks *allocCallbacks) const noexcept {
  outcome<VkDescriptorPool> outcome;
  outcome.result = api->pfn_createDescriptorPool(api->handle, &createInfo, allocCallbacks, &outcome.value);
  return outcome;
}
void     device::destroy_descriptor_pool(VkDescriptorPool descriptorPool, const VkAllocationCallbacks *allocCallbacks) const noexcept {
  api->pfn_destroyDescriptorPool(api->handle, descriptorPool, allocCallbacks);
}
VkResult device::reset_descriptor_pool(VkDescriptorPool descriptorPool, VkDescriptorPoolResetFlags flags) const noexcept {
  return api->pfn_resetDescriptorPool(api->handle, descriptorPool, flags);
}

outcome<VkDescriptorSetLayout> device::create_descriptor_set_layout(const VkDescriptorSetLayoutCreateInfo &createInfo, const VkAllocationCallbacks *allocCallbacks) const noexcept {
  outcome<VkDescriptorSetLayout> outcome;
  outcome.result = api->pfn_createDescriptorSetLayout(api->handle, &createInfo, allocCallbacks, &outcome.value);
  return outcome;
}
void device::destroy_descriptor_set_layout(VkDescriptorSetLayout descriptorSetLayout, const VkAllocationCallbacks *allocCallbacks) const noexcept {
  api->pfn_destroyDescriptorSetLayout(api->handle, descriptorSetLayout, allocCallbacks);
}
void device::get_descriptor_set_layout_support(const VkDescriptorSetLayoutCreateInfo &createInfo, VkDescriptorSetLayoutSupport &support) const noexcept {
  api->pfn_getDescriptorSetLayoutSupport(api->handle, &createInfo, &support);
}

outcome<VkDescriptorUpdateTemplate> device::create_descriptor_update_template(const VkDescriptorUpdateTemplateCreateInfo &createInfo, const VkAllocationCallbacks *allocCallbacks) const noexcept {
  outcome<VkDescriptorUpdateTemplate> outcome;
  outcome.result = api->pfn_createDescriptorUpdateTemplate(api->handle, &createInfo, allocCallbacks, &outcome.value);
  return outcome;
}
void device::destroy_descriptor_update_template(VkDescriptorUpdateTemplate descriptorUpdateTemplate, const VkAllocationCallbacks *allocCallbacks) const noexcept {
  api->pfn_destroyDescriptorUpdateTemplate(api->handle, descriptorUpdateTemplate, allocCallbacks);
}

VkResult device::allocate_descriptor_sets(const VkDescriptorSetAllocateInfo &allocateInfo, VkDescriptorSet *descriptorSets) const noexcept {
  return api->pfn_allocateDescriptorSets(api->handle, &allocateInfo, descriptorSets);
}
void device::free_descriptor_sets(VkDescriptorPool descriptorPool, std::span<const VkDescriptorSet> descriptorSets) const noexcept {
  api->pfn_freeDescriptorSets(api->handle, descriptorPool, descriptorSets.size(), descriptorSets.data());
}
void device::update_descriptor_sets(std::span<const VkWriteDescriptorSet> writes, std::span<const VkCopyDescriptorSet> copies) const noexcept {
  api->pfn_updateDescriptorSets(api->handle, writes.size(), writes.data(), copies.size(), copies.data());
}
void device::update_descriptor_set(VkDescriptorSet descriptorSet, VkDescriptorUpdateTemplate updateTemplate, const void *data) const noexcept {
  api->pfn_updateDescriptorSetWithTemplate(api->handle, descriptorSet, updateTemplate, data);
}

outcome<VkFramebuffer> device::create_framebuffer(const VkFramebufferCreateInfo &createInfo, const VkAllocationCallbacks *callbacks) const noexcept {
  outcome<VkFramebuffer> outcome;
  outcome.result = api->pfn_createFramebuffer(api->handle, &createInfo, callbacks, &outcome.value);
  return outcome;
}
void device::destroy_framebuffer(VkFramebuffer framebuffer, const VkAllocationCallbacks *callbacks) const noexcept {
  api->pfn_destroyFramebuffer(api->handle, framebuffer, callbacks);
}

outcome<VkRenderPass> device::create_render_pass(const VkRenderPassCreateInfo2 &createInfo, const VkAllocationCallbacks *callbacks) const noexcept {
  outcome<VkRenderPass> outcome;
  outcome.result = api->pfn_createRenderPass2(api->handle, &createInfo, callbacks, &outcome.value);
  return outcome;
}
void device::destroy_render_pass(VkRenderPass renderPass, const VkAllocationCallbacks *callbacks) const noexcept {
  api->pfn_destroyRenderPass(api->handle, renderPass, callbacks);
}
extent2d<u32> device::get_render_area_granularity(VkRenderPass renderPass) const noexcept {
  extent2d<u32> ext;
  api->pfn_getRenderAreaGranularity(api->handle, renderPass, reinterpret_cast<VkExtent2D*>(&ext));
  return ext;
}


outcome<VkCommandPool> device::create_command_pool(const VkCommandPoolCreateInfo &info, const VkAllocationCallbacks *callbacks) const noexcept {
  outcome<VkCommandPool> outcome;
  outcome.result = api->pfn_createCommandPool(api->handle, &info, callbacks, &outcome.value);
  return outcome;
}
void device::destroy_command_pool(VkCommandPool commandPool, const VkAllocationCallbacks *callbacks) const noexcept {
  api->pfn_destroyCommandPool(api->handle, commandPool, callbacks);
}
VkResult device::reset_command_pool(VkCommandPool commandPool, VkCommandPoolResetFlags flags) const noexcept {
  return api->pfn_resetCommandPool(api->handle, commandPool, flags);
}
outcome<VkCommandBuffer> device::allocate_command_buffer(VkCommandPool commandPool, VkCommandBufferLevel level) const noexcept {
  outcome<VkCommandBuffer> outcome;
  const VkCommandBufferAllocateInfo info{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .pNext = nullptr,
      .commandPool = commandPool,
      .level       = level,
      .commandBufferCount = 1
  };
  outcome.result = api->pfn_allocateCommandBuffers(api->handle, &info, &outcome.value);
  return outcome;
}
VkResult device::allocate_command_buffers(VkCommandPool commandPool, VkCommandBufferLevel level, std::span<VkCommandBuffer> commandBuffers) const noexcept {
  const VkCommandBufferAllocateInfo info{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .pNext = nullptr,
      .commandPool = commandPool,
      .level       = level,
      .commandBufferCount = static_cast<u32>(commandBuffers.size())
  };
  return api->pfn_allocateCommandBuffers(api->handle, &info, commandBuffers.data());
}
void device::free_command_buffer(VkCommandPool commandPool, VkCommandBuffer commandBuffer) const noexcept {
  api->pfn_freeCommandBuffers(api->handle, commandPool, 1, &commandBuffer);
}
void device::free_command_buffers(VkCommandPool commandPool, std::span<const VkCommandBuffer> commandBuffers) const noexcept {
  api->pfn_freeCommandBuffers(api->handle, commandPool, commandBuffers.size(), commandBuffers.data());
}
void device::trim_command_pool(VkCommandPool commandPool, VkCommandPoolTrimFlags flags) const noexcept {
  api->pfn_trimCommandPool(api->handle, commandPool, flags);
}


// command buffers

VkResult device::start_recording(VkCommandBuffer commandBuffer, const VkCommandBufferBeginInfo &info) const noexcept {
  return api->command_buffer.pfn_beginCommandBuffer(commandBuffer, &info);
}
VkResult device::finish_recording(VkCommandBuffer commandBuffer) const noexcept {
  return api->command_buffer.pfn_endCommandBuffer(commandBuffer);
}
VkResult device::reset(VkCommandBuffer commandBuffer, VkCommandBufferResetFlags flags) const noexcept {
  return api->command_buffer.pfn_resetCommandBuffer(commandBuffer, flags);
}

void device::cmd_execute_commands(VkCommandBuffer commandBuffer, std::span<const VkCommandBuffer> buffers) const noexcept {
  api->command_buffer.pfn_cmdExecuteCommands(commandBuffer, buffers.size(), buffers.data());
}
void device::cmd_begin_render_pass(VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo &passInfo, const VkSubpassBeginInfo &subpassInfo) const noexcept {
  api->command_buffer.pfn_cmdBeginRenderPass2(commandBuffer, &passInfo, &subpassInfo);
}
void device::cmd_next_subpass(VkCommandBuffer commandBuffer, const VkSubpassBeginInfo &beginInfo, const VkSubpassEndInfo &endInfo) const noexcept {
  api->command_buffer.pfn_cmdNextSubpass2(commandBuffer, &beginInfo, &endInfo);
}
void device::cmd_end_render_pass(VkCommandBuffer commandBuffer, const VkSubpassEndInfo &endInfo) const noexcept {
  api->command_buffer.pfn_cmdEndRenderPass2(commandBuffer, &endInfo);
}
void device::cmd_pipeline_barrier(VkCommandBuffer commandBuffer, const VkDependencyInfoKHR &dependencyInfo) const noexcept {
  api->command_buffer.pfn_cmdPipelineBarrier(commandBuffer, &dependencyInfo);
}

void device::cmd_set_device_mask(VkCommandBuffer commandBuffer, u32 mask) const noexcept {
  api->command_buffer.pfn_cmdSetDeviceMask(commandBuffer, mask);
}

void device::cmd_begin_conditional_rendering(VkCommandBuffer commandBuffer, const VkConditionalRenderingBeginInfoEXT &beginInfo) const noexcept {
  api->command_buffer.pfn_cmdBeginConditionalRenderingEXT(commandBuffer, &beginInfo);
}
void device::cmd_end_conditional_rendering(VkCommandBuffer commandBuffer) const noexcept {
  api->command_buffer.pfn_cmdEndConditionalRenderingEXT(commandBuffer);
}

void device::cmd_preprocess_generated_commands(VkCommandBuffer commandBuffer, const VkGeneratedCommandsInfoNV &commandsInfo) const noexcept {
  api->command_buffer.pfn_cmdPreprocessGeneratedCommandsNV(commandBuffer, &commandsInfo);
}
void device::cmd_execute_preprocessed_commands(VkCommandBuffer commandBuffer, const VkGeneratedCommandsInfoNV &commandsInfo) const noexcept {
  api->command_buffer.pfn_cmdExecuteGeneratedCommandsNV(commandBuffer, VK_TRUE, &commandsInfo);
}
void device::cmd_execute_and_preprocess_generated_commands(VkCommandBuffer commandBuffer, const VkGeneratedCommandsInfoNV &commandsInfo) const noexcept {
  api->command_buffer.pfn_cmdExecuteGeneratedCommandsNV(commandBuffer, VK_FALSE, &commandsInfo);
}

void device::cmd_set_event(VkCommandBuffer commandBuffer, VkEvent event, const VkDependencyInfoKHR &dependencyInfo) const noexcept {
  api->command_buffer.pfn_cmdSetEvent(commandBuffer, event, &dependencyInfo);
}
void device::cmd_reset_event(VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags2KHR pipelineStages) const noexcept {
  api->command_buffer.pfn_cmdResetEvent(commandBuffer, event, pipelineStages);
}
void device::cmd_wait_events(VkCommandBuffer commandBuffer, u32 eventCount, const VkEvent *events, const VkDependencyInfoKHR *dependencies) const noexcept {
  api->command_buffer.pfn_cmdWaitEvents(commandBuffer, eventCount, events, dependencies);
}

void device::cmd_push_constants(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, VkPipelineStageFlags2KHR stageFlags, u32 offset, std::span<const byte> data) const noexcept {
  api->command_buffer.pfn_cmdPushConstants(commandBuffer, pipelineLayout, stageFlags, offset, data.size(), data.data());
}
void device::cmd_push_descriptor_set(VkCommandBuffer commandBuffer, VkPipelineBindPoint bindPoint, VkPipelineLayout pipelineLayout, u32 set, std::span<const VkWriteDescriptorSet> descriptorSets) const noexcept {
  api->command_buffer.pfn_cmdPushDescriptorSetKHR(commandBuffer, bindPoint, pipelineLayout, set, descriptorSets.size(), descriptorSets.data());
}
void device::cmd_push_descriptor_set(VkCommandBuffer commandBuffer, VkDescriptorUpdateTemplate updateTemplate, VkPipelineLayout pipelineLayout, u32 set, const void *data) const noexcept {
  api->command_buffer.pfn_cmdPushDescriptorSetWithTemplateKHR(commandBuffer, updateTemplate, pipelineLayout, set, data);
}
void device::cmd_bind_pipeline(VkCommandBuffer commandBuffer, VkPipelineBindPoint bindPoint, VkPipeline pipeline) const noexcept {
  api->command_buffer.pfn_cmdBindPipeline(commandBuffer, bindPoint, pipeline);
}
void device::cmd_bind_descriptor_sets(VkCommandBuffer commandBuffer, VkPipelineBindPoint bindPoint, VkPipelineLayout layout, u32 firstSet, std::span<const VkDescriptorSet> descriptorSets, std::span<const u32> dynamicOffsets) const noexcept {
  api->command_buffer.pfn_cmdBindDescriptorSets(commandBuffer, bindPoint, layout, firstSet, descriptorSets.size(), descriptorSets.data(), dynamicOffsets.size(), dynamicOffsets.data());
}
void device::cmd_bind_index_buffer(VkCommandBuffer commandBuffer, VkBuffer buffer, u64 bufferSize, VkIndexType indexType) const noexcept {
  api->command_buffer.pfn_cmdBindIndexBuffer(commandBuffer, buffer, bufferSize, indexType);
}
void device::cmd_bind_vertex_buffers(VkCommandBuffer commandBuffer, u32 firstBinding, u32 bindingCount, const VkBuffer *pBuffers, const u64 *pOffsets) const noexcept {
  api->command_buffer.pfn_cmdBindVertexBuffers(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets);
}
void device::cmd_bind_vertex_buffers(VkCommandBuffer commandBuffer, u32 firstBinding, u32 bindingCount, const VkBuffer *pBuffers, const u64 *pOffsets, const u64 * sizes, const u64 * strides) const noexcept {
  api->command_buffer.pfn_cmdBindVertexBuffers2EXT(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets, sizes, strides);
}
void device::cmd_bind_shading_rate_image(VkCommandBuffer commandBuffer, VkImageView image, VkImageLayout layout) const noexcept {
  api->command_buffer.pfn_cmdBindShadingRateImageNV(commandBuffer, image, layout);
}
void device::cmd_bind_transform_feedback_buffers(VkCommandBuffer commandBuffer, u32 firstBinding, u32 bindingCount, const VkBuffer *pBuffers, const u64 *pOffsets, const u64 *pSizes) const noexcept {
  api->command_buffer.pfn_cmdBindTransformFeedbackBuffersEXT(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets, pSizes);
}
void device::cmd_bind_pipeline_shader_group(VkCommandBuffer commandBuffer, VkPipelineBindPoint bindPoint, VkPipeline pipeline, u32 groupIndex) const noexcept {
  api->command_buffer.pfn_cmdBindPipelineShaderGroupNV(commandBuffer, bindPoint, pipeline, groupIndex);
}


void device::cmd_set_viewport(VkCommandBuffer commandBuffer, u32 firstViewport, u32 viewportCount, const VkViewport *viewports) const noexcept {
  api->command_buffer.pfn_cmdSetViewport(commandBuffer, firstViewport, viewportCount, viewports);
}
void device::cmd_set_scissor(VkCommandBuffer commandBuffer, u32 firstScissor, u32 scissorCount, const rect2d *scissors) const noexcept {
  api->command_buffer.pfn_cmdSetScissor(commandBuffer, firstScissor, scissorCount, reinterpret_cast<const VkRect2D*>(scissors));
}
void device::cmd_set_line_width(VkCommandBuffer commandBuffer, f32 lineWidth) const noexcept {
  api->command_buffer.pfn_cmdSetLineWidth(commandBuffer, lineWidth);
}
void device::cmd_set_depth_bias(VkCommandBuffer commandBuffer, f32 constantFactor, f32 clamp, f32 slopeFactor) const noexcept {
  api->command_buffer.pfn_cmdSetDepthBias(commandBuffer, constantFactor, clamp, slopeFactor);
}
void device::cmd_set_blend_constants(VkCommandBuffer commandBuffer, std::span<const f32, 4> constants) const noexcept {
  api->command_buffer.pfn_cmdSetBlendConstants(commandBuffer, constants.data());
}
void device::cmd_set_depth_bounds(VkCommandBuffer commandBuffer, interval<f32> depthBounds) const noexcept {
  api->command_buffer.pfn_cmdSetDepthBounds(commandBuffer, depthBounds.min, depthBounds.max);
}
void device::cmd_set_stencil_compare_mask(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, u32 compareMask) const noexcept {
  api->command_buffer.pfn_cmdSetStencilCompareMask(commandBuffer, faceMask, compareMask);
}
void device::cmd_set_stencil_write_mask(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, u32 writeMask) const noexcept {
  api->command_buffer.pfn_cmdSetStencilWriteMask(commandBuffer, faceMask, writeMask);
}
void device::cmd_set_stencil_reference(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, u32 reference) const noexcept {
  api->command_buffer.pfn_cmdSetStencilReference(commandBuffer, faceMask, reference);
}
void device::cmd_set_viewport_w_scaling(VkCommandBuffer commandBuffer, u32 firstViewport, u32 viewportCount, const VkViewportWScalingNV *scaling) const noexcept {
  api->command_buffer.pfn_cmdSetViewportWScalingNV(commandBuffer, firstViewport, viewportCount, scaling);
}
void device::cmd_set_discard_rectangle(VkCommandBuffer commandBuffer, u32 firstRect, u32 rectCount, const rect2d *discardRectangles) const noexcept {
  api->command_buffer.pfn_cmdSetDiscardRectangleEXT(commandBuffer, firstRect, rectCount, reinterpret_cast<const VkRect2D*>(discardRectangles));
}
void device::cmd_set_sample_locations(VkCommandBuffer commandBuffer, const VkSampleLocationsInfoEXT &sampleLocations) const noexcept {
  api->command_buffer.pfn_cmdSetSampleLocationsEXT(commandBuffer, &sampleLocations);
}
void device::cmd_set_viewport_shading_rate_palette(VkCommandBuffer commandBuffer, u32 firstViewport, u32 viewportCount, const VkShadingRatePaletteNV *palettes) const noexcept {
  api->command_buffer.pfn_cmdSetViewportShadingRatePaletteNV(commandBuffer, firstViewport, viewportCount, palettes);
}
void device::cmd_set_coarse_sample_order(VkCommandBuffer commandBuffer, VkCoarseSampleOrderTypeNV type, std::span<const VkCoarseSampleOrderCustomNV> customOrders) const noexcept {
  api->command_buffer.pfn_cmdSetCoarseSampleOrderNV(commandBuffer, type, customOrders.size(), customOrders.data());
}
void device::cmd_set_exclusive_scissor(VkCommandBuffer commandBuffer, u32 firstScissor, u32 scissorCount, const rect2d *scissors) const noexcept {
  api->command_buffer.pfn_cmdSetExclusiveScissorNV(commandBuffer, firstScissor, scissorCount, reinterpret_cast<const VkRect2D*>(scissors));
}
void device::cmd_set_fragment_shading_rate(VkCommandBuffer commandBuffer, extent2d<u32> fragmentSize, const VkFragmentShadingRateCombinerOpKHR &combinerOps) const noexcept {
  api->command_buffer.pfn_cmdSetFragmentShadingRateKHR(commandBuffer, reinterpret_cast<const VkExtent2D*>(&fragmentSize), &combinerOps);
}
void device::cmd_set_fragment_shading_rate(VkCommandBuffer commandBuffer, VkFragmentShadingRateNV shadingRate, const VkFragmentShadingRateCombinerOpKHR &combinerOps) const noexcept {
  api->command_buffer.pfn_cmdSetFragmentShadingRateEnumNV(commandBuffer, shadingRate, &combinerOps);
}
void device::cmd_set_line_stipple(VkCommandBuffer commandBuffer, u32 factor, u16 pattern) const noexcept {
  api->command_buffer.pfn_cmdSetLineStippleEXT(commandBuffer, factor, pattern);
}
void device::cmd_set_cull_mode(VkCommandBuffer commandBuffer, VkCullModeFlags cullMode) const noexcept {
  api->command_buffer.pfn_cmdSetCullModeEXT(commandBuffer, cullMode);
}
void device::cmd_set_front_face(VkCommandBuffer commandBuffer, VkFrontFace frontFace) const noexcept {
  api->command_buffer.pfn_cmdSetFrontFaceEXT(commandBuffer, frontFace);
}
void device::cmd_set_primitive_topology(VkCommandBuffer commandBuffer, VkPrimitiveTopology primitiveTopology) const noexcept {
  api->command_buffer.pfn_cmdSetPrimitiveTopologyEXT(commandBuffer, primitiveTopology);
}
void device::cmd_set_viewport(VkCommandBuffer commandBuffer, std::span<const VkViewport> viewports) const noexcept {
  api->command_buffer.pfn_cmdSetViewportWithCountEXT(commandBuffer, viewports.size(), viewports.data());
}
void device::cmd_set_scissor(VkCommandBuffer commandBuffer, std::span<const rect2d> scissors) const noexcept {
  api->command_buffer.pfn_cmdSetScissorWithCountEXT(commandBuffer, scissors.size(), reinterpret_cast<const VkRect2D*>(scissors.data()));
}
void device::cmd_set_depth_test_enable(VkCommandBuffer commandBuffer, bool32 enabled) const noexcept {
  api->command_buffer.pfn_cmdSetDepthTestEnableEXT(commandBuffer, reinterpret_cast<VkBool32&>(enabled));
}
void device::cmd_set_depth_write_enable(VkCommandBuffer commandBuffer, bool32 enabled) const noexcept {
  api->command_buffer.pfn_cmdSetDepthWriteEnableEXT(commandBuffer, reinterpret_cast<VkBool32&>(enabled));
}
void device::cmd_set_depth_compare_op(VkCommandBuffer commandBuffer, VkCompareOp compareOp) const noexcept {
  api->command_buffer.pfn_cmdSetDepthCompareOpEXT(commandBuffer, compareOp);
}
void device::cmd_set_depth_bounds_test_enable(VkCommandBuffer commandBuffer, bool32 enabled) const noexcept {
  api->command_buffer.pfn_cmdSetDepthBoundsTestEnableEXT(commandBuffer, reinterpret_cast<VkBool32&>(enabled));
}
void device::cmd_set_stencil_test_enable(VkCommandBuffer commandBuffer, bool32 enabled) const noexcept {
  api->command_buffer.pfn_cmdSetStencilTestEnableEXT(commandBuffer, reinterpret_cast<VkBool32&>(enabled));
}
void device::cmd_set_stencil_op(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, VkStencilOp failOp, VkStencilOp passOp, VkStencilOp depthFailOp, VkCompareOp compareOp) const noexcept {
  api->command_buffer.pfn_cmdSetStencilOpEXT(commandBuffer, faceMask, failOp, passOp, depthFailOp, compareOp);
}
void device::cmd_set_ray_tracing_pipeline_stack_size(VkCommandBuffer commandBuffer, u32 stackSize) const noexcept {
  api->command_buffer.pfn_cmdSetRayTracingPipelineStackSizeKHR(commandBuffer, stackSize);
}
void device::cmd_set_patch_control_points(VkCommandBuffer commandBuffer, u32 patchControlPoints) const noexcept {
  api->command_buffer.pfn_cmdSetPatchControlPoints(commandBuffer, patchControlPoints);
}
void device::cmd_set_rasterizer_discard_enable(VkCommandBuffer commandBuffer, bool32 enabled) const noexcept {
  api->command_buffer.pfn_cmdSetRasterizerDiscardEnable(commandBuffer, reinterpret_cast<VkBool32&>(enabled));
}
void device::cmd_set_depth_bias_enable(VkCommandBuffer commandBuffer, bool32 enabled) const noexcept {
  api->command_buffer.pfn_cmdSetDepthBiasEnable(commandBuffer, reinterpret_cast<VkBool32&>(enabled));
}
void device::cmd_set_logic_op(VkCommandBuffer commandBuffer, VkLogicOp op) const noexcept {
  api->command_buffer.pfn_cmdSetLogicOp(commandBuffer, op);
}
void device::cmd_set_primitive_restart_enable(VkCommandBuffer commandBuffer, bool32 enabled) const noexcept {
  api->command_buffer.pfn_cmdSetPrimitiveRestartEnable(commandBuffer, reinterpret_cast<VkBool32&>(enabled));
}
void device::cmd_set_vertex_input(VkCommandBuffer commandBuffer, std::span<const VkVertexInputBindingDescription2EXT> bindingDescriptions, std::span<const VkVertexInputAttributeDescription2EXT> attributeDescriptions) const noexcept {
  api->command_buffer.pfn_cmdSetVertexInput(commandBuffer, bindingDescriptions.size(), bindingDescriptions.data(), attributeDescriptions.size(), attributeDescriptions.data());
}
void device::cmd_set_color_write_enable(VkCommandBuffer commandBuffer, std::span<const bool32> enabled) const noexcept {
  api->command_buffer.pfn_cmdSetColorWriteEnable(commandBuffer, enabled.size(), reinterpret_cast<const VkBool32*>(enabled.data()));
}




void device::cmd_insert_debug_label(VkCommandBuffer commandBuffer, const VkDebugUtilsLabelEXT &debugLabel) const noexcept {
  api->command_buffer.pfn_cmdInsertDebugUtilsLabelEXT(commandBuffer, &debugLabel);
}
void device::cmd_begin_debug_label(VkCommandBuffer commandBuffer, const VkDebugUtilsLabelEXT &debugLabel) const noexcept {
  api->command_buffer.pfn_cmdBeginDebugUtilsLabelEXT(commandBuffer, &debugLabel);
}
void device::cmd_end_debug_label(VkCommandBuffer commandBuffer) const noexcept {
  api->command_buffer.pfn_cmdEndDebugUtilsLabelEXT(commandBuffer);
}
void device::cmd_set_checkpoint(VkCommandBuffer commandBuffer, const void *checkpointData) const noexcept {
  api->command_buffer.pfn_cmdSetCheckpointNV(commandBuffer, checkpointData);
}
VkResult device::cmd_set_performance_marker(VkCommandBuffer commandBuffer, const VkPerformanceMarkerInfoINTEL &info) const noexcept {
  return api->command_buffer.pfn_cmdSetPerformanceMarkerINTEL(commandBuffer, &info);
}
VkResult device::cmd_set_performance_stream_marker(VkCommandBuffer commandBuffer, const VkPerformanceStreamMarkerInfoINTEL &info) const noexcept {
  return api->command_buffer.pfn_cmdSetPerformanceStreamMarkerINTEL(commandBuffer, &info);
}
VkResult device::cmd_set_performance_override(VkCommandBuffer commandBuffer, const VkPerformanceOverrideInfoINTEL &info) const noexcept {
  return api->command_buffer.pfn_cmdSetPerformanceOverrideINTEL(commandBuffer, &info);
}
void device::cmd_draw(VkCommandBuffer commandBuffer, u32 vertexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance) const noexcept {
  api->command_buffer.pfn_cmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}
void device::cmd_draw_indexed(VkCommandBuffer commandBuffer, u32 indexCount, u32 instanceCount, u32 firstIndex, i32 vertexOffset, u32 firstInstance) const noexcept {
  api->command_buffer.pfn_cmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}
void device::cmd_draw_indirect(VkCommandBuffer commandBuffer, device_buffer buffer, u32 drawCount, u32 stride) const noexcept {
  api->command_buffer.pfn_cmdDrawIndirect(commandBuffer, buffer.handle, buffer.offset, drawCount, stride);
}
void device::cmd_draw_indirect_count(VkCommandBuffer commandBuffer, device_buffer buffer, device_buffer countBuffer, u32 maxDrawCount, u32 stride) const noexcept {
  api->command_buffer.pfn_cmdDrawIndirectCount(commandBuffer, buffer.handle, buffer.offset, countBuffer.handle, countBuffer.offset, maxDrawCount, stride);
}
void device::cmd_draw_indexed_indirect(VkCommandBuffer commandBuffer, device_buffer buffer, u32 drawCount, u32 stride) const noexcept {
  api->command_buffer.pfn_cmdDrawIndexedIndirect(commandBuffer, buffer.handle, buffer.offset, drawCount, stride);
}
void device::cmd_draw_indexed_indirect_count(VkCommandBuffer commandBuffer, device_buffer buffer, device_buffer countBuffer, u32 maxDrawCount, u32 stride) const noexcept {
  api->command_buffer.pfn_cmdDrawIndexedIndirectCount(commandBuffer, buffer.handle, buffer.offset, countBuffer.handle, countBuffer.offset, maxDrawCount, stride);
}
void device::cmd_draw_indirect_byte_count(VkCommandBuffer commandBuffer, u32 instanceCount, u32 firstInstance, device_buffer counterBuffer, u32 counterOffset, u32 vertexStride) const noexcept {
  api->command_buffer.pfn_cmdDrawIndirectByteCountEXT(commandBuffer, instanceCount, firstInstance, counterBuffer.handle, counterBuffer.offset, counterOffset, vertexStride);
}
void device::cmd_draw_mesh_tasks(VkCommandBuffer commandBuffer, u32 taskCount, u32 firstTask) const noexcept {
  api->command_buffer.pfn_cmdDrawMeshTasksNV(commandBuffer, taskCount, firstTask);
}
void device::cmd_draw_mesh_tasks_indirect(VkCommandBuffer commandBuffer, device_buffer buffer, u32 drawCount, u32 stride) const noexcept {
  api->command_buffer.pfn_cmdDrawMeshTasksIndirectNV(commandBuffer, buffer.handle, buffer.offset, drawCount, stride);
}
void device::cmd_draw_mesh_tasks_indirect_count(VkCommandBuffer commandBuffer, device_buffer buffer, device_buffer countBuffer, u32 maxDrawCount, u32 stride) const noexcept {
  api->command_buffer.pfn_cmdDrawMeshTasksIndirectCountNV(commandBuffer, buffer.handle, buffer.offset, countBuffer.handle, countBuffer.offset, maxDrawCount, stride);
}
void device::cmd_dispatch(VkCommandBuffer commandBuffer, extent3d<u32> groupExtents) const noexcept {
  api->command_buffer.pfn_cmdDispatch(commandBuffer, groupExtents.width, groupExtents.height, groupExtents.depth);
}
void device::cmd_dispatch_indirect(VkCommandBuffer commandBuffer, device_buffer buffer) const noexcept {
  api->command_buffer.pfn_cmdDispatchIndirect(commandBuffer, buffer.handle, buffer.offset);
}
void device::cmd_dispatch_base(VkCommandBuffer commandBuffer, position3d<u32> baseGroup, extent3d<u32> groupExtents) const noexcept {
  api->command_buffer.pfn_cmdDispatchBase(commandBuffer, baseGroup.x, baseGroup.y, baseGroup.z, groupExtents.width, groupExtents.height, groupExtents.depth);
}
void device::cmd_trace_rays(VkCommandBuffer commandBuffer, const shader_table &shaders, extent3d<u32> extents) const noexcept {
  api->command_buffer.pfn_cmdTraceRaysKHR(commandBuffer, &shaders.raygen, &shaders.miss, &shaders.hit, &shaders.callable, extents.width, extents.height, extents.depth);
}
void device::cmd_trace_rays_indirect(VkCommandBuffer commandBuffer, const shader_table &shaders, u64 indirectAddress) const noexcept {
  api->command_buffer.pfn_cmdTraceRaysIndirectKHR(commandBuffer, &shaders.raygen, &shaders.miss, &shaders.hit, &shaders.callable, indirectAddress);
}
void device::cmd_launch_cuda_kernel(VkCommandBuffer commandBuffer, const VkCuLaunchInfoNVX &launchInfo) const noexcept {
  api->command_buffer.pfn_cmdCuLaunchKernelNVX(commandBuffer, &launchInfo);
}



void device::cmd_blit_image(VkCommandBuffer commandBuffer, device_image src, device_image dst, std::span<const VkImageBlit> blitz, VkFilter filter) const noexcept {
  api->command_buffer.pfn_cmdBlitImage(commandBuffer, src.handle, src.layout, dst.handle, dst.layout, blitz.size(), blitz.data(), filter);
}
void device::cmd_blit_image(VkCommandBuffer commandBuffer, const VkBlitImageInfo2KHR &info) const noexcept {
  api->command_buffer.pfn_cmdBlitImage2KHR(commandBuffer, &info);
}
void device::cmd_copy_image(VkCommandBuffer commandBuffer, device_image src, device_image dst, std::span<const VkImageCopy> copies) const noexcept {
  api->command_buffer.pfn_cmdCopyImage(commandBuffer, src.handle, src.layout, dst.handle, dst.layout, copies.size(), copies.data());
}
void device::cmd_copy_image(VkCommandBuffer commandBuffer, const VkCopyImageInfo2KHR &info) const noexcept {
  api->command_buffer.pfn_cmdCopyImage2KHR(commandBuffer, &info);
}
void device::cmd_resolve_image(VkCommandBuffer commandBuffer, device_image src, device_image dst, std::span<const VkImageResolve> resolves) const noexcept {
  api->command_buffer.pfn_cmdResolveImage(commandBuffer, src.handle, src.layout, dst.handle, dst.layout, resolves.size(), resolves.data());
}
void device::cmd_resolve_image(VkCommandBuffer commandBuffer, const VkResolveImageInfo2KHR &info) const noexcept {
  api->command_buffer.pfn_cmdResolveImage2KHR(commandBuffer, &info);
}
void device::cmd_clear_color_image(VkCommandBuffer commandBuffer, device_image image, const VkClearColorValue &value, std::span<const VkImageSubresourceRange> ranges) const noexcept {
  api->command_buffer.pfn_cmdClearColorImage(commandBuffer, image.handle, image.layout, &value, ranges.size(), ranges.data());
}
void device::cmd_clear_depth_stencil_image(VkCommandBuffer commandBuffer, device_image image, const VkClearDepthStencilValue &value, std::span<const VkImageSubresourceRange> ranges) const noexcept {
  api->command_buffer.pfn_cmdClearDepthStencilImage(commandBuffer, image.handle, image.layout, &value, ranges.size(), ranges.data());
}
void device::cmd_copy_buffer(VkCommandBuffer commandBuffer, VkBuffer src, VkBuffer dst, std::span<const VkBufferCopy> copies) const noexcept {
  api->command_buffer.pfn_cmdCopyBuffer(commandBuffer, src, dst, copies.size(), copies.data());
}
void device::cmd_copy_buffer(VkCommandBuffer commandBuffer, const VkCopyBufferInfo2KHR &info) const noexcept {
  api->command_buffer.pfn_cmdCopyBuffer2KHR(commandBuffer, &info);
}
void device::cmd_update_buffer(VkCommandBuffer commandBuffer, device_buffer dst, std::span<const byte> data) const noexcept {
  api->command_buffer.pfn_cmdUpdateBuffer(commandBuffer, dst.handle, dst.offset, data.size(), data.data());
}
void device::cmd_fill_buffer(VkCommandBuffer commandBuffer, device_buffer dst, u64 size, u32 data) const noexcept {
  api->command_buffer.pfn_cmdFillBuffer(commandBuffer, dst.handle, dst.offset, size, data);
}
void device::cmd_copy_buffer_to_image(VkCommandBuffer commandBuffer, VkBuffer src, device_image dst, std::span<const VkBufferImageCopy> copies) const noexcept {
  api->command_buffer.pfn_cmdCopyBufferToImage(commandBuffer, src, dst.handle, dst.layout, copies.size(), copies.data());
}
void device::cmd_copy_buffer_to_image(VkCommandBuffer commandBuffer, const VkCopyBufferToImageInfo2KHR &info) const noexcept {
  api->command_buffer.pfn_cmdCopyBufferToImage2KHR(commandBuffer, &info);
}
void device::cmd_copy_image_to_buffer(VkCommandBuffer commandBuffer, device_image src, VkBuffer dst, std::span<const VkBufferImageCopy> copies) const noexcept {
  api->command_buffer.pfn_cmdCopyImageToBuffer(commandBuffer, src.handle, src.layout, dst, copies.size(), copies.data());
}
void device::cmd_copy_image_to_buffer(VkCommandBuffer commandBuffer, const VkCopyImageToBufferInfo2KHR &info) const noexcept {
  api->command_buffer.pfn_cmdCopyImageToBuffer2KHR(commandBuffer, &info);
}
void device::cmd_clear_attachments(VkCommandBuffer commandBuffer, std::span<const VkClearAttachment> attachments, std::span<const VkClearRect> clearRectangles) const noexcept {
  api->command_buffer.pfn_cmdClearAttachments(commandBuffer, attachments.size(), attachments.data(), clearRectangles.size(), clearRectangles.data());
}



void device::cmd_build_acceleration_structures(VkCommandBuffer commandBuffer, u32 infoCount, const VkAccelerationStructureBuildGeometryInfoKHR *pInfos, const VkAccelerationStructureBuildRangeInfoKHR *const *ppBuildRangeInfos) const noexcept {
  api->command_buffer.pfn_cmdBuildAccelerationStructuresKHR(commandBuffer, infoCount, pInfos, ppBuildRangeInfos);
}
void device::cmd_build_acceleration_structures_indirect(VkCommandBuffer commandBuffer, u32 infoCount, const VkAccelerationStructureBuildGeometryInfoKHR *pInfos, const u64 *pIndirectDeviceAddresses, const u32 *pIndirectStrides, const u32 *const *ppMaxPrimitiveCounts) const noexcept {
  api->command_buffer.pfn_cmdBuildAccelerationStructuresIndirectKHR(commandBuffer, infoCount, pInfos, pIndirectDeviceAddresses, pIndirectStrides, ppMaxPrimitiveCounts);
}
void device::cmd_write_acceleration_structures_properties(VkCommandBuffer commandBuffer, std::span<const VkAccelerationStructureKHR> accelerationStructures, VkQueryType queryType, VkQueryPool queryPool, u32 firstQuery) const noexcept {
  api->command_buffer.pfn_cmdWriteAccelerationStructuresPropertiesKHR(commandBuffer, accelerationStructures.size(), accelerationStructures.data(), queryType, queryPool, firstQuery);
}
void device::cmd_copy_acceleration_structure(VkCommandBuffer commandBuffer, const VkCopyAccelerationStructureInfoKHR &info) const noexcept {
  api->command_buffer.pfn_cmdCopyAccelerationStructureKHR(commandBuffer, &info);
}
void device::cmd_copy_acceleration_structure_to_memory(VkCommandBuffer commandBuffer, const VkCopyAccelerationStructureToMemoryInfoKHR &info) const noexcept {
  api->command_buffer.pfn_cmdCopyAccelerationStructureToMemoryKHR(commandBuffer, &info);
}
void device::cmd_copy_memory_to_acceleration_structure(VkCommandBuffer commandBuffer, const VkCopyMemoryToAccelerationStructureInfoKHR &info) const noexcept {
  api->command_buffer.pfn_cmdCopyMemoryToAccelerationStructureKHR(commandBuffer, &info);
}



void device::cmd_write_timestamp(VkCommandBuffer commandBuffer, VkPipelineStageFlags2KHR pipelineStage, VkQueryPool queryPool, u32 query) const noexcept {
  api->command_buffer.pfn_cmdWriteTimestamp(commandBuffer, pipelineStage, queryPool, query);
}
void device::cmd_begin_transform_feedback(VkCommandBuffer commandBuffer, u32 firstCounterBuffer, u32 counterBufferCount, const VkBuffer *counterBuffers, const u64 *counterBufferOffsets) const noexcept {
  api->command_buffer.pfn_cmdBeginTransformFeedbackEXT(commandBuffer, firstCounterBuffer, counterBufferCount, counterBuffers, counterBufferOffsets);
}
void device::cmd_end_transform_feedback(VkCommandBuffer commandBuffer, u32 firstCounterBuffer, u32 counterBufferCount, const VkBuffer *counterBuffers, const u64 *counterBufferOffsets) const noexcept {
  api->command_buffer.pfn_cmdEndTransformFeedbackEXT(commandBuffer, firstCounterBuffer, counterBufferCount, counterBuffers, counterBufferOffsets);
}
void device::cmd_begin_query_indexed(VkCommandBuffer commandBuffer, VkQueryPool queryPool, u32 query, VkQueryControlFlags flags, u32 index) const noexcept {
  api->command_buffer.pfn_cmdBeginQueryIndexedEXT(commandBuffer, queryPool, query, flags, index);
}
void device::cmd_end_query_indexed(VkCommandBuffer commandBuffer, VkQueryPool queryPool, u32 query, u32 index) const noexcept {
  api->command_buffer.pfn_cmdEndQueryIndexedEXT(commandBuffer, queryPool, query, index);
}
void device::cmd_begin_query(VkCommandBuffer commandBuffer, VkQueryPool queryPool, u32 query, VkQueryControlFlags flags) const noexcept {
  api->command_buffer.pfn_cmdBeginQuery(commandBuffer, queryPool, query, flags);
}
void device::cmd_end_query(VkCommandBuffer commandBuffer, VkQueryPool queryPool, u32 query) const noexcept {
  api->command_buffer.pfn_cmdEndQuery(commandBuffer, queryPool, query);
}
void device::cmd_reset_query_pool(VkCommandBuffer commandBuffer, VkQueryPool queryPool, u32 firstQuery, u32 queryCount) const noexcept {
  api->command_buffer.pfn_cmdResetQueryPool(commandBuffer, queryPool, firstQuery, queryCount);
}
void device::cmd_copy_query_pool_results(VkCommandBuffer commandBuffer, VkQueryPool queryPool, u32 firstQuery, u32 queryCount, device_buffer dstBuffer, u64 stride, VkQueryResultFlags flags) const noexcept {
  api->command_buffer.pfn_cmdCopyQueryPoolResults(commandBuffer, queryPool, firstQuery, queryCount, dstBuffer.handle, dstBuffer.offset, stride, flags);
}
void device::cmd_write_buffer_marker2(VkCommandBuffer commandBuffer, VkPipelineStageFlags2KHR stage, device_buffer dstBuffer, u32 marker) const noexcept {
  api->command_buffer.pfn_cmdWriteBufferMarker(commandBuffer, stage, dstBuffer.handle, dstBuffer.offset, marker);
}


/*



struct CommandBufferFunctions{
  DeviceFunctionLoader loader;

#define VK_cmd_function(name_, ...) VK_function(Cmd##name_, cmd##name_)

  VK_function(BeginCommandBuffer, begin);
  VK_function(EndCommandBuffer, end);
  VK_function(ResetCommandBuffer, reset);


    =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*[ Vulkan Commands ]*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
   *
   *              Command                                 | Command Buffer Level | Render Pass Scope |         Queue Types         | Pipeline Type
   *



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
*/