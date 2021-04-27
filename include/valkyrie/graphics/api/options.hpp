//
// Created by Maxwell on 2020-11-12.
//

#ifndef VALKYRIE_VULKAN_OPTIONS_HPP
#define VALKYRIE_VULKAN_OPTIONS_HPP

#include <valkyrie/adt/Arrays.hpp>
#include <valkyrie/status/result.hpp>
#include <valkyrie/adt/string.hpp>
#include <valkyrie/adt/string_view.hpp>

namespace valkyrie::graphics::api{
  class Option{
  public:

    virtual array_ref<Option> doGetDependencies() const noexcept = 0;
  };
  class Feature : public Option{
    bool enabled_;
    bool supported_;
    String name_;

  public:
    inline StringView name() const noexcept {
      return {name_.data(), u32(name_.size())};
    }

  };
  template <typename>
  class Property;
  template <>
  class Property<void>{
    String name_;
    bool supported_;
  public:


    inline StringView name() const noexcept {
      return {name_.data(), u32(name_.size())};
    }

    virtual Maybe<utf8*>            writeToString(utf8* pString, u64 bufferLength) const noexcept = 0;
  };
  template <typename T>
  class Property : public Property<void>{

  };

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

namespace valkyrie::graphics::api::Options{

}

#undef VK_instance_extension
#undef VK_device_extension
#undef VK_layer


#endif//VALKYRIE_VULKAN_OPTIONS_HPP
