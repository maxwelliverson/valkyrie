//
// Created by Maxwell on 2021-01-10.
//

#include <valkyrie/Graphics/API/Swapchain.hpp>
#include "../../../../src/valkyrie/Graphics/API/InternalStructures.hpp"

#include <vulkan/vulkan_win32.h>

#include <iostream>
#include <vector>
#include <atomic>
#include <thread>


struct AllocationCache{
  void* address;
  size_t size;
  size_t alignment;
  VkSystemAllocationScope scope;
};
struct ReallocationCache{
  void* address;
  void* newAddress;
  size_t size;
  size_t alignment;
  VkSystemAllocationScope scope;
};
struct FreeCache{
  void* address;
};
struct InternalCache{
  size_t                   size;
  VkInternalAllocationType type;
  VkSystemAllocationScope  scope;
};

struct VulkanMemoryCache{
  std::vector<AllocationCache> allocationCache;
  std::vector<ReallocationCache> reallocationCache;
  std::vector<FreeCache> freeCache;
  uint32_t nullFreeCount = 0;
  std::vector<InternalCache> internalAllocationCache;
  std::vector<InternalCache> internalFreeCache;
};

using PFN_windowProc = LRESULT(WINAPI*)(_In_ HWND hWnd, _In_ UINT Msg, _In_ WPARAM wParam, _In_ LPARAM lParam);

inline static std::atomic<VkExtent2D> windowExtents{ VkExtent2D{ .width = 1920, .height = 1080 } };
inline static std::atomic_bool windowShouldClose{false};

LRESULT WINAPI windowProc(_In_ HWND hWnd, _In_ UINT Msg, _In_ WPARAM wParam, _In_ LPARAM lParam) {
  switch ( Msg ) {
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
  }
  return DefWindowProc(hWnd, Msg, wParam, lParam);
}





int main(){

  using cstring = const char*;

  const cstring className = "Sample Window Class";
  const cstring windowName = "Sample Vulkan Window";

  HINSTANCE hinst = GetModuleHandle(nullptr);
  HWND hwnd;
  HMONITOR hmonitor;

  WNDCLASS wndClass{
      .lpfnWndProc   = windowProc,
      .hInstance     = hinst,
      .lpszClassName = className
  };
  auto registrationAtom = RegisterClass(&wndClass);

  if (!registrationAtom)
    panic(valkyrie::Core::System::Win32::getLastError());


  hwnd = CreateWindow(
      className,
      windowName,
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      windowExtents.load().width,
      windowExtents.load().height,
      nullptr,
      nullptr,
      hinst,
      nullptr
      );
  hmonitor = MonitorFromWindow(
      hwnd,
      MONITOR_DEFAULTTONEAREST
      );


  const static auto makeAllocator = [](VulkanMemoryCache& cache){
    return VkAllocationCallbacks{
        .pUserData             = &cache,
        .pfnAllocation         = [](void* pUserData, size_t size, size_t alignment, VkSystemAllocationScope scope){
          auto* memoryCache = (VulkanMemoryCache*)pUserData;
          auto* address = _aligned_malloc(size, alignment);
          memoryCache->allocationCache.emplace_back(address, size, alignment, scope);
          return address;
        },
        .pfnReallocation       = [](void* pUserData, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope scope){
          auto* memoryCache = (VulkanMemoryCache*)pUserData;
          auto* address     = _aligned_realloc(pOriginal, size, alignment);
          memoryCache->reallocationCache.emplace_back(pOriginal, address, size, alignment, scope);
          return address;
        },
        .pfnFree               = [](void* pUserData, void* address){
          auto* memoryCache = (VulkanMemoryCache*)pUserData;
          if (address)
            memoryCache->freeCache.emplace_back(address);
          else
            ++memoryCache->nullFreeCount;
          _aligned_free(address);
        },
        .pfnInternalAllocation = [](void* pUserData, size_t size, VkInternalAllocationType type, VkSystemAllocationScope scope){
          ((VulkanMemoryCache*)pUserData)->internalAllocationCache.emplace_back(size, type, scope);
        },
        .pfnInternalFree       = [](void* pUserData, size_t size, VkInternalAllocationType type, VkSystemAllocationScope scope){
          ((VulkanMemoryCache*)pUserData)->internalFreeCache.emplace_back(size, type, scope);
        }
    };
  };



  VulkanMemoryCache instanceMemoryCache;
  VulkanMemoryCache surfaceMemoryCache;
  VulkanMemoryCache deviceMemoryCache;
  VulkanMemoryCache swapchainMemoryCache;
  VulkanMemoryCache commandPoolMemoryCache;
  VulkanMemoryCache commandBufferMemoryCache;
  VulkanMemoryCache pipelineMemoryCache;





  VkAllocationCallbacks instanceAllocatorSt{makeAllocator(instanceMemoryCache)};
  VkAllocationCallbacks surfaceAllocatorSt{makeAllocator(surfaceMemoryCache)};
  VkAllocationCallbacks deviceAllocatorSt{makeAllocator(deviceMemoryCache)};
  VkAllocationCallbacks swapchainAllocatorSt{makeAllocator(swapchainMemoryCache)};
  VkAllocationCallbacks commandPoolAllocatorSt{makeAllocator(commandPoolMemoryCache)};
  VkAllocationCallbacks commandBufferAllocatorSt{makeAllocator(commandBufferMemoryCache)};
  VkAllocationCallbacks pipelineAllocatorSt{makeAllocator(pipelineMemoryCache)};

  const auto* instanceAllocator = &instanceAllocatorSt;
  const auto* surfaceAllocator = &surfaceAllocatorSt;
  const auto* deviceAllocator = &deviceAllocatorSt;
  const auto* swapchainAllocator = &swapchainAllocatorSt;
  const auto* commandPoolAllocator = &commandPoolAllocatorSt;
  const auto* commandBufferAllocator = &commandBufferAllocatorSt;
  const auto* pipelineAllocator = &pipelineAllocatorSt;

  VkApplicationInfo appInfo{
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pNext = nullptr,
      .pApplicationName = "Sample Application",
      .applicationVersion = 0,
      .pEngineName   = "Sample Engine",
      .engineVersion = 0,
      .apiVersion = VK_API_VERSION_1_2
  };

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
      "VK_NV_acquire_winrt_display",
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
      .hinstance = hinst,
      .hwnd      = hwnd
  };

  VkInstance instance;
  VkSurfaceKHR surface;
  VkDevice   device;
  VkSwapchainKHR swapchain;
  VkPhysicalDevice physicalDevice;

  FreeFunctions freeApi{};

  VkResult instanceCreationResult = freeApi.createInstance(&instanceCreateInfo, instanceAllocator, &instance);

  InstanceFunctions instanceApi{InstanceFunctionLoader{ instance, freeApi.loader.getLoaderFunction() }};
  PhysicalDeviceFunctions physicalDeviceApi{ instanceApi.loader };

  VkResult surfaceCreationResult = instanceApi.createWin32Surface(instance, &surfaceCreateInfo, surfaceAllocator, &surface);

  uint32_t physicalDeviceCount = 1;

  VkResult physicalDeviceResult = instanceApi.enumeratePhysicalDevices(instance, &physicalDeviceCount, &physicalDevice);




  namespace Internal = valkyrie::Graphics::API::Internal;


  Internal::PhysicalDeviceFeatures deviceFeatures{};
  Internal::PhysicalDeviceProperties deviceProperties{};

  physicalDeviceApi.getProperties(physicalDevice, &deviceProperties.vulkan10);
  physicalDeviceApi.getFeatures(physicalDevice, &deviceFeatures.vulkan10);





  Internal::SurfaceCapabilities surfaceCapabilities;
  Internal::SurfaceInfo         surfaceInfo;

  /*VkQueueFamilyCheckpointPropertiesNV queueFamilyCheckpoint{
      .sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_CHECKPOINT_PROPERTIES_NV,
      .pNext = nullptr
  };

  VkQueueFamilyProperties2 queueFamily{
      .sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2,
      .pNext = &queueFamilyCheckpoint
  };

  physicalDeviceApi.getQueueFamilyProperties(physicalDevice, &queueFamily);*/

  const float transferPriority[]{ 0.7f };
  const float graphicsPriorities[]{ 1.0f, 0.9f };


  struct QueueFamilies{
    VkQueueFamilyCheckpointPropertiesNV* checkpointProperties;
    VkQueueFamilyProperties2* properties;
    uint32_t count;
  };

  QueueFamilies queueFamilies;

#define VK_calloc(type, count) (type*)std::calloc(count, sizeof(type))

  VkQueue graphicsQueue, presentQueue, transferQueue;

  physicalDeviceApi.getQueueFamilyProperties(physicalDevice, &queueFamilies.count, nullptr);
  queueFamilies.properties = VK_calloc(VkQueueFamilyProperties2, queueFamilies.count);
  queueFamilies.checkpointProperties = VK_calloc(VkQueueFamilyCheckpointPropertiesNV, queueFamilies.count);

  for (uint32_t i = 0; i < queueFamilies.count; ++i) {
    auto& properties = queueFamilies.properties[i];
    auto& checkpointProperties = queueFamilies.checkpointProperties[i];
    properties.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
    properties.pNext = &checkpointProperties;
    checkpointProperties.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_CHECKPOINT_PROPERTIES_NV;
    checkpointProperties.pNext = nullptr;
  }

  physicalDeviceApi.getQueueFamilyProperties(physicalDevice, &queueFamilies.count, queueFamilies.properties);

  (void)0;



  VkDeviceQueueCreateInfo queueCreateInfo[]{
      {
          .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
          .pNext = nullptr,
          .flags = 0,
          .queueFamilyIndex = 0,
          .queueCount = uint32_t(std::size(graphicsPriorities)),
          .pQueuePriorities = graphicsPriorities
      },
      {
          .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
          .pNext = nullptr,
          .flags = 0,
          .queueFamilyIndex = 1,
          .queueCount = uint32_t(std::size(transferPriority)),
          .pQueuePriorities = transferPriority
      }
  };


  std::free(queueFamilies.properties);
  std::free(queueFamilies.checkpointProperties);
  queueFamilies.count = 0;


  VkDeviceCreateInfo deviceCreateInfo{
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .pNext = &deviceFeatures.vulkan10,
      .flags = 0,
      .queueCreateInfoCount = uint32_t(std::size(queueCreateInfo)),
      .pQueueCreateInfos    = queueCreateInfo,
      .enabledLayerCount    = 0,
      .ppEnabledLayerNames  = nullptr,
      .enabledExtensionCount = uint32_t(std::size(enabledDeviceExtensions)),
      .ppEnabledExtensionNames = enabledDeviceExtensions,
      .pEnabledFeatures = nullptr
  };

  VkResult deviceCreateResult = physicalDeviceApi.createDevice(physicalDevice, &deviceCreateInfo, deviceAllocator, &device);

  VulkanFunction<PFN_vkGetDeviceProcAddr> getDeviceProcAddr(physicalDeviceApi.loader.load("vkGetDeviceProcAddr"));

  DeviceFunctions deviceApi{DeviceFunctionLoader{device, PFN_vkGetDeviceProcAddr(getDeviceProcAddr)}};

  VkDeviceQueueInfo2 queueInfo[]{
      {
          .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2,
          .pNext = nullptr,
          .flags = 0,
          .queueFamilyIndex = 0,
          .queueIndex = 0
      },
      {
          .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2,
          .pNext = nullptr,
          .flags = 0,
          .queueFamilyIndex = 0,
          .queueIndex = 1
      },
      {
          .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2,
          .pNext = nullptr,
          .flags = 0,
          .queueFamilyIndex = 1,
          .queueIndex = 0
      }
  };


  deviceApi.getQueue2(device, &queueInfo[0], &graphicsQueue);
  deviceApi.getQueue2(device, &queueInfo[1], &presentQueue);
  deviceApi.getQueue2(device, &queueInfo[2], &transferQueue);


  surfaceInfo.info.surface = surface;
  surfaceInfo.fullscreenInfo.fullScreenExclusive = VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT;
  surfaceInfo.fullscreenWin32Info.hmonitor       = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);

  physicalDeviceApi.getSurfaceCapabilities(physicalDevice, &surfaceInfo.info, &surfaceCapabilities.capabilities);

  const uint32_t queueFamilyIndicesSwapchain[]{
      0
  };

  VkBool32 surfaceIsSupported;
  auto supportResult = physicalDeviceApi.getSurfaceSupport(physicalDevice, 0, surface, &surfaceIsSupported);
  auto win32SupportResult = physicalDeviceApi.getWin32PresentationSupport(physicalDevice, 0);


  struct SurfaceFormats{
    VkSurfaceFormat2KHR* formats;
    uint32_t             count;
  };

  SurfaceFormats surfaceFormats;

  physicalDeviceApi.getSurfaceFormats(physicalDevice, &surfaceInfo.info, &surfaceFormats.count, nullptr);

  surfaceFormats.formats = VK_calloc(VkSurfaceFormat2KHR, surfaceFormats.count);

  std::span formats{surfaceFormats.formats, surfaceFormats.count};

  for (auto& format : formats)
    format.sType = VK_STRUCTURE_TYPE_SURFACE_FORMAT_2_KHR;

  physicalDeviceApi.getSurfaceFormats(physicalDevice, &surfaceInfo.info, &surfaceFormats.count, surfaceFormats.formats);

  uint32_t surfaceFormatIndex = 0;
  bool foundHDR = false;

  for (const auto& format : formats) {
    switch (format.surfaceFormat.colorSpace) {
      case VK_COLOR_SPACE_HDR10_ST2084_EXT:
      case VK_COLOR_SPACE_HDR10_HLG_EXT:
      case VK_COLOR_SPACE_DOLBYVISION_EXT:
        foundHDR = true;
      default: break;
    }
    if (foundHDR)
      break;
    ++surfaceFormatIndex;
  }

  // Uncomment the next line to enable HDR swapchains
  //if (!foundHDR)
    surfaceFormatIndex = 0;

  auto surfaceFormat = surfaceFormats.formats[surfaceFormatIndex].surfaceFormat;

  VkSwapchainCreateInfoKHR swapchainCreateInfo{
      .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
      .pNext = &surfaceInfo.fullscreenInfo,
      .flags = 0,
      .surface = surface,
      .minImageCount = 3,
      .imageFormat = surfaceFormat.format,
      .imageColorSpace = surfaceFormat.colorSpace,
      .imageExtent = surfaceCapabilities.capabilities.surfaceCapabilities.currentExtent,
      .imageArrayLayers = 1,
      .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
      .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .queueFamilyIndexCount = uint32_t(std::size(queueFamilyIndicesSwapchain)),
      .pQueueFamilyIndices = queueFamilyIndicesSwapchain,
      .preTransform = surfaceCapabilities.capabilities.surfaceCapabilities.currentTransform,
      .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
      .presentMode = VK_PRESENT_MODE_MAILBOX_KHR,
      .clipped = VK_TRUE,
      .oldSwapchain = nullptr
  };

  VkResult swapchainResult = deviceApi.presentation.createSwapchain(device, &swapchainCreateInfo, swapchainAllocator, &swapchain);

  std::free(surfaceFormats.formats);
  surfaceFormats.count = 0;

  VkCommandPool commandPool;
  VkCommandBuffer commandBuffers[]{ nullptr };
  VkFramebuffer framebuffer;
  VkRenderPass renderPass;
  VkPipeline pipeline;
  VkShaderModule shaderModule;


  VkCommandPoolCreateInfo commandPoolCreateInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .pNext = nullptr,
      .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
      .queueFamilyIndex = 0
  };

  VkResult commandPoolCreateResult = deviceApi.createCommandPool(device, &commandPoolCreateInfo, commandPoolAllocator, &commandPool);

  VkCommandBufferAllocateInfo commandBufferAllocateInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .pNext = nullptr,
      .commandPool = commandPool,
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = uint32_t(std::size(commandBuffers))
  };

  VkResult commandBufferAllocateResult = deviceApi.allocateCommandBuffers(device, &commandBufferAllocateInfo, commandBuffers);





  /*VkFramebufferCreateInfo framebufferCreateInfo{
      .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
      .pNext = nullptr
  };*/







  ShowWindow(hwnd, SW_SHOW);

  using namespace std::chrono_literals;
  namespace Win32 = valkyrie::Core::System::Win32;

  MSG msg;
  BOOL bRet;

  while( (bRet = GetMessage( &msg, NULL, 0, 0 )) ) {
    if (bRet == -1) {
      panic(Win32::getLastError());
      // handle the error and possibly exit
    } else {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    if (windowShouldClose)
      break;
  }



  VkResult finalWaitResult = deviceApi.waitIdle(device);


  deviceApi.presentation.destroySwapchain(device, swapchain, swapchainAllocator);
  deviceApi.freeCommandBuffers(device, commandPool, uint32_t(std::size(commandBuffers)), commandBuffers);
  deviceApi.destroyCommandPool(device, commandPool, commandPoolAllocator);
  deviceApi.destroy(device, deviceAllocator);
  instanceApi.destroySurface(instance, surface, surfaceAllocator);
  instanceApi.destroyInstance(instance, instanceAllocator);

  CloseWindow(hwnd);
  UnregisterClass("Sample Window Class", hinst);

  return 0;
}