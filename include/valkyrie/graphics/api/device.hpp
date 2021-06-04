//
// Created by Maxwell on 2020-11-09.
//

#ifndef VALKYRIE_GRAPHICS_API_DEVICE_HPP
#define VALKYRIE_GRAPHICS_API_DEVICE_HPP

#include "vulkan.hpp"
#include "queue.hpp"

namespace valkyrie::graphics::api{

  VK_declare_struct(device_create_info);
  VK_declare_struct(device_diagnostics_config_create_info_NV);
  VK_declare_struct(device_group_device_create_info);
  VK_declare_struct(device_memory_overallocation_create_info_AMD);
  VK_declare_struct(device_private_data_create_info_EXT);
  VK_declare_struct(device_memory_report_create_info_EXT);
  VK_declare_struct(device_memory_report_callback_data_EXT);

  using PFN_device_memory_report_callback_EXT = void(VK_stdcall*)(const device_memory_report_callback_data_EXT* pCallbackData,
      void* pUserData);

  VK_define_in_struct(device_memory_report_callback_data_EXT){
    u32                                 flags;
    device_memory_report_event_type_EXT type;
    u64                                 memoryObjectId;
    u64                                 size;
    u64                                 objectType;
    u64                                 objectHandle;
    u32                                 heapIndex;
  };


  VK_define_in_struct(device_create_info){

    device_create_info() = default;
    device_create_info(std::span<const device_queue_create_info> queueCreateInfos,
                       std::span<const char* const> enabledExtensions) noexcept
        : queueCreateInfoCount(queueCreateInfos.size()),
          pQueueCreateInfos(queueCreateInfos.data()),
          enabledExtensionCount(enabledExtensions.size()),
          ppEnabledExtensionNames(enabledExtensions.data()){}

    device_create_info& set_queue_create_infos(std::span<const device_queue_create_info> queueCreateInfos) noexcept {
      queueCreateInfoCount = queueCreateInfos.size();
      pQueueCreateInfos    = queueCreateInfos.data();
      return *this;
    }
    device_create_info& set_enabled_extensions(std::span<const char* const> enabledExtensions) noexcept {
      enabledExtensionCount   = enabledExtensions.size();
      ppEnabledExtensionNames = enabledExtensions.data();
      return *this;
    }

  private:
    u32                             flags = 0;
    u32                             queueCreateInfoCount;
    const device_queue_create_info* pQueueCreateInfos;
    u32                             enabledLayerCount = 0;
    const char* const*              ppEnabledLayerNames = nullptr;
    u32                             enabledExtensionCount;
    const char* const*              ppEnabledExtensionNames;
    const void*                     pEnabledFeatures = nullptr;
  };




  VK_define_in_struct(device_diagnostics_config_create_info_NV,     device_create_info){

    device_diagnostics_config_create_info_NV() = default;
    explicit device_diagnostics_config_create_info_NV(device_diagnostics_config_flags_NV flags) noexcept
        : flags(flags){}

  private:
    device_diagnostics_config_flags_NV flags;
  };
  VK_define_in_struct(device_group_device_create_info,              device_create_info){

    device_group_device_create_info() = default;
    explicit device_group_device_create_info(std::span<const physical_device> physDevs) noexcept
        : physicalDeviceCount(physDevs.size()), pPhysicalDevices(physDevs.data()){}


  private:
    u32                    physicalDeviceCount = 0;
    const physical_device* pPhysicalDevices = nullptr;
  };
  VK_define_in_struct(device_memory_overallocation_create_info_AMD, device_create_info){

    device_memory_overallocation_create_info_AMD() = default;
    explicit device_memory_overallocation_create_info_AMD(memory_overallocation_behavior_AMD overallocationBehavior) noexcept
        : behavior(overallocationBehavior){}

    device_memory_overallocation_create_info_AMD& set_behaviour(memory_overallocation_behavior_AMD overallocationBehavior) noexcept {
      behavior = overallocationBehavior;
      return *this;
    }

  private:
    memory_overallocation_behavior_AMD behavior;
  };
  VK_define_in_struct(device_private_data_create_info_EXT,          device_create_info){

    device_private_data_create_info_EXT() = default;
    explicit device_private_data_create_info_EXT(u32 count) noexcept : private_data_slot_request_count(count){}

    device_private_data_create_info_EXT& set_slot_request_count(u32 count) noexcept {
      private_data_slot_request_count = count;
      return *this;
    }

  private:
    u32 private_data_slot_request_count;
  };
  VK_define_in_struct(device_memory_report_create_info_EXT,         device_create_info){

    device_memory_report_create_info_EXT() = default;
    device_memory_report_create_info_EXT(PFN_device_memory_report_callback_EXT callback, void* userData) noexcept
        : pCallback(callback), pUserData(userData){}

    device_memory_report_create_info_EXT& set_user_data(void* userData) noexcept {
      pUserData = userData;
      return *this;
    }
    device_memory_report_create_info_EXT& set_callback(PFN_device_memory_report_callback_EXT callback) noexcept {
      pCallback = callback;
      return *this;
    }

  private:
    u32 flags = 0;
    PFN_device_memory_report_callback_EXT pCallback;
    void* pUserData;
  };






  /*class device_create_info;

  class device : public physical_device_object{
    class Impl;
    Impl* pImpl;
  public:
    device(physical_device, const device_create_info& createInfo);
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

  class device_create_info : public in_structure{
    u32 queueCreateInfoCount = 0;
    const device_queue_create_info* pQueueCreateInfos = 0;
    const u32 enabledLayerCount = 0;
    const utf8* const* const ppEnabledLayerNames = nullptr;
    u32 enabledExtensionCount = 0;
    const utf8* const* ppEnabledExtensionNames = nullptr;
    const void* const pEnabledFeatures = nullptr;
  public:
    constexpr device_create_info() noexcept : in_structure(structure_type::device_create_info){}
    constexpr device_create_info(span<const device_queue_create_info> queueCreateInfo,
                                 span<const utf8* const> enabledExtensions) noexcept
        : in_structure(structure_type::device_create_info),
          queueCreateInfoCount(queueCreateInfo.size()),
          pQueueCreateInfos(queueCreateInfo.data()),
          enabledExtensionCount(enabledExtensions.size()),
          ppEnabledExtensionNames(enabledExtensions.data()){}

    constexpr device_create_info& set_queue_create_info(span<const device_queue_create_info> createInfos) noexcept {
      this->queueCreateInfoCount = createInfos.size();
      this->pQueueCreateInfos = createInfos.data();
      return *this;
    }
    constexpr device_create_info& set_enabled_extensions(span<const utf8* const> extensions) noexcept {
      enabledExtensionCount = extensions.size();
      ppEnabledExtensionNames = extensions.data();
      return *this;
    }
  };

  void printFunctionTypeName();*/
}

#endif//VALKYRIE_GRAPHICS_API_DEVICE_HPP
