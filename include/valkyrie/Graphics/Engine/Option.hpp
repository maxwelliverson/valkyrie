//
// Created by Maxwell on 2020-11-15.
//

#ifndef VALKYRIE_OPTION_HPP
#define VALKYRIE_OPTION_HPP

#include <valkyrie/Core/Error/Status.hpp>
#include <valkyrie/Core/Utility/Arrays.hpp>
#include <valkyrie/Core/Utility/FlatMap.hpp>
#include <valkyrie/Core/Utility/StringView.hpp>
#include <valkyrie/Core/Utility/Version.hpp>

namespace valkyrie::Graphics::Engine{

  class Option;
  class Feature;
  class Property;
  class Features;
  class Properties;
  class EnumeratedSetting;
  class InstanceExtension;
  class DeviceExtension;
  class Configuration{
    struct InternalState;
    InternalState* pState;
  public:

  };


  class Option{
  public:
    virtual Core::Span<const Option*> dependsOn() const noexcept { return {}; }
    virtual bool isAvailable() const noexcept;

  };

  class Field{
  public:

  };
  class TextField : public Field{};
  class BooleanField : public Field{};
  class IntervalField : public Field{};
  class EnumeratedField : public Field{

  };
  class FieldGroup : public Field{
    Core::FlatSet<Field*> enabledFields;
    Core::FlatSet<Field*> disabledFields;
  };

  class Feature{
    Core::StringView name;
    Core::FlatSet<const Option*> enables;
    bool isSupported;
    bool isEnabled;
  };
  class Property{

  };

  class Features{

  };
  class Properties{

  };

  class Extension : public Option{
  public:
    virtual Core::StringView name() const noexcept = 0;
    virtual Core::Version specVersion() const noexcept = 0;
    virtual Features* features() const noexcept { return nullptr; }
    virtual Properties* properties() const noexcept { return nullptr; }
    virtual Core::Status setEnabled(bool value) noexcept;
    virtual bool getEnabled() const noexcept;
  };
  class Layer : public Option{};

  class InstanceExtension : public Extension{};
  class DeviceExtension : public Extension{};

}


#define PP_VK_impl_EXTENSION_EXPANDS_requires(...)





#define VK_instance_extension(name_, vendor_, version_, ...) inline constexpr static struct : InstanceExtension { \
  Core::StringView name() const noexcept override { return "VK_"#vendor_"_"#name_; }                                   \
  Core::Version specVersion() const noexcept override { return Core::Version(version_); }                         \
  } name_
#define VK_device_extension(name_, vendor_, version_, ...) inline constexpr static struct : DeviceExtension {  \
  Core::StringView name() const noexcept override { return "VK_"#vendor_"_"#name_; }                                   \
  Core::Version specVersion() const noexcept override { return Core::Version(version_); }                         \
   } name_;
//#define VK_layer(name_, vendor_, )

namespace valkyrie::Graphics::Engine::Options{
  /*VK_instance_extension(surface,                        KHR, 25);
  VK_instance_extension(surface_protected_capabilities, KHR, 1);
  VK_instance_extension(win32_surface,                  KHR, 6);
  VK_instance_extension(debug_report,                   EXT, 9);
  VK_instance_extension(debug_utils,                    EXT, 2);
  VK_instance_extension(swapchain_colorspace,           EXT, 4);

  VK_device_extension(  swapchain,                      KHR, 8);*/
}

#undef VK_instance_extension
#undef VK_device_extension
#undef VK_layer



#endif//VALKYRIE_OPTION_HPP
