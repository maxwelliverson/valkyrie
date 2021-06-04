//
// Created by maxwe on 2021-05-26.
//

#ifndef VALKYRIE_GRAPHICS_API_LIBRARY_HPP
#define VALKYRIE_GRAPHICS_API_LIBRARY_HPP

#include "vulkan.hpp"

#include <valkyrie/utility/library.hpp>
#include <valkyrie/utility/version.hpp>

namespace valkyrie::graphics::api{

  namespace impl{

    struct extension_properties{
      utf8 name[256];
      u32  version;
    };


    using PFN_voidFn = void(__stdcall*)();
    using PFN_getInstanceProcAddr = PFN_voidFn(__stdcall*)(void*, utf8_string);
    using PFN_enumerateInstanceVersion = vulkan_status_code(__stdcall*)(u32*);
    using PFN_enumerateInstanceExtensionProperties = vulkan_status_code(__stdcall*)(utf8_string, u32&, );
  }

  system_status initialize() noexcept;





}

#endif  //VALKYRIE_GRAPHICS_API_LIBRARY_HPP
