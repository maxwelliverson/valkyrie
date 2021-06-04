//
// Created by maxwe on 2021-04-23.
//

#ifndef VALKYRIE_INTERNAL_GRAPHICS_API_HPP
#define VALKYRIE_INTERNAL_GRAPHICS_API_HPP

#include <valkyrie/utility/version.hpp>
#include <valkyrie/adt/interval.hpp>
#include <valkyrie/utility/shapes.hpp>
#include <valkyrie/extended_primitives.hpp>

#include <span>

#if VK_system_windows
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define VK_USE_PLATFORM_WIN32_KHR
#elif VK_system_linux
#define VK_USE_PLATFORM_XCB_KHR
#elif VK_system_macos
#define VK_USE_PLATFORM_MACOS_MVK
#define VK_USE_PLATFORM_METAL_EXT
#endif

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>


//#define PFN(functionName) const PFN_vk##functionName functionName



extern "C" {

VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkCuModuleNVX);
VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkCuFunctionNVX);
/*using VkCuModuleNVX   = struct VkCuModuleNVX_T*;
using VkCuFunctionNVX = struct VkCuFunctionNVX_T*;*/

struct VkCuModuleCreateInfoNVX{
  VkStructureType sType;
  const void*     pNext;
  const void*     pData;
};

struct VkCuFunctionCreateInfoNVX{
  VkStructureType sType;
  const void*     pNext;
  VkCuModuleNVX   module;
  const char*     pName;
};

struct VkCuLaunchInfoNVX{
  VkStructureType sType;
  const void*     pNext;
  VkCuFunctionNVX function;
  uint32_t        gridDimX;
  uint32_t        gridDimY;
  uint32_t        gridDimZ;
  uint32_t        blockDimX;
  uint32_t        blockDimY;
  uint32_t        blockDimZ;
  uint32_t        sharedMemBytes;
  size_t          paramCount;
  const void*     pParams;
  size_t          extraCount;
  const void*     pExtra;
};


using PFN_vkCreateCuModuleNVX = VkResult(VKAPI_PTR*)(VkDevice device, const VkCuModuleCreateInfoNVX* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkCuModuleNVX* pModule);
using PFN_vkDestroyCuModuleNVX = void(VKAPI_PTR*)(VkDevice device, VkCuModuleNVX module, const VkAllocationCallbacks* pAllocator);
using PFN_vkCreateCuFunctionNVX = VkResult(VKAPI_PTR*)(VkDevice device, const VkCuFunctionCreateInfoNVX* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkCuFunctionNVX* pFunction);
using PFN_vkDestroyCuFunctionNVX = void(VKAPI_PTR*)(VkDevice device, VkCuFunctionNVX function, const VkAllocationCallbacks* pAllocator);
using PFN_vkCmdCuLaunchKernelNVX = void(VKAPI_PTR*)(VkCommandBuffer commandBuffer, const VkCuLaunchInfoNVX* pLaunchInfo);

}



namespace valkyrie::graphics::api{




  struct shader_table{
    VkStridedDeviceAddressRegionKHR raygen;
    VkStridedDeviceAddressRegionKHR hit;
    VkStridedDeviceAddressRegionKHR miss;
    VkStridedDeviceAddressRegionKHR callable;
  };
  struct device_buffer{
    VkBuffer handle;
    u64      offset;
  };
  struct device_image{
    VkImage       handle;
    VkImageLayout layout;
  };

  struct viewport : position2d<f32>, extent2d<f32>{
    interval<f32> depth;
  };






  using rect2d = box2d<i32, u32>;


  template <typename T>
  struct outcome{
    T        value;
    VkResult result;
  };

  class physical_device;
  class command_buffer;
  class device;
  class queue;



  bool                initialize() noexcept;


  VkResult            initialize_instance(const VkInstanceCreateInfo& createInfo, const VkAllocationCallbacks* callbacks = nullptr) noexcept;

  outcome<version>    instance_version() noexcept;
  outcome<u32>        count_instance_extensions(const char* layer) noexcept;
  outcome<u32>        count_layers() noexcept;
  VkResult            enumerate_instance_extensions(const char* layer, std::span<VkExtensionProperties> extensionProperties) noexcept;
  VkResult            enumerate_layers(std::span<VkLayerProperties> layerProperties) noexcept;




  outcome<u32> count_physical_devices() noexcept;
  outcome<u32> count_physical_device_groups() noexcept;
  VkResult     enumerate_physical_devices(std::span<physical_device> devices) noexcept;
  VkResult     enumerate_physical_device_groups(std::span<VkPhysicalDeviceGroupProperties> deviceGroups) noexcept;

  outcome<VkSurfaceKHR> create_surface(const VkWin32SurfaceCreateInfoKHR& createInfo, const VkAllocationCallbacks* allocCallbacks = nullptr) noexcept;
  outcome<VkSurfaceKHR> create_surface(const VkHeadlessSurfaceCreateInfoEXT& createInfo, const VkAllocationCallbacks* allocCallbacks = nullptr) noexcept;
  outcome<VkSurfaceKHR> create_surface(const VkDisplaySurfaceCreateInfoKHR& createInfo, const VkAllocationCallbacks* allocCallbacks = nullptr) noexcept;
  void                  destroy_surface(VkSurfaceKHR surface, const VkAllocationCallbacks* allocCallbacks = nullptr) noexcept;

  outcome<VkDebugUtilsMessengerEXT> create_debug_messenger(const VkDebugUtilsMessengerCreateInfoEXT& createInfo, const VkAllocationCallbacks* allocCallbacks = nullptr) noexcept;
  void                              submit_debug_message(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT& data) noexcept;
  void                              destroy_debug_messenger(VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* allocCallbacks = nullptr) noexcept;




  namespace impl{
    class device_api;
    class queue_api;
    class command_buffer_api;
  }

  class device{
    const impl::device_api* api;

    friend class physical_device;

  public:

    // [ device operations ]

    void     destroy() const noexcept;
    VkResult wait_idle() const noexcept;


    // [ queues ]

    VkQueue  get_queue(u32 queueFamily, u32 queueIndex) const noexcept;
    VkQueue  get_queue(const VkDeviceQueueInfo2& info) const noexcept;

    VkResult submit(VkQueue queue, std::span<const VkSubmitInfo2KHR> submissions, VkFence fence) const noexcept;
    VkResult bind_sparse(VkQueue queue, std::span<const VkBindSparseInfo> bindings, VkFence fence) const noexcept;
    VkResult present(VkQueue queue, const VkPresentInfoKHR& info) const noexcept;

    void     begin_debug_label(VkQueue queue, const VkDebugUtilsLabelEXT& label) const noexcept;
    void     end_debug_label(VkQueue queue) const noexcept;
    void     insert_debug_label(VkQueue queue, const VkDebugUtilsLabelEXT& label) const noexcept;

    u32      count_checkpoint_data(VkQueue queue) const noexcept;
    void     enumerate_checkpoint_data(VkQueue queue, std::span<VkCheckpointData2NV> checkpoints) const noexcept;

    VkResult set_performance_configuration(VkQueue queue, VkPerformanceConfigurationINTEL performanceConfig) const noexcept;

    VkResult wait_idle(VkQueue queue) const noexcept;







    outcome<VkDeviceMemory> allocate_memory(const VkMemoryAllocateInfo& info, const VkAllocationCallbacks* allocCallbacks = nullptr) const noexcept;
    void                    free_memory(VkDeviceMemory memory, const VkAllocationCallbacks* allocCallbacks = nullptr) const noexcept;
    outcome<void*>          map_memory(VkDeviceMemory memory, u64 offset, u64 size, VkMemoryMapFlags flags = 0) const noexcept;
    void                    unmap_memory(VkDeviceMemory memory) const noexcept;
    VkResult                flush_mapped_memory_ranges(std::span<const VkMappedMemoryRange> mappedRanges) const noexcept;
    VkResult                invalidate_mapped_memory_ranges(std::span<const VkMappedMemoryRange> mappedRanges) const noexcept;
    u64                     get_device_memory_commitment(VkDeviceMemory memory) const noexcept;
    u64                     get_opaque_capture_address(const VkDeviceMemoryOpaqueCaptureAddressInfo& info) const noexcept;

    outcome<VkEvent>        create_event(const VkEventCreateInfo& info, const VkAllocationCallbacks* allocCallbacks = nullptr) const noexcept;
    void                    destroy_event(VkEvent event, const VkAllocationCallbacks* allocCallbacks = nullptr) const noexcept;
    VkResult                get_event_status(VkEvent event) const noexcept;
    VkResult                set_event(VkEvent event) const noexcept;
    VkResult                reset_event(VkEvent event) const noexcept;

    outcome<VkQueryPool>    create_query_pool(const VkQueryPoolCreateInfo& info, const VkAllocationCallbacks* allocCallbacks = nullptr) const noexcept;
    void                    destroy_query_pool(VkQueryPool queryPool, const VkAllocationCallbacks* allocCallbacks = nullptr) const noexcept;
    VkResult                get_query_pool_results(VkQueryPool queryPool, u32 firstQuery, u32 queryCount, std::span<byte> data, u64 stride, VkQueryResultFlags flags) const noexcept;
    void                    reset_query_pool(VkQueryPool queryPool, u32 firstQuery, u32 queryCount);


    outcome<VkShaderModule> create_shader_module(const VkShaderModuleCreateInfo& info, const VkAllocationCallbacks* callbacks = nullptr) const noexcept;
    void                    destroy_shader_module(VkShaderModule shaderModule, const VkAllocationCallbacks* callbacks = nullptr) const noexcept;

    outcome<VkSampler>      create_sampler(const VkSamplerCreateInfo& info, const VkAllocationCallbacks* callbacks = nullptr) const noexcept;
    void                    destroy_sampler(VkSampler sampler, const VkAllocationCallbacks* callbacks = nullptr) const noexcept;

    outcome<VkSamplerYcbcrConversion> create_sampler_ycbcr_conversion(const VkSamplerYcbcrConversionCreateInfo& createInfo, const VkAllocationCallbacks* callbacks = nullptr) const noexcept;
    void                              destroy_sampler_ycbcr_conversion(VkSamplerYcbcrConversion samplerYcbcrConversion, const VkAllocationCallbacks* callbacks = nullptr) const noexcept;


    outcome<void*>          get_memory_win32_handle(const VkMemoryGetWin32HandleInfoKHR& info) const noexcept;
    VkResult                get_memory_win32_handle_properties(VkExternalMemoryHandleTypeFlagBits handleType, void* handle, VkMemoryWin32HandlePropertiesKHR& properties) const noexcept;
    outcome<int>            get_memory_fd(const VkMemoryGetFdInfoKHR& info) const noexcept;
    VkResult                get_memory_fd_properties(VkExternalMemoryHandleTypeFlagBits fdType, int fd, VkMemoryFdPropertiesKHR& properties) const noexcept;

    VkPeerMemoryFeatureFlags get_device_group_peer_memory_features(u32 heapIndex, u32 localDeviceIndex, u32 remoteDeviceIndex) const noexcept;
    VkResult                 get_memory_host_pointer_properties(VkExternalMemoryHandleTypeFlagBits handleType, const void* hostPointer, VkMemoryHostPointerPropertiesEXT& memoryHostPointerProperties) const noexcept;
    void                     get_generated_commands_memory_requirements(const VkGeneratedCommandsMemoryRequirementsInfoNV& info, VkMemoryRequirements2& req) const noexcept;


    outcome<VkSemaphore> create_semaphore(const VkSemaphoreCreateInfo& createInfo, const VkAllocationCallbacks* callbacks = nullptr) const noexcept;
    void                 destroy_semaphore(VkSemaphore semaphore, const VkAllocationCallbacks* callbacks = nullptr) const noexcept;
    outcome<u64>         get_semaphore_counter_value(VkSemaphore semaphore) const noexcept;
    VkResult             wait_semaphores(const VkSemaphoreWaitInfo& info, u64 timeout_ns) const noexcept;
    VkResult             signal_semaphore(const VkSemaphoreSignalInfo& info) const noexcept;
    VkResult             import_semaphore_win32_handle(const VkImportSemaphoreWin32HandleInfoKHR& info) const noexcept;
    outcome<void*>       get_semaphore_win32_handle(const VkSemaphoreGetWin32HandleInfoKHR& info) const noexcept;
    VkResult             import_semaphore_fd(const VkImportSemaphoreFdInfoKHR& info) const noexcept;
    outcome<int>         get_semaphore_fd(const VkSemaphoreGetFdInfoKHR& info) const noexcept;


    outcome<VkFence>     create_fence(const VkFenceCreateInfo& createInfo, const VkAllocationCallbacks* callbacks = nullptr) const noexcept;
    void                 destroy_fence(VkFence fence, const VkAllocationCallbacks* callbacks = nullptr) const noexcept;
    VkResult             reset_fences(std::span<const VkFence> fences) const noexcept;
    VkResult             get_fence_status(VkFence fence) const noexcept;
    VkResult             wait_for_fences(std::span<const VkFence> fences, bool32 waitAll, u64 timeout_ns) const noexcept;
    outcome<VkFence>     register_device_event(const VkDeviceEventInfoEXT& info, const VkAllocationCallbacks* callbacks = nullptr) const noexcept;
    outcome<VkFence>     register_display_event(VkDisplayKHR display, const VkDisplayEventInfoEXT& info, const VkAllocationCallbacks* callbacks = nullptr) const noexcept;
    VkResult             import_fence_win32_handle(const VkImportFenceWin32HandleInfoKHR& info) const noexcept;
    outcome<void*>       get_fence_win32_handle(const VkFenceGetWin32HandleInfoKHR& info) const noexcept;
    VkResult             import_fence_fd(const VkImportFenceFdInfoKHR& info) const noexcept;
    outcome<int>         get_fence_fd(const VkFenceGetFdInfoKHR& info) const noexcept;




    VkResult             set_object_debug_name(const VkDebugUtilsObjectNameInfoEXT& info) const noexcept;
    VkResult             set_object_debug_tag(const VkDebugUtilsObjectTagInfoEXT& info) const noexcept;

    outcome<VkValidationCacheEXT> create_validation_cache(const VkValidationCacheCreateInfoEXT& info, const VkAllocationCallbacks* callbacks = nullptr) const noexcept;
    void                          destroy_validation_cache(VkValidationCacheEXT validationCache, const VkAllocationCallbacks* callbacks = nullptr) const noexcept;
    VkResult                      merge_validation_caches(VkValidationCacheEXT dstCache, std::span<const VkValidationCacheEXT> srcCache) const noexcept;
    outcome<u64>                  get_validation_cache_size(VkValidationCacheEXT validationCache) const noexcept;
    VkResult                      get_validation_cache_data(VkValidationCacheEXT validationCache, std::span<byte> dest) const noexcept;

    VkResult             acquire_profiling_lock(const VkAcquireProfilingLockInfoKHR& info) const noexcept;
    void                 release_profiling_lock() const noexcept;

    outcome<u64>                  get_calibrated_timestamps(std::span<const VkCalibratedTimestampInfoEXT> timeDomains, u64* timestamps) const noexcept;

    VkResult                      initialize_performance_api(const VkInitializePerformanceApiInfoINTEL& info) const noexcept;
    void                          uninitialize_performance_api() const noexcept;
    outcome<VkPerformanceConfigurationINTEL> acquire_performance_configuration(const VkPerformanceConfigurationAcquireInfoINTEL& info) const noexcept;
    void                          release_performance_configuration(VkPerformanceConfigurationINTEL perfConfig) const noexcept;

    outcome<VkPerformanceValueINTEL> get_performance_parameter(VkPerformanceParameterTypeINTEL type) const noexcept;





    outcome<VkIndirectCommandsLayoutNV> create_indirect_commands_layout(const VkIndirectCommandsLayoutCreateInfoNV& info, const VkAllocationCallbacks* callbacks = nullptr) const noexcept;
    void                                destroy_indirect_commands_layout(VkIndirectCommandsLayoutNV indirectCommandsLayout, const VkAllocationCallbacks* callbacks = nullptr) const noexcept;



    outcome<VkPrivateDataSlotEXT> create_private_data_slot(const VkPrivateDataSlotCreateInfoEXT& info, const VkAllocationCallbacks* callbacks = nullptr) const noexcept;
    void                          destroy_private_data_slot(VkPrivateDataSlotEXT privateDataSlot, const VkAllocationCallbacks* callbacks = nullptr) const noexcept;
    VkResult                      set_private_data(VkObjectType objectType, u64 objectHandle, VkPrivateDataSlotEXT privateDataSlot, u64 value) const noexcept;
    u64                           get_private_data(VkObjectType objectType, u64 objectHandle, VkPrivateDataSlotEXT privateDataSlot) const noexcept;


    outcome<VkDeferredOperationKHR> create_deferred_operation(const VkAllocationCallbacks* callbacks = nullptr) const noexcept;
    void                            destroy_deferred_operation(VkDeferredOperationKHR deferredOp, const VkAllocationCallbacks* callbacks = nullptr) const noexcept;
    u32                             get_deferred_operation_max_concurrency(VkDeferredOperationKHR deferredOp) const noexcept;
    VkResult                        get_deferred_operation_result(VkDeferredOperationKHR deferredOp) const noexcept;
    VkResult                        deferred_operation_join(VkDeferredOperationKHR deferredOp) const noexcept;


    outcome<VkSwapchainKHR> create_swapchain(const VkSwapchainCreateInfoKHR& createInfo, const VkAllocationCallbacks* callbacks = nullptr) const noexcept;
    VkResult                create_shared_swapchains(std::span<const VkSwapchainCreateInfoKHR> infos, VkSwapchainKHR* swapchains, const VkAllocationCallbacks* callbacks = nullptr) const noexcept;
    void                    destroy_swapchain(VkSwapchainKHR swapchain, const VkAllocationCallbacks* callbacks = nullptr) const noexcept;
    outcome<u32>            count_swapchain_images(VkSwapchainKHR swapchain) const noexcept;
    VkResult                enumerate_swapchain_images(VkSwapchainKHR swapchain, std::span<VkImage> images) const noexcept;
    outcome<u32>            acquire_next_image(const VkAcquireNextImageInfoKHR& info) const noexcept;
    VkResult                get_swapchain_status(VkSwapchainKHR swapchain) const noexcept;
    VkResult acquire_fullscreen_exclusive_mode(VkSwapchainKHR swapchain) const noexcept;
    VkResult release_fullscreen_exclusive_mode(VkSwapchainKHR swapchain) const noexcept;
    outcome<u64>      get_swapchain_counter(VkSwapchainKHR swapchain, VkSurfaceCounterFlagBitsEXT counter) const noexcept;
    void              set_hdr_metadata(u32 swapchainCount, const VkSwapchainKHR* swapchains, const VkHdrMetadataEXT* metadata) const noexcept;
    void              set_local_dimming(VkSwapchainKHR swapchain, bool32 enabled) const noexcept;

    outcome<VkDeviceGroupPresentModeFlagsKHR> get_device_group_surface_present_modes(VkSurfaceKHR surface) const noexcept;
    outcome<VkDeviceGroupPresentModeFlagsKHR> get_device_group_surface_present_modes(const VkPhysicalDeviceSurfaceInfo2KHR& info) const noexcept;
    VkResult                                  get_device_group_present_capabilities(VkDeviceGroupPresentCapabilitiesKHR& capabilities) const noexcept;

    VkResult          display_power_control(VkDisplayKHR display, const VkDisplayPowerInfoEXT& info) const noexcept;



    outcome<VkBuffer> create_buffer(const VkBufferCreateInfo& info, const VkAllocationCallbacks* callbacks = nullptr) const noexcept;
    void              destroy_buffer(VkBuffer buffer, const VkAllocationCallbacks* callbacks = nullptr) const noexcept;
    VkResult          bind_buffer_memory(std::span<const VkBindBufferMemoryInfo> infos) const noexcept;
    void              get_buffer_memory_requirements(const VkBufferMemoryRequirementsInfo2& info, VkMemoryRequirements2& requirements) const noexcept;
    u64               get_opaque_capture_address(const VkBufferDeviceAddressInfo& info) const noexcept;
    VkDeviceAddress   get_buffer_device_address(const VkBufferDeviceAddressInfo& info) const noexcept;
    u64               get_buffer_opaque_capture_address(const VkBufferDeviceAddressInfo& info) const noexcept;


    outcome<VkBufferView> create_buffer_view(const VkBufferViewCreateInfo& createInfo, const VkAllocationCallbacks* callbacks = nullptr) const noexcept;
    void                  destroy_buffer_view(VkBufferView bufferView, const VkAllocationCallbacks* callbacks = nullptr) const noexcept;



    outcome<VkImage>    create_image(const VkImageCreateInfo& info, const VkAllocationCallbacks* callbacks = nullptr) const noexcept;
    void                destroy_image(VkImage image, const VkAllocationCallbacks* callbacks = nullptr) const noexcept;
    VkResult            bind_image_memory(std::span<const VkBindImageMemoryInfo> infos) const noexcept;
    void                get_image_memory_requirements(const VkImageMemoryRequirementsInfo2& info, VkMemoryRequirements2& requirements) const noexcept;
    u32                 count_image_sparse_memory_requirements(const VkImageSparseMemoryRequirementsInfo2& info) const noexcept;
    void                enumerate_image_sparse_memory_requirements(const VkImageSparseMemoryRequirementsInfo2& info, std::span<VkSparseImageMemoryRequirements2> requirements) const noexcept;
    VkSubresourceLayout get_image_subresource_layout(VkImage image, const VkImageSubresource& subresource) const noexcept;
    VkResult            get_image_drm_format_modifier_properties(VkImage image, VkImageDrmFormatModifierPropertiesEXT& properties) const noexcept;


    outcome<VkImageView> create_image_view(const VkImageViewCreateInfo& createInfo, const VkAllocationCallbacks* callbacks = nullptr) const noexcept;
    void                 destroy_image_view(VkImageView imageView, const VkAllocationCallbacks* callbacks = nullptr) const noexcept;
    u32                  get_image_view_handle(const VkImageViewHandleInfoNVX& info) const noexcept;
    VkResult             get_image_view_address(VkImageView imageView, VkImageViewAddressPropertiesNVX& addressProperties) const noexcept;


    outcome<VkAccelerationStructureKHR> create_acceleration_structure(const VkAccelerationStructureCreateInfoKHR& createInfo, const VkAllocationCallbacks* callbacks = nullptr) const noexcept;
    void     destroy_acceleration_structure(VkAccelerationStructureKHR accelerationStructure, const VkAllocationCallbacks* callbacks = nullptr) const noexcept;
    VkResult build_acceleration_structures(u32 infoCount, const VkAccelerationStructureBuildGeometryInfoKHR* pInfos, const VkAccelerationStructureBuildRangeInfoKHR* const* ppBuildRangeInfos) const noexcept;
    VkResult copy_acceleration_structure(const VkCopyAccelerationStructureInfoKHR& info) const noexcept;
    VkResult copy_acceleration_structure_to_memory(const VkCopyAccelerationStructureToMemoryInfoKHR& info) const noexcept;
    VkResult copy_memory_to_acceleration_structure(const VkCopyMemoryToAccelerationStructureInfoKHR& info) const noexcept;
    VkResult build_acceleration_structures(VkDeferredOperationKHR deferredOperation, u32 infoCount, const VkAccelerationStructureBuildGeometryInfoKHR* pInfos, const VkAccelerationStructureBuildRangeInfoKHR* const* ppBuildRangeInfos) const noexcept;
    VkResult copy_acceleration_structure(VkDeferredOperationKHR deferredOperation, const VkCopyAccelerationStructureInfoKHR& info) const noexcept;
    VkResult copy_acceleration_structure_to_memory(VkDeferredOperationKHR deferredOperation, const VkCopyAccelerationStructureToMemoryInfoKHR& info) const noexcept;
    VkResult copy_memory_to_acceleration_structure(VkDeferredOperationKHR deferredOperation, const VkCopyMemoryToAccelerationStructureInfoKHR& info) const noexcept;
    VkResult write_acceleration_structures_properties(std::span<const VkAccelerationStructureKHR> accelerationStructures, VkQueryType queryType, u64 dataSize, void* pData, u64 stride) const noexcept;
    VkDeviceAddress get_acceleration_structure_device_address(const VkAccelerationStructureDeviceAddressInfoKHR& info) const noexcept;
    VkAccelerationStructureCompatibilityKHR get_device_acceleration_structure_compatibility(const VkAccelerationStructureVersionInfoKHR& info) const noexcept;
    void     get_acceleration_structure_build_sizes(VkAccelerationStructureBuildTypeKHR buildType, const VkAccelerationStructureBuildGeometryInfoKHR& buildInfo, const uint32_t* maxPrimitiveCounts, VkAccelerationStructureBuildSizesInfoKHR& sizeInfo) const noexcept;




    outcome<VkPipelineCache> create_pipeline_cache(const VkPipelineCacheCreateInfo& createInfo, const VkAllocationCallbacks* allocCallbacks = nullptr) const noexcept;
    void                     destroy_pipeline_cache(VkPipelineCache pipelineCache, const VkAllocationCallbacks* allocCallbacks = nullptr) const noexcept;
    outcome<u64>             get_pipeline_cache_size(VkPipelineCache pipelineCache) const noexcept;
    VkResult                 get_pipeline_cache_data(VkPipelineCache pipelineCache, std::span<byte> data) const noexcept;
    VkResult                 merge_pipeline_caches(VkPipelineCache dstCache, std::span<const VkPipelineCache> fromCaches) const noexcept;


    outcome<VkPipelineLayout> create_pipeline_layout(const VkPipelineLayoutCreateInfo& createInfo, const VkAllocationCallbacks* allocCallbacks = nullptr) const noexcept;
    void                      destroy_pipeline_layout(VkPipelineLayout pipelineLayout, const VkAllocationCallbacks* allocCallbacks = nullptr) const noexcept;


    VkResult create_graphics_pipelines(VkPipelineCache pipelineCache, u32 pipelineCount, const VkGraphicsPipelineCreateInfo* createInfos, VkPipeline* pipelines, const VkAllocationCallbacks* allocCallbacks = nullptr) const noexcept;
    VkResult create_compute_pipelines(VkPipelineCache pipelineCache, u32 pipelineCount, const VkComputePipelineCreateInfo* createInfos, VkPipeline* pipelines, const VkAllocationCallbacks* allocCallbacks = nullptr) const noexcept;
    VkResult create_ray_tracing_pipelines(VkDeferredOperationKHR deferredOp, VkPipelineCache pipelineCache, u32 pipelineCount, const VkRayTracingPipelineCreateInfoKHR* createInfos, VkPipeline* pipelines, const VkAllocationCallbacks* allocCallbacks = nullptr) const noexcept;
    void     destroy_pipeline(VkPipeline pipeline, const VkAllocationCallbacks* allocCallbacks = nullptr) const noexcept;
    VkResult get_ray_tracing_shader_group_handles(VkPipeline pipeline, u32 firstGroup, u32 groupCount, std::span<byte> data) const noexcept;
    VkResult get_ray_tracing_capture_replay_shader_group_handles(VkPipeline pipeline, u32 firstGroup, u32 groupCount, std::span<byte> data) const noexcept;


    outcome<u32> count_pipeline_executable_properties(const VkPipelineInfoKHR& info) const noexcept;
    outcome<u32> count_pipeline_executable_statistics(const VkPipelineExecutableInfoKHR& info) const noexcept;
    outcome<u32> count_pipeline_executable_internal_representations(const VkPipelineExecutableInfoKHR& info) const noexcept;
    VkResult     enumerate_pipeline_executable_properties(const VkPipelineInfoKHR& info, std::span<VkPipelineExecutablePropertiesKHR> properties) const noexcept;
    VkResult     enumerate_pipeline_executable_statistics(const VkPipelineExecutableInfoKHR& info, std::span<VkPipelineExecutableStatisticKHR> statistics) const noexcept;
    VkResult     enumerate_pipeline_executable_internal_representations(const VkPipelineExecutableInfoKHR& info, std::span<VkPipelineExecutableInternalRepresentationKHR> internalRepresentations) const noexcept;




    outcome<VkDescriptorPool> create_descriptor_pool(const VkDescriptorPoolCreateInfo& createInfo, const VkAllocationCallbacks* allocCallbacks = nullptr) const noexcept;
    void                      destroy_descriptor_pool(VkDescriptorPool descriptorPool, const VkAllocationCallbacks* allocCallbacks = nullptr) const noexcept;
    VkResult                  reset_descriptor_pool(VkDescriptorPool descriptorPool, VkDescriptorPoolResetFlags flags = 0) const noexcept;

    outcome<VkDescriptorSetLayout> create_descriptor_set_layout(const VkDescriptorSetLayoutCreateInfo& createInfo, const VkAllocationCallbacks* allocCallbacks = nullptr) const noexcept;
    void                           destroy_descriptor_set_layout(VkDescriptorSetLayout descriptorSetLayout, const VkAllocationCallbacks* allocCallbacks = nullptr) const noexcept;
    void                           get_descriptor_set_layout_support(const VkDescriptorSetLayoutCreateInfo& createInfo, VkDescriptorSetLayoutSupport& support) const noexcept;

    outcome<VkDescriptorUpdateTemplate> create_descriptor_update_template(const VkDescriptorUpdateTemplateCreateInfo& createInfo, const VkAllocationCallbacks* allocCallbacks = nullptr) const noexcept;
    void                                destroy_descriptor_update_template(VkDescriptorUpdateTemplate descriptorUpdateTemplate, const VkAllocationCallbacks* allocCallbacks = nullptr) const noexcept;

    VkResult allocate_descriptor_sets(const VkDescriptorSetAllocateInfo& allocateInfo, VkDescriptorSet* descriptorSets) const noexcept;
    void     free_descriptor_sets(VkDescriptorPool descriptorPool, std::span<const VkDescriptorSet> descriptorSets) const noexcept;
    void     update_descriptor_sets(std::span<const VkWriteDescriptorSet> writes, std::span<const VkCopyDescriptorSet> copies) const noexcept;
    void     update_descriptor_set(VkDescriptorSet descriptorSet, VkDescriptorUpdateTemplate updateTemplate, const void* data) const noexcept;


    outcome<VkFramebuffer>  create_framebuffer(const VkFramebufferCreateInfo& createInfo, const VkAllocationCallbacks* callbacks = nullptr) const noexcept;
    void                    destroy_framebuffer(VkFramebuffer framebuffer, const VkAllocationCallbacks* callbacks = nullptr) const noexcept;

    outcome<VkRenderPass>   create_render_pass(const VkRenderPassCreateInfo2& createInfo, const VkAllocationCallbacks* callbacks = nullptr) const noexcept;
    void                    destroy_render_pass(VkRenderPass renderPass, const VkAllocationCallbacks* callbacks = nullptr) const noexcept;
    extent2d<u32>           get_render_area_granularity(VkRenderPass renderPass) const noexcept;

    outcome<VkCommandPool>  create_command_pool(const VkCommandPoolCreateInfo& info, const VkAllocationCallbacks* callbacks = nullptr) const noexcept;
    void                    destroy_command_pool(VkCommandPool commandPool, const VkAllocationCallbacks* callbacks = nullptr) const noexcept;
    VkResult                reset_command_pool(VkCommandPool commandPool, VkCommandPoolResetFlags flags) const noexcept;
    outcome<VkCommandBuffer>allocate_command_buffer(VkCommandPool commandPool, VkCommandBufferLevel level) const noexcept;
    VkResult                allocate_command_buffers(VkCommandPool commandPool, VkCommandBufferLevel level, std::span<VkCommandBuffer> commandBuffers) const noexcept;
    void                    free_command_buffer(VkCommandPool commandPool, VkCommandBuffer commandBuffer) const noexcept;
    void                    free_command_buffers(VkCommandPool commandPool, std::span<const VkCommandBuffer> commandBuffers) const noexcept;
    void                    trim_command_pool(VkCommandPool commandPool, VkCommandPoolTrimFlags flags = 0) const noexcept;


    VkResult start_recording(VkCommandBuffer commandBuffer, const VkCommandBufferBeginInfo& info) const noexcept;
    VkResult finish_recording(VkCommandBuffer commandBuffer) const noexcept;
    VkResult reset(VkCommandBuffer commandBuffer, VkCommandBufferResetFlags flags = 0) const noexcept;


    void     cmd_execute_commands(VkCommandBuffer commandBuffer, std::span<const VkCommandBuffer> buffers) const noexcept;
    void     cmd_begin_render_pass(VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo& passInfo, const VkSubpassBeginInfo& subpassInfo) const noexcept;
    void     cmd_next_subpass(VkCommandBuffer commandBuffer, const VkSubpassBeginInfo& beginInfo, const VkSubpassEndInfo& endInfo) const noexcept;
    void     cmd_end_render_pass(VkCommandBuffer commandBuffer, const VkSubpassEndInfo& endInfo) const noexcept;


    void     cmd_pipeline_barrier(VkCommandBuffer commandBuffer, const VkDependencyInfoKHR& dependencyInfo) const noexcept;
    void     cmd_set_device_mask(VkCommandBuffer commandBuffer, u32 mask) const noexcept;
    void     cmd_begin_conditional_rendering(VkCommandBuffer commandBuffer, const VkConditionalRenderingBeginInfoEXT& beginInfo) const noexcept;
    void     cmd_end_conditional_rendering(VkCommandBuffer commandBuffer) const noexcept;


    void     cmd_preprocess_generated_commands(VkCommandBuffer commandBuffer, const VkGeneratedCommandsInfoNV& commandsInfo) const noexcept;
    void     cmd_execute_preprocessed_commands(VkCommandBuffer commandBuffer, const VkGeneratedCommandsInfoNV& commandsInfo) const noexcept;
    void     cmd_execute_and_preprocess_generated_commands(VkCommandBuffer commandBuffer, const VkGeneratedCommandsInfoNV& commandsInfo) const noexcept;


    void     cmd_set_event(VkCommandBuffer commandBuffer, VkEvent event, const VkDependencyInfoKHR& dependencyInfo) const noexcept;
    void     cmd_reset_event(VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags2KHR pipelineStages) const noexcept;
    void     cmd_wait_events(VkCommandBuffer commandBuffer, u32 eventCount, const VkEvent* events, const VkDependencyInfoKHR* dependencies) const noexcept;



    void     cmd_push_constants(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, VkPipelineStageFlags2KHR stageFlags, u32 offset, std::span<const byte> data) const noexcept;
    void     cmd_push_descriptor_set(VkCommandBuffer commandBuffer, VkPipelineBindPoint bindPoint, VkPipelineLayout pipelineLayout, u32 set, std::span<const VkWriteDescriptorSet> descriptorSets) const noexcept;
    void     cmd_push_descriptor_set(VkCommandBuffer commandBuffer, VkDescriptorUpdateTemplate updateTemplate, VkPipelineLayout pipelineLayout, u32 set, const void* data) const noexcept;
    void     cmd_bind_pipeline(VkCommandBuffer commandBuffer, VkPipelineBindPoint bindPoint, VkPipeline pipeline) const noexcept;
    void     cmd_bind_descriptor_sets(VkCommandBuffer commandBuffer, VkPipelineBindPoint bindPoint, VkPipelineLayout layout, u32 firstSet, std::span<const VkDescriptorSet> descriptorSets, std::span<const u32> dynamicOffsets) const noexcept;
    void     cmd_bind_index_buffer(VkCommandBuffer commandBuffer, VkBuffer buffer, u64 bufferSize, VkIndexType indexType) const noexcept;
    void     cmd_bind_vertex_buffers(VkCommandBuffer commandBuffer, u32 firstBinding, u32 bindingCount, const VkBuffer* pBuffers, const u64* pOffsets) const noexcept;
    void     cmd_bind_vertex_buffers(VkCommandBuffer commandBuffer, u32 firstBinding, u32 bindingCount, const VkBuffer* pBuffers, const u64* pOffsets, const u64* size, const u64* strides) const noexcept;
    void     cmd_bind_shading_rate_image(VkCommandBuffer commandBuffer, VkImageView image, VkImageLayout layout) const noexcept;
    void     cmd_bind_transform_feedback_buffers(VkCommandBuffer commandBuffer, u32 firstBinding, u32 bindingCount, const VkBuffer* pBuffers, const u64* pOffsets, const u64* pSizes) const noexcept;
    void     cmd_bind_pipeline_shader_group(VkCommandBuffer commandBuffer, VkPipelineBindPoint bindPoint, VkPipeline pipeline, u32 groupIndex) const noexcept;

    void     cmd_set_viewport(VkCommandBuffer commandBuffer, u32 firstViewport, u32 viewportCount, const VkViewport* viewports) const noexcept;
    void     cmd_set_scissor(VkCommandBuffer commandBuffer, u32 firstScissor, u32 scissorCount, const rect2d* scissors) const noexcept;
    void     cmd_set_line_width(VkCommandBuffer commandBuffer, f32 lineWidth) const noexcept;
    void     cmd_set_depth_bias(VkCommandBuffer commandBuffer, f32 constantFactor, f32 clamp, f32 slopeFactor) const noexcept;
    void     cmd_set_blend_constants(VkCommandBuffer commandBuffer, std::span<const f32, 4> constants) const noexcept;
    void     cmd_set_depth_bounds(VkCommandBuffer commandBuffer, interval<f32> depthBounds) const noexcept;
    void     cmd_set_stencil_compare_mask(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, u32 compareMask) const noexcept;
    void     cmd_set_stencil_write_mask(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, u32 writeMask) const noexcept;
    void     cmd_set_stencil_reference(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, u32 reference) const noexcept;
    void     cmd_set_viewport_w_scaling(VkCommandBuffer commandBuffer, u32 firstViewport, u32 viewportCount, const VkViewportWScalingNV* scaling) const noexcept;
    void     cmd_set_discard_rectangle(VkCommandBuffer commandBuffer, u32 firstRect, u32 rectCount, const rect2d* discardRectangles) const noexcept;
    void     cmd_set_sample_locations(VkCommandBuffer commandBuffer, const VkSampleLocationsInfoEXT& sampleLocations) const noexcept;
    void     cmd_set_viewport_shading_rate_palette(VkCommandBuffer commandBuffer, u32 firstViewport, u32 viewportCount, const VkShadingRatePaletteNV* palettes) const noexcept;
    void     cmd_set_coarse_sample_order(VkCommandBuffer commandBuffer, VkCoarseSampleOrderTypeNV type, std::span<const VkCoarseSampleOrderCustomNV> customOrders) const noexcept;
    void     cmd_set_exclusive_scissor(VkCommandBuffer commandBuffer, u32 firstScissor, u32 scissorCount, const rect2d* scissors) const noexcept;
    void     cmd_set_fragment_shading_rate(VkCommandBuffer commandBuffer, extent2d<u32> fragmentSize, const VkFragmentShadingRateCombinerOpKHR& combinerOps) const noexcept;
    void     cmd_set_fragment_shading_rate(VkCommandBuffer commandBuffer, VkFragmentShadingRateNV shadingRate, const VkFragmentShadingRateCombinerOpKHR& combinerOps) const noexcept;
    void     cmd_set_line_stipple(VkCommandBuffer commandBuffer, u32 factor, u16 pattern) const noexcept;
    void     cmd_set_cull_mode(VkCommandBuffer commandBuffer, VkCullModeFlags cullMode) const noexcept;
    void     cmd_set_front_face(VkCommandBuffer commandBuffer, VkFrontFace frontFace) const noexcept;
    void     cmd_set_primitive_topology(VkCommandBuffer commandBuffer, VkPrimitiveTopology primitiveTopology) const noexcept;
    void     cmd_set_viewport(VkCommandBuffer commandBuffer, std::span<const VkViewport> viewports) const noexcept;
    void     cmd_set_scissor(VkCommandBuffer commandBuffer, std::span<const rect2d> scissors) const noexcept;
    void     cmd_set_depth_test_enable(VkCommandBuffer commandBuffer, bool32 enabled) const noexcept;
    void     cmd_set_depth_write_enable(VkCommandBuffer commandBuffer, bool32 enabled) const noexcept;
    void     cmd_set_depth_compare_op(VkCommandBuffer commandBuffer, VkCompareOp compareOp) const noexcept;
    void     cmd_set_depth_bounds_test_enable(VkCommandBuffer commandBuffer, bool32 enabled) const noexcept;
    void     cmd_set_stencil_test_enable(VkCommandBuffer commandBuffer, bool32 enabled) const noexcept;
    void     cmd_set_stencil_op(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, VkStencilOp failOp, VkStencilOp passOp, VkStencilOp depthFailOp, VkCompareOp compareOp) const noexcept;
    void     cmd_set_ray_tracing_pipeline_stack_size(VkCommandBuffer commandBuffer, u32 stackSize) const noexcept;
    void     cmd_set_patch_control_points(VkCommandBuffer commandBuffer, u32 patchControlPoints) const noexcept;
    void     cmd_set_rasterizer_discard_enable(VkCommandBuffer commandBuffer, bool32 enabled) const noexcept;
    void     cmd_set_depth_bias_enable(VkCommandBuffer commandBuffer, bool32 enabled) const noexcept;
    void     cmd_set_logic_op(VkCommandBuffer commandBuffer, VkLogicOp op) const noexcept;
    void     cmd_set_primitive_restart_enable(VkCommandBuffer commandBuffer, bool32 enabled) const noexcept;
    void     cmd_set_vertex_input(VkCommandBuffer commandBuffer, std::span<const VkVertexInputBindingDescription2EXT> bindingDescriptions, std::span<const VkVertexInputAttributeDescription2EXT> attributeDescriptions) const noexcept;
    void     cmd_set_color_write_enable(VkCommandBuffer commandBuffer, std::span<const bool32> enabled) const noexcept;


    void     cmd_insert_debug_label(VkCommandBuffer commandBuffer, const VkDebugUtilsLabelEXT& debugLabel) const noexcept;
    void     cmd_begin_debug_label(VkCommandBuffer commandBuffer, const VkDebugUtilsLabelEXT& debugLabel) const noexcept;
    void     cmd_end_debug_label(VkCommandBuffer commandBuffer) const noexcept;

    void     cmd_set_checkpoint(VkCommandBuffer commandBuffer, const void* checkpointData) const noexcept;

    VkResult cmd_set_performance_marker(VkCommandBuffer commandBuffer, const VkPerformanceMarkerInfoINTEL& info) const noexcept;
    VkResult cmd_set_performance_stream_marker(VkCommandBuffer commandBuffer, const VkPerformanceStreamMarkerInfoINTEL& info) const noexcept;
    VkResult cmd_set_performance_override(VkCommandBuffer commandBuffer, const VkPerformanceOverrideInfoINTEL& info) const noexcept;


    void     cmd_draw(VkCommandBuffer commandBuffer, u32 vertexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance) const noexcept;
    void     cmd_draw_indexed(VkCommandBuffer commandBuffer, u32 indexCount, u32 instanceCount, u32 firstIndex, i32 vertexOffset, u32 firstInstance) const noexcept;
    void     cmd_draw_indirect(VkCommandBuffer commandBuffer, device_buffer buffer, u32 drawCount, u32 stride) const noexcept;
    void     cmd_draw_indirect_count(VkCommandBuffer commandBuffer, device_buffer buffer, device_buffer countBuffer, u32 maxDrawCount, u32 stride) const noexcept;
    void     cmd_draw_indexed_indirect(VkCommandBuffer commandBuffer, device_buffer buffer, u32 drawCount, u32 stride) const noexcept;
    void     cmd_draw_indexed_indirect_count(VkCommandBuffer commandBuffer, device_buffer buffer, device_buffer countBuffer, u32 maxDrawCount, u32 stride) const noexcept;
    void     cmd_draw_indirect_byte_count(VkCommandBuffer commandBuffer, u32 instanceCount, u32 firstInstance, device_buffer counterBuffer, u32 counterOffset, u32 vertexStride) const noexcept;

    void     cmd_draw_mesh_tasks(VkCommandBuffer commandBuffer, u32 taskCount, u32 firstTask) const noexcept;
    void     cmd_draw_mesh_tasks_indirect(VkCommandBuffer commandBuffer, device_buffer buffer, u32 drawCount, u32 stride) const noexcept;
    void     cmd_draw_mesh_tasks_indirect_count(VkCommandBuffer commandBuffer, device_buffer buffer, device_buffer countBuffer, u32 maxDrawCount, u32 stride) const noexcept;

    void     cmd_dispatch(VkCommandBuffer commandBuffer, extent3d<u32> groupExtents) const noexcept;
    void     cmd_dispatch_indirect(VkCommandBuffer commandBuffer, device_buffer buffer) const noexcept;
    void     cmd_dispatch_base(VkCommandBuffer commandBuffer, position3d<u32> baseGroup, extent3d<u32> groupExtents) const noexcept;

    void     cmd_trace_rays(VkCommandBuffer commandBuffer, const shader_table& shaders, extent3d<u32> extents) const noexcept;
    void     cmd_trace_rays_indirect(VkCommandBuffer commandBuffer, const shader_table& shaders, u64 indirectAddress) const noexcept;

    void     cmd_launch_cuda_kernel(VkCommandBuffer commandBuffer, const VkCuLaunchInfoNVX& launchInfo) const noexcept;


    void     cmd_blit_image(VkCommandBuffer commandBuffer, device_image src, device_image dst, std::span<const VkImageBlit> blitz, VkFilter filter) const noexcept;
    void     cmd_blit_image(VkCommandBuffer commandBuffer, const VkBlitImageInfo2KHR& info) const noexcept;
    void     cmd_copy_image(VkCommandBuffer commandBuffer, device_image src, device_image dst, std::span<const VkImageCopy> copies) const noexcept;
    void     cmd_copy_image(VkCommandBuffer commandBuffer, const VkCopyImageInfo2KHR& info) const noexcept;
    void     cmd_resolve_image(VkCommandBuffer commandBuffer, device_image src, device_image dst, std::span<const VkImageResolve> resolves) const noexcept;
    void     cmd_resolve_image(VkCommandBuffer commandBuffer, const VkResolveImageInfo2KHR& info) const noexcept;
    void     cmd_clear_color_image(VkCommandBuffer commandBuffer, device_image image, const VkClearColorValue& value, std::span<const VkImageSubresourceRange> ranges) const noexcept;
    void     cmd_clear_depth_stencil_image(VkCommandBuffer commandBuffer, device_image image, const VkClearDepthStencilValue& value, std::span<const VkImageSubresourceRange> ranges) const noexcept;

    void     cmd_copy_buffer(VkCommandBuffer commandBuffer, VkBuffer src, VkBuffer dst, std::span<const VkBufferCopy> copies) const noexcept;
    void     cmd_copy_buffer(VkCommandBuffer commandBuffer, const VkCopyBufferInfo2KHR& info) const noexcept;
    void     cmd_update_buffer(VkCommandBuffer commandBuffer, device_buffer dst, std::span<const byte> data) const noexcept;
    void     cmd_fill_buffer(VkCommandBuffer commandBuffer, device_buffer dst, u64 size, u32 data) const noexcept;

    void     cmd_copy_buffer_to_image(VkCommandBuffer commandBuffer, VkBuffer src, device_image dst, std::span<const VkBufferImageCopy> copies) const noexcept;
    void     cmd_copy_buffer_to_image(VkCommandBuffer commandBuffer, const VkCopyBufferToImageInfo2KHR& info) const noexcept;
    void     cmd_copy_image_to_buffer(VkCommandBuffer commandBuffer, device_image src, VkBuffer dst, std::span<const VkBufferImageCopy> copies) const noexcept;
    void     cmd_copy_image_to_buffer(VkCommandBuffer commandBuffer, const VkCopyImageToBufferInfo2KHR& info) const noexcept;
    void     cmd_clear_attachments(VkCommandBuffer commandBuffer, std::span<const VkClearAttachment> attachments, std::span<const VkClearRect> clearRectangles) const noexcept;

    void     cmd_build_acceleration_structures(VkCommandBuffer commandBuffer, u32 infoCount, const VkAccelerationStructureBuildGeometryInfoKHR* pInfos, const VkAccelerationStructureBuildRangeInfoKHR* const* ppBuildRangeInfos) const noexcept;
    void     cmd_build_acceleration_structures_indirect(VkCommandBuffer commandBuffer, u32 infoCount, const VkAccelerationStructureBuildGeometryInfoKHR* pInfos, const u64* pIndirectDeviceAddresses, const u32* pIndirectStrides, const u32* const* ppMaxPrimitiveCounts) const noexcept;
    void     cmd_write_acceleration_structures_properties(VkCommandBuffer commandBuffer, std::span<const VkAccelerationStructureKHR> accelerationStructures, VkQueryType queryType, VkQueryPool queryPool, u32 firstQuery) const noexcept;
    void     cmd_copy_acceleration_structure(VkCommandBuffer commandBuffer, const VkCopyAccelerationStructureInfoKHR& info) const noexcept;
    void     cmd_copy_acceleration_structure_to_memory(VkCommandBuffer commandBuffer, const VkCopyAccelerationStructureToMemoryInfoKHR& info) const noexcept;
    void     cmd_copy_memory_to_acceleration_structure(VkCommandBuffer commandBuffer, const VkCopyMemoryToAccelerationStructureInfoKHR& info) const noexcept;





    void cmd_write_timestamp(VkCommandBuffer commandBuffer, VkPipelineStageFlags2KHR pipelineStage, VkQueryPool queryPool, u32 query) const noexcept;

    void cmd_begin_transform_feedback(VkCommandBuffer commandBuffer, u32 firstCounterBuffer, u32 counterBufferCount, const VkBuffer* counterBuffers, const u64* counterBufferOffsets) const noexcept;
    void cmd_end_transform_feedback(VkCommandBuffer commandBuffer, u32 firstCounterBuffer, u32 counterBufferCount, const VkBuffer* counterBuffers, const u64* counterBufferOffsets) const noexcept;

    void cmd_begin_query_indexed(VkCommandBuffer commandBuffer, VkQueryPool queryPool, u32 query, VkQueryControlFlags flags, u32 index) const noexcept;
    void cmd_end_query_indexed(VkCommandBuffer commandBuffer, VkQueryPool queryPool, u32 query, u32 index) const noexcept;

    void cmd_begin_query(VkCommandBuffer commandBuffer, VkQueryPool queryPool, u32 query, VkQueryControlFlags flags) const noexcept;
    void cmd_end_query(VkCommandBuffer commandBuffer, VkQueryPool queryPool, u32 query) const noexcept;

    void cmd_reset_query_pool(VkCommandBuffer commandBuffer, VkQueryPool queryPool, u32 firstQuery, u32 queryCount) const noexcept;
    void cmd_copy_query_pool_results(VkCommandBuffer commandBuffer, VkQueryPool queryPool, u32 firstQuery, u32 queryCount, device_buffer dstBuffer, u64 stride, VkQueryResultFlags flags) const noexcept;

    void cmd_write_buffer_marker2(VkCommandBuffer commandBuffer, VkPipelineStageFlags2KHR stage, device_buffer dstBuffer, u32 marker) const noexcept;
  };

  class physical_device{
    VkPhysicalDevice handle;

    friend class impl::device_api;

  public:

    outcome<device>           create_device(const VkDeviceCreateInfo& createInfo, const VkAllocationCallbacks* allocCallbacks = nullptr) const noexcept;
    outcome<VkDisplayModeKHR> create_display_mode(VkDisplayKHR display, const VkDisplayModeCreateInfoKHR& info, const VkAllocationCallbacks* allocCallbacks = nullptr) const noexcept;
    outcome<VkDisplayKHR>     get_winrt_display(u32 index) const noexcept;
    VkResult                  acquire_winrt_display(VkDisplayKHR display) const noexcept;
    VkResult                  release_display(VkDisplayKHR display) const noexcept;


    
    void          get_features(VkPhysicalDeviceFeatures2& features) const noexcept;
    
    void          get_properties(VkPhysicalDeviceProperties2& properties) const noexcept;
    void          get_format_properties(VkFormat format, VkFormatProperties2& properties) const noexcept;
    VkResult      get_image_format_properties(const VkPhysicalDeviceImageFormatInfo2& info, VkImageFormatProperties2& properties) const noexcept;
    void          get_memory_properties(VkPhysicalDeviceMemoryProperties2& properties) const noexcept;
    void          get_external_buffer_properties(const VkPhysicalDeviceExternalBufferInfo& info, VkExternalBufferProperties& properties) const noexcept;
    void          get_external_fence_properties(const VkPhysicalDeviceExternalFenceInfo& info, VkExternalFenceProperties& properties) const noexcept;
    void          get_external_semaphore_properties(const VkPhysicalDeviceExternalSemaphoreInfo& info, VkExternalSemaphoreProperties& properties) const noexcept;
    void          get_multisample_properties(VkSampleCountFlagBits sampleCount, VkMultisamplePropertiesEXT& properties) const noexcept;

    bool          get_win32_presentation_support(u32 queueFamily) const noexcept;
    outcome<bool> get_surface_support(u32 queueFamily, VkSurfaceKHR surface) const noexcept;

    VkResult      get_surface_capabilities(const VkPhysicalDeviceSurfaceInfo2KHR& info, VkSurfaceCapabilities2KHR& capabilities) const noexcept;
    VkResult      get_display_plane_capabilities(const VkDisplayPlaneInfo2KHR& info, VkDisplayPlaneCapabilities2KHR& capabilities) const noexcept;
    
    u32           get_queue_family_performance_query_passes(const VkQueryPoolPerformanceCreateInfoKHR& info) const noexcept;
    
    
    
    
    outcome<u32> count_device_extensions() const noexcept;
    u32          count_sparse_image_formats(const VkPhysicalDeviceSparseImageFormatInfo2& info) const noexcept;
    u32          count_queue_families() const noexcept;
    outcome<u32> count_queue_family_performance_query_counters(u32 queueFamily) const noexcept;
    outcome<u32> count_surface_formats(const VkPhysicalDeviceSurfaceInfo2KHR& info) const noexcept;
    outcome<u32> count_present_modes(const VkPhysicalDeviceSurfaceInfo2KHR& info) const noexcept;
    outcome<u32> count_present_rectangles(VkSurfaceKHR surface) const noexcept;
    outcome<u32> count_displays() const noexcept;
    outcome<u32> count_display_modes(VkDisplayKHR display) const noexcept;
    outcome<u32> count_display_planes() const noexcept;
    outcome<u32> count_supported_displays(u32 displayPlaneIndex) const noexcept;
    outcome<u32> count_calibrateable_time_domains() const noexcept;
    outcome<u32> count_cooperative_matrices() const noexcept;
    outcome<u32> count_supported_framebuffer_mixed_samples_combinations() const noexcept;

    

    VkResult     enumerate_device_extensions(std::span<VkExtensionProperties> extensions) const noexcept;
    void         enumerate_sparse_image_formats(const VkPhysicalDeviceSparseImageFormatInfo2& info, std::span<VkSparseImageFormatProperties2> properties) const noexcept;
    void         enumerate_queue_families(std::span<VkQueueFamilyProperties2> properties) const noexcept;
    VkResult     enumerate_present_modes(const VkPhysicalDeviceSurfaceInfo2KHR& info, std::span<VkPresentModeKHR> presentModes) const noexcept;
    VkResult     enumerate_surface_formats(const VkPhysicalDeviceSurfaceInfo2KHR& info, std::span<VkSurfaceFormat2KHR> formats) const noexcept;
    VkResult     enumerate_present_rectangles(VkSurfaceKHR surface, std::span<VkRect2D> presentRectangles) const noexcept;
    VkResult     enumerate_displays(std::span<VkDisplayProperties2KHR> displays) const noexcept;
    VkResult     enumerate_display_modes(VkDisplayKHR display, std::span<VkDisplayModeProperties2KHR> displayModes) const noexcept;
    VkResult     enumerate_display_planes(std::span<VkDisplayPlaneProperties2KHR> displayPlanes) const noexcept;
    VkResult     enumerate_supported_displays(u32 displayPlaneIndex, std::span<VkDisplayKHR> displays) const noexcept;
    VkResult     enumerate_queue_family_performance_query_counters(u32 queueFamily, std::span<VkPerformanceCounterKHR> counters, std::span<VkPerformanceCounterDescriptionKHR> descriptions) const noexcept;
    VkResult     enumerate_calibrateable_time_domains(std::span<VkTimeDomainEXT> domains) const noexcept;
    VkResult     enumerate_cooperative_matrices(std::span<VkCooperativeMatrixPropertiesNV> matrices) const noexcept;
    VkResult     enumerate_supported_framebuffer_mixed_samples_combinations(std::span<VkFramebufferMixedSamplesCombinationNV> combos) const noexcept;
  };
  
  /*class queue{
    VkQueue handle;
    const impl::queue_api* api;
  public:
    
    

  };
  class command_pool{
    VkCommandPool           handle;
    const impl::device_api* api;
  public:

  };
  class command_buffer{
    VkCommandBuffer handle;
    const impl::command_buffer_api* api;

    friend class command_buffer_array;
  public:


    VkResult begin(const VkCommandBufferBeginInfo& info) const noexcept;
    VkResult end() const noexcept;
    VkResult reset(VkCommandBufferResetFlags flags) const noexcept;


    void     cmd_execute_commands(std::span<command_buffer> buffers) const noexcept;
    void     cmd_begin_render_pass(const VkRenderPassBeginInfo& passInfo, const VkSubpassBeginInfo& subpassInfo) const noexcept;
    void     cmd_next_subpass(const VkSubpassBeginInfo& beginInfo, const VkSubpassEndInfo& endInfo) const noexcept;
    void     cmd_end_render_pass(const VkSubpassEndInfo& endInfo) const noexcept;


    void     cmd_pipeline_barrier(const VkDependencyInfoKHR& dependencyInfo) const noexcept;
    void     cmd_set_device_mask(u32 mask) const noexcept;
    void     cmd_begin_conditional_rendering(const VkConditionalRenderingBeginInfoEXT& beginInfo) const noexcept;
    void     cmd_end_conditional_rendering() const noexcept;


    void     cmd_preprocess_generated_commands(const VkGeneratedCommandsInfoNV& commandsInfo) const noexcept;
    void     cmd_execute_preprocessed_commands(const VkGeneratedCommandsInfoNV& commandsInfo) const noexcept;
    void     cmd_execute_and_preprocess_generated_commands(const VkGeneratedCommandsInfoNV& commandsInfo) const noexcept;


    void     cmd_set_event(VkEvent event, const VkDependencyInfoKHR& dependencyInfo) const noexcept;
    void     cmd_reset_event(VkEvent event, VkPipelineStageFlags2KHR pipelineStages) const noexcept;
    void     cmd_wait_event(u32 eventCount, VkEvent* events, const VkDependencyInfoKHR* dependencies) const noexcept;



    void     cmd_push_constants(VkPipelineLayout pipelineLayout, VkPipelineStageFlags2KHR stageFlags, u32 offset, u32 size, const void* values) const noexcept;
    void     cmd_push_descriptor_set(VkPipelineBindPoint bindPoint, VkPipelineLayout pipelineLayout, u32 set, std::span<const VkWriteDescriptorSet> descriptorSets) const noexcept;
    void     cmd_push_descriptor_set_with_template(VkDescriptorUpdateTemplate updateTemplate, VkPipelineLayout pipelineLayout, u32 set, const void* data) const noexcept;
    void     cmd_bind_pipeline(VkPipelineBindPoint bindPoint, VkPipeline pipeline) const noexcept;
    void     cmd_bind_descriptor_sets(VkPipelineBindPoint bindPoint, VkPipelineLayout layout, u32 firstSet, std::span<const VkDescriptorSet> descriptorSets, std::span<const u32> dynamicOffsets) const noexcept;
    void     cmd_bind_index_buffer(VkBuffer buffer, u64 bufferSize, VkIndexType indexType) const noexcept;
    void     cmd_bind_vertex_buffers(u32 firstBinding, u32 bindingCount, const VkBuffer* pBuffers, const u64* pOffsets) const noexcept;
    void     cmd_bind_shading_rate_image(VkImageView image, VkImageLayout layout) const noexcept;
    void     cmd_bind_transform_feedback_buffers(u32 firstBinding, u32 bindingCount, const VkBuffer* pBuffers, const u64* pOffsets, const u64* pSizes) const noexcept;
    void     cmd_bind_pipeline_shader_group(VkPipelineBindPoint bindPoint, VkPipeline pipeline, u32 groupIndex) const noexcept;

    void     cmd_set_viewport(u32 firstViewport, u32 viewportCount, const VkViewport* viewports) const noexcept;
    void     cmd_set_scissor(u32 firstScissor, u32 scissorCount, const rect2d* scissors) const noexcept;
    void     cmd_set_line_width(f32 lineWidth) const noexcept;
    void     cmd_set_depth_bias(f32 constantFactor, f32 clamp, f32 slopeFactor) const noexcept;
    void     cmd_set_blend_constants(std::span<const f32, 4> constants) const noexcept;
    void     cmd_set_depth_bounds(interval<f32> depthBounds) const noexcept;
    void     cmd_set_stencil_compare_mask(VkStencilFaceFlags faceMask, u32 compareMask) const noexcept;
    void     cmd_set_stencil_write_mask(VkStencilFaceFlags faceMask, u32 writeMask) const noexcept;
    void     cmd_set_stencil_reference(VkStencilFaceFlags faceMask, u32 reference) const noexcept;
    void     cmd_set_viewport_w_scaling(u32 firstViewport, u32 viewportCount, const VkViewportWScalingNV* scaling) const noexcept;
    void     cmd_set_discard_rectangle(u32 firstRect, u32 rectCount, const rect2d* discardRectangles) const noexcept;
    void     cmd_set_sample_locations(const VkSampleLocationsInfoEXT& sampleLocations) const noexcept;
    void     cmd_set_viewport_shading_rate_palette(u32 firstViewport, u32 viewportCount, const VkShadingRatePaletteNV* palettes) const noexcept;
    void     cmd_set_coarse_sample_order(VkCoarseSampleOrderTypeNV type, std::span<const VkCoarseSampleOrderCustomNV> customOrders) const noexcept;
    void     cmd_set_exclusive_scissor(u32 firstScissor, u32 scissorCount, const rect2d* scissors) const noexcept;
    void     cmd_set_fragment_shading_rate(extent2d<u32> fragmentSize, const VkFragmentShadingRateCombinerOpKHR& combinerOps) const noexcept;
    void     cmd_set_fragment_shading_rate_enum(VkFragmentShadingRateNV shadingRate, const VkFragmentShadingRateCombinerOpKHR& combinerOps) const noexcept;
    void     cmd_set_line_stipple(u32 factor, u16 pattern) const noexcept;
    void     cmd_set_cull_mode(VkCullModeFlags cullMode) const noexcept;
    void     cmd_set_front_face(VkFrontFace frontFace) const noexcept;
    void     cmd_set_primitive_topology(VkPrimitiveTopology primitiveTopology) const noexcept;
    void     cmd_set_viewport_with_count(std::span<const VkViewport> viewports) const noexcept;
    void     cmd_set_scissor_with_count(std::span<const rect2d> scissors) const noexcept;
    void     cmd_bind_vertex_buffers2(u32 firstBinding, u32 bindingCount, const VkBuffer* buffers, const u64* offsets, const u64* sizes, const u64* strides) const noexcept;
    void     cmd_set_depth_test_enable(bool32 enabled) const noexcept;
    void     cmd_set_depth_write_enable(bool32 enabled) const noexcept;
    void     cmd_set_depth_compare_op(VkCompareOp compareOp) const noexcept;
    void     cmd_set_depth_bounds_test_enable(bool32 enabled) const noexcept;
    void     cmd_set_stencil_test_enable(bool32 enabled) const noexcept;
    void     cmd_set_stencil_op(VkStencilFaceFlags faceMask, VkStencilOp failOp, VkStencilOp passOp, VkStencilOp depthFailOp, VkCompareOp compareOp) const noexcept;
    void     cmd_set_ray_tracing_pipeline_stack_size(u32 stackSize) const noexcept;
    void     cmd_set_patch_control_points(u32 patchControlPoints) const noexcept;
    void     cmd_set_rasterizer_discard_enable(bool32 enabled) const noexcept;
    void     cmd_set_depth_bias_enable(bool32 enabled) const noexcept;
    void     cmd_set_logic_op(VkLogicOp op) const noexcept;
    void     cmd_set_primitive_restart_enable(bool32 enabled) const noexcept;
    void     cmd_set_vertex_input(std::span<const VkVertexInputBindingDescription2EXT> bindingDescriptions, std::span<const VkVertexInputAttributeDescription2EXT> attributeDescriptions) const noexcept;
    void     cmd_set_color_write_enable(std::span<const bool32> enabled) const noexcept;


    void     cmd_insert_debug_label(const VkDebugUtilsLabelEXT& debugLabel) const noexcept;
    void     cmd_begin_debug_label(const VkDebugUtilsLabelEXT& debugLabel) const noexcept;
    void     cmd_end_debug_label() const noexcept;

    void     cmd_set_checkpoint(const void* checkpointData) const noexcept;

    VkResult cmd_set_performance_marker(const VkPerformanceMarkerInfoINTEL& info) const noexcept;
    VkResult cmd_set_performance_stream_marker(const VkPerformanceStreamMarkerInfoINTEL& info) const noexcept;
    VkResult cmd_set_performance_override(const VkPerformanceOverrideInfoINTEL& info) const noexcept;


    void     cmd_draw(u32 vertexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance) const noexcept;
    void     cmd_draw_indexed(u32 indexCount, u32 instanceCount, u32 firstIndex, i32 vertexOffset, u32 firstInstance) const noexcept;
    void     cmd_draw_indirect(device_buffer buffer, u32 drawCount, u32 stride) const noexcept;
    void     cmd_draw_indirect_count(device_buffer buffer, device_buffer countBuffer, u32 maxDrawCount, u32 stride) const noexcept;
    void     cmd_draw_indexed_indirect(device_buffer buffer, u32 drawCount, u32 stride) const noexcept;
    void     cmd_draw_indexed_indirect_count(device_buffer buffer, device_buffer countBuffer, u32 maxDrawCount, u32 stride) const noexcept;
    void     cmd_draw_indirect_byte_count(u32 instanceCount, u32 firstInstance, device_buffer counterBuffer, u32 counterOffset, u32 vertexStride) const noexcept;

    void     cmd_draw_mesh_tasks(u32 taskCount, u32 firstTask) const noexcept;
    void     cmd_draw_mesh_tasks_indirect(device_buffer buffer, u32 drawCount, u32 stride) const noexcept;
    void     cmd_draw_mesh_tasks_indirect_count(device_buffer buffer, device_buffer countBuffer, u32 maxDrawCount, u32 stride) const noexcept;
    
    void     cmd_dispatch(extent3d<u32> groupExtents) const noexcept;
    void     cmd_dispatch_indirect(device_buffer buffer) const noexcept;
    void     cmd_dispatch_base(position3d<u32> baseGroup, extent3d<u32> groupExtents) const noexcept;
    
    void     cmd_trace_rays(const shader_table& shaders, extent3d<u32> extents) const noexcept;
    void     cmd_trace_rays_indirect(const shader_table& shaders, u64 indirectAddress) const noexcept;

    void     cmd_launch_cuda_kernel(const VkCuLaunchInfoNVX& launchInfo) const noexcept;


    void     cmd_blit_image(device_image src, device_image dst, std::span<const VkImageBlit> blitz, VkFilter filter) const noexcept;
    void     cmd_blit_image2(const VkBlitImageInfo2KHR& info) const noexcept;
    void     cmd_copy_image(device_image src, device_image dst, std::span<const VkImageCopy> copies) const noexcept;
    void     cmd_copy_image2(const VkCopyImageInfo2KHR& info) const noexcept;
    void     cmd_resolve_image(device_image src, device_image dst, std::span<const VkImageResolve> resolves) const noexcept;
    void     cmd_resolve_image2(const VkResolveImageInfo2KHR& info) const noexcept;
    void     cmd_clear_color_image(device_image image, const VkClearColorValue& value, std::span<const VkImageSubresourceRange> ranges) const noexcept;
    void     cmd_clear_depth_stencil_image(device_image image, const VkClearDepthStencilValue& value, std::span<const VkImageSubresourceRange> ranges) const noexcept;

    void     cmd_copy_buffer(VkBuffer src, VkBuffer dst, std::span<const VkBufferCopy> copies) const noexcept;
    void     cmd_copy_buffer2(const VkCopyBufferInfo2KHR& info) const noexcept;
    void     cmd_update_buffer(device_buffer dst, u64 dataSize, const void* data) const noexcept;
    void     cmd_fill_buffer(device_buffer dst, u64 size, u32 data) const noexcept;

    void     cmd_copy_buffer_to_image(VkBuffer src, device_image dst, std::span<const VkBufferImageCopy> copies) const noexcept;
    void     cmd_copy_buffer_to_image2(const VkCopyBufferToImageInfo2KHR& info) const noexcept;
    void     cmd_copy_image_to_buffer(device_image src, VkBuffer dst, std::span<const VkBufferImageCopy> copies) const noexcept;
    void     cmd_copy_image_to_buffer2(const VkCopyImageToBufferInfo2KHR& info) const noexcept;
    void     cmd_clear_attachments(std::span<const VkClearAttachment> attachments, std::span<const VkClearRect> clearRectangles) const noexcept;

    void     cmd_build_acceleration_structures(u32 infoCount, const VkAccelerationStructureBuildGeometryInfoKHR* pInfos, const VkAccelerationStructureBuildRangeInfoKHR* const* ppBuildRangeInfos) const noexcept;
    void     cmd_build_acceleration_structures_indirect(u32 infoCount, const VkAccelerationStructureBuildGeometryInfoKHR* pInfos, const u64* pIndirectDeviceAddresses, const u32* pIndirectStrides, const u32* const* ppMaxPrimitiveCounts) const noexcept;
    void     cmd_write_acceleration_structures_properties(std::span<const VkAccelerationStructureKHR> accelerationStructures, VkQueryType queryType, VkQueryPool queryPool, u32 firstQuery) const noexcept;
    void     cmd_copy_acceleration_structure(const VkCopyAccelerationStructureInfoKHR& info) const noexcept;
    void     cmd_copy_acceleration_structure_to_memory(const VkCopyAccelerationStructureToMemoryInfoKHR& info) const noexcept;
    void     cmd_copy_memory_to_acceleration_structure(const VkCopyMemoryToAccelerationStructureInfoKHR& info) const noexcept;





    void cmd_write_timestamp(VkPipelineStageFlags2KHR pipelineStage, VkQueryPool queryPool, u32 query) const noexcept;

    void cmd_begin_transform_feedback(u32 firstCounterBuffer, u32 counterBufferCount, const VkBuffer* counterBuffers, const u64* counterBufferOffsets) const noexcept;
    void cmd_end_transform_feedback(u32 firstCounterBuffer, u32 counterBufferCount, const VkBuffer* counterBuffers, const u64* counterBufferOffsets) const noexcept;

    void cmd_begin_query_indexed(VkQueryPool queryPool, u32 query, VkQueryControlFlags flags, u32 index) const noexcept;
    void cmd_end_query_indexed(VkQueryPool queryPool, u32 query, u32 index) const noexcept;

    void cmd_begin_query(VkQueryPool queryPool, u32 query, VkQueryControlFlags flags) const noexcept;
    void cmd_end_query(VkQueryPool queryPool, u32 query) const noexcept;

    void cmd_reset_query_pool(VkQueryPool queryPool, u32 firstQuery, u32 queryCount) const noexcept;
    void cmd_copy_query_pool_results(VkQueryPool queryPool, u32 firstQuery, u32 queryCount, device_buffer dstBuffer, u64 stride, VkQueryResultFlags flags) const noexcept;

    void cmd_write_buffer_marker2(VkPipelineStageFlags2KHR stage, device_buffer dstBuffer, u32 marker) const noexcept;
  };
  class command_buffer_array{
    u32                             count;
    VkCommandBuffer*                commandBuffers;
    const impl::command_buffer_api* api;
    command_pool*   pool;
  public:

    command_buffer operator[](u32 index) const noexcept {
      command_buffer cmdBuf;
      cmdBuf.handle = commandBuffers[index];
      cmdBuf.api    = api;
      return cmdBuf;
    }
  };*/
  
}

#endif  //VALKYRIE_INTERNAL_GRAPHICS_API_HPP
