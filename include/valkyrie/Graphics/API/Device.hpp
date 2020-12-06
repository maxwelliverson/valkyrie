//
// Created by Maxwell on 2020-11-09.
//

#ifndef VALKYRIE_DEVICE_HPP
#define VALKYRIE_DEVICE_HPP

#include <valkyrie/Graphics/API/Vulkan.hpp>
#include <valkyrie/Graphics/API/PhysicalDevice.hpp>
#include <valkyrie/Graphics/API/Queue.hpp>

namespace valkyrie::Graphics::API{

  class DeviceCreateInfo;

  class Device : public VulkanObject{
    class Impl;
    Impl* pImpl;
  public:
    Device(PhysicalDevice, const DeviceCreateInfo& createInfo);
    Device(const Device&);
    Device(Device&&) noexcept;



    native_handle_t nativeHandle() const noexcept;
    api_handle_t    apiHandle() const noexcept;



    friend bool operator==(const Device& A, const Device& B) noexcept {
      return A.pImpl == B.pImpl;
    }
    friend auto operator<=>(const Device& A, const Device& B) noexcept {
      return A.pImpl <=> B.pImpl;
    }
  };

  class DeviceCreateInfo : public InputStruct{
    u32 queueCreateInfoCount = 0;
    const DeviceQueueCreateInfo* pQueueCreateInfos = 0;
    const u32 enabledLayerCount = 0;
    const utf8* const* const ppEnabledLayerNames = nullptr;
    u32 enabledExtensionCount = 0;
    const utf8* const* ppEnabledExtensionNames = nullptr;
    const void* const pEnabledFeatures = nullptr;
  public:
    constexpr DeviceCreateInfo() noexcept : InputStruct((StructureType)3){}
    constexpr DeviceCreateInfo(Core::Span<const DeviceQueueCreateInfo> queueCreateInfo, Core::Span<const utf8* const> enabledExtensions) noexcept
        : InputStruct((StructureType)3),
          queueCreateInfoCount(queueCreateInfo.size()),
          pQueueCreateInfos(queueCreateInfo.data()),
          enabledExtensionCount(enabledExtensions.size()),
          ppEnabledExtensionNames(enabledExtensions.data()){}

    constexpr DeviceCreateInfo& setQueueCreateInfo(Core::Span<DeviceQueueCreateInfo> createInfos) noexcept {
      this->queueCreateInfoCount = createInfos.size();
      this->pQueueCreateInfos = createInfos.data();
      return *this;
    }
    constexpr DeviceCreateInfo& setEnabledExtensions(Core::Span<const utf8*> extensions) noexcept {
      enabledExtensionCount = extensions.size();
      ppEnabledExtensionNames = extensions.data();
      return *this;
    }
  };

  void printFunctionTypeName();
}

#endif//VALKYRIE_DEVICE_HPP
