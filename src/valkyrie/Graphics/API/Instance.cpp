//
// Created by Maxwell on 2020-11-09.
//

#include <valkyrie/Graphics/API/Device.hpp>
#include <valkyrie/Graphics/API/Instance.hpp>
#include <valkyrie/Graphics/API/Queue.hpp>
#include <valkyrie/Graphics/API/Surface.hpp>

#include <valkyrie/Core/Utility/Arrays.hpp>
#include <valkyrie/Core/Utility/FlatSet.hpp>
#include <valkyrie/Core/Utility/StringView.hpp>
#include <valkyrie/Core/Utility/Version.hpp>

#include <string>

#include "FunctionPointers.hpp"
#include "StatusWrappers.hpp"
#include "InternalStructures.hpp"

using namespace valkyrie::Graphics::API;
namespace Core = valkyrie::Core;


class Instance::Impl : public Internal::InstanceImpl{

};

Surface Instance::createSurface(Extent2D<> extents) const noexcept {
  return {*this, extents};
}


/*

class Instance::Impl{
  VkInstance handle;
  FreeFunctions freeFunctions;
  const InstanceFunctions* instanceFunctions;


  VkApplicationInfo appInfo;
  VkAllocationCallbacks allocationCallbacks;

  std::u8string appName;
  std::u8string engineName;
  Core::SmallArray<Core::StringView, 16> availableExtensions;
  Core::SmallArray<Core::StringView, 8> availableLayers;
  Core::FlatSet<Core::StringView> enabledExtensions;
  Core::FlatSet<Core::StringView> enabledLayers;
  Core::SmallArray<PhysicalDevice, 4> physicalDevices;
  Core::SmallArray<Surface, 4> surfaces;
  Core::Version version;

  const VkAllocationCallbacks* pAllocationCallbacks = nullptr;


  inline static void initFunctions(Impl& impl){
    static const InstanceFunctions instanceFuncs{InstanceFunctionLoader(impl.handle, impl.freeFunctions.loader.getLoaderFunction())};
    impl.instanceFunctions = &instanceFuncs;
  }

public:
  Impl(Core::StringView appName, Core::Version appVersion, Core::StringView engineName, Core::Version engineVersion) :
    appName(appName.data(), appName.byteLength()), engineName(engineName.data(), engineName.byteLength()){
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = (const char*)this->appName.data();
    appInfo.pEngineName = (const char*)this->engineName.data();
    appInfo.applicationVersion = static_cast<u32>(appVersion);
    appInfo.engineVersion = static_cast<u32>(engineVersion);


    uint32_t instanceExtSize = 0;
    uint32_t layerSize = 0;
    freeFunctions.enumerateLayers(&layerSize, nullptr);
    freeFunctions.enumerateInstanceExtensions(nullptr, &instanceExtSize, nullptr);
    auto* availLayers = (VkLayerProperties*)alloca(layerSize * sizeof(VkLayerProperties));
    auto* availExt    = (VkExtensionProperties*)alloca(instanceExtSize * sizeof(VkExtensionProperties));
    freeFunctions.enumerateLayers(&layerSize, availLayers);
    freeFunctions.enumerateInstanceExtensions(nullptr, &instanceExtSize, availExt);
    for (const auto& layer : Core::Span<VkLayerProperties>{availLayers, layerSize})
      availableLayers.emplace_back(layer.layerName);
    for (const auto& ext : Core::Span<VkExtensionProperties>{availExt, instanceExtSize})
      availableExtensions.emplace_back(ext.extensionName);
    uint32_t versionNum = 0;
    freeFunctions.getInstanceVersion(&versionNum);
    version = Core::decodeCompactVersion(versionNum);

    appInfo.apiVersion = versionNum;
  }

  void enableExtension(Core::StringView sv) noexcept{
    enabledExtensions.insert(sv);
  }
  void enableLayer(Core::StringView sv) noexcept {
    enabledLayers.insert(sv);
  }

  VulkanStatus init(){

    VkResult result;

    Core::SmallArray<const char*, 16> extensions{};
    Core::SmallArray<const char*, 8> layers{};

    for (const auto& ext : enabledExtensions)
      extensions.push_back(ext.c_str());
    for (const auto& layer : enabledLayers)
      layers.push_back(layer.c_str());


    VkInstanceCreateInfo instanceCreateInfo{
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = (u32)layers.size(),
        .ppEnabledLayerNames = layers.data(),
        .enabledExtensionCount = (u32)extensions.size(),
        .ppEnabledExtensionNames = extensions.data()
    };

    if ((result = freeFunctions.createInstance(&instanceCreateInfo, pAllocationCallbacks, &handle)))
      return result;

    initFunctions(*this);

    uint32_t physicalDeviceCount = 0;
    if (auto physicalDeviceResult = instanceFunctions->enumeratePhysicalDevices(handle, &physicalDeviceCount, nullptr))
      return physicalDeviceResult;
    auto* physicalDeviceArray = (VkPhysicalDevice*)alloca(physicalDeviceCount * sizeof(VkPhysicalDevice));
    if (auto physicalDeviceResult = instanceFunctions->enumeratePhysicalDevices(handle, &physicalDeviceCount, physicalDeviceArray))
      return physicalDeviceResult;
    Impl* copyImpl = this;
    for (auto device : Core::Span<VkPhysicalDevice>{physicalDeviceArray, physicalDeviceCount}) {
      physicalDevices.emplace_back(*reinterpret_cast<Instance*>(&copyImpl), device);
    }

    return result;
  }



  Core::Span<const Core::StringView> getEnabledExtensions() const noexcept {
    return { enabledExtensions.data(), enabledExtensions.size() };
  }
  Core::Span<const Core::StringView> getEnabledLayers() const noexcept {
    return { enabledExtensions.data(), enabledExtensions.size() };
  }
};


Core::Span<const Core::StringView> Instance::enabledExtensions() const noexcept {
  return pImpl->getEnabledExtensions();
}
Core::Span<const Core::StringView> Instance::enabledLayers() const noexcept {
  return pImpl->getEnabledLayers();
}*/
