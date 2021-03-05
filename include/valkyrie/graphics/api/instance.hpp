//
// Created by Maxwell on 2020-11-12.
//

#ifndef VALKYRIE_GRAPHICS_API_INSTANCE_HPP
#define VALKYRIE_GRAPHICS_API_INSTANCE_HPP

#include <valkyrie/adt/arrays.hpp>
#include <valkyrie/graphics/api/physical_device.hpp>
#include <valkyrie/graphics/api/vulkan.hpp>

#include <valkyrie/utility/version.hpp>
#include <valkyrie/adt/string_view.hpp>
#include <valkyrie/status/status_code.hpp>


namespace valkyrie::graphics::api{

  enum class InstanceExtensionID : u32;
  enum class LayerID : u32;
  enum class ValidationFeatureID : u32;

  struct VersionedIdentifier{
    string_view name;
    version version;
  };

  class instance : public VulkanObject{
    class Impl;
    Impl* pImpl;
  public:
    //Instance();
    instance(const VersionedIdentifier& application, const VersionedIdentifier& engine);
    instance(const instance &);
    instance(instance &&) noexcept;
    instance & operator=(const instance &);
    instance & operator=(instance &&) noexcept;
    ~instance();


    static InstanceExtensionID parseExtension(std::string_view extName) noexcept;
    static LayerID parseLayer(std::string_view layerName) noexcept;


    surface createSurface(Extent2D<> extents) const noexcept;


    Status enable(InstanceExtensionID extensionId) noexcept;
    Status disable(InstanceExtensionID extensionId) noexcept;
    Status enable(LayerID layerId) noexcept;
    Status disable(LayerID layerId) noexcept;
    Status enable(ValidationFeatureID validationFeatureId) noexcept;
    Status disable(ValidationFeatureID validationFeatureId) noexcept;

    bool isEnabled(InstanceExtensionID extensionId) const noexcept;
    bool isAvailable(InstanceExtensionID extensionId) const noexcept;
    bool isEnabled(LayerID extensionId) const noexcept;
    bool isAvailable(LayerID extensionId) const noexcept;
    bool isEnabled(ValidationFeatureID validationFeatureId) const noexcept;
    bool isAvailable(ValidationFeatureID validationFeatureId) const noexcept;

    Span<const physical_device>   physicalDevices() const noexcept;
  };

  enum class ValidationFeatureEnable{
    GpuAssisted = 0,
    GpuAssistedReserveBindingSlot = 1,
    BestPractices = 2,
    DebugPrintf = 3,
    SynchronizationValidation = 4,
  };
  enum class ValidationFeatureDisable{
    All = 0,
    Shaders = 1,
    ThreadSafety = 2,
    ApiParameters = 3,
    ObjectLifetimes = 4,
    CoreChecks = 5,
    UniqueHandles = 6,
  };

  class ApplicationInfo : public InputStruct{
    const utf8*   applicationName = u8"";
    version applicationVersion{};
    const utf8*   engineName = u8"";
    version engineVersion{};
    version apiVersion{};
  public:
    constexpr ApplicationInfo() noexcept : InputStruct((StructureType)0){}

    constexpr ApplicationInfo& setName(string_view appName) noexcept {
      applicationName = appName.data();
      return *this;
    }
    constexpr ApplicationInfo& setVersion(version version) noexcept {
      this->applicationVersion = version;
      return *this;
    }
    constexpr ApplicationInfo& setEngineName(string_view engName) noexcept {
      engineName = engName.data();
      return *this;
    }
    constexpr ApplicationInfo& setEngineVersion(version version) noexcept {
      this->engineVersion = version;
      return *this;
    }
    constexpr ApplicationInfo& setApiVersion(version version) noexcept {
      this->apiVersion = version;
      return *this;
    }
  };
  class ValidationFeatures : public InputStruct{
    u32 enabledValidationFeatureCount = 0;
    const ValidationFeatureEnable* pEnabledValidationFeatures = nullptr;
    u32 disabledValidationFeatureCount = 0;
    const ValidationFeatureDisable* pDisabledValidationFeatures = nullptr;
  public:
    constexpr ValidationFeatures() noexcept : InputStruct((StructureType)1000247000){}

    constexpr ValidationFeatures& enable(Span<ValidationFeatureEnable> enabled) noexcept {
      enabledValidationFeatureCount = enabled.size();
      pEnabledValidationFeatures = enabled.data();
      return *this;
    }
    constexpr ValidationFeatures& disable(Span<ValidationFeatureDisable> disabled) noexcept {
      disabledValidationFeatureCount = disabled.size();
      pDisabledValidationFeatures = disabled.data();
      return *this;
    }
  };
}

#endif//VALKYRIE_GRAPHICS_API_INSTANCE_HPP
