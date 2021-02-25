//
// Created by Maxwell on 2021-01-17.
//

#include <valkyrie/Core/ADT/Handle.hpp>

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#include <vulkan/vulkan.hpp>
#include <atomic>


struct DeviceImpl{
  vk::UniqueDevice          handle;
  vk::DispatchLoaderDynamic api;
  std::atomic_uint32_t      refCount;
};
class Device{
  DeviceImpl* pImpl;
public:
  DeviceImpl() {}
};

using Allocator = const VkAllocationCallbacks*;

struct ThreadPool{
  Device device;
  VkCommandPool pool;
  VkCommandPool transientPool;
  uint32_t index;

  explicit ThreadPool(VkResult& result, const Device& device, Allocator alloc, uint32_t index)
      : device(device),
        pool(nullptr),
        transientPool(nullptr),
        index(index){

    vk::CommandPoolCreateInfo createInfo;

    createInfo
        .setFlags(vk::CommandPoolCreateFlagBits::eTransient | vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
        .setQueueFamilyIndex(index);


    result = vkCreateCommandPool(device, &createInfo, alloc, &transientPool);
    if (result == VK_SUCCESS) [[likely]] {
      createInfo.flags = 0;
      result = vkCreateCommandPool(device, &createInfo, alloc, &pool);
      if (result != VK_SUCCESS) [[unlikely]] {
        vkDestroyCommandPool(device, transientPool, alloc);
        transientPool = nullptr;
        pool = nullptr;
      }
    } else {
      transientPool = nullptr;
    }
  }
  ~ThreadPool(){
    vkDestroyCommandPool()
  }
};
struct Thread{
  ThreadPool* threadPool;
  VkQueue queue;
  float   priority;
};

struct Kernel{
  ThreadPool* threadPool;
  VkCommandBuffer buffer;
};
struct Subroutine{
  ThreadPool* threadPool;
  VkCommandBuffer buffer;
};

struct ShaderInterface{
  struct InputAttachment{};
  struct OutputAttachment{};
};




struct Shader{};
struct GraphicsShader   : Shader{};
struct ComputeShader    : Shader{};
struct RayTracingShader : Shader{};

struct VertexShader                 : GraphicsShader{};
struct GeometryShader               : GraphicsShader{};
struct FragmentShader               : GraphicsShader{};
struct TessellationShader           : GraphicsShader{};
struct TessellationControlShader    : TessellationShader{};
struct TessellationEvaluationShader : TessellationShader{};

struct VkMeshShader : GraphicsShader{};
struct VkTaskShader : GraphicsShader{};

struct RayGenShader       : RayTracingShader{};
struct AnyHitShader       : RayTracingShader{};
struct ClosestHitShader   : RayTracingShader{};
struct MissShader         : RayTracingShader{};
struct IntersectionShader : RayTracingShader{};
struct CallableShader     : RayTracingShader{};


struct ShaderGraph;
struct RenderPass;
struct ExecutionGraph;
struct FrameDescription;


int main(){

  return 0;
}
