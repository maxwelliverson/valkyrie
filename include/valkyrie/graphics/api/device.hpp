//
// Created by Maxwell on 2020-11-09.
//

#ifndef VALKYRIE_GRAPHICS_API_DEVICE_HPP
#define VALKYRIE_GRAPHICS_API_DEVICE_HPP

#include <valkyrie/graphics/api/physical_device.hpp>
#include <valkyrie/graphics/api/queue.hpp>
#include <valkyrie/graphics/api/vulkan.hpp>

namespace valkyrie::graphics::api{

  class DeviceCreateInfo;

  class device : public VulkanObject{
    class Impl;
    Impl* pImpl;
  public:
    device(physical_device, const DeviceCreateInfo& createInfo);
    device(const device &);
    device(device &&) noexcept;



    native_handle_t nativeHandle() const noexcept;
    api_handle_t    apiHandle() const noexcept;



    friend bool operator==(const device & A, const device & B) noexcept {
      return A.pImpl == B.pImpl;
    }
    friend auto operator<=>(const device & A, const device & B) noexcept {
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
    constexpr DeviceCreateInfo(span<const DeviceQueueCreateInfo> queueCreateInfo, span<const utf8* const> enabledExtensions) noexcept
        : InputStruct((StructureType)3),
          queueCreateInfoCount(queueCreateInfo.size()),
          pQueueCreateInfos(queueCreateInfo.data()),
          enabledExtensionCount(enabledExtensions.size()),
          ppEnabledExtensionNames(enabledExtensions.data()){}

    constexpr DeviceCreateInfo& setQueueCreateInfo(span<DeviceQueueCreateInfo> createInfos) noexcept {
      this->queueCreateInfoCount = createInfos.size();
      this->pQueueCreateInfos = createInfos.data();
      return *this;
    }
    constexpr DeviceCreateInfo& setEnabledExtensions(span<const utf8*> extensions) noexcept {
      enabledExtensionCount = extensions.size();
      ppEnabledExtensionNames = extensions.data();
      return *this;
    }
  };

  void printFunctionTypeName();
}

#endif//VALKYRIE_GRAPHICS_API_DEVICE_HPP
