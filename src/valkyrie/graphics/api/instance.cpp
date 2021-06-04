//
// Created by Maxwell on 2020-11-09.
//

#include <valkyrie/graphics/api/device.hpp>
#include <valkyrie/graphics/api/instance.hpp>
#include <valkyrie/graphics/api/queue.hpp>
#include <valkyrie/graphics/api/surface.hpp>

#include <valkyrie/adt/arrays.hpp>
#include <valkyrie/adt/flat_set.hpp>
#include <valkyrie/utility/version.hpp>
#include <valkyrie/adt/string_view.hpp>
#include <valkyrie/utility/shapes.hpp>

#include <string>

#include "function_pointers.hpp"
#include "internal_structures.hpp"
#include "status_wrappers.hpp"

using namespace valkyrie::graphics::api;
using namespace valkyrie;


class instance::Impl : public internal::instance_impl{

};

surface instance::createSurface(extent2d<> extents) const noexcept {
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
  small_vector<string_view, 16> availableExtensions;
  small_vector<string_view, 8> availableLayers;
  flat_set<string_view> enabledExtensions;
  flat_set<string_view> enabledLayers;
  small_vector<PhysicalDevice, 4> physicalDevices;
  small_vector<Surface, 4> surfaces;
  Version version;

  const VkAllocationCallbacks* pAllocationCallbacks = nullptr;


  inline static void initFunctions(Impl& impl){
    static const InstanceFunctions instanceFuncs{InstanceFunctionLoader(impl.handle, impl.freeFunctions.loader.getLoaderFunction())};
    impl.instanceFunctions = &instanceFuncs;
  }

public:
  Impl(string_view appName, Version appVersion, string_view engineName, Version engineVersion) :
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
    for (const auto& layer : span<VkLayerProperties>{availLayers, layerSize})
      availableLayers.emplace_back(layer.layerName);
    for (const auto& ext : span<VkExtensionProperties>{availExt, instanceExtSize})
      availableExtensions.emplace_back(ext.extensionName);
    uint32_t versionNum = 0;
    freeFunctions.getInstanceVersion(&versionNum);
    version = decodeCompactVersion(versionNum);

    appInfo.apiVersion = versionNum;
  }

  void enableExtension(string_view sv) noexcept{
    enabledExtensions.insert(sv);
  }
  void enableLayer(string_view sv) noexcept {
    enabledLayers.insert(sv);
  }

  vulkan_status init(){

    VkResult result;

    small_vector<const char*, 16> extensions{};
    small_vector<const char*, 8> layers{};

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
    for (auto device : span<VkPhysicalDevice>{physicalDeviceArray, physicalDeviceCount}) {
      physicalDevices.emplace_back(*reinterpret_cast<Instance*>(&copyImpl), device);
    }

    return result;
  }



  span<const string_view> getEnabledExtensions() const noexcept {
    return { enabledExtensions.data(), enabledExtensions.size() };
  }
  span<const string_view> getEnabledLayers() const noexcept {
    return { enabledExtensions.data(), enabledExtensions.size() };
  }
};


span<const string_view> Instance::enabledExtensions() const noexcept {
  return pImpl->getEnabledExtensions();
}
span<const string_view> Instance::enabledLayers() const noexcept {
  return pImpl->getEnabledLayers();
}*/
