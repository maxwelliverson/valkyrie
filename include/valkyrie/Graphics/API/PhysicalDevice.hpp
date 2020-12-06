//
// Created by Maxwell on 2020-11-09.
//

#ifndef VALKYRIE_PHYSICAL_DEVICE_HPP
#define VALKYRIE_PHYSICAL_DEVICE_HPP

#include <valkyrie/Graphics/API/Vulkan.hpp>
#include <valkyrie/Core/Utility/Interval.hpp>
#include <valkyrie/Graphics/API/Queue.hpp>

namespace valkyrie::Graphics::API{


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

  class PhysicalDevice : public VulkanObject{
    class Impl;
    Impl* pImpl;

  public:
    PhysicalDevice();
    PhysicalDevice(Instance instance, void* nativeHandle) noexcept;
    PhysicalDevice(const PhysicalDevice&);
    PhysicalDevice(PhysicalDevice&&) noexcept;
    PhysicalDevice& operator=(const PhysicalDevice&);
    PhysicalDevice& operator=(PhysicalDevice&&) noexcept;
    ~PhysicalDevice();


    Instance instance() const noexcept;
    Device createDevice();

    bool queueFamilySupportsPresentation(u32 familyIndex, const Surface& surface) const noexcept;

    Rectangle<> getPresentRectangle(const Surface& surface) const noexcept;
    //void getQueueFamilyProperties(u32& length, QueueFamily* families) noexcept;

    friend bool operator==(PhysicalDevice A, PhysicalDevice B) noexcept {
      return A.pImpl == B.pImpl;
    }
    friend auto operator<=>(PhysicalDevice A, PhysicalDevice B) noexcept {
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
    Core::MaxU32 imageDimension1D;
    Core::MaxU32 imageDimension2D;
    Core::MaxU32 imageDimension3D;
    Core::MaxU32 imageDimensionCube;
    Core::MaxU32 imageArrayLayers;
    Core::MaxU32 texelBufferElements;
    Core::MaxU32 uniformBufferRange;
    Core::MaxU32 storageBufferRange;
    Core::MaxU32 pushConstantsSize;
    Core::MaxU32 memoryAllocationCount;
    Core::MaxU32 samplerAllocationCount;
    u64          bufferImageGranularity;
    u64          sparseAddressSpaceSize;
    Core::MaxU32 boundDescriptorSets;
    Core::MaxU32 perStageDescriptorSamplers;
    Core::MaxU32 perStageDescriptorUniformBuffers;
    Core::MaxU32 perStageDescriptorStorageBuffers;
    Core::MaxU32 perStageDescriptorSampledImages;
    Core::MaxU32 perStageDescriptorStorageImages;
    Core::MaxU32 perStageDescriptorInputAttachments;
    Core::MaxU32 perStageResources;
    Core::MaxU32 descriptorSetSamplers;
    Core::MaxU32 descriptorSetUniformBuffers;
    Core::MaxU32 descriptorSetUniformBuffersDynamic;
    Core::MaxU32 descriptorSetStorageBuffers;
    Core::MaxU32 descriptorSetStorageBuffersDynamic;
    Core::MaxU32 descriptorSetSampledImages;
    Core::MaxU32 descriptorSetStorageImages;
    Core::MaxU32 descriptorSetInputAttachments;
    Core::MaxU32 vertexInputAttributes;
    Core::MaxU32 vertexInputBindings;
    Core::MaxU32 vertexInputAttributeOffset;
    Core::MaxU32 vertexInputBindingStride;
    Core::MaxU32 vertexOutputComponents;
    Core::MaxU32 tessellationGenerationLevel;
    Core::MaxU32 tessellationPatchSize;
    Core::MaxU32 tessellationControlPerVertexInputComponents;
    Core::MaxU32 tessellationControlPerVertexOutputComponents;
    Core::MaxU32 tessellationControlPerPatchOutputComponents;
    Core::MaxU32 tessellationControlTotalOutputComponents;
    Core::MaxU32 tessellationEvaluationInputComponents;
    Core::MaxU32 tessellationEvaluationOutputComponents;
    Core::MaxU32 geometryShaderInvocations;
    Core::MaxU32 geometryInputComponents;
    Core::MaxU32 geometryOutputComponents;
    Core::MaxU32 geometryOutputVertices;
    Core::MaxU32 geometryTotalOutputComponents;
    Core::MaxU32 fragmentInputComponents;
    Core::MaxU32 fragmentOutputAttachments;
    Core::MaxU32 fragmentDualSrcAttachments;
    Core::MaxU32 fragmentCombinedOutputResources;
    Core::MaxU32 computeSharedMemorySize;
    Core::MaxU32 computeWorkGroupCount[3];
    Core::MaxU32 computeWorkGroupInvocations;
    Core::MaxU32 computeWorkGroupSize[3];
    Core::MaxU32 subPixelPrecisionBits;
    Core::MaxU32 subTexelPrecisionBits;
    Core::MaxU32 mipmapPrecisionBits;
    Core::MaxU32 drawIndexedIndexValue;
    Core::MaxU32 drawIndirectCount;
    Core::MaxF32 samplerLodBias;
    Core::MaxF32 samplerAnisotropy;
    Core::MaxU32 viewports;
    Core::MaxU32 viewportDimensions[2];
    Core::RangeF32 viewportBoundsRange;
    u32            viewportSubPixelBits;
    size_t       memoryMapAlignment;
    Core::MinU64 texelBufferOffsetAlignment;
    Core::MinU64 uniformBufferOffsetAlignment;
    Core::MinU64 storageBufferOffsetAlignment;
    Core::RangeI32 texelOffset;
    Core::RangeI32 texelGatherOffset;
    Core::RangeF32 interpolationOffset;
    u32            subPixelInterpolationOffsetBits;
    Core::MaxU32 framebufferWidth;
    Core::MaxU32 framebufferHeight;
    Core::MaxU32 framebufferLayers;
    VkSampleCountFlags    framebufferColorSampleCounts;
    VkSampleCountFlags    framebufferDepthSampleCounts;
    VkSampleCountFlags    framebufferStencilSampleCounts;
    VkSampleCountFlags    framebufferNoAttachmentsSampleCounts;
    Core::MaxU32 colorAttachments;
    VkSampleCountFlags    sampledImageColorSampleCounts;
    VkSampleCountFlags    sampledImageIntegerSampleCounts;
    VkSampleCountFlags    sampledImageDepthSampleCounts;
    VkSampleCountFlags    sampledImageStencilSampleCounts;
    VkSampleCountFlags    storageImageSampleCounts;
    Core::MaxU32 sampleMaskWords;
    VkBool32              timestampComputeAndGraphics;
    float                 timestampPeriod;
    Core::MaxU32 clipDistances;
    Core::MaxU32 cullDistances;
    Core::MaxU32 combinedClipAndCullDistances;
    u32              discreteQueuePriorities;
    Core::RangeF32        pointSize;
    Core::RangeF32        lineWidth;
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
