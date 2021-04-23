//
// Created by maxwe on 2021-04-23.
//

#ifndef VALKYRIE_INTERNAL_GRAPHICS_API_HPP
#define VALKYRIE_INTERNAL_GRAPHICS_API_HPP

#include <valkyrie/utility/library.hpp>

#if VK_system_windows
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define VK_USE_PLATFORM_WIN32_KHR
#elif VK_system_linux
#define VK_USE_PLATFORM_XCB_KHR
#elif VK_system_macos
#define VK_USE_PLATFORM_MACOS_MVK
#define VK_USE_PLATFORM_METAL_EXT
#endif

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>


#define PFN(functionName) const PFN_vk##functionName functionName

namespace valkyrie::graphics::api{

  extern library vulkan_lib;

  struct free_api{
    PFN(GetInstanceProcAddr);
    PFN(CreateInstance);
    PFN(EnumerateInstanceExtensionProperties);
    PFN(EnumerateInstanceLayerProperties);
    PFN(EnumerateInstanceVersion);

    free_api() : GetInstanceProcAddr(vulkan_lib.load("vkGetInstanceProcAddr")){}
  };
  struct instance_api{


  };
  class physical_device_api{};
  class queue_api{};
  class command_buffer_api{};
  class device_api{};
}

#endif  //VALKYRIE_INTERNAL_GRAPHICS_API_HPP
