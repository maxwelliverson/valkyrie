//
// Created by Maxwell on 2021-03-23.
//

#include <valkyrie/adt/small_vector.hpp>
#include <valkyrie/utility/shapes.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


#define APPLICATION_NAME "graph_gui"
#define ENGINE_NAME "valkyrie (imgui frontend)"
#define APPLICATION_VERSION 0
#define ENGINE_VERSION 0

#include <array>
#include <algorithm>

#if defined(NDEBUG)
inline constexpr static std::array<const char*, 0> enabled_layers{};
#else
inline constexpr static std::array<const char*, 1> enabled_layers { "VK_LAYER_KHRONOS_validation" };
#endif

inline constexpr static std::array enabled_device_extensions{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_EXT_INLINE_UNIFORM_BLOCK_EXTENSION_NAME,
    VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
    VK_KHR_RAY_QUERY_EXTENSION_NAME,
    VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
    VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
    VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME
};



using valkyrie::Rect2D, valkyrie::Extent2D, valkyrie::i32, valkyrie::u32;

struct GLFWcontext{
  GLFWcontext() noexcept{
    glfwInit();
  }
  ~GLFWcontext(){
    glfwTerminate();
  }
};

const static GLFWcontext GLFW_CONTEXT{};

class  Frame{
  VkCommandPool       commandPool;
  VkCommandBuffer     commandBuffer;
  VkFence             fence;
  VkImage             backbuffer;
  VkImageView         backbufferView;
  VkFramebuffer       framebuffer;
};
struct FrameSemaphores{
  VkSemaphore imageAcquired;
  VkSemaphore renderComplete;
};

class Swapchain{
  VkSwapchainKHR   swapchain;
  VkPresentModeKHR presentMode;
  VkRenderPass     renderPass;
  VkPipeline                  pipeline;
  std::optional<VkClearValue> clearValue;
  u32              frameIndex;
  u32              semaphoreIndex;
  u32              frameCount;
  Frame*           frames;
  FrameSemaphores* frameSemaphores;


public:

  Swapchain(VkSurfaceKHR surface){
    VkStructureType                  sType;
    const void*                      pNext;
    VkSwapchainCreateFlagsKHR        flags;
    VkSurfaceKHR                     surface;
    uint32_t                         minImageCount;
    VkFormat                         imageFormat;
    VkColorSpaceKHR                  imageColorSpace;
    VkExtent2D                       imageExtent;
    uint32_t                         imageArrayLayers;
    VkImageUsageFlags                imageUsage;
    VkSharingMode                    imageSharingMode;
    uint32_t                         queueFamilyIndexCount;
    const uint32_t*                  pQueueFamilyIndices;
    VkSurfaceTransformFlagBitsKHR    preTransform;
    VkCompositeAlphaFlagBitsKHR      compositeAlpha;
    VkPresentModeKHR                 presentMode;
    VkBool32                         clipped;
    VkSwapchainKHR                   oldSwapchain;
    VkSwapchainCreateInfoKHR createInfo{
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0
    };
  }
};

#define VK_alloca(type, size) (type*)alloca((size) * sizeof(type))

inline constexpr static u32 InvalidQueueFamily = (u32)-1;

class VulkanContext{
  const VkAllocationCallbacks* allocator     = nullptr;
  VkInstance             instance            = VK_NULL_HANDLE;
  VkPhysicalDevice       physicalDevice      = VK_NULL_HANDLE;
  VkDevice               device              = VK_NULL_HANDLE;
  u32                    graphicsQueueFamily = InvalidQueueFamily;
  u32                    computeQueueFamily  = InvalidQueueFamily;
  u32                    transferQueueFamily = InvalidQueueFamily;
  VkQueue                guiQueue            = VK_NULL_HANDLE;
  VkQueue                graphicsQueue       = VK_NULL_HANDLE;
  VkQueue                computeQueue        = VK_NULL_HANDLE;
  VkQueue                transferQueue       = VK_NULL_HANDLE;
  VkQueue                presentQueue        = VK_NULL_HANDLE;

  VkDescriptorPool       descriptorPool      = VK_NULL_HANDLE;
  u32                    maxInlineUniformBlockSize = 0;

  inline void createInstance(){
    VkApplicationInfo appInfo{
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = APPLICATION_NAME,
        .applicationVersion = APPLICATION_VERSION,
        .pEngineName = ENGINE_NAME,
        .engineVersion = ENGINE_VERSION,
        .apiVersion = VK_API_VERSION_1_2
    };

    u32 glfwReqExtensionCount;
    auto glfwReqExtensions = glfwGetRequiredInstanceExtensions(&glfwReqExtensionCount);


    VkInstanceCreateInfo instanceCreateInfo{
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = enabled_layers.size(),
        .ppEnabledLayerNames = enabled_layers.data(),
        .enabledExtensionCount = glfwReqExtensionCount,
        .ppEnabledExtensionNames = glfwReqExtensions
    };

    auto result = vkCreateInstance(&instanceCreateInfo, allocator, &instance);
    assert(result == VK_SUCCESS);
  }
  inline void selectPhysicalDevice(){
    u32 physicalDeviceCount;
    VkPhysicalDevice* physicalDeviceMemory;

    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
    assert( physicalDeviceCount != 0 );
    physicalDeviceMemory = VK_alloca(VkPhysicalDevice, physicalDeviceCount);

    VkPhysicalDeviceProperties2 properties{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
        .pNext = nullptr
    };

    std::span physicalDevices{physicalDeviceMemory, physicalDeviceCount};
    for (auto physDev : physicalDevices) {
      vkGetPhysicalDeviceProperties2(physDev, &properties);
      if (properties.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        physicalDevice = physDev;
        return;
      }
    }

    // no discrete GPU found, fallback on default GPU
    physicalDevice = physicalDevices.front();

    VkPhysicalDeviceInlineUniformBlockPropertiesEXT inlineUniformBlockProperties{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_PROPERTIES_EXT,
        .pNext = nullptr
    };
    properties.pNext = &inlineUniformBlockProperties;
    vkGetPhysicalDeviceProperties2(physicalDevice, &properties);
    maxInlineUniformBlockSize = inlineUniformBlockProperties.maxInlineUniformBlockSize;
  }
  inline void findQueueFamilies(){
    u32 queueFamilyPropertyCount;
    VkQueueFamilyProperties* queueFamilyPropertiesMemory;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, nullptr);
    queueFamilyPropertiesMemory = VK_alloca(VkQueueFamilyProperties, queueFamilyPropertyCount);


    std::span queueFamilies{queueFamilyPropertiesMemory, queueFamilyPropertyCount};
    for (u32 currentIndex = 0; currentIndex < queueFamilyPropertyCount; ++currentIndex) {
      const VkQueueFamilyProperties& currentQueue = queueFamilyPropertiesMemory[currentIndex];
      if (currentQueue.queueFlags & VK_QUEUE_GRAPHICS_BIT && graphicsQueueFamily == InvalidQueueFamily)
        graphicsQueueFamily = currentIndex;
      if (currentQueue.queueFlags & VK_QUEUE_TRANSFER_BIT) {
        if (transferQueueFamily == InvalidQueueFamily || (!(currentQueue.queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT))))
          transferQueueFamily = currentIndex;
      }
      if (currentQueue.queueFlags & VK_QUEUE_COMPUTE_BIT) {
        if (computeQueueFamily == InvalidQueueFamily || !(currentQueue.queueFlags & VK_QUEUE_GRAPHICS_BIT))
          computeQueueFamily = currentIndex;
      }
    }
    assert(graphicsQueueFamily != InvalidQueueFamily);
    assert(transferQueueFamily != InvalidQueueFamily);
    assert(computeQueueFamily  != InvalidQueueFamily);
  }
  inline void createDevice(){
    u32 queueFamilyCount = 1;
    if (transferQueueFamily != graphicsQueueFamily)
      ++queueFamilyCount;
    if (computeQueueFamily != transferQueueFamily && computeQueueFamily != graphicsQueueFamily)
      ++queueFamilyCount;

    bool graphicsClaimed = false;
    bool transferClaimed = false;
    bool computeClaimed  = false;

    auto* queueCI = VK_alloca(VkDeviceQueueCreateInfo, queueFamilyCount);
    memset(queueCI, 0, queueFamilyCount * sizeof(VkDeviceQueueCreateInfo));
    for (u32 queueIndex = 0; queueIndex < queueFamilyCount; ++queueIndex) {
      auto& currentFamily = queueCI[queueIndex];
      currentFamily.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      currentFamily.pNext = nullptr;
      currentFamily.flags = 0;
      currentFamily.queueCount = 0;
      currentFamily.queueFamilyIndex = InvalidQueueFamily;

      if (!graphicsClaimed) {
        currentFamily.queueFamilyIndex = graphicsQueueFamily;
        currentFamily.queueCount += 3; //graphicsQueue, guiQueue, presentQueue
        graphicsClaimed = true;
      }
      if (!transferClaimed && (currentFamily.queueFamilyIndex == InvalidQueueFamily || currentFamily.queueFamilyIndex == transferQueueFamily)) {
        currentFamily.queueFamilyIndex = transferQueueFamily;
        ++currentFamily.queueCount;
        transferClaimed = true;
      }
      if (!computeClaimed && (currentFamily.queueFamilyIndex == InvalidQueueFamily || currentFamily.queueFamilyIndex == computeQueueFamily)) {
        currentFamily.queueFamilyIndex = computeQueueFamily;
        ++currentFamily.queueCount;
        computeClaimed = true;
      }

      assert(currentFamily.queueFamilyIndex != InvalidQueueFamily);
      assert(currentFamily.queueCount > 0);


      auto queuePriorities = VK_alloca(float, currentFamily.queueCount);
      for (float& f : std::span{queuePriorities, currentFamily.queueCount})
        f = 1.0f;
    }



    VkPhysicalDeviceRayQueryFeaturesKHR rayQueryFeatures{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR,
        .pNext = nullptr
    };
    VkPhysicalDeviceRayTracingPipelineFeaturesKHR rayTraceFeatures{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR,
        .pNext = &rayQueryFeatures
    };
    VkPhysicalDeviceSynchronization2FeaturesKHR synchronizationFeatures{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR,
        .pNext = &rayTraceFeatures
    };
    VkPhysicalDeviceInlineUniformBlockFeaturesEXT inlineUniformBlockFeatures{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES_EXT,
        .pNext = &synchronizationFeatures
    };

    VkPhysicalDeviceVulkan12Features vulkan12Features{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
        .pNext = &inlineUniformBlockFeatures
    };

    VkPhysicalDeviceVulkan11Features vulkan11Features{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES,
        .pNext = &vulkan12Features
    };

    VkPhysicalDeviceFeatures2 features{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
        .pNext = &vulkan11Features
    };

    vkGetPhysicalDeviceFeatures2(physicalDevice, &features);

    VkDeviceCreateInfo deviceCreateInfo{
        .sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext                   = &features,
        .flags                   = 0,
        .queueCreateInfoCount    = queueFamilyCount,
        .pQueueCreateInfos       = queueCI,
        .enabledLayerCount       = 0,
        .ppEnabledLayerNames     = nullptr,
        .enabledExtensionCount   = enabled_device_extensions.size(),
        .ppEnabledExtensionNames = enabled_device_extensions.data(),
        .pEnabledFeatures        = nullptr
    };

    auto result = vkCreateDevice(physicalDevice, &deviceCreateInfo, allocator, &device);
    assert(result == VK_SUCCESS);
  }
  inline void fetchQueues(){
    const u32 maxQueueFamily = std::max(graphicsQueueFamily, std::max(computeQueueFamily, transferQueueFamily));
    auto      queueFamilyCounts = VK_alloca(u32, maxQueueFamily + 1);
    memset(queueFamilyCounts, 0, sizeof(u32) * (maxQueueFamily + 1));
    vkGetDeviceQueue(device, graphicsQueueFamily, queueFamilyCounts[graphicsQueueFamily]++, &graphicsQueue);
    vkGetDeviceQueue(device, graphicsQueueFamily, queueFamilyCounts[graphicsQueueFamily]++, &guiQueue);
    vkGetDeviceQueue(device, graphicsQueueFamily, queueFamilyCounts[graphicsQueueFamily]++, &presentQueue);
    vkGetDeviceQueue(device, transferQueueFamily, queueFamilyCounts[transferQueueFamily]++, &transferQueue);
    vkGetDeviceQueue(device, computeQueueFamily, queueFamilyCounts[computeQueueFamily]++, &computeQueue);

    assert(graphicsQueue != VK_NULL_HANDLE);
    assert(guiQueue != VK_NULL_HANDLE);
    assert(presentQueue != VK_NULL_HANDLE);
    assert(transferQueue != VK_NULL_HANDLE);
    assert(computeQueue != VK_NULL_HANDLE);
  }
  inline void makeDescriptorPool(){

    constexpr static auto DescriptorPerPool = 1024;

    VkDescriptorPoolSize pool_sizes[] =
        {
            { VK_DESCRIPTOR_TYPE_SAMPLER,                    DescriptorPerPool },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,     DescriptorPerPool },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,              DescriptorPerPool },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,              DescriptorPerPool },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,       DescriptorPerPool },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,       DescriptorPerPool },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,             DescriptorPerPool },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,             DescriptorPerPool },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,     DescriptorPerPool },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,     DescriptorPerPool },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,           DescriptorPerPool },
            { VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT,   maxInlineUniformBlockSize },
            { VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, DescriptorPerPool }
        };

    VkDescriptorPoolInlineUniformBlockCreateInfoEXT inlineBlockCreateInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_INLINE_UNIFORM_BLOCK_CREATE_INFO_EXT,
        .pNext = nullptr,
        .maxInlineUniformBlockBindings = DescriptorPerPool
    };

    VkDescriptorPoolCreateInfo poolInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = &inlineBlockCreateInfo,
        .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT | VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT,
        .maxSets = DescriptorPerPool * std::size(pool_sizes),
        .poolSizeCount = std::size(pool_sizes),
        .pPoolSizes = pool_sizes
    };

    auto result = vkCreateDescriptorPool(device, &poolInfo, allocator, &descriptorPool);
    assert( result == VK_SUCCESS );
    assert( descriptorPool != VK_NULL_HANDLE );
  }

  void destroyInstance() const noexcept {
    vkDestroyInstance(instance, allocator);
  }
  void destroyDevice(){
    vkDestroyDevice(device, allocator);
  }
  void destroyDescriptorPool(){
    vkDestroyDescriptorPool(device, descriptorPool, allocator);
  }



public:
  VulkanContext() noexcept : VulkanContext(nullptr){}
  explicit VulkanContext(const VkAllocationCallbacks* pAllocator) noexcept : allocator(pAllocator){
    createInstance();
    selectPhysicalDevice();
    findQueueFamilies();
    createDevice();
    fetchQueues();
    makeDescriptorPool();
  }
  ~VulkanContext(){
    destroyDescriptorPool();
    destroyDevice();
    destroyInstance();
  }


  VkSurfaceKHR getSurface(GLFWwindow* window) const noexcept {
    VkSurfaceKHR surface;
    glfwCreateWindowSurface(instance, window, allocator, &surface);
    return surface;
  }
};

class Window{
  VulkanContext*              vulkanContext;
  GLFWwindow*                 glfwWindow;
  ImGuiContext*               imguiContext;
  VkSurfaceKHR                surface;
  VkSurfaceFormatKHR          surfaceFormat;
  Swapchain                   swapchain;


  static GLFWwindow* makeGLFWWindow(Extent2D<> windowSize, const char* pName = nullptr) noexcept {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    return glfwCreateWindow((i32)windowSize.width, (i32)windowSize.height, pName, nullptr, nullptr);
  }

public:
  Window(Extent2D<> windowSize, VulkanContext& context) noexcept
      : vulkanContext(&context),
        glfwWindow(makeGLFWWindow(windowSize)),
        imguiContext(ImGui::CreateContext()),
        surface(context.getSurface(glfwWindow)),
        surfaceFormat(),
        swapchain(){}

  ~Window(){

  }
};



struct Context{
  GLFWwindow*   window;
  ImGuiContext* context;
  ImGui_ImplVulkan_InitInfo vulkanInitInfo;
  explicit Context(Extent2D<i32> extents) : window(nullptr),
                                            context(ImGui::GetCurrentContext()),
                                            vulkanInitInfo(){
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(extents.width, extents.height, nullptr, nullptr, nullptr);
    ImGui_ImplGlfw_InitForVulkan(window, true);
    ImGui_ImplVulkan_Init(&vulkanInitInfo, );
  }
  ~Context(){
    glfwDestroyWindow(window);
    ImGui_ImplGlfw_Shutdown();
  }

  explicit operator bool() const noexcept {
    return !glfwWindowShouldClose(window);
  }

  void close() const noexcept {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }

  static void next_frame(){

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
  }
};


//#define IMGUI_UNLIMITED_FRAME_RATE
#ifdef _DEBUG
#define IMGUI_VULKAN_DEBUG_REPORT
#endif

static VkAllocationCallbacks*   g_Allocator = NULL;
static VkInstance               g_Instance = VK_NULL_HANDLE;
static VkPhysicalDevice         g_PhysicalDevice = VK_NULL_HANDLE;
static VkDevice                 g_Device = VK_NULL_HANDLE;
static uint32_t                 g_QueueFamily = (uint32_t)-1;
static VkQueue                  g_Queue = VK_NULL_HANDLE;
static VkDebugReportCallbackEXT g_DebugReport = VK_NULL_HANDLE;
static VkPipelineCache          g_PipelineCache = VK_NULL_HANDLE;
static VkDescriptorPool         g_DescriptorPool = VK_NULL_HANDLE;

static ImGui_ImplVulkanH_Window g_MainWindowData;
static int                      g_MinImageCount = 2;
static bool                     g_SwapChainRebuild = false;

static void check_vk_result(VkResult err)
{
  if (err == 0)
    return;
  fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
  if (err < 0)
    abort();
}


static void SetupVulkan(const char** extensions, uint32_t extensions_count)
{
  VkResult err;

  // Create Vulkan Instance
  {
    VkInstanceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.enabledExtensionCount = extensions_count;
    create_info.ppEnabledExtensionNames = extensions;
#ifdef IMGUI_VULKAN_DEBUG_REPORT
    // Enabling validation layers
    const char* layers[] = { "VK_LAYER_KHRONOS_validation" };
    create_info.enabledLayerCount = 1;
    create_info.ppEnabledLayerNames = layers;

    // Enable debug report extension (we need additional storage, so we duplicate the user array to add our new extension to it)
    const char** extensions_ext = (const char**)malloc(sizeof(const char*) * (extensions_count + 1));
    memcpy(extensions_ext, extensions, extensions_count * sizeof(const char*));
    extensions_ext[extensions_count] = "VK_EXT_debug_report";
    create_info.enabledExtensionCount = extensions_count + 1;
    create_info.ppEnabledExtensionNames = extensions_ext;

    // Create Vulkan Instance
    err = vkCreateInstance(&create_info, g_Allocator, &g_Instance);
    check_vk_result(err);
    free(extensions_ext);

    // Get the function pointer (required for any extensions)
    auto vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(g_Instance, "vkCreateDebugReportCallbackEXT");
    IM_ASSERT(vkCreateDebugReportCallbackEXT != NULL);

    // Setup the debug report callback
    VkDebugReportCallbackCreateInfoEXT debug_report_ci = {};
    debug_report_ci.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    debug_report_ci.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
    debug_report_ci.pfnCallback = debug_report;
    debug_report_ci.pUserData = NULL;
    err = vkCreateDebugReportCallbackEXT(g_Instance, &debug_report_ci, g_Allocator, &g_DebugReport);
    check_vk_result(err);
#else
    // Create Vulkan Instance without any debug feature
        err = vkCreateInstance(&create_info, g_Allocator, &g_Instance);
        check_vk_result(err);
        IM_UNUSED(g_DebugReport);
#endif
  }

  // Select GPU
  {
    uint32_t gpu_count;
    err = vkEnumeratePhysicalDevices(g_Instance, &gpu_count, NULL);
    check_vk_result(err);
    IM_ASSERT(gpu_count > 0);

    VkPhysicalDevice* gpus = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * gpu_count);
    err = vkEnumeratePhysicalDevices(g_Instance, &gpu_count, gpus);
    check_vk_result(err);

    // If a number >1 of GPUs got reported, you should find the best fit GPU for your purpose
    // e.g. VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU if available, or with the greatest memory available, etc.
    // for sake of simplicity we'll just take the first one, assuming it has a graphics queue family.
    g_PhysicalDevice = gpus[0];
    free(gpus);
  }

  // Select graphics queue family
  {
    uint32_t count;
    vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &count, NULL);
    VkQueueFamilyProperties* queues = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * count);
    vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &count, queues);
    for (uint32_t i = 0; i < count; i++)
      if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
      {
        g_QueueFamily = i;
        break;
      }
    free(queues);
    IM_ASSERT(g_QueueFamily != (uint32_t)-1);
  }

  // Create Logical Device (with 1 queue)
  {
    int device_extension_count = 1;
    const char* device_extensions[] = { "VK_KHR_swapchain" };
    const float queue_priority[] = { 1.0f };
    VkDeviceQueueCreateInfo queue_info[1] = {};
    queue_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_info[0].queueFamilyIndex = g_QueueFamily;
    queue_info[0].queueCount = 1;
    queue_info[0].pQueuePriorities = queue_priority;
    VkDeviceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.queueCreateInfoCount = sizeof(queue_info) / sizeof(queue_info[0]);
    create_info.pQueueCreateInfos = queue_info;
    create_info.enabledExtensionCount = device_extension_count;
    create_info.ppEnabledExtensionNames = device_extensions;
    err = vkCreateDevice(g_PhysicalDevice, &create_info, g_Allocator, &g_Device);
    check_vk_result(err);
    vkGetDeviceQueue(g_Device, g_QueueFamily, 0, &g_Queue);
  }

  // Create Descriptor Pool
  {
    VkDescriptorPoolSize pool_sizes[] =
        {
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
        };
    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
    pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;
    err = vkCreateDescriptorPool(g_Device, &pool_info, g_Allocator, &g_DescriptorPool);
    check_vk_result(err);
  }
}

// All the ImGui_ImplVulkanH_XXX structures/functions are optional helpers used by the demo.
// Your real engine/app may not use them.
static void SetupVulkanWindow(ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface, int width, int height)
{
  wd->Surface = surface;

  // Check for WSI support
  VkBool32 res;
  vkGetPhysicalDeviceSurfaceSupportKHR(g_PhysicalDevice, g_QueueFamily, wd->Surface, &res);
  if (res != VK_TRUE)
  {
    fprintf(stderr, "Error no WSI support on physical device 0\n");
    exit(-1);
  }

  // Select Surface Format
  const VkFormat requestSurfaceImageFormat[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM };
  const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
  wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(g_PhysicalDevice, wd->Surface, requestSurfaceImageFormat, (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat), requestSurfaceColorSpace);

  // Select Present Mode
#ifdef IMGUI_UNLIMITED_FRAME_RATE
    VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR };
#else
  VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_FIFO_KHR };
#endif
  wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(g_PhysicalDevice, wd->Surface, &present_modes[0], IM_ARRAYSIZE(present_modes));
  //printf("[vulkan] Selected PresentMode = %d\n", wd->PresentMode);

  // Create SwapChain, RenderPass, Framebuffer, etc.
  IM_ASSERT(g_MinImageCount >= 2);
  ImGui_ImplVulkanH_CreateOrResizeWindow(g_Instance, g_PhysicalDevice, g_Device, wd, g_QueueFamily, g_Allocator, width, height, g_MinImageCount);
}

static void CleanupVulkan()
{
  vkDestroyDescriptorPool(g_Device, g_DescriptorPool, g_Allocator);

#ifdef IMGUI_VULKAN_DEBUG_REPORT
  // Remove the debug report callback
  auto vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(g_Instance, "vkDestroyDebugReportCallbackEXT");
  vkDestroyDebugReportCallbackEXT(g_Instance, g_DebugReport, g_Allocator);
#endif // IMGUI_VULKAN_DEBUG_REPORT

  vkDestroyDevice(g_Device, g_Allocator);
  vkDestroyInstance(g_Instance, g_Allocator);
}

static void CleanupVulkanWindow()
{
  ImGui_ImplVulkanH_DestroyWindow(g_Instance, g_Device, &g_MainWindowData, g_Allocator);
}

static void FrameRender(ImGui_ImplVulkanH_Window* wd, ImDrawData* draw_data)
{
  VkResult err;

  VkSemaphore image_acquired_semaphore  = wd->FrameSemaphores[wd->SemaphoreIndex].ImageAcquiredSemaphore;
  VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
  err = vkAcquireNextImageKHR(g_Device, wd->Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &wd->FrameIndex);
  if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
  {
    g_SwapChainRebuild = true;
    return;
  }
  check_vk_result(err);

  ImGui_ImplVulkanH_Frame* fd = &wd->Frames[wd->FrameIndex];
  {
    err = vkWaitForFences(g_Device, 1, &fd->Fence, VK_TRUE, UINT64_MAX);    // wait indefinitely instead of periodically checking
    check_vk_result(err);

    err = vkResetFences(g_Device, 1, &fd->Fence);
    check_vk_result(err);
  }
  {
    err = vkResetCommandPool(g_Device, fd->CommandPool, 0);
    check_vk_result(err);
    VkCommandBufferBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    err = vkBeginCommandBuffer(fd->CommandBuffer, &info);
    check_vk_result(err);
  }
  {
    VkRenderPassBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    info.renderPass = wd->RenderPass;
    info.framebuffer = fd->Framebuffer;
    info.renderArea.extent.width = wd->Width;
    info.renderArea.extent.height = wd->Height;
    info.clearValueCount = 1;
    info.pClearValues = &wd->ClearValue;
    vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
  }

  // Record dear imgui primitives into command buffer
  ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);

  // Submit command buffer
  vkCmdEndRenderPass(fd->CommandBuffer);
  {
    VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = &image_acquired_semaphore;
    info.pWaitDstStageMask = &wait_stage;
    info.commandBufferCount = 1;
    info.pCommandBuffers = &fd->CommandBuffer;
    info.signalSemaphoreCount = 1;
    info.pSignalSemaphores = &render_complete_semaphore;

    err = vkEndCommandBuffer(fd->CommandBuffer);
    check_vk_result(err);
    err = vkQueueSubmit(g_Queue, 1, &info, fd->Fence);
    check_vk_result(err);
  }
}

static void FramePresent(ImGui_ImplVulkanH_Window* wd)
{
  if (g_SwapChainRebuild)
    return;
  VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
  VkPresentInfoKHR info = {};
  info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  info.waitSemaphoreCount = 1;
  info.pWaitSemaphores = &render_complete_semaphore;
  info.swapchainCount = 1;
  info.pSwapchains = &wd->Swapchain;
  info.pImageIndices = &wd->FrameIndex;
  VkResult err = vkQueuePresentKHR(g_Queue, &info);
  if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
  {
    g_SwapChainRebuild = true;
    return;
  }
  check_vk_result(err);
  wd->SemaphoreIndex = (wd->SemaphoreIndex + 1) % wd->ImageCount; // Now we can use the next set of semaphores
}

static void glfw_error_callback(int error, const char* description)
{
  fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main(int, char**)
{
  // Setup GLFW window
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit())
    return 1;

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+Vulkan example", NULL, NULL);

  // Setup Vulkan
  if (!glfwVulkanSupported())
  {
    printf("GLFW: Vulkan Not Supported\n");
    return 1;
  }
  uint32_t extensions_count = 0;
  const char** extensions = glfwGetRequiredInstanceExtensions(&extensions_count);
  SetupVulkan(extensions, extensions_count);

  // Create Window Surface
  VkSurfaceKHR surface;
  VkResult err = glfwCreateWindowSurface(g_Instance, window, g_Allocator, &surface);
  check_vk_result(err);

  // Create Framebuffers
  int w, h;
  glfwGetFramebufferSize(window, &w, &h);
  ImGui_ImplVulkanH_Window* wd = &g_MainWindowData;
  SetupVulkanWindow(wd, surface, w, h);

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  //ImGui::StyleColorsClassic();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForVulkan(window, true);
  ImGui_ImplVulkan_InitInfo init_info = {};
  init_info.Instance = g_Instance;
  init_info.PhysicalDevice = g_PhysicalDevice;
  init_info.Device = g_Device;
  init_info.QueueFamily = g_QueueFamily;
  init_info.Queue = g_Queue;
  init_info.PipelineCache = g_PipelineCache;
  init_info.DescriptorPool = g_DescriptorPool;
  init_info.Allocator = g_Allocator;
  init_info.MinImageCount = g_MinImageCount;
  init_info.ImageCount = wd->ImageCount;
  init_info.CheckVkResultFn = check_vk_result;
  ImGui_ImplVulkan_Init(&init_info, wd->RenderPass);

  // Load Fonts
  // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
  // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
  // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
  // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
  // - Read 'docs/FONTS.md' for more instructions and details.
  // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
  //io.Fonts->AddFontDefault();
  //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
  //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
  //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
  //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
  //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
  //IM_ASSERT(font != NULL);

  // Upload Fonts
  {
    // Use any command queue
    VkCommandPool command_pool = wd->Frames[wd->FrameIndex].CommandPool;
    VkCommandBuffer command_buffer = wd->Frames[wd->FrameIndex].CommandBuffer;

    err = vkResetCommandPool(g_Device, command_pool, 0);
    check_vk_result(err);
    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    err = vkBeginCommandBuffer(command_buffer, &begin_info);
    check_vk_result(err);

    ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

    VkSubmitInfo end_info = {};
    end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    end_info.commandBufferCount = 1;
    end_info.pCommandBuffers = &command_buffer;
    err = vkEndCommandBuffer(command_buffer);
    check_vk_result(err);
    err = vkQueueSubmit(g_Queue, 1, &end_info, VK_NULL_HANDLE);
    check_vk_result(err);

    err = vkDeviceWaitIdle(g_Device);
    check_vk_result(err);
    ImGui_ImplVulkan_DestroyFontUploadObjects();
  }

  // Our state
  bool show_demo_window = true;
  bool show_another_window = false;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  // Main loop
  while (!glfwWindowShouldClose(window))
  {
    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    glfwPollEvents();

    // Resize swap chain?
    if (g_SwapChainRebuild)
    {
      int width, height;
      glfwGetFramebufferSize(window, &width, &height);
      if (width > 0 && height > 0)
      {
        ImGui_ImplVulkan_SetMinImageCount(g_MinImageCount);
        ImGui_ImplVulkanH_CreateOrResizeWindow(g_Instance, g_PhysicalDevice, g_Device, &g_MainWindowData, g_QueueFamily, g_Allocator, width, height, g_MinImageCount);
        g_MainWindowData.FrameIndex = 0;
        g_SwapChainRebuild = false;
      }
    }

    // Start the Dear ImGui frame
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window)
      ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {
      static float f = 0.0f;
      static int counter = 0;

      ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

      ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
      ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
      ImGui::Checkbox("Another Window", &show_another_window);

      ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
      ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

      if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
        counter++;
      ImGui::SameLine();
      ImGui::Text("counter = %d", counter);

      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
      ImGui::End();
    }

    // 3. Show another simple window.
    if (show_another_window)
    {
      ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
      ImGui::Text("Hello from another window!");
      if (ImGui::Button("Close Me"))
        show_another_window = false;
      ImGui::End();
    }

    // Rendering
    ImGui::Render();
    ImDrawData* draw_data = ImGui::GetDrawData();
    const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);
    if (!is_minimized)
    {
      wd->ClearValue.color.float32[0] = clear_color.x * clear_color.w;
      wd->ClearValue.color.float32[1] = clear_color.y * clear_color.w;
      wd->ClearValue.color.float32[2] = clear_color.z * clear_color.w;
      wd->ClearValue.color.float32[3] = clear_color.w;
      FrameRender(wd, draw_data);
      FramePresent(wd);
    }
  }

  // Cleanup
  err = vkDeviceWaitIdle(g_Device);
  check_vk_result(err);
  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  CleanupVulkanWindow();
  CleanupVulkan();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}




int main(){
  Context ctx({1920, 1080});


  while (ctx) {
    ImGui::Begin("Main Window");
    ctx.next_frame();
    ImGui::End();
  }
}