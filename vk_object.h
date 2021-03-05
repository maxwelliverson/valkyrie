//
// Created by Maxwell on 2020-11-04.
//

#ifndef VULKAN_WRAPPER_VK_OBJECT_H
#define VULKAN_WRAPPER_VK_OBJECT_H

#include "vk_util.h"

#include <bit>
#include <span>

/*extern "C" {
    using VkInstance = struct VkInstance_T*;
    using VkDevice = struct VkDevice_T*;
    using VkPhysicalDevice = struct VkPhysicalDevice_T*;
    using VkSurfaceKHR = struct VkSurfaceKHR_T*;
}*/

namespace Vk{
  enum class ObjectType{
    Unknown                  = 0x0000'0000,
    Instance                 = 0x0000'0001,
    PhysicalDevice           = 0x0000'0002,
    Device                   = 0x0000'0003,
    Queue                    = 0x0000'0004,
    Semaphore                = 0x0000'0005,
    CommandBuffer            = 0x0000'0006,
    Fence                    = 0x0000'0007,
    DeviceMemory             = 0x0000'0008,
    Buffer                   = 0x0000'0009,
    Image                    = 0x0000'000A,
    Event                    = 0x0000'000B,
    QueryPool                = 0x0000'000C,
    BufferView               = 0x0000'000D,
    ImageView                = 0x0000'000E,
    ShaderModule             = 0x0000'000F,
    PipelineCache            = 0x0000'0010,
    PipelineLayout           = 0x0000'0011,
    RenderPass               = 0x0000'0012,
    Pipeline                 = 0x0000'0013,
    DescriptorSetLayout      = 0x0000'0014,
    Sampler                  = 0x0000'0015,
    DescriptorPool           = 0x0000'0016,
    DescriptorSet            = 0x0000'0017,
    Framebuffer              = 0x0000'0018,
    CommandPool              = 0x0000'0019,
    SamplerYcbcrConversion   = 0x3B9D'2B60,
    DescriptorUpdateTemplate = 0x3B9C'1608,
    SurfaceKHR               = 0x3B9A'CA00,
    SwapchainKHR             = 0x3B9A'CDE8,
    DisplayKHR               = 0x3B9A'D1D0,
    DisplayModeKHR           = 0x3B9A'D1D1,
    DebugReportCallbackEXT   = 0x3B9A'F4F8,
    DebugUtilsMessengerEXT   = 0x3B9C'BE00,
    AccelerationStructureKHR = 0x3B9D'4E88,
    ValidationCacheEXT       = 0x3B9D'3B00,
    PerformanceConfigurationINTEL = 0x3B9D'FE50,
    DeferredOperationKHR     = 0x3B9E'E0E0,
    IndirectCommandsLayoutNV = 0x3B9F'0408,
    PrivateDataSlotEXT       = 0x3B9F'4A58,
    DescriptorUpdateTemplateKHR = DescriptorUpdateTemplate,
    SamplerYcbcrConversionKHR   = SamplerYcbcrConversion,
    AccelerationStructureNV     = AccelerationStructureKHR
  };
  enum class ValidationFeatures
  {
    GpuAssisted,
    GpuAssistedReserveBindingSlot,
    BestPractices,
    DebugPrintf,

    All,
    Shaders,
    ThreadSafety,
    ApiParameters,
    ObjectLifetimes,
    CoreChecks,
    UniqueHandles
  };

  class Resource{
    enum class StorageClass{
      StorageImage,
      SampledImage,
      UniformBuffer,
      UniformTexelBuffer,
      StorageBuffer,
      StorageTexelBuffer,
      InlineUniformBlock
    };
  };

  inline static constexpr ValidationFeatures GpuAssistedValidation = ValidationFeatures::GpuAssisted;
  inline static constexpr ValidationFeatures GpuAssistedReserveBindingSlotValidation = ValidationFeatures::GpuAssistedReserveBindingSlot;
  inline static constexpr ValidationFeatures BestPracticesValidation = ValidationFeatures::BestPractices;
  inline static constexpr ValidationFeatures DebugPrintf = ValidationFeatures::DebugPrintf;
  inline static constexpr ValidationFeatures AllValidation = ValidationFeatures::All;
  inline static constexpr ValidationFeatures ShaderValidation = ValidationFeatures::Shaders;
  inline static constexpr ValidationFeatures ThreadSafetyValidation = ValidationFeatures::ThreadSafety;
  inline static constexpr ValidationFeatures ApiParameterValidation = ValidationFeatures::ApiParameters;
  inline static constexpr ValidationFeatures ObjectLifetimeValidation = ValidationFeatures::ObjectLifetimes;
  inline static constexpr ValidationFeatures CoreValidation = ValidationFeaturesChecks;
  inline static constexpr ValidationFeatures HandleUniquenessValidation = ValidationFeatures::UniqueHandles;

  class Version {
    uint32_t Major : 10;
    uint32_t Minor : 10;
    uint32_t Patch : 12;

  public:

    constexpr Version() = default;
    constexpr explicit Version(uint32_t version_) noexcept :
        Major(version_ >> 22), Minor((version_ >> 12) & 0x0000'03FFu), Patch(version_ & 0x0000'0FFFu){}
    constexpr Version(uint32_t Major, uint32_t Minor, uint32_t Patch = 0) noexcept :
        Major(Major), Minor(Minor), Patch(Patch){}

    constexpr uint32_t major() const noexcept { return Major; }
    constexpr uint32_t minor() const noexcept { return Minor; }
    constexpr uint32_t patch() const noexcept { return Patch; }

    constexpr explicit operator uint32_t() const noexcept {
      return std::bit_cast<uint32_t>(*this);
    }

    friend constexpr bool operator==(Version A, Version B) noexcept = default;
    friend constexpr std::strong_ordering operator<=>(Version A, Version B) noexcept {
      return (uint32_t)A <=> (uint32_t)B;
    }
  };

  class Engine;

  class Option {
  public:
    virtual void enable(Engine& engine) const noexcept = 0;
  };
  class InstanceExtension {};
  class DeviceExtension {};
  class Layer {};

  enum class Necessity{
    Required,
    Preferred,
    NiceToHave,
    NotNeeded
  };

  class AppRequirement{
    const Option* pOption;
    Necessity necessity;
  };

  class Object;

  template <std::derived_from<Object> T>
  class Handle;


  class Abstract{
  public:
    Abstract() = delete;
    Abstract(const Abstract&) = delete;
    Abstract(Abstract&&) = delete;
  };


  class Object{
    template <std::derived_from<Object> T>
    friend class Handle;

  protected:
    Object() = default;
    Object(const Object&) = default;
    Object(Object&&) = default;
    virtual ~Object() = default;

    [[nodiscard]] virtual Object* retain() const noexcept = 0;
    [[nodiscard]] virtual uint32_t release() const noexcept = 0;

  public:

    [[nodiscard]] virtual ObjectType getType() const noexcept = 0;
    [[nodiscard]] virtual uint64_t nativeHandle() const noexcept = 0;


    virtual void destroy() = 0;

    virtual void setName(std::string_view ObjectName) noexcept{}
    virtual void setTaggedData(std::span<const std::byte> dataTag) noexcept{}
  };

  class Instance;
  class PhysicalDevice;
  class Device;
  class Queue;
  class Semaphore;
  class CommandBuffer;
  class Fence;
  class DeviceMemory;
  class Buffer;
  class Image;
  class Event;
  class QueryPool;
  class BufferView;
  class ImageView;
  class ShaderModule;
  class PipelineCache;
  class PipelineLayout;
  class RenderPass;
  class Pipeline;
  class DescriptorSetLayout;
  class Sampler;
  class DescriptorPool;
  class DescriptorSet;
  class Framebuffer;
  class CommandPool;
  class SamplerYcbcrConversion;
  class DescriptorUpdateTemplate;
  class SurfaceKHR;
  class SwapchainKHR;
  class DisplayKHR;
  class DisplayModeKHR;
  class DebugReportCallbackEXT;
  class DebugUtilsMessengerEXT;
  class AccelerationStructureKHR;
  class ValidationCacheEXT;
  class PerformanceConfigurationINTEL;
  class DeferredOperationKHR;
  class IndirectCommandsLayoutNV;
  class PrivateDataSlotEXT;

  class Application;
  class PhysicalDevice;
  class Device;

  template <std::derived_from<Object> T>
  class Handle{

    template <std::derived_from<Object>>
    friend class Handle;

    inline static T* const NullHandle = T::Null;

  public:

    using pointer = T*;
    using Pointer = pointer;
    using reference = T&;
    using Reference = reference;

    Handle() = default;
    Handle(std::nullptr_t) noexcept{}
    explicit Handle(T* pObject) noexcept : pObject(pObject->retain()){}
    Handle(const Handle& Other) noexcept : pObject(Other.pObject->retain()){}
    Handle(Handle&& Other) noexcept{
      this->swap(Other);
    }

    template <typename U> requires(ConstructibleFrom<T*, U*>)
    explicit(!std::convertible_to<U*, T*>) Handle(const Handle<U>& Other) noexcept :
        pObject(static_cast<T*>(Other.pObject->retain())){}
    template <typename U> requires(ConstructibleFrom<T*, U*>)
    explicit(!std::convertible_to<U*, T*>) Handle(Handle<U>&& Other) noexcept :
        pObject(static_cast<T*>(Other.pObject)){
      Other.pObject = Handle<U>::NullHandle;
    }

    Handle& operator=(const Handle& Other) noexcept {
      if (this == &Other) return *this;
      this->~Handle();
      new(this) Handle(Other);
      return *this;
    }
    Handle& operator=(Handle&& Other) noexcept{
      this->~Handle();
      new(this) Handle(std::move(Other));
      return *this;
    }

    template <typename U> requires(std::convertible_to<U*, T*>)
    Handle& operator=(const Handle<U>& Other) noexcept {
      this->~Handle();
      new(this) Handle(Other);
      return *this;
    }
    template <typename U> requires(std::convertible_to<U*, T*>)
    Handle& operator=(Handle<U>&& Other) {
      this->~Handle();
      new(this) Handle(std::move(Other));
      return *this;
    }

    ~Handle() {
      if (pObject && !pObject->release()) {
        pObject->destroy();
      }
    }


    pointer operator->() const noexcept { return pObject; }
    reference operator*() const noexcept { return *pObject; }

    void reset(pointer pNewObject = NullHandle) noexcept {
      if (pNewObject == pObject) return;
      this->~Handle();
      new(this) Handle(pNewObject);
    }
    [[nodiscard]] pointer release() noexcept {
      pointer pTmp = pObject;
      pObject = NullHandle;
      return pTmp;
    }
    void swap(Handle& Other) noexcept{
      T* pTmp = pObject;
      pObject = Other.pObject;
      Other.pObject = pTmp;
    }

    friend bool operator==(const Handle& A, const Handle& B) noexcept = default;
    friend std::strong_ordering operator<=>(const Handle& A, const Handle& B) noexcept = default;
  private:
    pointer pObject = NullHandle;
  };




  /*class Instance*//* : public OwnedBy<Application>, public Owns<AFew<PhysicalDevice>, AFew<Device>>*//*{
    VkInstance hInstance;
  public:
    ~Instance();
  };
*/

  class Instance;
  class PhysicalDevice;
  class Device;
  class InstanceObject;
  class PhysicalDeviceObject;
  class DeviceObject;

  class Instance : public Object{
    template <std::derived_from<Object>>
    friend class Handle;
    static Instance* const Null;
  public:

    inline constexpr static ObjectType Type = ObjectType::Instance;

    [[nodiscard]] Instance* retain() const noexcept override;
    [[nodiscard]] uint32_t release() const noexcept override;


    void setName(std::string_view ObjectName) noexcept override;
    void setTaggedData(std::span<const std::byte> dataTag) noexcept override;





    void attachChild(InstanceObject* pChild) noexcept;
    void removeChild(InstanceObject* pChild) noexcept;

    [[nodiscard]] ObjectType getType() const noexcept override {
      return Type;
    }
  };

  class InstanceObject : public Object{
    template <std::derived_from<Object>>
    friend class Handle;
    static InstanceObject* const Null;
  protected:
    Instance* pInstance = Handle<Instance>().release();

    InstanceObject() noexcept = default;
    explicit InstanceObject(Instance* pInstance) noexcept : pInstance(pInstance){
      this->pInstance->attachChild(this);
    }
    ~InstanceObject() override;

  public:

    [[nodiscard]] InstanceObject* retain() const noexcept override = 0;


    [[nodiscard]] Handle<Instance> getInstance() const noexcept{
      return Handle(pInstance);
    }
  };

  class PhysicalDevice : public InstanceObject{
    template <std::derived_from<Object>>
    friend class Handle;
    static PhysicalDevice* const Null;
  protected:
    using InstanceObject::InstanceObject;
  public:

    PhysicalDevice() noexcept = default;

    inline constexpr static ObjectType Type = ObjectType::PhysicalDevice;

    [[nodiscard]] PhysicalDevice* retain() const noexcept override;
    [[nodiscard]] uint32_t release() const noexcept override;

    void attachChild(PhysicalDeviceObject* pChild) noexcept;
    void removeChild(PhysicalDeviceObject* pChild) noexcept;

    void init();

    void setName(std::string_view ObjectName) noexcept override;
    void setTaggedData(std::span<const std::byte> dataTag) noexcept override;

    [[nodiscard]] ObjectType getType() const noexcept override {
      return Type;
    }
  };

  class PhysicalDeviceObject : public Object{
    template <std::derived_from<Object>>
    friend class Handle;
    static PhysicalDeviceObject* const Null;
  protected:
    PhysicalDevice* pPhysicalDevice = Handle<Vk::PhysicalDevice>().release();

    PhysicalDeviceObject() = default;
    explicit PhysicalDeviceObject(PhysicalDevice* pPhysicalDevice) noexcept : pPhysicalDevice(pPhysicalDevice){
      this->pPhysicalDevice->attachChild(this);
    }
    ~PhysicalDeviceObject() override;

  public:

    [[nodiscard]] PhysicalDeviceObject* retain() const noexcept override = 0;

    [[nodiscard]] Handle<PhysicalDevice> getPhysicalDevice() const noexcept{
      return Handle(pPhysicalDevice);
    }
  };

  class Device : public PhysicalDeviceObject{
    template <std::derived_from<Object>>
    friend class Handle;
    static Device* const Null;
  public:
    inline constexpr static ObjectType Type = ObjectType::PhysicalDevice;

    [[nodiscard]] Device* retain() const noexcept override;
    [[nodiscard]] uint32_t release() const noexcept override;

    [[nodiscard]] ObjectType getType() const noexcept override {
      return Type;
    }

    void setName(std::string_view ObjectName) noexcept override;
    void setTaggedData(std::span<const std::byte> dataTag) noexcept override;

    void attachChild(DeviceObject* pChild) noexcept;
    void removeChild(DeviceObject* pChild) noexcept;
  };

  class DeviceObject : public Object{
    template <std::derived_from<Object>>
    friend class Handle;
    static DeviceObject* const Null;
  protected:
    Device* pDevice = Handle<Vk::Device>().release();

    DeviceObject() = default;
    explicit DeviceObject(Device* pDevice) noexcept : pDevice(pDevice){
      this->pDevice->attachChild(this);
    }
    ~DeviceObject() override;



  public:

    void setName(std::string_view ObjectName) noexcept override;
    void setTaggedData(std::span<const std::byte> dataTag) noexcept override;
    [[nodiscard]] DeviceObject* retain() const noexcept override = 0;

    [[nodiscard]] Handle<Device> getDevice() const noexcept{
      return Handle(pDevice);
    }
  };

  class Debugger {
    friend class Application;

    uint32_t Data;

  public:
    void enable(ValidationFeatures feature) noexcept;
    void disable(ValidationFeatures feature) noexcept;
    bool isEnabled(ValidationFeatures feature) const noexcept;
  };

  class Engine {
  protected:
    std::string_view name_;
    FlatSet<UniqueString> instanceExtensions_;
    FlatSet<UniqueString> enabledLayers_;
    FlatSet<UniqueString> deviceExtensions_;
    Version version_;
    Version api_version_;
  public:
    [[nodiscard]] std::string_view name() const noexcept { return name_; }
    [[nodiscard]] Version version() const noexcept { return version_; }
    [[nodiscard]] Version apiVersion() const noexcept { return api_version_; }

    [[nodiscard]] std::span<const UniqueString> enabledLayers() const noexcept{
      return {enabledLayers_.data(), enabledLayers_.size()};
    }
    [[nodiscard]] std::span<const UniqueString> instanceExtensions() const noexcept{
      return {instanceExtensions_.data(), instanceExtensions_.size()};
    }
    [[nodiscard]] std::span<const UniqueString> deviceExtensions() const noexcept{
      return {deviceExtensions_.data(), deviceExtensions_.size()};
    }
  };


  class HostAllocator {

  };

  class Application/* : public Owns<Instance> */{
    Handle<Instance> instance_;
  public:

    virtual ~Application() = default;

    virtual std::string_view name() const noexcept = 0;
    virtual Version version() const noexcept = 0;
    virtual const Engine& engine() const noexcept = 0;
    virtual void init(Debugger debug);

    [[nodiscard]] const Handle<Instance>& instance() const noexcept { return instance_; }

    [[nodiscard]] Handle<PhysicalDevice> getPhysicalDevice() const noexcept;

  private:
    virtual std::span<const UniqueString> enabledLayers() const noexcept = 0;
    virtual std::span<const UniqueString> enabledInstanceExtensions() const noexcept = 0;
  };
}

#endif //VULKAN_WRAPPER_VK_OBJECT_H
