//
// Created by Maxwell on 2020-11-09.
//
#include <valkyrie/graphics/api/device.hpp>
#include <valkyrie/graphics/api/instance.hpp>
#include <valkyrie/graphics/api/queue.hpp>
#include <valkyrie/preprocessor.hpp>

#include "function_pointers.hpp"
#include "status_wrappers.hpp"

#include <iostream>
#include <string_view>


using namespace valkyrie::graphics::api;

/*
namespace {

  template <typename T>
  struct type_WRAPPER_t;

  template <typename T>
  constexpr std::string_view getTypeName() noexcept{
    std::string_view SV = VK_function_name;
    return SV;
  }





  */
/*struct Functions{
    struct Instance{
      PFN_vkGetInstanceProcAddr getProcAddr;

      PFN_vkCreateInstance create;
      PFN_vkDestroyInstance destroy;

      PFN_vkEnumeratePhysicalDevices enumeratePhysicalDevices;
      PFN_vkEnumeratePhysicalDeviceGroups enumeratePhysicalDeviceGroups;
      PFN_vkEnumerateInstanceVersion enumerateInstanceVersion;

    } instance;
    struct PhysicalDevice{
      PFN_vkGetPhysicalDeviceFeatures getFeatures;
      PFN_vkGetPhysicalDeviceFeatures2 getFeatures2;

      PFN_vkGetPhysicalDeviceProperties getProperties;
      PFN_vkGetPhysicalDeviceProperties2 getProperties2;

      PFN_vkGetPhysicalDeviceFormatProperties getFormatProperties;
      PFN_vkGetPhysicalDeviceFormatProperties2 getFormatProperties2;


      PFN_vkGetPhysicalDeviceImageFormatProperties getImageFormatProperties;
      PFN_vkGetPhysicalDeviceImageFormatProperties2 getImageFormatProperties2;

      PFN_vkGetPhysicalDeviceSparseImageFormatProperties getSparseImageFormatProperties;
      PFN_vkGetPhysicalDeviceSparseImageFormatProperties2 getSparseImageFormatProperties2;

      PFN_vkGetPhysicalDeviceQueueFamilyProperties getQueueFamilyProperties;
      PFN_vkGetPhysicalDeviceQueueFamilyProperties2 getQueueFamilyProperties2;

      PFN_vkGetPhysicalDeviceMemoryProperties getMemoryProperties;
      PFN_vkGetPhysicalDeviceMemoryProperties2 getMemoryProperties2;

      PFN_vkGetPhysicalDeviceExternalBufferProperties getExternalBufferProperties;
      PFN_vkGetPhysicalDeviceExternalFenceProperties getExternalFenceProperties;
      PFN_vkGetPhysicalDeviceExternalSemaphoreProperties getExternalSemaphoreProperties;

    } physicalDevice;
    struct Device{

      PFN_vkGetDeviceProcAddr getProcAddr;

      PFN_vkDeviceWaitIdle waitIdle;
      PFN_vkGetDeviceQueue getQueue;
      PFN_vkGetDeviceQueue2 getQueue2;
      PFN_vkGetDeviceGroupPeerMemoryFeatures getDeviceGroupPeerMemoryFeatures;
      PFN_vkGetDeviceGroupPresentCapabilitiesKHR getDeviceGroupPresentCapabilities;
      PFN_vkGetDeviceMemoryOpaqueCaptureAddress getMemoryOpaqueCaptureAddress;
      PFN_vkDestroyDevice destroy;



      struct DeviceMemory{
        PFN_vkAllocateMemory allocate;
        PFN_vkFreeMemory free;
        PFN_vkMapMemory map;
        PFN_vkUnmapMemory unmap;
        PFN_vkFlushMappedMemoryRanges flushMappedRanges;
        PFN_vkInvalidateMappedMemoryRanges invalidateMappedRanges;
        PFN_vkGetDeviceMemoryCommitment getCommitment;
        PFN_vkGetDeviceMemoryOpaqueCaptureAddress getOpaqueCaptureAddress;
      } memory;

      struct Buffer{
        PFN_vkCreateBuffer create;
        PFN_vkDestroyBuffer destroy;

        PFN_vkBindBufferMemory bindMemory;
        PFN_vkBindBufferMemory2 bindMemory2;
        PFN_vkGetBufferMemoryRequirements getMemoryRequirements;
        PFN_vkGetBufferMemoryRequirements2 getMemoryRequirements2;

        PFN_vkGetBufferDeviceAddress getDeviceAddress;
        PFN_vkGetBufferOpaqueCaptureAddress getOpaqueCaptureAddress;
      } buffer;
      struct BufferView{
        PFN_vkCreateBufferView create;
        PFN_vkDestroyBufferView destroy;
      } bufferView;
      struct Image{
        PFN_vkCreateImage create;
        PFN_vkDestroyImage destroy;
        PFN_vkBindImageMemory bindMemory;
        PFN_vkBindImageMemory2 bindMemory2;
        PFN_vkGetImageMemoryRequirements getMemoryRequirements;
        PFN_vkGetImageMemoryRequirements2 getMemoryRequirements2;
        PFN_vkGetImageSparseMemoryRequirements getSparseMemoryRequirements;
        PFN_vkGetImageSparseMemoryRequirements2 getSparseMemoryRequirements2;
        PFN_vkGetImageSubresourceLayout getSubresourceLayout;
      } image;
      struct ImageView{
        PFN_vkCreateImageView create;
        PFN_vkDestroyImageView destroy;
      } imageView;

      struct Fence{
        PFN_vkCreateFence create;
        PFN_vkDestroyFence destroy;
        PFN_vkResetFences reset;
        PFN_vkGetFenceStatus getStatus;
        PFN_vkWaitForFences wait;
      } fence;
      struct Semaphore{
        PFN_vkCreateSemaphore create;
        PFN_vkDestroySemaphore destroy;
        PFN_vkGetSemaphoreCounterValue getCounterValue;
        PFN_vkWaitSemaphores wait;
        PFN_vkSignalSemaphore signal;
      } semaphore;
      struct Event{
        PFN_vkCreateEvent create;
        PFN_vkDestroyEvent destroy;
        PFN_vkGetEventStatus getStatus;
        PFN_vkSetEvent set;
        PFN_vkResetEvent reset;
      } event;

      struct QueryPool{
        PFN_vkCreateQueryPool create;
        PFN_vkDestroyQueryPool destroy;
        PFN_vkGetQueryPoolResults getResults;
        PFN_vkResetQueryPool reset;
      } queryPool;

      struct ShaderModule{
        PFN_vkCreateShaderModule create;
        PFN_vkDestroyShaderModule destroy;
      } shaderModule;

      struct Pipeline{
        PFN_vkCreateGraphicsPipelines createGraphics;
        PFN_vkCreateComputePipelines createCompute;
        PFN_vkCreateRayTracingPipelinesNV createRayTracingNV;
        PFN_vkCreateRayTracingPipelinesKHR createRayTracingKHR;
        PFN_vkDestroyPipeline destroy;
        struct Cache{
          PFN_vkCreatePipelineCache create;
          PFN_vkDestroyPipelineCache destroy;
          PFN_vkGetPipelineCacheData getData;
          PFN_vkMergePipelineCaches mergeCaches;
        } cache;
        struct Layout{
          PFN_vkCreatePipelineLayout create;
          PFN_vkDestroyPipelineLayout destroy;
        } layout;
      } pipeline;

      struct Sampler{
        PFN_vkCreateSampler create;
        PFN_vkDestroySampler destroy;
      } sampler;

      struct Descriptor{
        struct Set{
          struct Layout{
            PFN_vkCreateDescriptorSetLayout create;
            PFN_vkDestroyDescriptorSetLayout destroy;
            PFN_vkGetDescriptorSetLayoutSupport getSupport;
          } layout;
          PFN_vkAllocateDescriptorSets allocate;
          PFN_vkFreeDescriptorSets free;
          PFN_vkUpdateDescriptorSets update;
          PFN_vkUpdateDescriptorSetWithTemplate updateWithTemplate;
        } set;
        struct Pool{
          PFN_vkCreateDescriptorPool create;
          PFN_vkDestroyDescriptorPool destroy;
          PFN_vkResetDescriptorPool reset;
        } pool;
        struct UpdateTemplate{
          PFN_vkCreateDescriptorUpdateTemplate create;
          PFN_vkDestroyDescriptorUpdateTemplate destroy;
        } updateTemplate;
      } descriptor;

      struct Framebuffer{
        PFN_vkCreateFramebuffer create;
        PFN_vkDestroyFramebuffer destroy;
      } framebuffer;
      struct RenderPass{
        PFN_vkCreateRenderPass create;
        PFN_vkCreateRenderPass2 create2;
        PFN_vkDestroyRenderPass destroy;
        PFN_vkGetRenderAreaGranularity getRenderAreaGranularity;
      } renderPass;
      struct CommandPool{
        PFN_vkCreateCommandPool create;
        PFN_vkDestroyCommandPool destroy;
        PFN_vkResetCommandPool reset;
        PFN_vkAllocateCommandBuffers allocateCommandBuffers;
        PFN_vkFreeCommandBuffers freeCommandBuffers;

        PFN_vkTrimCommandPool trim;
      } commandPool;


      struct SamplerYcbcrConversion {
        PFN_vkCreateSamplerYcbcrConversion create;
        PFN_vkDestroySamplerYcbcrConversion destroy;
      } samplerYcbcrConversion;
    } device;
    struct Queue{
      PFN_vkQueueSubmit submit;
      PFN_vkQueueWaitIdle waitIdle;
      PFN_vkQueueBindSparse bindSparse;
    } queue;
    struct CommandBuffer{
      PFN_vkBeginCommandBuffer begin;
      PFN_vkEndCommandBuffer end;
      PFN_vkResetCommandBuffer reset;
      struct Cmd{
        PFN_vkCmdBindPipeline bindPipeline;

        PFN_vkCmdSetViewport setViewport;
        PFN_vkCmdSetScissor setScissor;
        PFN_vkCmdSetLineWidth setLineWidth;
        PFN_vkCmdSetDepthBias setDepthBias;
        PFN_vkCmdSetBlendConstants setBlendConstants;
        PFN_vkCmdSetDepthBounds setDepthBounds;
        PFN_vkCmdSetStencilCompareMask setStencilCompareMask;
        PFN_vkCmdSetStencilWriteMask setStencilWriteMask;
        PFN_vkCmdSetStencilReference setStencilReference;

        PFN_vkCmdBindDescriptorSets bindDescriptorSets;
        PFN_vkCmdBindIndexBuffer bindIndexBuffer;
        PFN_vkCmdBindVertexBuffers bindVertexBuffers;
        PFN_vkCmdBindVertexBuffers2EXT bindVertexBuffers2;

        PFN_vkCmdDraw draw;
        PFN_vkCmdDrawIndexed drawIndexed;
        PFN_vkCmdDrawIndirect drawIndirect;
        PFN_vkCmdDrawIndirectCount drawIndirectCount;
        PFN_vkCmdDrawIndexedIndirect drawIndexedIndirect;
        PFN_vkCmdDrawIndexedIndirectCount drawIndexedIndirectCount;

        PFN_vkCmdDispatch dispatch;
        PFN_vkCmdDispatchIndirect dispatchIndirect;
        PFN_vkCmdDispatchBase dispatchBase;

        PFN_vkCmdCopyBuffer copyBuffer;
        PFN_vkCmdCopyImage copyImage;
        PFN_vkCmdBlitImage blitImage;
        PFN_vkCmdCopyBufferToImage copyBufferToImage;
        PFN_vkCmdCopyImageToBuffer copyImageToBuffer;
        PFN_vkCmdUpdateBuffer updateBuffer;
        PFN_vkCmdFillBuffer fillBuffer;

        PFN_vkCmdClearColorImage clearColorImage;
        PFN_vkCmdClearDepthStencilImage clearDepthStencilImage;
        PFN_vkCmdClearAttachments clearAttachments;
        PFN_vkCmdResolveImage resolveImage;

        PFN_vkCmdSetEvent setEvent;
        PFN_vkCmdResetEvent resetEvent;
        PFN_vkCmdWaitEvents waitEvents;

        PFN_vkCmdPipelineBarrier pipelineBarrier;
        PFN_vkCmdBeginQuery beginQuery;
        PFN_vkCmdEndQuery endQuery;
        PFN_vkCmdResetQueryPool resetQueryPool;
        PFN_vkCmdCopyQueryPoolResults copyQueryPoolResults;

        PFN_vkCmdWriteTimestamp writeTimestamp;
        PFN_vkCmdPushConstants pushConstants;

        PFN_vkCmdBeginRenderPass beginRenderPass;
        PFN_vkCmdBeginRenderPass2 beginRenderPass2;
        PFN_vkCmdEndRenderPass endRenderPass;
        PFN_vkCmdEndRenderPass2 endRenderPass2;
        PFN_vkCmdNextSubpass nextSubpass;
        PFN_vkCmdNextSubpass2 nextSubpass2;

        PFN_vkCmdExecuteCommands executeCommands;
        PFN_vkCmdExecuteGeneratedCommandsNV executeGeneratedCommands;



        PFN_vkCmdSetDeviceMask setDeviceMask;


      } cmd;
    } commandBuffer;
  };*//*

}



void valkyrie::graphics::api::printFunctionTypeName() {
  std::cout << getTypeName<PFN_vkCmdDrawIndirectCount>() << std::endl;
}*/

class device::Impl{
  instance instance;
  physical_device physicalDevice;
  const DeviceFunctions* pFunctions;

  VkDevice handle;

public:
  Impl(physical_device physicalDevice) : instance(physicalDevice.instance()), physicalDevice(physicalDevice){

  }



  /*PhysicalDevice* pPhysicalDevice;
  Instance* pInstance;*/
};