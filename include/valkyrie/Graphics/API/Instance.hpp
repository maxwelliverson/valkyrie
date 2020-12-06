//
// Created by Maxwell on 2020-11-12.
//

#ifndef VALKYRIE_INSTANCE_HPP
#define VALKYRIE_INSTANCE_HPP

#include <valkyrie/Graphics/API/Vulkan.hpp>
#include <valkyrie/Graphics/API/PhysicalDevice.hpp>
#include <valkyrie/Core/Utility/Arrays.hpp>

#include <valkyrie/Core/Utility/StringView.hpp>
#include <valkyrie/Core/Utility/Version.hpp>
#include <valkyrie/Core/Error/Status.hpp>


namespace valkyrie::Graphics::API{

  enum class InstanceExtensionID : u32;
  enum class LayerID : u32;
  enum class ValidationFeatureID : u32;

  struct VersionedIdentifier{
    Core::StringView name;
    Core::Version    version;
  };

  class Instance : public VulkanObject{
    class Impl;
    Impl* pImpl;
  public:
    //Instance();
    Instance(const VersionedIdentifier& application, const VersionedIdentifier& engine);
    Instance(const Instance&);
    Instance(Instance&&) noexcept;
    Instance& operator=(const Instance&);
    Instance& operator=(Instance&&) noexcept;
    ~Instance();


    static InstanceExtensionID parseExtension(std::string_view extName) noexcept;
    static LayerID parseLayer(std::string_view layerName) noexcept;



    Surface createSurface(Extent2D<> extents) const noexcept;


    Core::Status enable(InstanceExtensionID extensionId) noexcept;
    Core::Status disable(InstanceExtensionID extensionId) noexcept;
    Core::Status enable(LayerID layerId) noexcept;
    Core::Status disable(LayerID layerId) noexcept;
    Core::Status enable(ValidationFeatureID validationFeatureId) noexcept;
    Core::Status disable(ValidationFeatureID validationFeatureId) noexcept;

    bool isEnabled(InstanceExtensionID extensionId) const noexcept;
    bool isAvailable(InstanceExtensionID extensionId) const noexcept;
    bool isEnabled(LayerID extensionId) const noexcept;
    bool isAvailable(LayerID extensionId) const noexcept;
    bool isEnabled(ValidationFeatureID validationFeatureId) const noexcept;
    bool isAvailable(ValidationFeatureID validationFeatureId) const noexcept;

    Core::Span<const PhysicalDevice>   physicalDevices() const noexcept;
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
    Core::Version applicationVersion{};
    const utf8*   engineName = u8"";
    Core::Version engineVersion{};
    Core::Version apiVersion{};
  public:
    constexpr ApplicationInfo() noexcept : InputStruct((StructureType)0){}

    constexpr ApplicationInfo& setName(Core::StringView appName) noexcept {
      applicationName = appName.data();
      return *this;
    }
    constexpr ApplicationInfo& setVersion(Core::Version version) noexcept {
      this->applicationVersion = version;
      return *this;
    }
    constexpr ApplicationInfo& setEngineName(Core::StringView engName) noexcept {
      engineName = engName.data();
      return *this;
    }
    constexpr ApplicationInfo& setEngineVersion(Core::Version version) noexcept {
      this->engineVersion = version;
      return *this;
    }
    constexpr ApplicationInfo& setApiVersion(Core::Version version) noexcept {
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

    constexpr ValidationFeatures& enable(Core::Span<ValidationFeatureEnable> enabled) noexcept {
      enabledValidationFeatureCount = enabled.size();
      pEnabledValidationFeatures = enabled.data();
      return *this;
    }
    constexpr ValidationFeatures& disable(Core::Span<ValidationFeatureDisable> disabled) noexcept {
      disabledValidationFeatureCount = disabled.size();
      pDisabledValidationFeatures = disabled.data();
      return *this;
    }
  };
}

#endif//VALKYRIE_INSTANCE_HPP
