//
// Created by Maxwell on 2020-11-12.
//

#ifndef VALKYRIE_VULKAN_OPTIONS_HPP
#define VALKYRIE_VULKAN_OPTIONS_HPP

#include <valkyrie/Core/Utility/Arrays.hpp>
#include <valkyrie/Core/Utility/StringView.hpp>

namespace valkyrie::Graphics::API{
  class Option{};
  class Features{};
  class Properties{};

  class Extension : public Option{};
  class Layer : public Option{};

  class InstanceExtension : public Extension{
    virtual
  };
  class DeviceExtension : public Extension{};

}

#define VK_instance_extension(name_, vendor_, version_, ...) inline constexpr static struct : InstanceExtension {   } name_;
#define VK_device_extension(name_, vendor_, version_, ...) inline constexpr static struct : DeviceExtension {   } name_;
#define VK_layer(name_, vendor_, )

namespace valkyrie::Graphics::API::Options{

}

#undef VK_instance_extension
#undef VK_device_extension
#undef VK_layer


#endif//VALKYRIE_VULKAN_OPTIONS_HPP
