//
// Created by Maxwell on 2020-11-15.
//

#ifndef VALKYRIE_GRAPHICS_OPTION_HPP
#define VALKYRIE_GRAPHICS_OPTION_HPP


#include <valkyrie/Core/Utility/Arrays.hpp>
#include <valkyrie/Core/Utility/FlatMap.hpp>
#include <valkyrie/Core/Utility/String.hpp>
#include <valkyrie/Core/Utility/Version.hpp>
#include <valkyrie/Core/Error/Maybe.hpp>

namespace valkyrie::Graphics::Engine{

  class Option;
  class Feature;
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
    virtual Option* doGetDependencies(u32& arraySize) const noexcept = 0;
    virtual bool    doIsSupported()     const noexcept = 0;
  };

  template <std::derived_from<Option> O>
  inline auto dependencies(const O& option) noexcept {
    u32 depSize = 0;
    auto* pDeps = option.doGetDependencies(depSize);
    return Core::ArrayRef{pDeps, depSize};
  }

  class Enumeration{
    Core::FlatMap<u32, Core::String> entries;
  };
  class BitFlags{
    Core::FlatMap<u32, Core::String> entries;
  };
  class Group{};


  template <typename>
  class Field;
  template <>
  class Field<void>{
    Core::StringView name_;
  protected:
    constexpr explicit Field(Core::StringView name) noexcept : name_(name){}
  public:
    Core::StringView name() const noexcept {
      return {name_.data(), static_cast<u32>(name_.size())};
    }
  };
  template <>
  class Field<Core::String> : public Field<void>{};
  template <>
  class Field<bool> : public Field<void>{};
  template <typename T, typename ...Opt>
  class Field<Core::Interval<T, Opt...>> : public Field<void>{
    Core::Interval<T, Opt...> value_;
  public:
    constexpr Field(Core::StringView name) noexcept : Field<void>(name), value_(){}

    constexpr auto& value() noexcept {
      return value_;
    }
    constexpr const auto& value() const noexcept {
      return value_;
    }
  };

  class EnumeratedField : Field<void>{
    Enumeration enumeration;

  protected:
    EnumeratedField();
  };
  template <Enum E>
  class Field<E> : public EnumeratedField{};
  class BitFlagField : public Field<void>{};
  template <BitFlagEnum E>
  class Field<E> : public BitFlagField{};
  template <>
  class Field<Group> : public Field<void>{
    Core::FlatSet<Field<void>*> enabledFields;
    Core::FlatSet<Field<void>*> disabledFields;
  };

  class EnabledOption : public Option{
    Core::StringView name_;
    bool isSupported_ = false;
    bool isEnabled_ = false;
    virtual Core::Status doSetEnabled(bool enabled) noexcept = 0;
  protected:
    constexpr explicit EnabledOption(Core::StringView name) noexcept : name_(name){}
  public:
    inline Core::StringView name() const noexcept {
      return {name_.data(), u32(name_.size())};
    }
    inline Core::Status enable(bool b = true)  noexcept {

      if (!isSupported())
        return Core::Code::NotSupported;

      auto result = this->doSetEnabled(b);

      if (result.success())
        isEnabled_ = b;

      return std::move(result);
    }
    inline Core::Status disable() noexcept {
      auto result = this->doSetEnabled(false);

      if (result.success())
        isEnabled_ = false;

      return std::move(result);
    }

    inline void setSupport(bool value) noexcept {
      isSupported_ = value;
    }
    inline bool isSupported() const noexcept {
      return isSupported_;
    }
  };

  class Feature : public EnabledOption{

    Core::Status doSetEnabled(bool enabled) noexcept override;

  public:

    constexpr Feature(Core::StringView name) noexcept : EnabledOption(name){}

    EnabledOption* doGetDependencies(u32& arraySize) const noexcept override;
  };

  class Extension : public EnabledOption{
    Core::Version specVersion_;

    Core::Status doSetEnabled(bool enabled) noexcept override {

      for (Extension& dep : dependencies(*this)){
        auto result = dep.enable(enabled);
        if (result.failure())
          return std::move(result);
      }

      return Core::Code::Success;
    }

  protected:
    constexpr Extension(Core::StringView name, Core::Version specVersion) noexcept
        : EnabledOption(name), specVersion_(specVersion){}
  public:
    inline Core::Version version() const noexcept {
      return specVersion_;
    }

    Extension* doGetDependencies(u32& arraySize) const noexcept override = 0;

    virtual Core::ArrayRef<Feature>     features() noexcept;
    virtual Core::ArrayRef<Field<void>> properties() noexcept;
  };





  class InstanceExtension : public Extension{
  public:
    using Extension::Extension;
  };
  class DeviceExtension : public Extension{
  public:
    using Extension::Extension;
  };

  class Layer : public Option{
    Core::StringView name_;
    Core::Version specVersion_;
    Core::Version implVersion_;
    Core::SmallArray<Extension*, 2> enabledExtensions_;
  public:
    inline Core::StringView name() const noexcept {
      return name_;
    }
    inline Core::Version implVersion() const noexcept {
      return implVersion_;
    }
    inline Core::Version specVersion() const noexcept {
      return specVersion_;
    }
  };
}


#define PP_VK_impl_EXTENSION_EXPANDS_requires(...)

#define PP_VK_impl_WRAP_FEATURE_NAME(name) Feature(VK_string(#name))

#define VK_expand_features(...) VK_if(VK_not(VK_pack_is_empty(__VA_ARGS__))(Feature features_[VK_get_arg_count(__VA_ARGS__)]{ \
    VK_foreach_delimit(PP_VK_impl_WRAP_FEATURE_NAME, VK_comma_delimiter, __VA_ARGS__) };))


#define VK_instance_extension(name_, vendor_, version_, ...) inline constexpr static struct : InstanceExtension { \
  Core::StringView name() const noexcept override { return "VK_"#vendor_"_"#name_; }                                   \
  Core::Version specVersion() const noexcept override { return Core::Version(version_); }                         \
  } name_
#define VK_device_extension(name_, vendor_, version_, ...) inline constexpr static struct : DeviceExtension {  \
  Core::StringView name() const noexcept override { return "VK_"#vendor_"_"#name_; }                                   \
  Core::Version specVersion() const noexcept override { return Core::Version(version_); }                         \
   } name_
//#define VK_layer(name_, vendor_, )

namespace valkyrie::Graphics::Engine::Options{
  /*VK_instance_extension(surface,                        KHR, 25);
  VK_instance_extension(surface_protected_capabilities, KHR, 1);
  VK_instance_extension(win32_surface,                  KHR, 6);
  VK_instance_extension(debug_report,                   EXT, 9);
  VK_instance_extension(debug_utils,                    EXT, 2);
  VK_instance_extension(swapchain_colorspace,           EXT, 4);

  VK_device_extension(  swapchain,                      KHR, 8);*/

  namespace Detail{

    namespace {
      template <size_t N>
      class Padding{
        char padding_BUFFER_[N];
      };
      template <>
      class Padding<0>{};
      template <typename T, size_t N>
      class Padded : public T, public Padding<N - sizeof(T)>{
      public:
        using T::T;
      };
      class KHRAccelerationStructure : public DeviceExtension{
        VK_expand_features(
            accelerationStructure,
            captureReplay,
            indirectBuild,
            hostCommands,
            descriptorBindingUpdateAfterBind
        )

        VK_constant size_t MaxSize = std::max({ sizeof(Field<Core::MaxU64>), sizeof(Field<Core::MaxU32>), sizeof(Field<Core::MinU32>) });

        template <typename T>
        using PaddedField = Padded<Field<T>, MaxSize>;


        PaddedField<Core::MaxU64> geometryCount;
        PaddedField<Core::MaxU64> instanceCount;
        PaddedField<Core::MaxU64> primitiveCount;
        PaddedField<Core::MaxU32> perStageDescriptor;
        PaddedField<Core::MaxU32> perStageDescriptorUpdateAfterBind;
        PaddedField<Core::MaxU32> descriptorSet;
        PaddedField<Core::MaxU32> descriptorSetUpdateAfterBind;
        PaddedField<Core::MinU32> scratchOffsetAlignment;

      public:
        constexpr KHRAccelerationStructure() noexcept : DeviceExtension(VK_raw_string(KHR_acceleration_structure), Core::Version(11)){}

        Core::ArrayRef<Feature> features() noexcept override {
          return features_;
        }
        Core::ArrayRef<Field<void>> properties() noexcept override {
          return Core::ArrayRef<Field<void>, 8, MaxSize>{ &geometryCount };
        }
      };
    }


  }


  inline constexpr static Detail::KHRAccelerationStructure KHR_acceleration_structure;
}

#undef VK_instance_extension
#undef VK_device_extension
#undef VK_layer



#endif//VALKYRIE_GRAPHICS_OPTION_HPP
