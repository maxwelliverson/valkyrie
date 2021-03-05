//
// Created by Maxwell on 2020-11-04.
//

#include "vk_object.h"

#define VK_NO_PROTOTYPES
#define VK_ENABLE_BETA_EXTENSIONS
#define VK_USE_PLATFORM_WIN32_KHR

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
//#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan.h>
#include <algorithm>
#include <array>
#include <vector>
#include <memory_resource>
#include <unordered_map>
#include <compare>
#include <sstream>
#include <boost/container/static_vector.hpp>
#include <boost/static_string.hpp>

#include "vk_features.h"

using HINSTANCE = struct HINSTANCE__*;
using FARPROC = int64_t(__stdcall*)();
extern "C" __declspec( dllimport ) HINSTANCE __stdcall LoadLibraryA( char const * lpLibFileName );
extern "C" __declspec( dllimport ) int __stdcall FreeLibrary( HINSTANCE hLibModule );
extern "C" __declspec( dllimport ) FARPROC __stdcall GetProcAddress( HINSTANCE hModule, const char * lpProcName );


namespace Vk{
  using SampleCountFlags = BitFlags<VkSampleCountFlagBits>;
  using SubgroupFeatureFlags = BitFlags<VkSubgroupFeatureFlagBits>;
  using ShaderStageFlags = BitFlags<VkShaderStageFlagBits>;
  using ShaderModuleCreateFlags = BitFlags<VkShaderModuleCreateFlagBits>;
  using AccessFlags = BitFlags<VkAccessFlagBits>;
  using DependencyFlags = BitFlags<VkDependencyFlagBits>;
  using BufferUsageFlags = BitFlags<VkBufferUsageFlagBits>;
  using BufferCreateFlags = BitFlags<VkBufferCreateFlagBits>;
  using QueueFlags = BitFlags<VkQueueFlagBits>;
  using ColorComponentFlags = BitFlags<VkColorComponentFlagBits>;
  using ExternalMemoryHandleTypeFlags = BitFlags<VkExternalMemoryHandleTypeFlagBits>;
  using ExternalSemaphoreHandleTypeFlags = BitFlags<VkExternalSemaphoreFeatureFlagBits>;
  using ExternalFenceHandleTypeFlags = BitFlags<VkExternalFenceHandleTypeFlagBits>;
  using ResolveModeFlags = BitFlags<VkResolveModeFlagBits>;
  using MemoryAllocateFlags = BitFlags<VkMemoryAllocateFlagBits>;
  using MemoryHeapFlags = BitFlags<VkMemoryHeapFlagBits>;
  using MemoryPropertyFlags = BitFlags<VkMemoryPropertyFlagBits>;

  using SurfaceTransformFlags = BitFlags<VkSurfaceTransformFlagBitsKHR>;
  using CompositeAlphaFlags = BitFlags<VkCompositeAlphaFlagBitsKHR>;
  using ImageUsageFlags = BitFlags<VkImageUsageFlagBits>;
  using SurfaceCounterFlags = BitFlags<VkSurfaceCounterFlagBitsEXT>;

  struct MemoryHeap{
    uint64_t size;
    uint64_t budget;
    uint64_t usage;
    MemoryHeapFlags flags;
    boost::container::static_vector<MemoryPropertyFlags, 4> availableTypes;

    explicit MemoryHeap(VkMemoryHeap heap) noexcept : size(heap.size), flags(heap.flags){}
  };
}


#define VK_type_bool      VK_typedef(bool)
#define VK_type_i64       VK_typedef(int64_t)
#define VK_type_i32       VK_typedef(int32_t)
#define VK_type_u64       VK_typedef(uint64_t)
#define VK_type_u32       VK_typedef(uint32_t)
#define VK_type_f32       VK_typedef(float)
#define VK_type_f64       VK_typedef(double)
#define VK_type_size_t    VK_typedef(size_t)
#define VK_type_u32_max   VK_typedef(::Vk::MaxU32)
#define VK_type_u32_min   VK_typedef(::Vk::MinU32)
#define VK_type_u64_max   VK_typedef(::Vk::MaxU64)
#define VK_type_u64_min   VK_typedef(::Vk::MinU64)
#define VK_type_f32_range VK_typedef(::Vk::RangeF32)
#define VK_type_i32_range VK_typedef(::Vk::RangeI32)
#define VK_type_string    VK_typedef(boost::static_strings::static_string<256>)
#define VK_type_uuid_t    VK_typedef(std::array<uint8_t, VK_UUID_SIZE>)
#define VK_type_luid_t    VK_typedef(std::array<uint8_t, VK_LUID_SIZE>)
#define VK_type_array(...) VK_typedef(std::array<__VA_ARGS__>)
#define VK_type_vector(...) VK_typedef(std::vector<__VA_ARGS__>)
#define VK_type_static_vector(...) VK_typedef(boost::container::static_vector<__VA_ARGS__>)
#define VK_type_version VK_typedef(Vk::Version)

#define VK_type_bitflags(enumType) VK_typedef(::Vk::BitFlags<Vk##enumType##FlagBits>)

namespace {

  inline constexpr VkAllocationCallbacks allocationCallbacks{
      .pUserData = nullptr,
      .pfnAllocation = [](void* pUserData, size_t size, size_t alignment, VkSystemAllocationScope scope) { return _aligned_malloc(size, alignment); },
      .pfnReallocation = [](void* pUserData, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope scope) { return _aligned_realloc(pOriginal, size, alignment); },
      .pfnFree = [](void* pUserData, void* pMemory) { _aligned_free(pMemory); },
      .pfnInternalAllocation = [](void* pUserData, size_t bytes, VkInternalAllocationType type, VkSystemAllocationScope scope) {},
      .pfnInternalFree = [](void* pUserData, size_t bytes, VkInternalAllocationType type, VkSystemAllocationScope scope) {}
  };

  using Vk::MaxU32;
  using Vk::MinU32;
  using Vk::MaxU64;
  using Vk::MinU64;
  using Vk::Interval;
  using Vk::Mixin;
  using Vk::LowerBound;
  using Vk::UpperBound;
  using Vk::LimitMax;
  using Vk::LimitMin;
  using Vk::LimitRange;
  using Vk::RangeF32;
  using Vk::RangeI32;
  using Vk::RangeU32;
  using Vk::RangeU64;

  struct CurrentMixin{
    uint32_t current;
  };
  struct OptimalMixin{
    uint32_t optimal;
  };

  using MaxU32WithCurrent = Interval<uint32_t, LowerBound<0u>, Mixin<CurrentMixin>>;
  using MinU32WithOptimal = Interval<uint32_t, UpperBound<std::numeric_limits<uint32_t>::max()>, Mixin<OptimalMixin>>;

  using Vk::MaxU32WithUAB;

  using static_string = boost::static_strings::static_string<256>;
  using uuid_t = std::array<uint8_t, VK_UUID_SIZE>;
  using luid_t = std::array<uint8_t, VK_LUID_SIZE>;



  class Library{
  public:
    Library(){
      library_ = LoadLibraryA("vulkan-1.dll");
    }
    ~Library(){
      FreeLibrary(library_);
    }

    template <typename T>
    T getProcAddress( const char* function ) const noexcept {
      return (T)::GetProcAddress(library_, function );
    }


  private:
    HINSTANCE library_;
  };

  const static Library lib{};

  struct InstanceImpl : Vk::Instance{
    VkInstance nativeHandle_ = VK_NULL_HANDLE;

    mutable std::atomic_uint32_t refCount;

    struct {
      PFN_vkGetInstanceProcAddr getFunctionAddr;
      PFN_vkCreateInstance create;
      PFN_vkDestroyInstance destroy;
      PFN_vkEnumeratePhysicalDevices getPhysicalDevices;
      PFN_vkEnumerateInstanceVersion getVersion;
      PFN_vkEnumerateInstanceExtensionProperties getExtensions;
      PFN_vkEnumerateInstanceLayerProperties getLayers;
    } api;

    Vk::Version version;
    std::vector<VkPhysicalDevice> physicalDevices;
    std::vector<VkExtensionProperties> extensions;
    std::vector<VkLayerProperties> layers;

    Vk::FlatSet<Vk::Handle<Vk::InstanceObject>> children;
    std::string name;
    std::vector<std::byte> taggedData;

    InstanceImpl(){

      api.getFunctionAddr = lib.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
#define getFunctionAddress(Func) (PFN_##Func)(api.getFunctionAddr(nativeHandle_, #Func))
      api.getVersion   = getFunctionAddress(vkEnumerateInstanceVersion);
      api.getExtensions = getFunctionAddress(vkEnumerateInstanceExtensionProperties);
      api.getLayers = getFunctionAddress(vkEnumerateInstanceLayerProperties);
      api.create    = getFunctionAddress(vkCreateInstance);

      uint32_t extensionSize;
      auto extResult = api.getExtensions(nullptr, &extensionSize, nullptr);
      extensions.resize(extensionSize);
      extResult = api.getExtensions(nullptr, &extensionSize, extensions.data());
      uint32_t layersSize;
      auto layerResult = api.getLayers(&layersSize, nullptr);
      layers.resize(layersSize);
      layerResult = api.getLayers(&layersSize, layers.data());
      uint32_t version_;
      auto versionResult = api.getVersion(&version_);
      version = Vk::Version(version_);
    }

    void init() noexcept{
      api.getPhysicalDevices = getFunctionAddress(vkEnumeratePhysicalDevices);
      api.destroy            = getFunctionAddress(vkDestroyInstance);
#undef getFunctionAddress
    }

    void destroy() final {
      api.destroy(nativeHandle_, &allocationCallbacks);
      delete this;
    }
    [[nodiscard]] uint64_t nativeHandle() const noexcept final{
      return (uint64_t)nativeHandle_;
    }
  };
  struct PhysicalDeviceImpl : public Vk::PhysicalDevice{
    VkPhysicalDevice nativeHandle_;

    mutable std::atomic_uint32_t refCount;

    struct {
      PFN_vkEnumerateDeviceExtensionProperties getExtensions;
      PFN_vkGetPhysicalDeviceFeatures2 getFeatures;
      PFN_vkGetPhysicalDeviceProperties2 getProperties;
      PFN_vkGetPhysicalDeviceToolPropertiesEXT getToolProperties;
      PFN_vkGetPhysicalDeviceCooperativeMatrixPropertiesNV getCooperativeMatrixProperties;
      PFN_vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV getSupportedFramebufferMixedSamplesCombinations;

      struct {
        PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR getCapabilities;
        PFN_vkGetPhysicalDeviceSurfaceFormats2KHR getFormats;
        PFN_vkGetPhysicalDeviceSurfacePresentModes2EXT getPresentModes;
        PFN_vkGetPhysicalDeviceSurfaceSupportKHR getSupport;
        PFN_vkGetPhysicalDevicePresentRectanglesKHR getPresentRectangles;
      } surface;
      struct {
        PFN_vkGetPhysicalDeviceDisplayPropertiesKHR getProperties;
        PFN_vkReleaseDisplayEXT release;
        struct {
          PFN_vkGetPhysicalDeviceDisplayPlaneProperties2KHR getProperties;
          PFN_vkGetDisplayPlaneSupportedDisplaysKHR getSupportedDisplays;
          PFN_vkGetDisplayPlaneCapabilitiesKHR getCapabilities;
        } plane;
        struct {
          PFN_vkGetDisplayModePropertiesKHR getProperties;
          PFN_vkCreateDisplayModeKHR create;
        } mode;
      } display;
      struct {
        PFN_vkGetPhysicalDeviceQueueFamilyProperties2KHR getProperties;
        PFN_vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR enumeratePerformanceQueryCounters;
        PFN_vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR getPerformanceQueryPasses;
      } queueFamily;
      struct {
        PFN_vkGetPhysicalDeviceFormatProperties2 getProperties;
        struct {
          PFN_vkGetPhysicalDeviceImageFormatProperties2 getProperties;
          struct {
            PFN_vkGetPhysicalDeviceExternalImageFormatPropertiesNV getProperties;
          } external;
        } image;
      } format;
      struct {
        PFN_vkGetPhysicalDeviceMultisamplePropertiesEXT getProperties;
      } multisample;
      struct {
        PFN_vkGetPhysicalDeviceMemoryProperties2 getProperties;
        struct {
          PFN_vkGetPhysicalDeviceExternalBufferProperties getBufferProperties;
          PFN_vkGetPhysicalDeviceExternalFenceProperties getFenceProperties;
          PFN_vkGetPhysicalDeviceExternalSemaphoreProperties getSemaphoreProperties;
        } external;
      } memory;

      PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT getCalibrateableTimeDomains;
      PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR getPresentationSupport;
    } api;

    VK_property_group(
      driver,
        type(VkDriverId) id,
        group(
          u32 id
        ) vendor,
        version version,
        string name,
        string info,
        uuid_t uuid
    );

    /*struct {
      VkDriverId id;
      struct {
        uint32_t id;
      } vendor;
      Vk::Version version;
      static_string name;
      static_string info;
      uuid_t uuid;
    } driver;*/

    VK_property_group(
      device,
        u32 id,
        group(
            version api,
            type(VkConformanceVersion) conformance
        ) version,
        group(
          u32 domain,
          u32 bus,
          u32 device,
          u32 function
        ) pci,
        feature(
            group(
                u32_min bufferOffsetAlignment,
                group(
                  u32_max count,
                  u32_max stride
                ) stream,
                group(
                  u32_max count,
                  u32_max offset
                ) token
            ) indirect,
            group(
                group(
                    u32_min bufferOffsetAlignment
                ) count,
                group(
                    u32_min bufferOffsetAlignment
                ) index
            ) sequence
        ) generatedCommands,
        string name,
        type(VkPhysicalDeviceType) type,
        uuid_t uuid,
        uuid_t pipelineCacheUuid,
        luid_t luid,
        u32 coreCount,
        u32 subgroupsPerCore,
        u32 nodeMask,
        bool luidValid,
        bool clock
    );


    /*struct Device {
      uint32_t id;
      struct {
        Vk::Version api;
        VkConformanceVersion conformance;
      } version;
      struct {
        uint32_t domain;
        uint32_t bus;
        uint32_t device;
        uint32_t function;
      } pci;
      static_string name;
      VkPhysicalDeviceType type;
      uuid_t uuid;
      uuid_t pipelineCacheUuid;
      luid_t luid;
      uint32_t nodeMask;
      bool luidValid;

      *//*VK_define_feature(generatedCommands)(
        VK_property_group(sequences)(
          VK_property_group(count)(
            VK_property(u32_min bufferOffsetAlignment);
            VK_property_list(bufferOffsetAlignment);
          );
          VK_property_group(index)(
            VK_property(u32_min bufferOffsetAlignment);
            VK_property_list(bufferOffsetAlignment);
          );
          VK_property_list(count, index);
        );
        VK_property_group(indirect)(
          VK_property_group(stream)(
            VK_property(u32_max count);
            VK_property(u32_max stride);
            VK_property_list(count, stride);
          );
          VK_property_group(token)(
            VK_property(u32_max offset);
            VK_property(u32_max stride);
            VK_property_list(offset, stride);
          );
          VK_property(u32_min bufferOffsetAlignment);
          VK_property_list(stream, token, bufferOffsetAlignment);
        );
      );*//*
      VK_feature(
        generatedCommands,
          property group(
            type(MinU32) bufferCountOffsetAlignment,
            type(MinU32) bufferIndexOffsetAlignment
          ) sequences,
          property group(
            type(MinU32) bufferOffsetAlignment
          ) indirect
      );
      struct {
        struct {
          MaxU32 count;
          MaxU32 stride;
        } stream;
        struct {
          MaxU32 count;
          MaxU32 stride;
        } token;
        MinU32 bufferOffsetAlignment;
      } indirect;
      struct {
        struct {
          MinU32 bufferOffsetAlignment;
        } count, index;
      } sequences;
    } device;*/


    struct {
      struct {
        struct {
          Vk::ResolveModeFlags depth;
          Vk::ResolveModeFlags stencil;
        } modes;
        bool independentNone;
        bool independent;
      } resolve;
      struct {
        struct {
          VkShaderFloatControlsIndependence independence;
          struct {
            bool f16;
            bool f32;
            bool f64;
          } preserve, flushToZero;
        } denorm;
        struct {
          VkShaderFloatControlsIndependence independence;
          struct {
            bool f16;
            bool f32;
            bool f64;
          } toEven, toZero;
        } roundingMode;
        struct {
          struct {
            bool f16;
            bool f32;
            bool f64;
          } preserve;
        } signedZeroInfNaN;
      } fenv;
      struct {
        MaxU32 size;
        struct {
          bool b8;
          bool b16;
        } storage;
      } pushConstants;
      struct {
        /*uint64_t matrix : 1;
        uint64_t shader : 1;
        uint64_t inputAttachment : 1;
        uint64_t sampled1D : 1;
        uint64_t image1D : 1;
        uint64_t sampledBuffer : 1;
        uint64_t imageBuffer : 1;
        uint64_t imageQuery : 1;
        uint64_t derivativeControl : 1;
        uint64_t geometry : 1;
        uint64_t tessellation : 1;
        uint64_t float64 : 1;
        uint64_t int64 : 1;
        uint64_t int64Atomics : 1;
        uint64_t int16 : 1;
        uint64_t tessellationPointSize : 1;
        uint64_t geometryPointSize : 1;
        uint64_t imageGatherExtended : 1;
        uint64_t storageImageMultisample : 1;
        uint64_t uniformBufferArrayDynamicIndexing : 1;
        uint64_t sampledImageArrayDynamicIndexing : 1;
        uint64_t storageBufferArrayDynamicIndexing : 1;
        uint64_t storageImageArrayDynamicIndexing : 1;
        uint64_t clipDistance : 1;
        uint64_t cullDistance : 1;
        uint64_t imageCubeArray : 1;
        uint64_t sampleRateShading : 1;
        uint64_t sparseResidency : 1;
        uint64_t minLod : 1;
        uint64_t sampledCubeArray : 1;
        uint64_t imageMultiSampleArray : 1;*/
        uint64_t transformFeedback : 1;
        uint64_t geometryStreams : 1;
      } spirv;
    } capabilities;

    struct Resource {
      struct {
        bool indexing;
        bool runtimeArray;
        MaxU32 perSet;
        MaxU32 updateAfterBind;
        struct {
          MaxU32WithUAB sets;
          bool updateUnusedWhilePending;
          bool partiallyBound;
          bool variableDescriptorCount;
        } binding;
      } descriptor;
      struct Sampler {
        VK_feature(
          filterMinMax,
            property bool singleComponentFormats,
            property bool imageComponentMapping
        );

        VK_feature(
          anisotropy,
            property type(float) max
        );

        bool mirrorClampToEdge;
        bool ycbcrConversion;

        /*VK_feature(mirrorClampToEdge);
        VK_feature(ycbcrConversion);*/

        Interval<float, LowerBound<0>> lodBias;
        MaxU32 allocationCount;
        MaxU32 customBorderColor;
        struct {
          MaxU32WithUAB perStage;
          MaxU32WithUAB sets;
        } descriptor;
      } sampler;
      struct {
        MaxU32 size;
        struct {
          MaxU32WithUAB perStage;
          MaxU32WithUAB set;
        } descriptor;
      } inlineUniformBlock;
      struct {
        MinU64 offsetAlignment;
        uint64_t robustAccessAlignment;
        MaxU32 range;
        struct {
          bool b8;
          bool b16;
        } access;
        struct {
          bool dynamic;
          bool nonUniform;
          bool nativeNonUniform;
        } arrayIndexing;
        struct {
          MaxU32WithUAB perStage;
          MaxU32WithUAB sets;
          bool bindingUpdateAfterBind;
        } descriptor;
        struct {
          struct {
            bool dynamic;
            bool nonUniform;
          } arrayIndexing;
          struct {
            bool bindingUpdateAfterBind;
          } descriptor;
          struct : MinU32 {
            VkDeviceSize bytes;
            bool singleTexel;
          } offsetAlignment;
          MaxU32 elementCount;
        } texel;
        struct {
          struct {
            MaxU32WithUAB sets;
          } descriptor;
        } dynamic;
      } storageBuffer;
      struct {
        bool standardLayout;
        MinU64 offsetAlignment;
        uint64_t robustAccessAlignment;
        MaxU32 range;
        struct {
          bool b8;
          bool b16;
        } access;
        struct {
          bool dynamic;
          bool nonUniform;
          bool nativeNonUniform;
        } arrayIndexing;
        struct {
          MaxU32WithUAB perStage;
          MaxU32WithUAB sets;
          bool bindingUpdateAfterBind;
        } descriptor;
        struct {
          struct {
            bool dynamic;
            bool nonUniform;
          } arrayIndexing;
          struct {
            bool bindingUpdateAfterBind;
          } descriptor;
          struct : MinU32 {
            VkDeviceSize bytes;
            bool singleTexel;
          } offsetAlignment;
          MaxU32 elementCount;
        } texel;
        struct {
          struct {
            MaxU32WithUAB sets;
          } descriptor;
        } dynamic;
      } uniformBuffer;
      struct {
        struct {
          Vk::SampleCountFlags color;
          Vk::SampleCountFlags integer;
          Vk::SampleCountFlags depth;
          Vk::SampleCountFlags stencil;
        } sampleCounts;
        struct {
          MaxU32WithUAB perStage;
          MaxU32WithUAB sets;
          bool bindingUpdateAfterBind;
        } descriptor;
        struct {
          bool dynamic;
          bool nonUniform;
          bool nativeNonUniform;
        } arrayIndexing;
      } sampledImage;
      struct {
        struct {
          MaxU32WithUAB perStage;
          MaxU32WithUAB sets;
          bool bindingUpdateAfterBind;
        } descriptor;
        struct {
          bool dynamic;
          bool nonUniform;
          bool nativeNonUniform;
        } arrayIndexing;
        Vk::SampleCountFlags sampleCounts;
        bool extendedFormats;
        bool multisample;
        bool readWithoutFormat;
        bool writeWithoutFormat;
      } storageImage;
      struct {
        struct {
          MaxU32WithUAB sets;
          MaxU32WithUAB perStage;
        } descriptor;
        struct {
          bool dynamic;
          bool nonUniform;
          bool nativeNonUniform;
        } arrayIndexing;
        struct {
          bool b8;
          bool b16;
        } access;
      } input;
      struct {
        struct {
          bool b8;
          bool b16;
        } access;
        bool layer;
        bool viewportIndex;
      } output;
      MaxU32WithUAB perStage;
      bool residency;
      bool minLod;
      bool scalarBlockLayout;
    } resource;
    VK_property_group(
      memory,
        //static_vector(Vk::MemoryHeap, VK_MAX_MEMORY_HEAPS) heaps,
        vector(Vk::MemoryHeap) heaps,
        group(
          u64_max size,
          u32_max count,
          bool dedicatedImageAliasing
        ) allocation,
        group(
          size_t addressSpaceSize,
          bool binding,
          group(
            bool aliased,
            bool buffer,
            bool image2D,
            bool image3D,
            bool alignedMipSize,
            bool nonResidentStrict,
            group(
              bool standard3D,
              feature(
                bool multisample
              ) standard2D
            ) blockShape,
            group(
              bool x2,
              bool x4,
              bool x8,
              bool x16
            ) samples
          ) residency
        ) sparse,
        group(
          u64_min alignment
        ) map,
        group(
          group(
            size_t optimalOffsetAlignment,
            size_t optimalRowPitchAlignment
          ) copy,
          bool indexU8,
          group(
            bool i64,
            bool f32,
            bool f64,
            bool addF32,
            bool addF64
          ) atomic,
          feature(
            bool captureReplay,
            bool multiDevice
          ) deviceAddress,
          feature(
            bool updateAfterBind,
            bool cooperativeMatrix
          ) robustAccess
        ) buffer,
        group(
          bool separateDepthStencilLayouts,
          bool gatherExtended,
          bool cubeArray,
          group(
            bool f32,
            bool addF32
          ) atomic,
          group(
            u32_max x1D,
            u32_max x2D,
            u32_max x3D,
            u32_max cube
          ) dimension,
          u32_max arrayLayers
        ) image,
        group(
          group(
            bool i64,
            bool f32,
            bool f64,
            bool addF32,
            bool addF64
          ) atomic
        ) shared,
        size_t granularity,
        u64_min importedHostPointerAlignment,
        size_t nonCoherentAtomSize,
        feature(
          bool noFault
        ) protection,
        feature(
          bool deviceScope,
          bool availabilityVisibilityChains
        ) model,
        bool priority
    );
    /*struct Memory {
      boost::container::static_vector<Vk::MemoryHeap, VK_MAX_MEMORY_HEAPS> heaps;
      struct {
        MaxU64 size;
        MaxU32 count;
        bool dedicatedImageAliasing;
      } allocation;
      struct {
        struct {
          struct {
            struct {
              bool enabled;
              bool multisample;

              inline constexpr auto& operator=(bool B) noexcept {
                enabled = B;
                return *this;
              }
              inline constexpr explicit operator bool() const noexcept {
                return enabled;
              }
            } standard2D;
            bool standard3D;
          } blockShape;
          struct {
            bool x2;
            bool x4;
            bool x8;
            bool x16;
          } samples;
          bool aliased;
          bool buffer;
          bool image2D;
          bool image3D;
          bool alignedMipSize;
          bool nonResidentStrict;
        } residency;
        bool binding;
        VkDeviceSize addressSpaceSize;
      } sparse;
      struct {
        MinU64 alignment;
      } map;
      struct Buffer {
        struct {
          struct{
            VkDeviceSize optimalAlignment;
          } offset, rowPitch;
        } copy;
        struct {
          bool i64;
          bool f32;
          bool f64;
          bool addF32;
          bool addF64;
        } atomic;
        VK_feature(
          robustAccess,
            property bool updateAfterBind,
            property bool cooperativeMatrix
        );
        VK_feature(
          deviceAddress,
            property bool captureReplay,
            property bool multiDevice
        );
      } buffer;
      struct {
        bool separateDepthStencilLayouts;
        bool gatherExtended;
        bool cubeArray;
        MaxU32 arrayLayers;
        struct {
          MaxU32 x1D;
          MaxU32 x2D;
          MaxU32 x3D;
          MaxU32 cube;
        } dimension;
        struct {
          bool f32;
          bool f64;
          bool addF32;
          bool addF64;
        } atomic;
      } image;
      struct {
        struct {
          bool i64;
          bool f32;
          bool f64;
          bool addF32;
          bool addF64;
        } atomic;
      } shared;
      VkDeviceSize granularity;
      MinU64 importedHostPointerAlignment;
      VkDeviceSize nonCoherentAtomSize;
      VK_feature(
        protection,
          property bool noFault
      );
      VK_feature(
        model,
          property bool deviceScope,
          property bool availabilityVisibilityChains
      );
      bool priority;
    } memory;*/
    struct Render {
      struct {
        struct {
          struct : MaxU32{
            bool enabled;
          } count;
          bool multi;
          bool firstInstance;
        } indirect;
        struct {
          MaxU32 value;
          bool   fullU32;
        } index;
        bool parameters;
      } draw;
      struct {
        bool standardLocations;
        bool variableLocations;
        Vk::SampleCountFlags sampleCounts;
        struct {
          MaxU64 width, height;
        } gridSize;
        RangeF32 coordinateRange;
        uint32_t subPixelBits;
      } sampleLocation;
      struct {
        bool independent;
        bool dualSource;
        bool logicOps;
        struct {
          bool independent;
          bool correlatedOverlap;
          bool allOperations;
          struct {
            bool sourceColor;
            bool destinationColor;
          } nonPremultiplied;
          MaxU32 colorAttachments;
        } advanced;
        MaxU32 colorAttachments;
      } blend;
      struct {
        Vk::Extent2D texelSize;
        uint32_t paletteSize;
        MaxU32 coarseSamples;
      } shadingRate;
      VkPointClippingBehavior pointClippingBehavior;
      MaxU32 sampleMaskWords;
      struct Rasterization{
        struct {
          bool rectangular;
          bool bresenham;
          bool smooth;
          bool stippledRectangular;
          bool stippledBresenham;
          bool stippledSmooth;
          bool strict;
        } line;
      } rasterize;
      bool sampleRateShading;
      bool fillModeNonSolid;
      bool alphaToOne;
      bool variableMultisampleRate;
      struct {
        bool executableInfo;
        bool creationCacheControl;
        bool extendedDynamicState;
      } pipeline;
    } render;



    struct {
      struct {
        struct : RangeU32{
          uint32_t current;
          bool control;

          auto& operator=(uint32_t value_) noexcept { current = value_; return *this; }
          operator uint32_t& () noexcept {
            return current;
          }
          operator const uint32_t& () const noexcept {
            return current;
          }
        } size;
        Vk::ShaderStageFlags     supportedStages;
        Vk::SubgroupFeatureFlags supportedOperations;
        Vk::ShaderStageFlags requiredSizeStages;
        bool quadOperationsInAllStages;
        bool broadcastDynamicId;
        bool extendedTypes;
        bool clock;
      } subgroup;
      struct {
        bool enabled;
        bool geometryShader;
        bool tessellationShader;
        MaxU32 count;
        MaxU32 instanceIndex;
      } multiview;

      struct : MaxU32 {
        RangeF32 boundsRange;
        uint32_t subPixelBits;
        struct {
          MaxU32 x, y;
        } dimensions;
        bool multi;
      } viewport;
      struct {
        struct {
          Vk::SampleCountFlags color;
          Vk::SampleCountFlags integerColor;
          Vk::SampleCountFlags depth;
          Vk::SampleCountFlags stencil;
          Vk::SampleCountFlags noAttachments;
        } sampleCounts;
        MaxU32 width;
        MaxU32 height;
        MaxU32 layers;
        bool imageless;
      } framebuffer;

      struct {
        bool computeAndGraphics;
        float period;
      } timestamp;
      struct {
        RangeF32 offset;
        uint32_t subpixelOffsetBits;
      } interpolation;
      struct {
        uint32_t lineSubPixel;
        uint32_t subPixel;
        uint32_t subTexel;
        uint32_t mipmap;
      } precisionBits;

      struct {
        bool inherited;
        bool hostReset;
        bool preciseOcclusion;
        bool pipelineStatistics;
      } query;

      struct {
        bool queries;
        bool draw;
        struct {
          bool rasterizationSelect;
          bool linesTriangles;
          MaxU32 count;
          struct {
            MaxU32 size;
          } data;
        } stream;
        struct {
          MaxU32 count;
          MaxU64 size;
          struct {
            MaxU32 size;
            MaxU32 stride;
          } data;
        } buffer;
      } transformFeedback;

      struct {
        RangeI32 offset;
        RangeI32 gatherOffset;
      } texel;




      uint32_t discreteQueuePriorities;

      VkBool32 quadDivergentImplicitLod;


      std::pmr::vector<VkQueueFamilyProperties2> queueFamilies;
      std::pmr::unordered_map<VkFormat, VkFormatProperties2> format;
    } properties;

    VK_property_group(
      features,
        feature(
          bool storageBuffer
        ) variablePointers,
        feature(
          bool geometryShader,
          bool tessellationShader
        ) multiview,
        feature(
          f32_range width,
          f32 granularity
        ) wideLines,
        feature(
          f32_range size,
          f32 granularity
        ) largePoints,
        feature(
          u64_max valueDifference
        ) timelineSemaphore,
        group(
          feature(
            bool multiple
          ) queryPools
        ) performanceCounter,
        group(
          bool ETC2,
          bool BC,
          group(
            bool LDR,
            bool HDR
          ) ASTC
        ) textureCompression,
        feature(
            bool robustBufferAccess,
            bitflags(ShaderStage) supportedStages
        ) cooperativeMatrix,
        group(
          feature(
            bool bias
          ) clamp,
          bool bounds
        ) depth,
        group(
          type(Interval<Vk::Extent2D>) texelSize,
          group(
            u32_max descriptorSetSamplers,
            u32_max arrayLayers,
            bool loads,
            bool coarseReconstructionEarlyAccess
          ) subsampled,
          feature(
            bool dynamic,
            bool deferred,
            bool nonSubsampledImages
          ) map,
          bool invocations
        ) fragmentDensity,
        group(
          group(
            bool storesAndAtomics,
            group(
              u32_max attributes,
              u32_max bindings,
              u32_max attributeOffset,
              u32_max bindingStride
            ) input,
            group(
              u32_max components
            ) output
          ) vertex,
          feature(
            bool storesAndAtomics,
            bool pointSize,
            u32_max generationLevel,
            u32_max patchSize,
            group(
              group(
                u32_max componentsPerVertex
              ) input,
              group(
                u32_max componentsPerVertex,
                u32_max componentsPerPatch,
                u32_max totalComponents
              ) output
            ) control,
            group(
              group(
                u32_max components
              ) input,
              group(
                u32_max components
              ) output
            ) evaluation
          ) tessellation,
          feature(
            bool storesAndAtomics,
            bool pointSize,
            u32_max invocations,
            group(
              u32_max components
            ) input,
            group(
              u32_max components,
              u32_max vertices,
              u32_max totalComponents
            ) output
          ) geometry,
          group(
            bool storesAndAtomics,
            group(
              bool sample,
              bool pixel,
              bool shadingRate
            ) interlock,
            group(
              u32_max components
            ) input,
            group(
              u32_max attachments,
              u32_max combinedResources
            ) output,
            u32_max dualSourceAttachments
          ) fragment,
          group(
            bool fullSubgroups,
            group(
              bool quadGroup,
              bool linearGroup
            ) derivative,
            u32_max sharedMemorySize,
            group(
              u32_max subgroups,
              u32_max invocations,
              group(
                u32_max x,
                u32_max y,
                u32_max z
              ) count,
              group(
                u32_max x,
                u32_max y,
                u32_max z
              ) size
            ) workGroup
          ) compute,
          feature(
            u32_max totalMemorySize,
            u32_max viewCount,
            group(
              u32_max drawMeshCount,
              u32_max totalMemorySize,
              u32_max outputCount,
              group(
                u32_max invocations,
                group(
                  u32_max x,
                  u32_max y,
                  u32_max z
                ) size
              ) workGroup
            ) task,
            group(
              u32_max invocations,
              group(
                u32_max x,
                u32_max y,
                u32_max z
              ) size
            ) workGroup,
            group(
              u32_max vertices,
              u32_max primitives,
              u32 perVertexGranularity,
              u32 perPrimitiveGranularity
            ) output
          ) mesh,
          group(
            bool f16,
            bool f32,
            bool f64,
            bool i8,
            bool i16,
            bool i32,
            bool i64
          ) type,
          feature(
            u32_max count
          ) clipDistance,
          feature(
            u32_max count
          ) cullDistance,
          group(
            u32_max count
          ) combinedClipAndCullDistance,
          bool demoteToHelperInvocation
        ) shader,
        bool coverageReduction
    );

    /*struct Features{
      VK_feature(
        variablePointers,
          property bool storageBuffer
      );

      VK_feature(
        multiview,
          property bool geometryShader,
          property bool tessellationShader
      );



      VK_feature(
        wideLines,
          property type(RangeF32) width,
          property type(float) granularity
      );

      VK_feature(
        largePoints,
          property type(RangeF32) size,
          property type(float) granularity
      );

      VK_feature(
        timelineSemaphore,
          property type(MaxU64) valueDifference
      );

      VK_feature(
        performanceCounters,
          property bool queryPools,
          property bool multipleQueryPools
      );

      VK_feature(
        textureCompression,
          property bool ETC2,
          property bool ASTC_LDR,
          property bool ASTC_HDR,
          property bool BC
      );

      VK_feature(
        cooperativeMatrix,
          property bool robustBufferAccess,
          property type(Vk::ShaderStageFlags) supportedStages
      );

      bool coverageReduction;
      //VK_feature(coverageReduction);

    private:
      struct Depth{
        VK_feature(
          clamp,
            property bool bias
        );
        bool bounds;
        *//*VK_feature(
          bounds
        );*//*
      };
      struct Shader{

        VK_feature(
          vertex,
            property bool storesAndAtomics,
            property group(
              type(MaxU32) attributes,
              type(MaxU32) bindings,
              type(MaxU32) attributeOffset,
              type(MaxU32) bindingStride
            ) input,
            property group(
              type(MaxU32) components
            ) output
        );
        VK_feature(
          tessellation,
            property bool storesAndAtomics,
            property bool pointSize,
            property type(MaxU32) generationLevel,
            property type(MaxU32) patchSize,
            property group(
              type(MaxU32) inputComponentsPerVertex,
              type(MaxU32) outputComponentsPerVertex,
              type(MaxU32) outputComponentsPerPatch,
              type(MaxU32) totalOutputComponents
            ) control,
            property group(
              type(MaxU32) inputComponents,
              type(MaxU32) outputComponents
            ) evaluation
        );
        VK_feature(
          geometry,
            property bool storesAndAtomics,
            property bool pointSize,
            property type(MaxU32) invocations,
            property group(
              type(MaxU32) components
            ) input,
            property group(
              type(MaxU32) components,
              type(MaxU32) vertices,
              type(MaxU32) totalComponents
            ) output
        );
        VK_feature(
          fragment,
            property bool storesAndAtomics,
            property group(
              type(MaxU32) components
            ) input,
            property group(
              type(MaxU32) attachments,
              type(MaxU32) combinedResources
            ) output,
            property type(MaxU32) dualSrcAttachments,
            property group(
              bool sample,
              bool pixel,
              bool shadingRate
            ) interlock
        );
        VK_feature(
          type,
            property bool f16,
            property bool f32,
            property bool f64,
            property bool i8,
            property bool i16,
            property bool i32,
            property bool i64
        );
        struct {
          struct {
            struct {
              MaxU32 x, y, z;
            } count, size;
            MaxU32 subgroups;
            MaxU32 invocations;
          } workGroup;
          struct {
            struct {
              bool quad;
              bool linear;
            } group;
          } derivative;
          bool fullSubgroups;
          MaxU32 sharedMemorySize;
        } compute;
        struct {
          bool enabled;
          MaxU32 drawTaskCount;
          MaxU32 totalMemorySize;
          MaxU32 viewCount;
          struct {
            MaxU32 drawMeshCount;
            MaxU32 totalMemorySize;
            struct {
              MaxU32 invocations;
              struct {
                MaxU32 x, y, z;
              } size;
            } workGroup;
            MaxU32 outputCount;
          } task;
          struct {
            MaxU32 invocations;
            struct {
              MaxU32 x, y, z;
            } size;
          } workGroup;
          struct {
            MaxU32 vertices;
            MaxU32 primitives;
            uint32_t perVertexGranularity;
            uint32_t perPrimitiveGranularity;
          } output;
        } mesh;

      };
      struct FragmentDensity {
        interval<Vk::Extent2D> texelSize;
        struct {
          MaxU32 descriptorSetSamplers;
          MaxU32 arrayLayers;
          bool loads;
          bool coarseReconstructionEarlyAccess;
        } subsampled;
        bool invocations;
        VK_feature(
          map,
            property bool dynamic,
            property bool deferred,
            property bool nonSubsampledImages
        );
      };
    public:
      Depth depth;
      Shader shader;
      FragmentDensity fragmentDensity;
    } features;*/


    struct {
      struct {
        MaxU32 arrayLayers;
        RangeU32 count;
        Vk::ImageUsageFlags supportedUsage;
        struct {
          MaxU32WithCurrent width;
          MaxU32WithCurrent height;
        } extent;
      } image;
      struct {
        Vk::SurfaceTransformFlags supported;
        Vk::SurfaceTransformFlags current;
      } transforms;

      Vk::CompositeAlphaFlags supportedCompositeAlpha;
      Vk::SurfaceCounterFlags supportedSurfaceCounters;


      std::pmr::vector<VkSurfaceFormat2KHR> formats;
      std::pmr::vector<VkPresentModeKHR> presentModes;
    } swapchain;

    std::pmr::vector<VkExtensionProperties> extensions;
    //boost::container::small_vector<VkPhysicalDeviceToolPropertiesEXT, 4> tools;
    //boost::container::small_vector<VkCooperativeMatrixPropertiesNV, 8> coopMatrixProps;
    std::vector<VkPhysicalDeviceToolPropertiesEXT> tools;
    std::vector<VkCooperativeMatrixPropertiesNV> coopMatrixProps;
    Vk::FlatSet<Vk::Handle<Vk::PhysicalDeviceObject>> children;

    std::string name;
    std::vector<std::byte> taggedData;

    inline void initFunctions(){
      auto& inst = ((InstanceImpl&)*getInstance());
      VkInstance instance = inst.nativeHandle_;
#define getFunctionAddress(Func) (PFN_##Func)(inst.api.getFunctionAddr(instance, #Func))
      api.getProperties = getFunctionAddress(vkGetPhysicalDeviceProperties2);
      api.getFeatures   = getFunctionAddress(vkGetPhysicalDeviceFeatures2);
      api.getExtensions = getFunctionAddress(vkEnumerateDeviceExtensionProperties);
      api.getToolProperties = getFunctionAddress(vkGetPhysicalDeviceToolPropertiesEXT);
      api.getCooperativeMatrixProperties = getFunctionAddress(vkGetPhysicalDeviceCooperativeMatrixPropertiesNV);
      api.getSupportedFramebufferMixedSamplesCombinations = getFunctionAddress(vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV);

      api.surface.getCapabilities = getFunctionAddress(vkGetPhysicalDeviceSurfaceCapabilities2KHR);
      api.surface.getFormats      = getFunctionAddress(vkGetPhysicalDeviceSurfaceFormats2KHR);
      api.surface.getPresentModes = getFunctionAddress(vkGetPhysicalDeviceSurfacePresentModes2EXT);
      api.surface.getSupport      = getFunctionAddress(vkGetPhysicalDeviceSurfaceSupportKHR);
      api.surface.getPresentRectangles = getFunctionAddress(vkGetPhysicalDevicePresentRectanglesKHR);

      api.display.getProperties = getFunctionAddress(vkGetPhysicalDeviceDisplayPropertiesKHR);
      api.display.release       = getFunctionAddress(vkReleaseDisplayEXT);
      api.display.plane.getProperties = getFunctionAddress(vkGetPhysicalDeviceDisplayPlaneProperties2KHR);
      api.display.plane.getSupportedDisplays = getFunctionAddress(vkGetDisplayPlaneSupportedDisplaysKHR);
      api.display.plane.getCapabilities      = getFunctionAddress(vkGetDisplayPlaneCapabilitiesKHR);
      api.display.mode.getProperties = getFunctionAddress(vkGetDisplayModePropertiesKHR);
      api.display.mode.create        = getFunctionAddress(vkCreateDisplayModeKHR);

      api.queueFamily.getProperties = getFunctionAddress(vkGetPhysicalDeviceQueueFamilyProperties2KHR);
      api.queueFamily.enumeratePerformanceQueryCounters = getFunctionAddress(vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR);
      api.queueFamily.getPerformanceQueryPasses = getFunctionAddress(vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR);

      api.format.getProperties = getFunctionAddress(vkGetPhysicalDeviceFormatProperties2);
      api.format.image.getProperties = getFunctionAddress(vkGetPhysicalDeviceImageFormatProperties2);
      api.format.image.external.getProperties = getFunctionAddress(vkGetPhysicalDeviceExternalImageFormatPropertiesNV);

      api.memory.getProperties = getFunctionAddress(vkGetPhysicalDeviceMemoryProperties2);
      api.memory.external.getBufferProperties = getFunctionAddress(vkGetPhysicalDeviceExternalBufferProperties);
      api.memory.external.getSemaphoreProperties = getFunctionAddress(vkGetPhysicalDeviceExternalSemaphoreProperties);
      api.memory.external.getFenceProperties = getFunctionAddress(vkGetPhysicalDeviceExternalFenceProperties);


      api.multisample.getProperties = getFunctionAddress(vkGetPhysicalDeviceMultisamplePropertiesEXT);

      api.getCalibrateableTimeDomains = getFunctionAddress(vkGetPhysicalDeviceCalibrateableTimeDomainsEXT);

      api.getPresentationSupport = getFunctionAddress(vkGetPhysicalDeviceWin32PresentationSupportKHR);
    }
    inline void initFeatures(){
#pragma region feature structs
      VkPhysicalDevicePortabilitySubsetFeaturesKHR portabilitySubsetFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR,
          .pNext = nullptr
      };
      VkPhysicalDeviceRayTracingFeaturesKHR rayTracingFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_FEATURES_KHR,
          .pNext = &portabilitySubsetFeatures
      };
      VkPhysicalDeviceCornerSampledImageFeaturesNV cornerSampledImageFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CORNER_SAMPLED_IMAGE_FEATURES_NV,
          .pNext = &rayTracingFeatures
      };
      VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT textureCompressionAstchdrFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXTURE_COMPRESSION_ASTC_HDR_FEATURES_EXT,
          .pNext = &cornerSampledImageFeatures
      };
      VkPhysicalDeviceASTCDecodeFeaturesEXT astcDecodeFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ASTC_DECODE_FEATURES_EXT,
          .pNext = &textureCompressionAstchdrFeatures
      };
      VkPhysicalDeviceConditionalRenderingFeaturesEXT conditionalRenderingFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CONDITIONAL_RENDERING_FEATURES_EXT,
          .pNext = &astcDecodeFeatures
      };
      VkPhysicalDeviceDepthClipEnableFeaturesEXT depthClipEnableFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_ENABLE_FEATURES_EXT,
          .pNext = &conditionalRenderingFeatures
      };
      VkPhysicalDeviceInlineUniformBlockFeaturesEXT inlineUniformBlockFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES_EXT,
          .pNext = &depthClipEnableFeatures
      };
      VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT blendOperationAdvancedFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_FEATURES_EXT,
          .pNext = &inlineUniformBlockFeatures
      };
      VkPhysicalDeviceShaderSMBuiltinsFeaturesNV shaderSmBuiltinsFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SM_BUILTINS_FEATURES_NV,
          .pNext = &blendOperationAdvancedFeatures
      };
      VkPhysicalDeviceShadingRateImageFeaturesNV shadingRateImageFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADING_RATE_IMAGE_FEATURES_NV,
          .pNext = &shaderSmBuiltinsFeatures
      };
      VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV representativeFragmentTestFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_REPRESENTATIVE_FRAGMENT_TEST_FEATURES_NV,
          .pNext = &shadingRateImageFeatures
      };
      VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT vertexAttributeDivisorFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_FEATURES_EXT,
          .pNext = &representativeFragmentTestFeatures
      };
      VkPhysicalDeviceMeshShaderFeaturesNV meshShaderFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_NV,
          .pNext = &vertexAttributeDivisorFeatures
      };
      VkPhysicalDeviceFragmentShaderBarycentricFeaturesNV fragmentShaderBarycentricFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_BARYCENTRIC_FEATURES_NV,
          .pNext = &meshShaderFeatures
      };
      VkPhysicalDeviceShaderImageFootprintFeaturesNV shaderImageFootprintFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_IMAGE_FOOTPRINT_FEATURES_NV,
          .pNext = &fragmentShaderBarycentricFeatures
      };
      VkPhysicalDeviceExclusiveScissorFeaturesNV exclusiveScissorFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXCLUSIVE_SCISSOR_FEATURES_NV,
          .pNext = &shaderImageFootprintFeatures
      };
      VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL shaderIntegerFunctions2Features{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_FUNCTIONS_2_FEATURES_INTEL,
          .pNext = &exclusiveScissorFeatures
      };
      VkPhysicalDevice4444FormatsFeaturesEXT formats4444FeaturesExt{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_4444_FORMATS_FEATURES_EXT,
          .pNext = &shaderIntegerFunctions2Features
      };
      VkPhysicalDeviceRobustness2FeaturesEXT robustness2Features{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT,
          .pNext = &formats4444FeaturesExt
      };
      VkPhysicalDeviceImageRobustnessFeaturesEXT imageRobustnessFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_ROBUSTNESS_FEATURES_EXT,
          .pNext = &robustness2Features
      };
      VkPhysicalDeviceDiagnosticsConfigFeaturesNV diagnosticsConfigFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DIAGNOSTICS_CONFIG_FEATURES_NV,
          .pNext = &imageRobustnessFeatures
      };
      VkPhysicalDevicePrivateDataFeaturesEXT privateDataFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIVATE_DATA_FEATURES_EXT,
          .pNext = &diagnosticsConfigFeatures
      };
      VkPhysicalDeviceCustomBorderColorFeaturesEXT customBorderColorFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_BORDER_COLOR_FEATURES_EXT,
          .pNext = &privateDataFeatures
      };
      VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT texelBufferAlignmentFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_FEATURES_EXT,
          .pNext = &customBorderColorFeatures
      };
      VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV deviceDeviceGeneratedCommandsFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_GENERATED_COMMANDS_FEATURES_NV,
          .pNext = &texelBufferAlignmentFeatures
      };
      VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT shaderDemoteToHelperInvocationFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DEMOTE_TO_HELPER_INVOCATION_FEATURES_EXT,
          .pNext = &deviceDeviceGeneratedCommandsFeatures
      };
      VkPhysicalDeviceExtendedDynamicStateFeaturesEXT extendedDynamicStateFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT,
          .pNext = &shaderDemoteToHelperInvocationFeatures
      };


      ////

      VkPhysicalDeviceIndexTypeUint8FeaturesEXT indexTypeUint8Features{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INDEX_TYPE_UINT8_FEATURES_EXT,
          .pNext = &extendedDynamicStateFeatures
      };
      VkPhysicalDeviceShaderAtomicFloatFeaturesEXT shaderAtomicFloatFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_FEATURES_EXT,
          .pNext = &indexTypeUint8Features
      };
      VkPhysicalDeviceYcbcrImageArraysFeaturesEXT ycbcrImageArraysFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_IMAGE_ARRAYS_FEATURES_EXT,
          .pNext = &shaderAtomicFloatFeatures
      };
      VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT fragmentShaderInterlockFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_INTERLOCK_FEATURES_EXT,
          .pNext = &ycbcrImageArraysFeatures
      };
      VkPhysicalDeviceCoverageReductionModeFeaturesNV coverageReductionModeFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COVERAGE_REDUCTION_MODE_FEATURES_NV,
          .pNext = &fragmentShaderInterlockFeatures
      };
      VkPhysicalDeviceCooperativeMatrixFeaturesNV cooperativeMatrixFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_FEATURES_NV,
          .pNext = &coverageReductionModeFeatures
      };
      VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV dedicatedAllocationImageAliasingFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEDICATED_ALLOCATION_IMAGE_ALIASING_FEATURES_NV,
          .pNext = &cooperativeMatrixFeatures
      };
      VkPhysicalDeviceSubgroupSizeControlFeaturesEXT subgroupSizeControlFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_FEATURES_EXT,
          .pNext = &dedicatedAllocationImageAliasingFeatures
      };

      // TODO: Add features from the remaining structs.

      VkPhysicalDeviceFragmentDensityMap2FeaturesEXT fragmentDensityMap2Features{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_2_FEATURES_EXT,
          .pNext = &subgroupSizeControlFeatures
      };
      VkPhysicalDeviceFragmentDensityMapFeaturesEXT fragmentDensityMapFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_FEATURES_EXT,
          .pNext = &fragmentDensityMap2Features
      };
      VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT pipelineCreationCacheControlFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_CREATION_CACHE_CONTROL_FEATURES_EXT,
          .pNext = &fragmentDensityMapFeatures
      };
      VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR pipelineExecutablePropertiesFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_EXECUTABLE_PROPERTIES_FEATURES_KHR,
          .pNext = &pipelineCreationCacheControlFeatures
      };
      VkPhysicalDeviceLineRasterizationFeaturesEXT lineRasterizationFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_FEATURES_EXT,
          .pNext = &pipelineExecutablePropertiesFeatures
      };
      VkPhysicalDeviceMemoryPriorityFeaturesEXT memoryPriorityFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PRIORITY_FEATURES_EXT,
          .pNext = &lineRasterizationFeatures
      };
      VkPhysicalDeviceTransformFeedbackFeaturesEXT transformFeedbackFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_FEATURES_EXT,
          .pNext = &memoryPriorityFeatures
      };
      VkPhysicalDevicePerformanceQueryFeaturesKHR performanceQueryFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_QUERY_FEATURES_KHR,
          .pNext = &transformFeedbackFeatures
      };
      VkPhysicalDeviceShaderClockFeaturesKHR shaderClockFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CLOCK_FEATURES_KHR,
          .pNext = &performanceQueryFeatures
      };
      VkPhysicalDeviceVulkan12Features vulkan12Features{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
          .pNext = &shaderClockFeatures
      };
      VkPhysicalDeviceVulkan11Features vulkan11Features{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES,
          .pNext = &vulkan12Features
      };
      VkPhysicalDeviceFeatures2 deviceFeatures{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
          .pNext = &vulkan11Features
      };
#pragma endregion feature structs
      api.getFeatures(nativeHandle_, &deviceFeatures);
#pragma region feature initialization
      features.shader.type.f64 = deviceFeatures.features.shaderFloat64;
      features.shader.type.f32 = true;
      features.shader.type.f16 = vulkan12Features.shaderFloat16;
      features.shader.type.i64 = deviceFeatures.features.shaderInt64;
      features.shader.type.i32 = true;
      features.shader.type.i16 = deviceFeatures.features.shaderInt16;
      features.shader.type.i8  = vulkan12Features.shaderInt8;


      memory.buffer.robustAccess             = deviceFeatures.features.robustBufferAccess;
      render.draw.index.fullU32              = deviceFeatures.features.fullDrawIndexUint32;
      memory.image.cubeArray                 = deviceFeatures.features.imageCubeArray;
      render.blend.independent               = deviceFeatures.features.independentBlend;
      features.shader.geometry               = deviceFeatures.features.geometryShader;
      features.shader.tessellation           = deviceFeatures.features.tessellationShader;
      render.sampleRateShading               = deviceFeatures.features.sampleRateShading;
      render.blend.dualSource                = deviceFeatures.features.dualSrcBlend;
      render.blend.logicOps                  = deviceFeatures.features.logicOp;
      render.draw.indirect.multi         = deviceFeatures.features.multiDrawIndirect;
      render.draw.indirect.firstInstance = deviceFeatures.features.drawIndirectFirstInstance;
      features.depth.clamp                   = deviceFeatures.features.depthClamp;
      features.depth.clamp.bias              = deviceFeatures.features.depthBiasClamp;
      features.depth.bounds                  = deviceFeatures.features.depthBounds;
      features.wideLines                     = deviceFeatures.features.wideLines;
      features.largePoints                   = deviceFeatures.features.largePoints;
      render.fillModeNonSolid               = deviceFeatures.features.fillModeNonSolid;
      render.alphaToOne                     = deviceFeatures.features.alphaToOne;
      properties.viewport.multi             = deviceFeatures.features.multiViewport;
      resource.sampler.anisotropy           = deviceFeatures.features.samplerAnisotropy;
      features.textureCompression.ETC2      = deviceFeatures.features.textureCompressionETC2;
      features.textureCompression.ASTC.LDR  = deviceFeatures.features.textureCompressionASTC_LDR;
      features.textureCompression.BC        = deviceFeatures.features.textureCompressionBC;

      properties.query.preciseOcclusion               = deviceFeatures.features.occlusionQueryPrecise;
      properties.query.pipelineStatistics             = deviceFeatures.features.pipelineStatisticsQuery;
      features.shader.vertex.storesAndAtomics       = deviceFeatures.features.vertexPipelineStoresAndAtomics;
      features.shader.geometry.storesAndAtomics     = deviceFeatures.features.vertexPipelineStoresAndAtomics;
      features.shader.tessellation.storesAndAtomics = deviceFeatures.features.vertexPipelineStoresAndAtomics;
      features.shader.fragment.storesAndAtomics     = deviceFeatures.features.fragmentStoresAndAtomics;
      features.shader.geometry.pointSize            = deviceFeatures.features.shaderTessellationAndGeometryPointSize;
      features.shader.tessellation.pointSize        = deviceFeatures.features.shaderTessellationAndGeometryPointSize;

      memory.image.gatherExtended                = deviceFeatures.features.shaderImageGatherExtended;
      resource.storageImage.extendedFormats        = deviceFeatures.features.shaderStorageImageExtendedFormats;
      resource.storageImage.multisample            = deviceFeatures.features.shaderStorageImageMultisample;
      resource.storageImage.readWithoutFormat      = deviceFeatures.features.shaderStorageImageReadWithoutFormat;
      resource.storageImage.writeWithoutFormat     = deviceFeatures.features.shaderStorageImageWriteWithoutFormat;
      resource.uniformBuffer.arrayIndexing.dynamic = deviceFeatures.features.shaderUniformBufferArrayDynamicIndexing;
      resource.sampledImage.arrayIndexing.dynamic  = deviceFeatures.features.shaderSampledImageArrayDynamicIndexing;
      resource.storageBuffer.arrayIndexing.dynamic = deviceFeatures.features.shaderStorageBufferArrayDynamicIndexing;
      resource.storageImage.arrayIndexing.dynamic  = deviceFeatures.features.shaderStorageImageArrayDynamicIndexing;

      features.shader.clipDistance = deviceFeatures.features.shaderClipDistance;
      features.shader.cullDistance = deviceFeatures.features.shaderCullDistance;

      resource.residency = deviceFeatures.features.shaderResourceResidency;
      resource.minLod    = deviceFeatures.features.shaderResourceMinLod;

      memory.sparse.binding               = deviceFeatures.features.sparseBinding;
      memory.sparse.residency.buffer      = deviceFeatures.features.sparseResidencyBuffer;
      memory.sparse.residency.image2D     = deviceFeatures.features.sparseResidencyImage2D;
      memory.sparse.residency.image3D     = deviceFeatures.features.sparseResidencyImage3D;
      memory.sparse.residency.samples.x2  = deviceFeatures.features.sparseResidency2Samples;
      memory.sparse.residency.samples.x4  = deviceFeatures.features.sparseResidency4Samples;
      memory.sparse.residency.samples.x8  = deviceFeatures.features.sparseResidency8Samples;
      memory.sparse.residency.samples.x16 = deviceFeatures.features.sparseResidency16Samples;
      memory.sparse.residency.aliased     = deviceFeatures.features.sparseResidencyAliased;

      render.variableMultisampleRate = deviceFeatures.features.variableMultisampleRate;
      properties.query.inherited        = deviceFeatures.features.inheritedQueries;

      resource.storageBuffer.access.b16         = vulkan11Features.storageBuffer16BitAccess;
      resource.uniformBuffer.access.b16 = vulkan11Features.uniformAndStorageBuffer16BitAccess;
      capabilities.pushConstants.storage.b16           = vulkan11Features.storagePushConstant16;
      resource.input.access.b16            = vulkan11Features.storageInputOutput16;
      resource.output.access.b16            = vulkan11Features.storageInputOutput16;
      features.multiview                           = vulkan11Features.multiview;
      features.multiview.geometryShader            = vulkan11Features.multiviewGeometryShader;
      features.multiview.tessellationShader        = vulkan11Features.multiviewTessellationShader;
      features.variablePointers                    = vulkan11Features.variablePointers;
      features.variablePointers.storageBuffer      = vulkan11Features.variablePointersStorageBuffer;
      memory.protection                     = vulkan11Features.protectedMemory;
      resource.sampler.ycbcrConversion             = vulkan11Features.samplerYcbcrConversion;
      render.draw.parameters             = vulkan11Features.shaderDrawParameters;

      resource.sampler.mirrorClampToEdge                 = vulkan12Features.samplerMirrorClampToEdge;
      render.draw.indirect.count.enabled       = vulkan12Features.drawIndirectCount;
      resource.storageBuffer.access.b8            = vulkan12Features.storageBuffer8BitAccess;
      resource.uniformBuffer.access.b8  = vulkan12Features.uniformAndStorageBuffer8BitAccess;
      capabilities.pushConstants.storage.b8            = vulkan12Features.storagePushConstant8;
      memory.buffer.atomic.i64                     = vulkan12Features.shaderBufferInt64Atomics;
      memory.shared.atomic.i64                     = vulkan12Features.shaderSharedInt64Atomics;
      resource.descriptor.indexing                 = vulkan12Features.descriptorIndexing;

      resource.input.arrayIndexing.dynamic        = vulkan12Features.shaderInputAttachmentArrayDynamicIndexing;
      resource.uniformBuffer.texel.arrayIndexing.dynamic    = vulkan12Features.shaderUniformTexelBufferArrayDynamicIndexing;
      resource.storageBuffer.texel.arrayIndexing.dynamic    = vulkan12Features.shaderStorageTexelBufferArrayDynamicIndexing;
      resource.uniformBuffer.arrayIndexing.nonUniform       = vulkan12Features.shaderUniformBufferArrayNonUniformIndexing;
      resource.sampledImage.arrayIndexing.nonUniform        = vulkan12Features.shaderSampledImageArrayNonUniformIndexing;
      resource.storageBuffer.arrayIndexing.nonUniform       = vulkan12Features.shaderStorageBufferArrayNonUniformIndexing;
      resource.storageImage.arrayIndexing.nonUniform        = vulkan12Features.shaderStorageImageArrayNonUniformIndexing;
      resource.input.arrayIndexing.nonUniform     = vulkan12Features.shaderInputAttachmentArrayNonUniformIndexing;
      resource.uniformBuffer.texel.arrayIndexing.nonUniform = vulkan12Features.shaderUniformTexelBufferArrayNonUniformIndexing;
      resource.storageBuffer.texel.arrayIndexing.nonUniform = vulkan12Features.shaderStorageTexelBufferArrayNonUniformIndexing;

      resource.uniformBuffer.descriptor.bindingUpdateAfterBind = vulkan12Features.descriptorBindingUniformBufferUpdateAfterBind;
      resource.sampledImage.descriptor.bindingUpdateAfterBind  = vulkan12Features.descriptorBindingSampledImageUpdateAfterBind;
      resource.storageImage.descriptor.bindingUpdateAfterBind  = vulkan12Features.descriptorBindingStorageImageUpdateAfterBind;
      resource.storageBuffer.descriptor.bindingUpdateAfterBind = vulkan12Features.descriptorBindingStorageBufferUpdateAfterBind;
      resource.uniformBuffer.texel.descriptor.bindingUpdateAfterBind = vulkan12Features.descriptorBindingUniformTexelBufferUpdateAfterBind;
      resource.storageBuffer.texel.descriptor.bindingUpdateAfterBind = vulkan12Features.descriptorBindingStorageTexelBufferUpdateAfterBind;


      resource.descriptor.binding.updateUnusedWhilePending = vulkan12Features.descriptorBindingUpdateUnusedWhilePending;
      resource.descriptor.binding.partiallyBound           = vulkan12Features.descriptorBindingPartiallyBound;
      resource.descriptor.binding.variableDescriptorCount  = vulkan12Features.descriptorBindingVariableDescriptorCount;
      resource.descriptor.runtimeArray                     = vulkan12Features.runtimeDescriptorArray;
      resource.sampler.filterMinMax                     = vulkan12Features.samplerFilterMinmax;
      resource.scalarBlockLayout                       = vulkan12Features.scalarBlockLayout;
      properties.framebuffer.imageless                    = vulkan12Features.imagelessFramebuffer;
      resource.uniformBuffer.standardLayout        = vulkan12Features.uniformBufferStandardLayout;
      properties.subgroup.extendedTypes                 = vulkan12Features.shaderSubgroupExtendedTypes;
      memory.image.separateDepthStencilLayouts             = vulkan12Features.separateDepthStencilLayouts;
      properties.query.hostReset                          = vulkan12Features.hostQueryReset;
      features.timelineSemaphore                        = vulkan12Features.timelineSemaphore;
      memory.buffer.deviceAddress                       = vulkan12Features.bufferDeviceAddress;
      memory.buffer.deviceAddress.multiDevice          = vulkan12Features.bufferDeviceAddressMultiDevice;
      memory.buffer.deviceAddress.captureReplay        = vulkan12Features.bufferDeviceAddressCaptureReplay;
      memory.model                              = vulkan12Features.vulkanMemoryModel;
      memory.model.deviceScope                  = vulkan12Features.vulkanMemoryModelDeviceScope;
      memory.model.availabilityVisibilityChains = vulkan12Features.vulkanMemoryModelAvailabilityVisibilityChains;
      resource.output.viewportIndex            = vulkan12Features.shaderOutputViewportIndex;
      resource.output.layer                    = vulkan12Features.shaderOutputLayer;
      properties.subgroup.broadcastDynamicId            = vulkan12Features.subgroupBroadcastDynamicId;

      properties.subgroup.clock = shaderClockFeatures.shaderSubgroupClock;
      device.clock   = shaderClockFeatures.shaderDeviceClock;

      features.performanceCounter.queryPools = performanceQueryFeatures.performanceCounterQueryPools;
      features.performanceCounter.queryPools.multiple = performanceQueryFeatures.performanceCounterMultipleQueryPools;

      capabilities.spirv.geometryStreams = transformFeedbackFeatures.geometryStreams;
      capabilities.spirv.transformFeedback = transformFeedbackFeatures.transformFeedback;

      memory.priority = memoryPriorityFeatures.memoryPriority;

      render.rasterize.line.rectangular = lineRasterizationFeatures.rectangularLines;
      render.rasterize.line.bresenham   = lineRasterizationFeatures.bresenhamLines;
      render.rasterize.line.smooth      = lineRasterizationFeatures.smoothLines;
      render.rasterize.line.stippledRectangular = lineRasterizationFeatures.stippledRectangularLines;
      render.rasterize.line.stippledBresenham   = lineRasterizationFeatures.stippledBresenhamLines;
      render.rasterize.line.stippledSmooth      = lineRasterizationFeatures.stippledSmoothLines;

      render.pipeline.executableInfo = pipelineExecutablePropertiesFeatures.pipelineExecutableInfo;
      render.pipeline.creationCacheControl = pipelineCreationCacheControlFeatures.pipelineCreationCacheControl;

      features.fragmentDensity.map = fragmentDensityMapFeatures.fragmentDensityMap;
      features.fragmentDensity.map.dynamic = fragmentDensityMapFeatures.fragmentDensityMapDynamic;
      features.fragmentDensity.map.nonSubsampledImages = fragmentDensityMapFeatures.fragmentDensityMapNonSubsampledImages;
      features.fragmentDensity.map.deferred = fragmentDensityMap2Features.fragmentDensityMapDeferred;


      properties.subgroup.size.control = subgroupSizeControlFeatures.subgroupSizeControl;
      features.shader.compute.fullSubgroups = subgroupSizeControlFeatures.computeFullSubgroups;

      memory.allocation.dedicatedImageAliasing = dedicatedAllocationImageAliasingFeatures.dedicatedAllocationImageAliasing;

      features.cooperativeMatrix = cooperativeMatrixFeatures.cooperativeMatrix;
      features.cooperativeMatrix.robustBufferAccess = cooperativeMatrixFeatures.cooperativeMatrixRobustBufferAccess;

      features.coverageReduction = coverageReductionModeFeatures.coverageReductionMode;


      features.shader.fragment.interlock.sample = fragmentShaderInterlockFeatures.fragmentShaderSampleInterlock;
      features.shader.fragment.interlock.pixel = fragmentShaderInterlockFeatures.fragmentShaderPixelInterlock;
      features.shader.fragment.interlock.shadingRate = fragmentShaderInterlockFeatures.fragmentShaderShadingRateInterlock;


      memory.buffer.atomic.f32 = shaderAtomicFloatFeatures.shaderBufferFloat32Atomics;
      memory.buffer.atomic.addF32 = shaderAtomicFloatFeatures.shaderBufferFloat32AtomicAdd;
      memory.buffer.atomic.f64 = shaderAtomicFloatFeatures.shaderBufferFloat64Atomics;
      memory.buffer.atomic.addF64 = shaderAtomicFloatFeatures.shaderBufferFloat64AtomicAdd;
      memory.image.atomic.f32 = shaderAtomicFloatFeatures.shaderImageFloat32Atomics;
      memory.image.atomic.addF32 = shaderAtomicFloatFeatures.shaderImageFloat32AtomicAdd;
      memory.shared.atomic.f32 = shaderAtomicFloatFeatures.shaderSharedFloat32Atomics;
      memory.shared.atomic.addF32 = shaderAtomicFloatFeatures.shaderSharedFloat32AtomicAdd;
      memory.shared.atomic.f64 = shaderAtomicFloatFeatures.shaderSharedFloat64Atomics;
      memory.shared.atomic.addF64 = shaderAtomicFloatFeatures.shaderSharedFloat64AtomicAdd;

      memory.buffer.indexU8 = indexTypeUint8Features.indexTypeUint8;

      render.pipeline.extendedDynamicState = extendedDynamicStateFeatures.extendedDynamicState;

      features.shader.demoteToHelperInvocation = shaderDemoteToHelperInvocationFeatures.shaderDemoteToHelperInvocation;

      device.generatedCommands = deviceDeviceGeneratedCommandsFeatures.deviceGeneratedCommands;



      VkBool32           extendedDynamicState;

      VkBool32           shaderDemoteToHelperInvocation;

      VkBool32           deviceGeneratedCommands;

      VkBool32           texelBufferAlignment;

      VkBool32           customBorderColors;
      VkBool32           customBorderColorWithoutFormat;

      VkBool32           privateData;

      VkBool32           diagnosticsConfig;

      VkBool32           robustImageAccess;

      VkBool32           robustBufferAccess2;
      VkBool32           robustImageAccess2;
      VkBool32           nullDescriptor;

      VkBool32           formatA4R4G4B4;
      VkBool32           formatA4B4G4R4;

      VkBool32           shaderIntegerFunctions2;

      VkBool32           exclusiveScissor;

      VkBool32           imageFootprint;

      VkBool32           fragmentShaderBarycentric;

      VkBool32           taskShader;
      VkBool32           meshShader;

      VkBool32           vertexAttributeInstanceRateDivisor;
      VkBool32           vertexAttributeInstanceRateZeroDivisor;

      VkBool32           representativeFragmentTest;

      VkBool32           shadingRateImage;
      VkBool32           shadingRateCoarseSampleOrder;

      VkBool32           shaderSMBuiltins;

      VkBool32           advancedBlendCoherentOperations;

      VkBool32           inlineUniformBlock;
      VkBool32           descriptorBindingInlineUniformBlockUpdateAfterBind;

      VkBool32           depthClipEnable;

      VkBool32           conditionalRendering;
      VkBool32           inheritedConditionalRendering;

      VkBool32           decodeModeSharedExponent;

      VkBool32           cornerSampledImage;

      features.textureCompression.ASTC.HDR  = textureCompressionAstchdrFeatures.textureCompressionASTC_HDR;

      VkBool32           rayTracing;
      VkBool32           rayTracingShaderGroupHandleCaptureReplay;
      VkBool32           rayTracingShaderGroupHandleCaptureReplayMixed;
      VkBool32           rayTracingAccelerationStructureCaptureReplay;
      VkBool32           rayTracingIndirectTraceRays;
      VkBool32           rayTracingIndirectAccelerationStructureBuild;
      VkBool32           rayTracingHostAccelerationStructureCommands;
      VkBool32           rayQuery;
      VkBool32           rayTracingPrimitiveCulling;

      VkBool32           constantAlphaColorBlendFactors;
      VkBool32           events;
      VkBool32           imageViewFormatReinterpretation;
      VkBool32           imageViewFormatSwizzle;
      VkBool32           imageView2DOn3DImage;
      VkBool32           multisampleArrayImage;
      VkBool32           mutableComparisonSamplers;
      VkBool32           pointPolygons;
      VkBool32           samplerMipLodBias;
      VkBool32           separateStencilMaskRef;
      VkBool32           shaderSampleRateInterpolationFunctions;
      VkBool32           tessellationIsolines;
      VkBool32           tessellationPointMode;
      VkBool32           triangleFans;
      VkBool32           vertexAttributeAccessBeyondStride;

#pragma endregion feature initialization
    }
    inline void initProperties(){
#pragma region property structs
      VkPhysicalDevicePortabilitySubsetPropertiesKHR portabilitySubsetProperties{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_PROPERTIES_KHR,
          .pNext = nullptr
      };
      VkPhysicalDeviceGroupProperties groupProperties{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES,
          .pNext = &portabilitySubsetProperties
      };
      VkPhysicalDevicePushDescriptorPropertiesKHR pushDescriptorProperties{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PUSH_DESCRIPTOR_PROPERTIES_KHR,
          .pNext = &groupProperties
      };
      VkPhysicalDevicePerformanceQueryPropertiesKHR performanceQueryProperties{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_QUERY_PROPERTIES_KHR,
          .pNext = &pushDescriptorProperties
      };
      VkPhysicalDeviceTransformFeedbackPropertiesEXT transformFeedbackProperties{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_PROPERTIES_EXT,
          .pNext = &performanceQueryProperties
      };
      VkPhysicalDeviceMultiviewPerViewAttributesPropertiesNVX multiviewPerViewAttributesProperties{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PER_VIEW_ATTRIBUTES_PROPERTIES_NVX,
          .pNext = &transformFeedbackProperties
      };
      VkPhysicalDeviceDiscardRectanglePropertiesEXT discardRectangleProperties{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DISCARD_RECTANGLE_PROPERTIES_EXT,
          .pNext = &multiviewPerViewAttributesProperties
      };
      VkPhysicalDeviceConservativeRasterizationPropertiesEXT conservativeRasterizationProperties{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CONSERVATIVE_RASTERIZATION_PROPERTIES_EXT,
          .pNext = &discardRectangleProperties
      };
      VkPhysicalDeviceInlineUniformBlockPropertiesEXT inlineUniformBlockProperties{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_PROPERTIES_EXT,
          .pNext = &conservativeRasterizationProperties
      };
      VkPhysicalDeviceSampleLocationsPropertiesEXT sampleLocationsProperties{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLE_LOCATIONS_PROPERTIES_EXT,
          .pNext = &inlineUniformBlockProperties
      };
      VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT blendOperationAdvancedProperties{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_PROPERTIES_EXT,
          .pNext = &sampleLocationsProperties
      };
      VkPhysicalDeviceShaderSMBuiltinsPropertiesNV shaderSmBuiltinsProperties{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SM_BUILTINS_PROPERTIES_NV,
          .pNext = &blendOperationAdvancedProperties
      };
      VkPhysicalDeviceShadingRateImagePropertiesNV shadingRateImageProperties{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADING_RATE_IMAGE_PROPERTIES_NV,
          .pNext = &shaderSmBuiltinsProperties
      };
      VkPhysicalDeviceRayTracingPropertiesKHR rayTracingPropertiesBETA{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PROPERTIES_KHR,
          .pNext = &shadingRateImageProperties
      };
      VkPhysicalDeviceRayTracingPropertiesNV rayTracingProperties{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PROPERTIES_NV,
          .pNext = &rayTracingPropertiesBETA
      };
      VkPhysicalDeviceExternalMemoryHostPropertiesEXT externalMemoryHostProperties{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_HOST_PROPERTIES_EXT,
          .pNext = &rayTracingProperties
      };
      VkPhysicalDeviceShaderCoreProperties2AMD shaderCoreProperties2{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CORE_PROPERTIES_2_AMD,
          .pNext = &externalMemoryHostProperties
      };
      VkPhysicalDeviceShaderCorePropertiesAMD shaderCoreProperties{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CORE_PROPERTIES_AMD,
          .pNext = &shaderCoreProperties2
      };
      VkPhysicalDeviceVertexAttributeDivisorPropertiesEXT vertexAttributeDivisorProperties{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_PROPERTIES_EXT,
          .pNext = &shaderCoreProperties
      };
      VkPhysicalDeviceMeshShaderPropertiesNV meshShaderProperties{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_PROPERTIES_NV,
          .pNext = &vertexAttributeDivisorProperties
      };
      VkPhysicalDeviceDeviceGeneratedCommandsPropertiesNV deviceGeneratedCommandsProperties{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_GENERATED_COMMANDS_PROPERTIES_NV,
          .pNext = &meshShaderProperties
      };
      VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT texelBufferAlignmentProperties{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_PROPERTIES_EXT,
          .pNext = &deviceGeneratedCommandsProperties
      };
      VkPhysicalDeviceRobustness2PropertiesEXT robustness2Properties{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_PROPERTIES_EXT,
          .pNext = &texelBufferAlignmentProperties
      };
      VkPhysicalDeviceCustomBorderColorPropertiesEXT customBorderColorProperties{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_BORDER_COLOR_PROPERTIES_EXT,
          .pNext = &robustness2Properties
      };
      VkPhysicalDeviceCooperativeMatrixPropertiesNV cooperativeMatrixProperties{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_PROPERTIES_NV,
          .pNext = &customBorderColorProperties
      };
      VkPhysicalDeviceMemoryBudgetPropertiesEXT memoryBudgetProperties{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_BUDGET_PROPERTIES_EXT,
          .pNext = &cooperativeMatrixProperties
      };
      VkPhysicalDeviceSubgroupSizeControlPropertiesEXT subgroupSizeControlProperties{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_PROPERTIES_EXT,
          .pNext = &memoryBudgetProperties
      };
      VkPhysicalDeviceFragmentDensityMap2PropertiesEXT fragmentDensityMap2Properties{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_2_PROPERTIES_EXT,
          .pNext = &subgroupSizeControlProperties
      };
      VkPhysicalDeviceFragmentDensityMapPropertiesEXT fragmentDensityMapProperties{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_PROPERTIES_EXT,
          .pNext = &fragmentDensityMap2Properties
      };
      VkPhysicalDevicePCIBusInfoPropertiesEXT pciBusInfoProperties{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PCI_BUS_INFO_PROPERTIES_EXT,
          .pNext = &fragmentDensityMapProperties
      };
      VkPhysicalDeviceLineRasterizationPropertiesEXT lineRasterizationProperties{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_PROPERTIES_EXT,
          .pNext = &pciBusInfoProperties
      };
      VkPhysicalDeviceMemoryProperties2       memoryProperties{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2,
          .pNext = &lineRasterizationProperties
      };
      VkPhysicalDeviceVulkan12Properties vulkan12Properties{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES,
          .pNext = &memoryProperties
      };
      VkPhysicalDeviceVulkan11Properties vulkan11Properties{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES,
          .pNext = &vulkan12Properties
      };
      VkPhysicalDeviceProperties2 deviceProperties{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
          .pNext = &vulkan11Properties
      };
#pragma endregion property structs
      api.getProperties(nativeHandle_, &deviceProperties);
#pragma region property initialization
      device.version.api = Vk::Version(deviceProperties.properties.apiVersion);
      device.version.conformance = vulkan12Properties.conformanceVersion;
      driver.version = Vk::Version(deviceProperties.properties.driverVersion);
      driver.vendor.id      = deviceProperties.properties.vendorID;
      device.id      = deviceProperties.properties.deviceID;
      device.type    = deviceProperties.properties.deviceType;
      device.name    = deviceProperties.properties.deviceName;
      std::memcpy(device.pipelineCacheUuid.data(), deviceProperties.properties.pipelineCacheUUID, VK_UUID_SIZE);

      const auto& limits = deviceProperties.properties.limits;
      const auto& sparse = deviceProperties.properties.sparseProperties;

      memory.image.dimension.x1D.max = limits.maxImageDimension1D;
      memory.image.dimension.x2D.max = limits.maxImageDimension2D;
      memory.image.dimension.x3D.max = limits.maxImageDimension3D;
      memory.image.dimension.cube.max = limits.maxImageDimensionCube;
      memory.image.arrayLayers.max   = limits.maxImageArrayLayers;
      resource.uniformBuffer.texel.elementCount.max = limits.maxTexelBufferElements;
      resource.storageBuffer.texel.elementCount.max = limits.maxTexelBufferElements;
      resource.uniformBuffer.range.max     = limits.maxUniformBufferRange;
      resource.storageBuffer.range.max     = limits.maxStorageBufferRange;
      capabilities.pushConstants.size.max  = limits.maxPushConstantsSize;
      memory.allocation.count.max = limits.maxMemoryAllocationCount;
      resource.sampler.allocationCount.max = limits.maxSamplerAllocationCount;
      memory.granularity = limits.bufferImageGranularity;
      memory.sparse.addressSpaceSize = limits.sparseAddressSpaceSize;
      resource.descriptor.binding.sets.max = limits.maxBoundDescriptorSets;
      resource.sampler.descriptor.perStage.max = limits.maxPerStageDescriptorSamplers;
      resource.uniformBuffer.descriptor.perStage.max = limits.maxPerStageDescriptorUniformBuffers;
      resource.storageBuffer.descriptor.perStage.max = limits.maxPerStageDescriptorStorageBuffers;
      resource.sampledImage.descriptor.perStage.max  = limits.maxPerStageDescriptorSampledImages;
      resource.storageImage.descriptor.perStage.max  = limits.maxPerStageDescriptorStorageImages;
      resource.input.descriptor.perStage.max = limits.maxPerStageDescriptorInputAttachments;
      resource.perStage.max = limits.maxPerStageResources;
      resource.sampler.descriptor.sets.max = limits.maxDescriptorSetSamplers;
      resource.uniformBuffer.descriptor.sets.max = limits.maxDescriptorSetUniformBuffers;
      resource.uniformBuffer.dynamic.descriptor.sets.max = limits.maxDescriptorSetUniformBuffersDynamic;
      resource.storageBuffer.descriptor.sets.max = limits.maxDescriptorSetStorageBuffers;
      resource.storageBuffer.dynamic.descriptor.sets.max = limits.maxDescriptorSetStorageBuffersDynamic;
      resource.sampledImage.descriptor.sets.max = limits.maxDescriptorSetSampledImages;
      resource.storageImage.descriptor.sets.max = limits.maxDescriptorSetStorageImages;
      resource.input.descriptor.sets.max = limits.maxDescriptorSetInputAttachments;

      features.shader.vertex.input.attributes.max = limits.maxVertexInputAttributes;
      features.shader.vertex.input.bindings.max   = limits.maxVertexInputBindings;
      features.shader.vertex.input.attributeOffset.max = limits.maxVertexInputAttributeOffset;
      features.shader.vertex.input.bindingStride.max = limits.maxVertexInputBindingStride;
      features.shader.vertex.output.components.max = limits.maxVertexOutputComponents;

      features.shader.tessellation.generationLevel.max = limits.maxTessellationGenerationLevel;
      features.shader.tessellation.patchSize.max = limits.maxTessellationPatchSize;
      features.shader.tessellation.control.input.componentsPerVertex.max = limits.maxTessellationControlPerVertexInputComponents;
      features.shader.tessellation.control.output.componentsPerVertex.max = limits.maxTessellationControlPerVertexOutputComponents;
      features.shader.tessellation.control.output.componentsPerPatch.max = limits.maxTessellationControlPerPatchOutputComponents;
      features.shader.tessellation.control.output.totalComponents.max    = limits.maxTessellationControlTotalOutputComponents;
      features.shader.tessellation.evaluation.input.components.max = limits.maxTessellationEvaluationInputComponents;
      features.shader.tessellation.evaluation.output.components.max = limits.maxTessellationEvaluationOutputComponents;

      features.shader.geometry.invocations.max = limits.maxGeometryShaderInvocations;
      features.shader.geometry.input.components.max = limits.maxGeometryInputComponents;
      features.shader.geometry.output.components.max = limits.maxGeometryOutputComponents;
      features.shader.geometry.output.vertices.max  = limits.maxGeometryOutputVertices;
      features.shader.geometry.output.totalComponents.max = limits.maxGeometryTotalOutputComponents;

      features.shader.fragment.input.components.max = limits.maxFragmentInputComponents;
      features.shader.fragment.output.attachments.max = limits.maxFragmentOutputAttachments;
      features.shader.fragment.dualSourceAttachments.max = limits.maxFragmentDualSrcAttachments;
      features.shader.fragment.output.combinedResources.max = limits.maxFragmentCombinedOutputResources;

      features.shader.compute.sharedMemorySize.max = limits.maxComputeSharedMemorySize;
      features.shader.compute.workGroup.count.x.max = limits.maxComputeWorkGroupCount[0];
      features.shader.compute.workGroup.count.y.max = limits.maxComputeWorkGroupCount[1];
      features.shader.compute.workGroup.count.z.max = limits.maxComputeWorkGroupCount[2];
      features.shader.compute.workGroup.invocations.max = limits.maxComputeWorkGroupInvocations;
      features.shader.compute.workGroup.size.x.max  = limits.maxComputeWorkGroupSize[0];
      features.shader.compute.workGroup.size.y.max  = limits.maxComputeWorkGroupSize[1];
      features.shader.compute.workGroup.size.z.max  = limits.maxComputeWorkGroupSize[2];

      properties.precisionBits.subPixel = limits.subPixelPrecisionBits;
      properties.precisionBits.subTexel = limits.subTexelPrecisionBits;
      properties.precisionBits.mipmap   = limits.mipmapPrecisionBits;

      render.draw.index.value.max = limits.maxDrawIndexedIndexValue;
      render.draw.indirect.count.max = limits.maxDrawIndirectCount;

      resource.sampler.lodBias.max = limits.maxSamplerLodBias;
      resource.sampler.anisotropy.max = limits.maxSamplerAnisotropy;

      properties.viewport.max = limits.maxViewports;
      properties.viewport.dimensions.x.max = limits.maxViewportDimensions[0];
      properties.viewport.dimensions.y.max = limits.maxViewportDimensions[1];
      properties.viewport.subPixelBits     = limits.viewportSubPixelBits;
      properties.viewport.boundsRange.min  = limits.viewportBoundsRange[0];
      properties.viewport.boundsRange.max  = limits.viewportBoundsRange[1];

      memory.map.alignment.min             = limits.minMemoryMapAlignment;
      resource.uniformBuffer.texel.offsetAlignment.min = limits.minTexelBufferOffsetAlignment;
      resource.storageBuffer.texel.offsetAlignment.min = limits.minTexelBufferOffsetAlignment;
      resource.uniformBuffer.offsetAlignment.min = limits.minUniformBufferOffsetAlignment;
      resource.storageBuffer.offsetAlignment.min = limits.minStorageBufferOffsetAlignment;

      properties.texel.offset.min = limits.minTexelOffset;
      properties.texel.offset.max = limits.maxTexelOffset;
      properties.texel.gatherOffset.min = limits.minTexelGatherOffset;
      properties.texel.gatherOffset.max = limits.maxTexelGatherOffset;
      properties.interpolation.offset.min = limits.minInterpolationOffset;
      properties.interpolation.offset.max = limits.maxInterpolationOffset;
      properties.interpolation.subpixelOffsetBits = limits.subPixelInterpolationOffsetBits;

      properties.framebuffer.width.max = limits.maxFramebufferWidth;
      properties.framebuffer.height.max = limits.maxFramebufferHeight;
      properties.framebuffer.layers.max = limits.maxFramebufferLayers;
      properties.framebuffer.sampleCounts.color = Vk::SampleCountFlags(limits.framebufferColorSampleCounts);
      properties.framebuffer.sampleCounts.depth = Vk::SampleCountFlags(limits.framebufferDepthSampleCounts);
      properties.framebuffer.sampleCounts.stencil = Vk::SampleCountFlags(limits.framebufferStencilSampleCounts);
      properties.framebuffer.sampleCounts.noAttachments = Vk::SampleCountFlags(limits.framebufferNoAttachmentsSampleCounts);
      render.blend.colorAttachments.max = limits.maxColorAttachments;

      resource.sampledImage.sampleCounts.color = Vk::SampleCountFlags(limits.sampledImageColorSampleCounts);
      resource.sampledImage.sampleCounts.integer = Vk::SampleCountFlags(limits.sampledImageIntegerSampleCounts);
      resource.sampledImage.sampleCounts.depth = Vk::SampleCountFlags(limits.sampledImageDepthSampleCounts);
      resource.sampledImage.sampleCounts.stencil = Vk::SampleCountFlags(limits.sampledImageStencilSampleCounts);
      resource.storageImage.sampleCounts         = Vk::SampleCountFlags(limits.storageImageSampleCounts);

      render.sampleMaskWords.max = limits.maxSampleMaskWords;

      properties.timestamp.period = limits.timestampPeriod;
      properties.timestamp.computeAndGraphics = limits.timestampComputeAndGraphics;

      features.shader.clipDistance.count.max = limits.maxClipDistances;
      features.shader.cullDistance.count.max = limits.maxCullDistances;
      features.shader.combinedClipAndCullDistance.count.max = limits.maxCombinedClipAndCullDistances;
      properties.discreteQueuePriorities = limits.discreteQueuePriorities;

      features.largePoints.size.min = limits.pointSizeRange[0];
      features.largePoints.size.max = limits.pointSizeRange[1];
      features.largePoints.granularity = limits.pointSizeGranularity;
      features.wideLines.width.min  = limits.lineWidthRange[0];
      features.wideLines.width.max  = limits.lineWidthRange[1];
      features.wideLines.granularity = limits.lineWidthGranularity;

      render.rasterize.line.strict = limits.strictLines;
      render.sampleLocation.standardLocations = limits.standardSampleLocations;
      memory.buffer.copy.optimalOffsetAlignment = limits.optimalBufferCopyOffsetAlignment;
      memory.buffer.copy.optimalRowPitchAlignment = limits.optimalBufferCopyRowPitchAlignment;
      memory.nonCoherentAtomSize = limits.nonCoherentAtomSize;



      memory.sparse.residency.alignedMipSize = sparse.residencyAlignedMipSize;
      memory.sparse.residency.nonResidentStrict = sparse.residencyNonResidentStrict;
      memory.sparse.residency.blockShape.standard2D = sparse.residencyStandard2DBlockShape;
      memory.sparse.residency.blockShape.standard2D.multisample = sparse.residencyStandard2DMultisampleBlockShape;
      memory.sparse.residency.blockShape.standard3D = sparse.residencyStandard3DBlockShape;




      std::memcpy(device.uuid.data(), vulkan11Properties.deviceUUID, VK_UUID_SIZE);
      std::memcpy(driver.uuid.data(), vulkan11Properties.driverUUID, VK_UUID_SIZE);
      std::memcpy(device.luid.data(), vulkan11Properties.deviceLUID, VK_LUID_SIZE);
      device.nodeMask = vulkan11Properties.deviceNodeMask;
      device.luidValid = vulkan11Properties.deviceLUIDValid;

      properties.subgroup.size = vulkan11Properties.subgroupSize;
      properties.subgroup.supportedStages = Vk::ShaderStageFlags(vulkan11Properties.subgroupSupportedStages);
      properties.subgroup.supportedOperations = Vk::SubgroupFeatureFlags(vulkan11Properties.subgroupSupportedOperations);
      properties.subgroup.quadOperationsInAllStages = vulkan11Properties.subgroupQuadOperationsInAllStages;

      render.pointClippingBehavior = vulkan11Properties.pointClippingBehavior;

      properties.multiview.count.max = vulkan11Properties.maxMultiviewViewCount;
      properties.multiview.instanceIndex.max = vulkan11Properties.maxMultiviewInstanceIndex;
      memory.protection.noFault = vulkan11Properties.protectedNoFault;

      resource.descriptor.perSet.max = vulkan11Properties.maxPerSetDescriptors;
      memory.allocation.size.max     = vulkan11Properties.maxMemoryAllocationSize;


      driver.id = vulkan12Properties.driverID;
      driver.name = vulkan12Properties.driverName;
      driver.info = vulkan12Properties.driverInfo;
      device.version.conformance = vulkan12Properties.conformanceVersion;

      capabilities.fenv.denorm.independence = vulkan12Properties.denormBehaviorIndependence;
      capabilities.fenv.roundingMode.independence = vulkan12Properties.roundingModeIndependence;
      capabilities.fenv.signedZeroInfNaN.preserve.f16 = vulkan12Properties.shaderSignedZeroInfNanPreserveFloat16;
      capabilities.fenv.signedZeroInfNaN.preserve.f32 = vulkan12Properties.shaderSignedZeroInfNanPreserveFloat32;
      capabilities.fenv.signedZeroInfNaN.preserve.f64 = vulkan12Properties.shaderSignedZeroInfNanPreserveFloat64;
      capabilities.fenv.denorm.preserve.f16 = vulkan12Properties.shaderDenormPreserveFloat16;
      capabilities.fenv.denorm.preserve.f32 = vulkan12Properties.shaderDenormPreserveFloat32;
      capabilities.fenv.denorm.preserve.f64 = vulkan12Properties.shaderDenormPreserveFloat64;
      capabilities.fenv.denorm.flushToZero.f16 = vulkan12Properties.shaderDenormFlushToZeroFloat16;
      capabilities.fenv.denorm.flushToZero.f32 = vulkan12Properties.shaderDenormFlushToZeroFloat32;
      capabilities.fenv.denorm.flushToZero.f64 = vulkan12Properties.shaderDenormFlushToZeroFloat64;
      capabilities.fenv.roundingMode.toEven.f16 = vulkan12Properties.shaderRoundingModeRTEFloat16;
      capabilities.fenv.roundingMode.toEven.f32 = vulkan12Properties.shaderRoundingModeRTEFloat32;
      capabilities.fenv.roundingMode.toEven.f64 = vulkan12Properties.shaderRoundingModeRTEFloat64;
      capabilities.fenv.roundingMode.toZero.f16 = vulkan12Properties.shaderRoundingModeRTZFloat16;
      capabilities.fenv.roundingMode.toZero.f32 = vulkan12Properties.shaderRoundingModeRTZFloat32;
      capabilities.fenv.roundingMode.toZero.f64 = vulkan12Properties.shaderRoundingModeRTZFloat64;


      resource.descriptor.updateAfterBind.max  = vulkan12Properties.maxUpdateAfterBindDescriptorsInAllPools;
      resource.uniformBuffer.arrayIndexing.nativeNonUniform = vulkan12Properties.shaderUniformBufferArrayNonUniformIndexingNative;
      resource.storageBuffer.arrayIndexing.nativeNonUniform = vulkan12Properties.shaderStorageBufferArrayNonUniformIndexingNative;
      resource.sampledImage.arrayIndexing.nativeNonUniform = vulkan12Properties.shaderSampledImageArrayNonUniformIndexingNative;
      resource.storageImage.arrayIndexing.nativeNonUniform = vulkan12Properties.shaderStorageImageArrayNonUniformIndexingNative;
      resource.input.arrayIndexing.nativeNonUniform = vulkan12Properties.shaderInputAttachmentArrayNonUniformIndexingNative;

      memory.buffer.robustAccess.updateAfterBind = vulkan12Properties.robustBufferAccessUpdateAfterBind;

      properties.quadDivergentImplicitLod = vulkan12Properties.quadDivergentImplicitLod;

      resource.sampler.descriptor.perStage.updateAfterBind.max = vulkan12Properties.maxPerStageDescriptorUpdateAfterBindSamplers;
      resource.uniformBuffer.descriptor.perStage.updateAfterBind.max = vulkan12Properties.maxPerStageDescriptorUpdateAfterBindUniformBuffers;
      resource.storageBuffer.descriptor.perStage.updateAfterBind.max = vulkan12Properties.maxPerStageDescriptorUpdateAfterBindStorageBuffers;
      resource.sampledImage.descriptor.perStage.updateAfterBind.max = vulkan12Properties.maxPerStageDescriptorUpdateAfterBindSampledImages;
      resource.storageImage.descriptor.perStage.updateAfterBind.max = vulkan12Properties.maxPerStageDescriptorUpdateAfterBindStorageImages;
      resource.input.descriptor.perStage.updateAfterBind.max = vulkan12Properties.maxPerStageDescriptorUpdateAfterBindInputAttachments;
      resource.perStage.updateAfterBind.max = vulkan12Properties.maxPerStageUpdateAfterBindResources;
      resource.sampler.descriptor.sets.updateAfterBind.max = vulkan12Properties.maxDescriptorSetUpdateAfterBindSamplers;
      resource.uniformBuffer.descriptor.sets.updateAfterBind.max = vulkan12Properties.maxDescriptorSetUpdateAfterBindUniformBuffers;
      resource.uniformBuffer.dynamic.descriptor.sets.updateAfterBind.max = vulkan12Properties.maxDescriptorSetUpdateAfterBindUniformBuffersDynamic;
      resource.storageBuffer.descriptor.sets.updateAfterBind.max = vulkan12Properties.maxDescriptorSetUpdateAfterBindStorageBuffers;
      resource.storageBuffer.dynamic.descriptor.sets.updateAfterBind.max = vulkan12Properties.maxDescriptorSetUpdateAfterBindStorageBuffersDynamic;
      resource.sampledImage.descriptor.sets.updateAfterBind.max = vulkan12Properties.maxDescriptorSetUpdateAfterBindSampledImages;
      resource.storageImage.descriptor.sets.updateAfterBind.max = vulkan12Properties.maxDescriptorSetUpdateAfterBindStorageImages;
      resource.input.descriptor.sets.updateAfterBind.max = vulkan12Properties.maxDescriptorSetUpdateAfterBindInputAttachments;

      capabilities.resolve.modes.depth = Vk::ResolveModeFlags(vulkan12Properties.supportedDepthResolveModes);
      capabilities.resolve.modes.stencil = Vk::ResolveModeFlags(vulkan12Properties.supportedStencilResolveModes);
      capabilities.resolve.independent = vulkan12Properties.independentResolve;
      capabilities.resolve.independentNone = vulkan12Properties.independentResolveNone;

      resource.sampler.filterMinMax.singleComponentFormats = vulkan12Properties.filterMinmaxSingleComponentFormats;
      resource.sampler.filterMinMax.imageComponentMapping = vulkan12Properties.filterMinmaxImageComponentMapping;

      features.timelineSemaphore.valueDifference.max = vulkan12Properties.maxTimelineSemaphoreValueDifference;

      properties.framebuffer.sampleCounts.integerColor = Vk::SampleCountFlags(vulkan12Properties.framebufferIntegerColorSampleCounts);

      auto memoryHeaps = std::span{memoryProperties.memoryProperties.memoryHeaps, memoryProperties.memoryProperties.memoryHeapCount};
      auto memoryTypes = std::span{memoryProperties.memoryProperties.memoryTypes, memoryProperties.memoryProperties.memoryTypeCount};

      for (const auto& heap : memoryHeaps)
        memory.heaps.emplace_back(heap);
      for (const auto& memType : memoryTypes)
        memory.heaps[memType.heapIndex].availableTypes.emplace_back(memType.propertyFlags);


      properties.precisionBits.lineSubPixel = lineRasterizationProperties.lineSubPixelPrecisionBits;

      device.pci.bus = pciBusInfoProperties.pciBus;
      device.pci.device = pciBusInfoProperties.pciDevice;
      device.pci.function = pciBusInfoProperties.pciFunction;
      device.pci.domain = pciBusInfoProperties.pciDomain;

      features.fragmentDensity.texelSize.min.width = fragmentDensityMapProperties.minFragmentDensityTexelSize.width;
      features.fragmentDensity.texelSize.min.height = fragmentDensityMapProperties.minFragmentDensityTexelSize.height;
      features.fragmentDensity.texelSize.max.width = fragmentDensityMapProperties.maxFragmentDensityTexelSize.width;
      features.fragmentDensity.texelSize.max.height = fragmentDensityMapProperties.maxFragmentDensityTexelSize.height;
      features.fragmentDensity.invocations = fragmentDensityMapProperties.fragmentDensityInvocations;

      features.fragmentDensity.subsampled.loads = fragmentDensityMap2Properties.subsampledLoads;
      features.fragmentDensity.subsampled.coarseReconstructionEarlyAccess = fragmentDensityMap2Properties.subsampledCoarseReconstructionEarlyAccess;
      features.fragmentDensity.subsampled.arrayLayers.max = fragmentDensityMap2Properties.maxSubsampledArrayLayers;
      features.fragmentDensity.subsampled.descriptorSetSamplers.max = fragmentDensityMap2Properties.maxDescriptorSetSubsampledSamplers;


      properties.subgroup.size.min = subgroupSizeControlProperties.minSubgroupSize;
      properties.subgroup.size.max = subgroupSizeControlProperties.maxSubgroupSize;
      features.shader.compute.workGroup.subgroups.max = subgroupSizeControlProperties.maxComputeWorkgroupSubgroups;
      properties.subgroup.requiredSizeStages = Vk::ShaderStageFlags(subgroupSizeControlProperties.requiredSubgroupSizeStages);

      for (uint32_t i = 0; i < memory.heaps.size(); ++i) {
        memory.heaps[i].budget = memoryBudgetProperties.heapBudget[i];
        memory.heaps[i].usage  = memoryBudgetProperties.heapUsage[i];
      }

      features.cooperativeMatrix.supportedStages = Vk::ShaderStageFlags(cooperativeMatrixProperties.cooperativeMatrixSupportedStages);

      resource.sampler.customBorderColor.max = customBorderColorProperties.maxCustomBorderColorSamplers;

      resource.uniformBuffer.robustAccessAlignment = robustness2Properties.robustUniformBufferAccessSizeAlignment;
      resource.storageBuffer.robustAccessAlignment = robustness2Properties.robustStorageBufferAccessSizeAlignment;

      resource.storageBuffer.texel.offsetAlignment.bytes = texelBufferAlignmentProperties.storageTexelBufferOffsetAlignmentBytes;
      resource.storageBuffer.texel.offsetAlignment.singleTexel = texelBufferAlignmentProperties.storageTexelBufferOffsetSingleTexelAlignment;
      resource.uniformBuffer.texel.offsetAlignment.bytes = texelBufferAlignmentProperties.uniformTexelBufferOffsetAlignmentBytes;
      resource.uniformBuffer.texel.offsetAlignment.singleTexel = texelBufferAlignmentProperties.uniformTexelBufferOffsetSingleTexelAlignment;


      features.shader.mesh.task.outputCount.max = meshShaderProperties.maxDrawMeshTasksCount;
      features.shader.mesh.task.workGroup.invocations.max = meshShaderProperties.maxTaskWorkGroupInvocations;
      features.shader.mesh.task.workGroup.size.x.max = meshShaderProperties.maxTaskWorkGroupSize[0];
      features.shader.mesh.task.workGroup.size.y.max = meshShaderProperties.maxTaskWorkGroupSize[1];
      features.shader.mesh.task.workGroup.size.z.max = meshShaderProperties.maxTaskWorkGroupSize[2];
      features.shader.mesh.task.totalMemorySize.max      = meshShaderProperties.maxTaskTotalMemorySize;
      features.shader.mesh.task.outputCount.max = meshShaderProperties.maxTaskOutputCount;
      features.shader.mesh.workGroup.invocations.max = meshShaderProperties.maxMeshWorkGroupInvocations;
      features.shader.mesh.workGroup.size.x.max = meshShaderProperties.maxMeshWorkGroupSize[0];
      features.shader.mesh.workGroup.size.y.max = meshShaderProperties.maxMeshWorkGroupSize[1];
      features.shader.mesh.workGroup.size.z.max = meshShaderProperties.maxMeshWorkGroupSize[2];
      features.shader.mesh.totalMemorySize.max = meshShaderProperties.maxMeshTotalMemorySize;
      features.shader.mesh.output.vertices.max = meshShaderProperties.maxMeshOutputVertices;
      features.shader.mesh.output.primitives.max = meshShaderProperties.maxMeshOutputPrimitives;
      features.shader.mesh.viewCount.max = meshShaderProperties.maxMeshMultiviewViewCount;
      features.shader.mesh.output.perVertexGranularity = meshShaderProperties.meshOutputPerVertexGranularity;
      features.shader.mesh.output.perPrimitiveGranularity = meshShaderProperties.meshOutputPerPrimitiveGranularity;


      memory.importedHostPointerAlignment.min = externalMemoryHostProperties.minImportedHostPointerAlignment;



      uint32_t           maxGraphicsShaderGroupCount;
      uint32_t           maxIndirectSequenceCount;
      uint32_t           maxIndirectCommandsTokenCount;
      uint32_t           maxIndirectCommandsStreamCount;
      uint32_t           maxIndirectCommandsTokenOffset;
      uint32_t           maxIndirectCommandsStreamStride;
      uint32_t           minSequencesCountBufferOffsetAlignment;
      uint32_t           minSequencesIndexBufferOffsetAlignment;
      uint32_t           minIndirectCommandsBufferOffsetAlignment;


      uint32_t           maxVertexAttribDivisor;


      uint32_t           shaderGroupHandleSize;
      uint32_t           maxRecursionDepth;
      uint32_t           maxShaderGroupStride;
      uint32_t           shaderGroupBaseAlignment;
      uint64_t           maxGeometryCount;
      uint64_t           maxInstanceCount;
      uint64_t           maxTriangleCount;
      uint32_t           maxDescriptorSetAccelerationStructures;

      /*uint32_t           shaderGroupHandleSize;
      uint32_t           maxRecursionDepth;
      uint32_t           maxShaderGroupStride;
      uint32_t           shaderGroupBaseAlignment;
      uint64_t           maxGeometryCount;
      uint64_t           maxInstanceCount;
      uint64_t           maxPrimitiveCount;
      uint32_t           maxDescriptorSetAccelerationStructures;
      uint32_t           shaderGroupHandleCaptureReplaySize;*/

      render.shadingRate.texelSize.width = shadingRateImageProperties.shadingRateTexelSize.width;
      render.shadingRate.texelSize.height = shadingRateImageProperties.shadingRateTexelSize.height;
      render.shadingRate.paletteSize = shadingRateImageProperties.shadingRatePaletteSize;
      render.shadingRate.coarseSamples.max = shadingRateImageProperties.shadingRateMaxCoarseSamples;

      device.coreCount = shaderSmBuiltinsProperties.shaderSMCount;
      device.subgroupsPerCore = shaderSmBuiltinsProperties.shaderWarpsPerSM;

      render.blend.advanced.colorAttachments.max = blendOperationAdvancedProperties.advancedBlendMaxColorAttachments;
      render.blend.advanced.independent = blendOperationAdvancedProperties.advancedBlendIndependentBlend;
      render.blend.advanced.nonPremultiplied.sourceColor = blendOperationAdvancedProperties.advancedBlendNonPremultipliedSrcColor;
      render.blend.advanced.nonPremultiplied.destinationColor = blendOperationAdvancedProperties.advancedBlendNonPremultipliedDstColor;
      render.blend.advanced.correlatedOverlap = blendOperationAdvancedProperties.advancedBlendCorrelatedOverlap;
      render.blend.advanced.allOperations = blendOperationAdvancedProperties.advancedBlendAllOperations;


      resource.inlineUniformBlock.size.max = inlineUniformBlockProperties.maxInlineUniformBlockSize;
      resource.inlineUniformBlock.descriptor.perStage.max = inlineUniformBlockProperties.maxPerStageDescriptorInlineUniformBlocks;
      resource.inlineUniformBlock.descriptor.perStage.updateAfterBind.max = inlineUniformBlockProperties.maxPerStageDescriptorUpdateAfterBindInlineUniformBlocks;
      resource.inlineUniformBlock.descriptor.set.max = inlineUniformBlockProperties.maxDescriptorSetInlineUniformBlocks;
      resource.inlineUniformBlock.descriptor.set.updateAfterBind.max = inlineUniformBlockProperties.maxDescriptorSetUpdateAfterBindInlineUniformBlocks;

      float              primitiveOverestimationSize;
      float              maxExtraPrimitiveOverestimationSize;
      float              extraPrimitiveOverestimationSizeGranularity;
      VkBool32           primitiveUnderestimation;
      VkBool32           conservativePointAndLineRasterization;
      VkBool32           degenerateTrianglesRasterized;
      VkBool32           degenerateLinesRasterized;
      VkBool32           fullyCoveredFragmentShaderInputVariable;
      VkBool32           conservativeRasterizationPostDepthCoverage;

      uint32_t           maxDiscardRectangles;

      VkBool32           perViewPositionAllComponents;

      uint32_t            physicalDeviceCount;
      VkPhysicalDevice    physicalDevices[VK_MAX_DEVICE_GROUP_SIZE];
      VkBool32            subsetAllocation;

      VkBool32           allowCommandBufferQueryCopies;

      uint32_t           maxPushDescriptors;

      uint32_t           minVertexInputBindingStrideAlignment;


      uint32_t toolCount = 0;
      auto toolResult = api.getToolProperties(nativeHandle_, &toolCount, nullptr);
      tools.resize(toolCount);
      for (auto&& tool : tools) {
        tool.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TOOL_PROPERTIES_EXT;
        tool.pNext = nullptr;
      }
      toolResult = api.getToolProperties(nativeHandle_, &toolCount, tools.data());


      uint32_t coopMatrixCount;
      auto coopMatrixResult = api.getCooperativeMatrixProperties(nativeHandle_, &coopMatrixCount, nullptr);
      coopMatrixProps.resize(coopMatrixCount);
      for (auto&& coopMat : coopMatrixProps) {
        coopMat.sType = VK_STRUCTURE_TYPE_COOPERATIVE_MATRIX_PROPERTIES_NV;
        coopMat.pNext = nullptr;
      }
      coopMatrixResult = api.getCooperativeMatrixProperties(nativeHandle_, &coopMatrixCount, coopMatrixProps.data());
#pragma endregion property initialization
    }

    PhysicalDeviceImpl(Vk::Instance* pInstance) noexcept :
        Vk::PhysicalDevice(pInstance){}



    void destroy() final {
      /*this->~PhysicalDeviceImpl();
      allocationCallbacks.pfnFree(allocationCallbacks.pUserData, this);*/
      delete this;
    }
    [[nodiscard]] uint64_t nativeHandle() const noexcept final{
      return (uint64_t)nativeHandle_;
    }


  };
  class DeviceImpl : public Vk::Device{

  public:

    VkDevice nativeHandle_;

    struct {
      PFN_vkDestroyDevice destroy;
      PFN_vkSetDebugUtilsObjectTagEXT setObjectTag;
      PFN_vkSetDebugUtilsObjectNameEXT setObjectName;
    } api;

    mutable std::atomic_uint32_t refCount;

    Vk::FlatSet<Vk::Handle<Vk::DeviceObject>> children;


    void destroy() final {
      api.destroy(nativeHandle_, &allocationCallbacks);
      delete this;
    }
    [[nodiscard]] uint64_t nativeHandle() const noexcept final{
      return (uint64_t)nativeHandle_;
    }
  };




  template <typename T, T ...EnumVals>
  class GetEnumArray
  {
    inline constexpr static T      InvalidValue = static_cast<T>(std::numeric_limits<std::underlying_type_t<T>>::max());
    inline constexpr static size_t Size = sizeof...(EnumVals);

    template <size_t I, T Val>
    constexpr static void fillArray(std::array<T, Size>& Arr, uint32_t Bits) noexcept
    {
      Arr[I] = (Bits & (0x1u << I)) ? Val : InvalidValue;
    }
    template <size_t ...I>
    constexpr static void fillArray(std::array<T, Size>& Arr, uint32_t Bits, std::index_sequence<I...>) noexcept
    {
      (fillArray<I, EnumVals>(Arr, Bits), ...);
    }

  public:

    constexpr std::pair<std::array<T, Size>, uint32_t> operator()(uint32_t Bits) const noexcept
    {
      std::array<T, Size> Values;
      fillArray(Values, Bits, std::make_index_sequence<Size>{});
      std::sort(Values.begin(), Values.end());
      uint32_t Length = std::ranges::find(Values, InvalidValue) - Values.begin();
      return {Values, Length};
    }
  };

  template <auto ...EnumVals>
  using getEnumArray = GetEnumArray<decltype((EnumVals , ...)), EnumVals...>;

  inline constexpr getEnumArray<
      VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT,
      VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_RESERVE_BINDING_SLOT_EXT,
      VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
      VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT>		getEnabledFeatures{};
  inline constexpr getEnumArray<
      VK_VALIDATION_FEATURE_DISABLE_ALL_EXT,
      VK_VALIDATION_FEATURE_DISABLE_SHADERS_EXT,
      VK_VALIDATION_FEATURE_DISABLE_THREAD_SAFETY_EXT,
      VK_VALIDATION_FEATURE_DISABLE_API_PARAMETERS_EXT,
      VK_VALIDATION_FEATURE_DISABLE_OBJECT_LIFETIMES_EXT,
      VK_VALIDATION_FEATURE_DISABLE_CORE_CHECKS_EXT,
      VK_VALIDATION_FEATURE_DISABLE_UNIQUE_HANDLES_EXT>	getDisabledFeatures{};

  struct DebugStateSetup
  {
    struct
    {
      union
      {
        struct
        {
          uint32_t gpuAssisted : 1;
          uint32_t gpuAssistedReserveBindingSlot : 1;
          uint32_t bestPractices : 1;
          uint32_t debugPrintf : 1;
          uint32_t all : 1;
          uint32_t shader : 1;
          uint32_t threadSafety : 1;
          uint32_t apiParameters : 1;
          uint32_t objectLifetimes : 1;
          uint32_t coreChecks : 1;
          uint32_t uniqueHandles : 1;
        };
        struct
        {
          uint32_t enabledFeatures  : 4;
          uint32_t disabledFeatures : 7;
        };
      };

    }validation;
  };

  template <std::derived_from<Vk::Object> VkType>
  class NullObject : public VkType{
  public:

    NullObject() noexcept = default;

    [[nodiscard]] NullObject* retain() const noexcept final {
      return (NullObject*)this;
    }
    [[nodiscard]] uint32_t    release() const noexcept final {
      return 1;
    }
    void destroy() final {}

    [[nodiscard]] Vk::ObjectType getType() const noexcept final { return Vk::ObjectType::Unknown; }
    [[nodiscard]] uint64_t nativeHandle() const noexcept final { return 0; }
    void setName(std::string_view ObjectName) noexcept final {
      std::cerr << "NullHandleException: called null->setName(\"" << ObjectName << "\")" << std::endl;
      std::terminate();
    }
    void setTaggedData(std::span<const std::byte> dataTag) noexcept final {
      std::cerr << "NullHandleException: called null->setTaggedData(...)" << std::endl;
      std::terminate();
    }

  };

  inline NullObject<Vk::Instance> nullInstance{};
  inline NullObject<Vk::PhysicalDevice> nullPhysicalDevice{};
  inline NullObject<Vk::Device> nullDevice{};
  inline NullObject<Vk::InstanceObject> nullInstanceObject{};
  inline NullObject<Vk::PhysicalDeviceObject> nullPhysicalDeviceObject{};
  inline NullObject<Vk::DeviceObject> nullDeviceObject{};
}

Vk::Instance* const Vk::Instance::Null = &nullInstance;
Vk::PhysicalDevice* const Vk::PhysicalDevice::Null = &nullPhysicalDevice;
Vk::Device* const Vk::Device::Null = &nullDevice;
Vk::InstanceObject* const Vk::InstanceObject::Null = &nullInstanceObject;
Vk::PhysicalDeviceObject* const Vk::PhysicalDeviceObject::Null = &nullPhysicalDeviceObject;
Vk::DeviceObject* const Vk::DeviceObject::Null = &nullDeviceObject;



void Vk::Debugger::enable(ValidationFeatures feature) noexcept
{
  auto* pState = reinterpret_cast<DebugStateSetup*>(&Data);
  switch(feature)
  {
    case ValidationFeatures::GpuAssisted:
      pState->validation.gpuAssisted = true;
      break;
    case ValidationFeatures::GpuAssistedReserveBindingSlot:
      pState->validation.gpuAssistedReserveBindingSlot = true;
      break;
    case ValidationFeatures::BestPractices:
      pState->validation.bestPractices = true;
      break;
    case ValidationFeatures::DebugPrintf:
      pState->validation.debugPrintf = true;
      break;

    case ValidationFeatures::All:
      pState->validation.all = false;
      break;
    case ValidationFeatures::Shaders:
      pState->validation.shader = false;
      break;
    case ValidationFeatures::ThreadSafety:
      pState->validation.threadSafety = false;
      break;
    case ValidationFeatures::ApiParameters:
      pState->validation.apiParameters = false;
      break;
    case ValidationFeatures::ObjectLifetimes:
      pState->validation.objectLifetimes = false;
      break;
    case ValidationFeaturesChecks:
      pState->validation.coreChecks = false;
      break;
    case ValidationFeatures::UniqueHandles:
      pState->validation.uniqueHandles = false;
      break;
  }
}
void Vk::Debugger::disable(ValidationFeatures feature) noexcept
{
  auto* pState = reinterpret_cast<DebugStateSetup*>(&Data);
  switch (feature)
  {
    case ValidationFeatures::GpuAssisted:
      pState->validation.gpuAssisted = false;
      break;
    case ValidationFeatures::GpuAssistedReserveBindingSlot:
      pState->validation.gpuAssistedReserveBindingSlot = false;
      break;
    case ValidationFeatures::BestPractices:
      pState->validation.bestPractices = false;
      break;
    case ValidationFeatures::DebugPrintf:
      pState->validation.debugPrintf = false;
      break;

    case ValidationFeatures::All:
      pState->validation.all = true;
      break;
    case ValidationFeatures::Shaders:
      pState->validation.shader = true;
      break;
    case ValidationFeatures::ThreadSafety:
      pState->validation.threadSafety = true;
      break;
    case ValidationFeatures::ApiParameters:
      pState->validation.apiParameters = true;
      break;
    case ValidationFeatures::ObjectLifetimes:
      pState->validation.objectLifetimes = true;
      break;
    case ValidationFeaturesChecks:
      pState->validation.coreChecks = true;
      break;
    case ValidationFeatures::UniqueHandles:
      pState->validation.uniqueHandles = true;
      break;
  }
}
bool Vk::Debugger::isEnabled(ValidationFeatures feature) const noexcept
{
  const auto* pState = reinterpret_cast<const DebugStateSetup*>(&Data);
  switch (feature)
  {
    case ValidationFeatures::GpuAssisted:
      return pState->validation.gpuAssisted;
    case ValidationFeatures::GpuAssistedReserveBindingSlot:
      return pState->validation.gpuAssistedReserveBindingSlot;
    case ValidationFeatures::BestPractices:
      return pState->validation.bestPractices;
    case ValidationFeatures::DebugPrintf:
      return pState->validation.debugPrintf;

    case ValidationFeatures::All:
      return !pState->validation.all;
      break;
    case ValidationFeatures::Shaders:
      return !pState->validation.shader;
      break;
    case ValidationFeatures::ThreadSafety:
      return !pState->validation.threadSafety;
    case ValidationFeatures::ApiParameters:
      return !pState->validation.apiParameters;
    case ValidationFeatures::ObjectLifetimes:
      return !pState->validation.objectLifetimes;
    case ValidationFeaturesChecks:
      return !pState->validation.coreChecks;
    case ValidationFeatures::UniqueHandles:
      return !pState->validation.uniqueHandles;
  }
}



Vk::Instance * Vk::Instance::retain() const noexcept {
  ((const InstanceImpl*)this)->refCount += 1;
  return (Vk::Instance*)this;
}
uint32_t Vk::Instance::release() const noexcept {
  return --((const InstanceImpl*)this)->refCount;
}
void Vk::Instance::attachChild(InstanceObject *pChild) noexcept {
  //((InstanceImpl*)this)->children.insert(Handle(pChild));
}
void Vk::Instance::removeChild(InstanceObject *pChild) noexcept {
  //((InstanceImpl*)this)->children.erase(Handle(pChild));
}
void Vk::Instance::setName(std::string_view ObjectName) noexcept {
  ((InstanceImpl*)this)->name = std::string(ObjectName);
}
void Vk::Instance::setTaggedData(std::span<const std::byte> dataTag) noexcept {
  ((InstanceImpl*)this)->taggedData.assign(dataTag.begin(), dataTag.end());
}
Vk::InstanceObject::~InstanceObject() {
  if (pInstance) {
    pInstance->removeChild(this);
  }
}


Vk::PhysicalDevice * Vk::PhysicalDevice::retain() const noexcept {
  ((const PhysicalDeviceImpl*)this)->refCount += 1;
  return (PhysicalDeviceImpl*)this;
}
uint32_t Vk::PhysicalDevice::release() const noexcept {
  return --((const PhysicalDeviceImpl*)this)->refCount;
}
void Vk::PhysicalDevice::attachChild(PhysicalDeviceObject *pChild) noexcept {
  //((PhysicalDeviceImpl*)this)->children.insert(Handle(pChild));
}
void Vk::PhysicalDevice::removeChild(PhysicalDeviceObject *pChild) noexcept {
  //((PhysicalDeviceImpl*)this)->children.erase(Handle(pChild));
}
void Vk::PhysicalDevice::setName(std::string_view ObjectName) noexcept {
  ((PhysicalDeviceImpl*)this)->name = std::string(ObjectName);
}
void Vk::PhysicalDevice::setTaggedData(std::span<const std::byte> dataTag) noexcept {
  ((PhysicalDeviceImpl*)this)->taggedData.assign(dataTag.begin(), dataTag.end());
}
void Vk::PhysicalDevice::init() {
  VkSurfaceCapabilities2EXT surfaceCapabilities{};

  auto* pThis = (PhysicalDeviceImpl*)this;

  pThis->initFunctions();
  pThis->initFeatures();
  pThis->initProperties();

  std::cout << serialize(pThis->device) << std::endl;
  std::cout << serialize(pThis->features) << std::endl;
  std::cout << serialize(pThis->memory) << std::endl;
}
Vk::PhysicalDeviceObject::~PhysicalDeviceObject(){
  if (pPhysicalDevice)
    pPhysicalDevice->removeChild(this);
}

Vk::Device * Vk::Device::retain() const noexcept {
  ((const DeviceImpl*)this)->refCount += 1;
  return (DeviceImpl*)this;
}
uint32_t Vk::Device::release() const noexcept {
  return --((const DeviceImpl*)this)->refCount;
}
void Vk::Device::attachChild(DeviceObject *pChild) noexcept {
  //((DeviceImpl*)this)->children.insert(Handle(pChild));
}
void Vk::Device::removeChild(DeviceObject *pChild) noexcept {
  //((DeviceImpl*)this)->children.erase(Handle(pChild));
}
void Vk::Device::setName(std::string_view ObjectName) noexcept {
  auto* tempBuffer = (char*)alloca(ObjectName.size() + 1);
  std::memcpy(tempBuffer, ObjectName.data(), ObjectName.size());
  tempBuffer[ObjectName.size()] = '\0';
  VkDebugUtilsObjectNameInfoEXT nameInfo{
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
      .pNext = nullptr,
      .objectType = VK_OBJECT_TYPE_DEVICE,
      .objectHandle = (uint64_t)((DeviceImpl*)this)->nativeHandle_,
      .pObjectName = tempBuffer
  };
  auto result = ((DeviceImpl*)this)->api.setObjectName(((DeviceImpl*)this)->nativeHandle_, &nameInfo);
  assert(!result);
}
void Vk::Device::setTaggedData(std::span<const std::byte> dataTag) noexcept {
  auto& device = *(DeviceImpl*)this;
  VkDebugUtilsObjectTagInfoEXT tagInfoStruct{
      .sType        = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_TAG_INFO_EXT,
      .pNext        = nullptr,
      .objectType   = VK_OBJECT_TYPE_DEVICE,
      .objectHandle = (uint64_t)device.nativeHandle_,
      .tagName      = 0,
      .tagSize      = dataTag.size_bytes(),
      .pTag         = dataTag.data()
  };
  auto result = device.api.setObjectTag(device.nativeHandle_, &tagInfoStruct);
  assert(!result);
}
Vk::DeviceObject::~DeviceObject() {
  if (pDevice) {
    pDevice->removeChild(this);
  }
}
void Vk::DeviceObject::setName(std::string_view ObjectName) noexcept {
  auto& device = *(DeviceImpl*)pDevice;
  auto* tempBuffer = (char*)alloca(ObjectName.size() + 1);
  std::memcpy(tempBuffer, ObjectName.data(), ObjectName.size());
  tempBuffer[ObjectName.size()] = '\0';
  VkDebugUtilsObjectNameInfoEXT nameInfo{
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
      .pNext = nullptr,
      .objectType = (VkObjectType)this->getType(),
      .objectHandle = (uint64_t)((DeviceImpl*)this)->nativeHandle_,
      .pObjectName = tempBuffer
  };
  auto result = ((DeviceImpl*)this)->api.setObjectName(((DeviceImpl*)this)->nativeHandle_, &nameInfo);
  assert(!result);
}
void Vk::DeviceObject::setTaggedData(std::span<const std::byte> dataTag) noexcept {
  auto& device = *(DeviceImpl*)pDevice;
  VkDebugUtilsObjectTagInfoEXT tagInfoStruct{
      .sType        = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_TAG_INFO_EXT,
      .pNext        = nullptr,
      .objectType   = (VkObjectType)getType(),
      .objectHandle = nativeHandle(),
      .tagName      = 0,
      .tagSize      = dataTag.size_bytes(),
      .pTag         = dataTag.data()
  };
  auto result = device.api.setObjectTag(device.nativeHandle_, &tagInfoStruct);
  assert(!result);
}

Vk::Handle<Vk::PhysicalDevice> Vk::Application::getPhysicalDevice() const noexcept{
  auto* inst = (InstanceImpl*)&*instance_;
  auto* physicalDevice = new PhysicalDeviceImpl(inst);
  physicalDevice->nativeHandle_ = inst->physicalDevices.front();
  physicalDevice->init();
  return Handle((Vk::PhysicalDevice*)physicalDevice);
}

void Vk::Application::init(Vk::Debugger debugger) {
  const Vk::Engine& engine = this->engine();
  DebugStateSetup debugParams = *(DebugStateSetup*)(void*)&debugger;

  auto&& [enabledFeatures, enabledFeaturesSize] = getEnabledFeatures(debugParams.validation.enabledFeatures);
  auto&& [disabledFeatures, disabledFeaturesSize] = getDisabledFeatures(debugParams.validation.disabledFeatures);

  VkApplicationInfo appInfo{
      .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pNext              = nullptr,
      .pApplicationName   = this->name().data(),
      .applicationVersion = (uint32_t)this->version(),
      .pEngineName        = engine.name().data(),
      .engineVersion      = (uint32_t)engine.version(),
      .apiVersion         = (uint32_t)engine.apiVersion(),
  };


  VkValidationFeaturesEXT validationFeatures{
      .sType                          = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT,
      .pNext                          = nullptr,
      .enabledValidationFeatureCount  = enabledFeaturesSize,
      .pEnabledValidationFeatures     = enabledFeatures.data(),
      .disabledValidationFeatureCount = disabledFeaturesSize,
      .pDisabledValidationFeatures    = disabledFeatures.data()
  };

  /*VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo{
    .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
    .pNext = &validationFeatures,
    .flags = 0,
    .
  };*/

  auto layers = this->enabledLayers();
  auto instanceExtensions = this->enabledInstanceExtensions();

  VkInstanceCreateInfo createInfo{
    .sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    .pNext                   = &validationFeatures,
    .flags                   = 0,
    .pApplicationInfo        = &appInfo,
    .enabledLayerCount       = (uint32_t)layers.size(),
    .ppEnabledLayerNames     = (const char* const *)layers.data(),
    .enabledExtensionCount   = (uint32_t)instanceExtensions.size(),
    .ppEnabledExtensionNames = (const char* const *)instanceExtensions.data()
  };
  auto* inst = new InstanceImpl();

  auto createResult = inst->api.create(&createInfo, &allocationCallbacks, &inst->nativeHandle_);
  inst->init();
  uint32_t physicalDeviceCount;
  auto deviceResult = inst->api.getPhysicalDevices(inst->nativeHandle_, &physicalDeviceCount, nullptr);
  inst->physicalDevices.resize(physicalDeviceCount);
  deviceResult = inst->api.getPhysicalDevices(inst->nativeHandle_, &physicalDeviceCount, inst->physicalDevices.data());

  instance_ = Handle((Vk::Instance*)inst);

  //VkResult createResult = vkCreateInstance(&createInfo, &allocationCallbacks, this->get<Instance>());
}