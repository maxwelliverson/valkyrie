//
// Created by Maxwell on 2020-11-09.
//

#ifndef VALKYRIE_PHYSICAL_DEVICE_HPP
#define VALKYRIE_PHYSICAL_DEVICE_HPP

#include <valkyrie/adt/interval.hpp>
#include <valkyrie/graphics/api/queue.hpp>
#include <valkyrie/graphics/api/vulkan.hpp>

//#include <wayland-client-protocol.hpp>

namespace valkyrie::graphics::api{


  //class PhysicalDeviceFeatures;
  //class PhysicalDeviceProperties;
  /*class PhysicalDeviceFormatProperties;
  class PhysicalDeviceImageFormatProperties;
  class PhysicalDeviceSparseImageFormatProperties;
  class PhysicalDeviceQueueFamilyProperties;
  class PhysicalDeviceMemoryProperties;
  class PhysicalDeviceExternalBufferProperties;
  class PhysicalDeviceExternalFenceProperties;
  class PhysicalDeviceExternalSemaphoreProperties;*/

  class InstanceObject{
  protected:

  };
  class DeviceObject{};

  class QueueFamily{
  public:
    struct Handle;

    QueueFamily() = default;
    QueueFamily(const QueueFamily&) = default;

    u32 getIndex() const noexcept;
    u32 getQueueCount() const noexcept;
    QueueFlags getFlags() const noexcept;

    bool supportsPresentation(const surface & surface) const noexcept;


  private:
    Handle* pData;
  };

  class physical_device : public VulkanObject{

    friend class PhysicalDeviceObject;

    class Impl;
    Impl* pImpl;

  public:
    physical_device();
    physical_device(instance instance, void* nativeHandle) noexcept;
    physical_device(const physical_device &);
    physical_device(physical_device &&) noexcept;
    physical_device & operator=(const physical_device &);
    physical_device & operator=(physical_device &&) noexcept;
    ~physical_device();


    instance instance() const noexcept;
    device createDevice();

    bool queueFamilySupportsPresentation(u32 familyIndex, const surface & surface) const noexcept;

    Box2D<> getPresentRectangle(const surface & surface) const noexcept;
    //void getQueueFamilyProperties(u32& length, QueueFamily* families) noexcept;

    friend bool operator==(physical_device A, physical_device B) noexcept {
      return A.pImpl == B.pImpl;
    }
    friend auto operator<=>(physical_device A, physical_device B) noexcept {
      return A.pImpl <=> B.pImpl;
    }
  };




  
  /*class PhysicalDeviceFeatures : public OutputStruct{
    u32 robustBufferAccess = 0;
    u32 fullDrawIndexUint32 = 0;
    u32 imageCubeArray = 0;
    u32 independentBlend = 0;
    u32 geometryShader= 0;
    u32 tessellationShader= 0;
    u32 sampleRateShading= 0;
    u32 dualSrcBlend= 0;
    u32 logicOp= 0;
    u32 multiDrawIndirect= 0;
    u32 drawIndirectFirstInstance= 0;
    u32 depthClamp= 0;
    u32 depthBiasClamp= 0;
    u32 fillModeNonSolid= 0;
    u32 depthBounds= 0;
    u32 wideLines= 0;
    u32 largePoints= 0;
    u32 alphaToOne= 0;
    u32 multiViewport= 0;
    u32 samplerAnisotropy= 0;
    u32 textureCompressionETC2= 0;
    u32 textureCompressionASTC_LDR= 0;
    u32 textureCompressionBC= 0;
    u32 occlusionQueryPrecise= 0;
    u32 pipelineStatisticsQuery= 0;
    u32 vertexPipelineStoresAndAtomics= 0;
    u32 fragmentStoresAndAtomics= 0;
    u32 shaderTessellationAndGeometryPointSize= 0;
    u32 shaderImageGatherExtended= 0;
    u32 shaderStorageImageExtendedFormats= 0;
    u32 shaderStorageImageMultisample= 0;
    u32 shaderStorageImageReadWithoutFormat= 0;
    u32 shaderStorageImageWriteWithoutFormat= 0;
    u32 shaderUniformBufferArrayDynamicIndexing= 0;
    u32 shaderSampledImageArrayDynamicIndexing= 0;
    u32 shaderStorageBufferArrayDynamicIndexing= 0;
    u32 shaderStorageImageArrayDynamicIndexing= 0;
    u32 shaderClipDistance= 0;
    u32 shaderCullDistance= 0;
    u32 shaderFloat64= 0;
    u32 shaderInt64= 0;
    u32 shaderInt16= 0;
    u32 shaderResourceResidency= 0;
    u32 shaderResourceMinLod= 0;
    u32 sparseBinding= 0;
    u32 sparseResidencyBuffer= 0;
    u32 sparseResidencyImage2D= 0;
    u32 sparseResidencyImage3D= 0;
    u32 sparseResidency2Samples= 0;
    u32 sparseResidency4Samples= 0;
    u32 sparseResidency8Samples= 0;
    u32 sparseResidency16Samples= 0;
    u32 sparseResidencyAliased= 0;
    u32 variableMultisampleRate= 0;
    u32 inheritedQueries= 0;
  public:
    constexpr PhysicalDeviceFeatures() noexcept : OutputStruct((StructureType)1000059000){}
  };*/
  /*class PhysicalDeviceLimits{
    MaxU32 imageDimension1D;
    MaxU32 imageDimension2D;
    MaxU32 imageDimension3D;
    MaxU32 imageDimensionCube;
    MaxU32 imageArrayLayers;
    MaxU32 texelBufferElements;
    MaxU32 uniformBufferRange;
    MaxU32 storageBufferRange;
    MaxU32 pushConstantsSize;
    MaxU32 memoryAllocationCount;
    MaxU32 samplerAllocationCount;
    u64          bufferImageGranularity;
    u64          sparseAddressSpaceSize;
    MaxU32 boundDescriptorSets;
    MaxU32 perStageDescriptorSamplers;
    MaxU32 perStageDescriptorUniformBuffers;
    MaxU32 perStageDescriptorStorageBuffers;
    MaxU32 perStageDescriptorSampledImages;
    MaxU32 perStageDescriptorStorageImages;
    MaxU32 perStageDescriptorInputAttachments;
    MaxU32 perStageResources;
    MaxU32 descriptorSetSamplers;
    MaxU32 descriptorSetUniformBuffers;
    MaxU32 descriptorSetUniformBuffersDynamic;
    MaxU32 descriptorSetStorageBuffers;
    MaxU32 descriptorSetStorageBuffersDynamic;
    MaxU32 descriptorSetSampledImages;
    MaxU32 descriptorSetStorageImages;
    MaxU32 descriptorSetInputAttachments;
    MaxU32 vertexInputAttributes;
    MaxU32 vertexInputBindings;
    MaxU32 vertexInputAttributeOffset;
    MaxU32 vertexInputBindingStride;
    MaxU32 vertexOutputComponents;
    MaxU32 tessellationGenerationLevel;
    MaxU32 tessellationPatchSize;
    MaxU32 tessellationControlPerVertexInputComponents;
    MaxU32 tessellationControlPerVertexOutputComponents;
    MaxU32 tessellationControlPerPatchOutputComponents;
    MaxU32 tessellationControlTotalOutputComponents;
    MaxU32 tessellationEvaluationInputComponents;
    MaxU32 tessellationEvaluationOutputComponents;
    MaxU32 geometryShaderInvocations;
    MaxU32 geometryInputComponents;
    MaxU32 geometryOutputComponents;
    MaxU32 geometryOutputVertices;
    MaxU32 geometryTotalOutputComponents;
    MaxU32 fragmentInputComponents;
    MaxU32 fragmentOutputAttachments;
    MaxU32 fragmentDualSrcAttachments;
    MaxU32 fragmentCombinedOutputResources;
    MaxU32 computeSharedMemorySize;
    MaxU32 computeWorkGroupCount[3];
    MaxU32 computeWorkGroupInvocations;
    MaxU32 computeWorkGroupSize[3];
    MaxU32 subPixelPrecisionBits;
    MaxU32 subTexelPrecisionBits;
    MaxU32 mipmapPrecisionBits;
    MaxU32 drawIndexedIndexValue;
    MaxU32 drawIndirectCount;
    MaxF32 samplerLodBias;
    MaxF32 samplerAnisotropy;
    MaxU32 viewports;
    MaxU32 viewportDimensions[2];
    RangeF32 viewportBoundsRange;
    u32            viewportSubPixelBits;
    size_t       memoryMapAlignment;
    MinU64 texelBufferOffsetAlignment;
    MinU64 uniformBufferOffsetAlignment;
    MinU64 storageBufferOffsetAlignment;
    RangeI32 texelOffset;
    RangeI32 texelGatherOffset;
    RangeF32 interpolationOffset;
    u32            subPixelInterpolationOffsetBits;
    MaxU32 framebufferWidth;
    MaxU32 framebufferHeight;
    MaxU32 framebufferLayers;
    VkSampleCountFlags    framebufferColorSampleCounts;
    VkSampleCountFlags    framebufferDepthSampleCounts;
    VkSampleCountFlags    framebufferStencilSampleCounts;
    VkSampleCountFlags    framebufferNoAttachmentsSampleCounts;
    MaxU32 colorAttachments;
    VkSampleCountFlags    sampledImageColorSampleCounts;
    VkSampleCountFlags    sampledImageIntegerSampleCounts;
    VkSampleCountFlags    sampledImageDepthSampleCounts;
    VkSampleCountFlags    sampledImageStencilSampleCounts;
    VkSampleCountFlags    storageImageSampleCounts;
    MaxU32 sampleMaskWords;
    VkBool32              timestampComputeAndGraphics;
    float                 timestampPeriod;
    MaxU32 clipDistances;
    MaxU32 cullDistances;
    MaxU32 combinedClipAndCullDistances;
    u32              discreteQueuePriorities;
    RangeF32        pointSize;
    RangeF32        lineWidth;
    float                 pointSizeGranularity;
    float                 lineWidthGranularity;
    VkBool32              strictLines;
    VkBool32              standardSampleLocations;
    VkDeviceSize          optimalBufferCopyOffsetAlignment;
    VkDeviceSize          optimalBufferCopyRowPitchAlignment;
    VkDeviceSize          nonCoherentAtomSize;
  };*/
}

#endif//VALKYRIE_PHYSICAL_DEVICE_HPP
