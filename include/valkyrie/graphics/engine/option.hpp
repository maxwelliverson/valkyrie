//
// Created by Maxwell on 2020-11-15.
//

#ifndef VALKYRIE_GRAPHICS_OPTION_HPP
#define VALKYRIE_GRAPHICS_OPTION_HPP


#include <valkyrie/adt/array_ref.hpp>
#include <valkyrie/adt/flat_map.hpp>
#include <valkyrie/utility/version.hpp>
#include <valkyrie/adt/string.hpp>


namespace valkyrie::graphics::engine{

  class option;
  class Feature;
  class EnumeratedSetting;
  class InstanceExtension;
  class DeviceExtension;
  class Configuration{
    struct InternalState;
    InternalState* pState;
  public:

  };


  class option {
  public:
    virtual option * doGetDependencies(u32& arraySize) const noexcept = 0;
    virtual bool    doIsSupported()     const noexcept = 0;
  };

  template <std::derived_from<option> O>
  inline auto dependencies(const O& option) noexcept {
    u32 depSize = 0;
    auto* pDeps = option.doGetDependencies(depSize);
    return array_ref{pDeps, depSize};
  }

  class Enumeration{
    flat_map<u32, string> entries;
  };
  class BitFlags{
    flat_map<u32, string> entries;
  };
  class Group{};


  template <typename>
  class Field;
  template <>
  class Field<void> : public option {
    string_view name_;
  protected:
    constexpr Field() = default;
    constexpr explicit Field(string_view name) noexcept : name_(name){}
  public:
    string_view name() const noexcept {
      return {name_.data(), static_cast<u32>(name_.size())};
    }
    option * doGetDependencies(u32& arraySize) const noexcept override;
    bool    doIsSupported()     const noexcept override;
  };
  template <>
  class Field<string> : public Field<void>{
  public:
    Field() = default;
  };
  template <>
  class Field<bool> : public Field<void>{
  public:
    Field() = default;
  };
  template <typename T, typename ...Opt>
  class Field<Interval<T, Opt...>> : public Field<void>{
    Interval<T, Opt...> value_;
  public:
    constexpr Field() = default;
    constexpr Field(StringView name) noexcept : Field<void>(name), value_(){}

    constexpr auto& value() noexcept {
      return value_;
    }
    constexpr const auto& value() const noexcept {
      return value_;
    }
  };

  class EnumeratedField : public Field<void>{
    Enumeration enumeration;

  protected:
    EnumeratedField();
  };
  template <enumerator E>
  class Field<E> : public EnumeratedField{
  public:
    Field() = default;
  };

  class BitFlagField : public Field<void>{};

  template <bitflag_enum_type E>
  class Field<E> : public BitFlagField{
  public:
    Field() = default;
  };
  template <>
  class Field<Group> : public Field<void>{
    FlatSet<Field<void>*> enabledFields;
    FlatSet<Field<void>*> disabledFields;
  public:
    Field() = default;
  };

  class EnabledOption : public Option{
    StringView name_;
    bool isSupported_ = false;
    bool isEnabled_ = false;
    virtual status doSetEnabled(bool enabled) noexcept = 0;
  protected:
    constexpr explicit EnabledOption(StringView name) noexcept : name_(name){}
  public:
    inline StringView name() const noexcept {
      return {name_.data(), u32(name_.size())};
    }
    inline status enable(bool b = true)  noexcept {

      if (!doIsSupported())
        return code::NotSupported;

      auto result = this->doSetEnabled(b);

      if (result.success())
        isEnabled_ = b;

      return std::move(result);
    }
    inline status disable() noexcept {
      auto result = this->doSetEnabled(false);

      if (result.success())
        isEnabled_ = false;

      return std::move(result);
    }

    inline void setSupport(bool value) noexcept {
      isSupported_ = value;
    }
    bool    doIsSupported()     const noexcept override {
      return isSupported_;
    }
  };

  class Feature : public EnabledOption{

    status doSetEnabled(bool enabled) noexcept override;

  public:

    constexpr Feature(StringView name) noexcept : EnabledOption(name){}

    EnabledOption* doGetDependencies(u32& arraySize) const noexcept override;

  };

  class Extension : public EnabledOption{
    Version specVersion_;

    status doSetEnabled(bool enabled) noexcept override {

      for (Extension& dep : dependencies(*this)){
        auto result = dep.enable(enabled);
        if (result.failure())
          return std::move(result);
      }

      return code::Success;
    }

  protected:
    constexpr Extension(StringView name, Version specVersion) noexcept
        : EnabledOption(name), specVersion_(specVersion){}
  public:
    inline Version version() const noexcept {
      return specVersion_;
    }

    Extension* doGetDependencies(u32& arraySize) const noexcept override = 0;

    virtual array_ref<Feature>     features() noexcept;
    virtual array_ref<Field<void>> properties() noexcept;
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
    StringView name_;
    Version specVersion_;
    Version implVersion_;
    small_array<Extension*, 2> enabledExtensions_;
  public:
    inline StringView name() const noexcept {
      return name_;
    }
    inline Version implVersion() const noexcept {
      return implVersion_;
    }
    inline Version specVersion() const noexcept {
      return specVersion_;
    }

  };
}


#define PP_VK_impl_EXTENSION_EXPANDS_requires(...)

#define PP_VK_impl_WRAP_FEATURE_NAME(name) Feature(VK_string(#name))

#define VK_expand_features(...) VK_if(VK_not(VK_pack_is_empty(__VA_ARGS__))(Feature features_[VK_get_arg_count(__VA_ARGS__)]{ \
    VK_foreach_delimit(PP_VK_impl_WRAP_FEATURE_NAME, VK_comma_delimiter, __VA_ARGS__) };))


#define VK_instance_extension(name_, vendor_, version_, ...) inline constexpr static struct : InstanceExtension { \
  StringView name() const noexcept override { return "VK_"#vendor_"_"#name_; }                                   \
  Version specVersion() const noexcept override { return Version(version_); }                         \
  } name_
#define VK_device_extension(name_, vendor_, version_, ...) inline constexpr static struct : DeviceExtension {  \
  StringView name() const noexcept override { return "VK_"#vendor_"_"#name_; }                                   \
  Version specVersion() const noexcept override { return Version(version_); }                         \
   } name_
//#define VK_layer(name_, vendor_, )

namespace valkyrie::graphics::engine::Options{
  /*VK_instance_extension(surface,                        KHR, 25);
  VK_instance_extension(surface_protected_capabilities, KHR, 1);
  VK_instance_extension(win32_surface,                  KHR, 6);
  VK_instance_extension(debug_report,                   EXT, 9);
  VK_instance_extension(debug_utils,                    EXT, 2);
  VK_instance_extension(swapchain_colorspace,           EXT, 4);

  VK_device_extension(  swapchain,                      KHR, 8);*/

  namespace detail{

    namespace {
      template <size_t N>
      class Padding{
        char padding_BUFFER_[N];
        template <size_t ...I>
        constexpr explicit Padding(const Padding& other, std::index_sequence<I...>) noexcept : padding_BUFFER_{ other.padding_BUFFER_[I]... }{}
      public:
        constexpr Padding() = default;
        constexpr Padding(const Padding& other) noexcept : Padding(other, std::make_index_sequence<N>{}){}
      };
      template <>
      class Padding<0>{};

      template <size_t N>
      struct aligned_storage : std::aligned_storage<N>{};
      template <>
      struct aligned_storage<0> : std::aligned_storage<1>{};

      template <size_t N>
      using aligned_storage_t = typename aligned_storage<N>::type;

      template <typename T, size_t N>
      class Padded : public T/*, public Padding<N - sizeof(T)>*/{
        [[no_unique_address]] aligned_storage_t<N - sizeof(T)> paddingLMAO{};
      public:
        using T::T;
      };
      class EXTDescriptorIndexing : public DeviceExtension{};
      class KHRBufferDeviceAddress : public DeviceExtension{};
      class KHRDeferredHostOperations : public DeviceExtension{};
      class KHRAccelerationStructure : public DeviceExtension{
        VK_expand_features(
            accelerationStructure,
            captureReplay,
            indirectBuild,
            hostCommands,
            descriptorBindingUpdateAfterBind
        )

        VK_constant size_t MaxSize = std::max({ sizeof(Field<MaxU64>), sizeof(Field<MaxU32>), sizeof(Field<MinU32>) });

        template <typename T>
        using PaddedField = Padded<Field<T>, MaxSize>;

        using MaxU64Field = Padded<Field<MaxU64>, MaxSize>;
        using MaxU32Field = Padded<Field<MaxU32>, MaxSize>;
        using MinU32Field = Padded<Field<MinU32>, MaxSize>;

        MaxU64Field geometryCount;
        MaxU64Field instanceCount;
        MaxU64Field primitiveCount;
        MaxU32Field perStageDescriptor;
        MaxU32Field perStageDescriptorUpdateAfterBind;
        MaxU32Field descriptorSet;
        MaxU32Field descriptorSetUpdateAfterBind;
        MinU32Field scratchOffsetAlignment;

      public:
        constexpr KHRAccelerationStructure() noexcept : DeviceExtension(VK_raw_string(KHR_acceleration_structure), Version(11)){}

        array_ref<Feature> features() noexcept override {
          return features_;
        }
        array_ref<Field<void>> properties() noexcept override {
          return array_ref<Field<void>, 8>(&geometryCount);
        }

        DeviceExtension* doGetDependencies(u32& arraySize) const noexcept override{
          //EXTDescriptorIndexing
          //KHRBufferDeviceAddress
          //KHRDeferredHostOperations
          arraySize = 3;
          return nullptr;
        }
      };
    }


  }


  //inline constexpr static detail::KHRAccelerationStructure KHR_acceleration_structure;
}

#undef VK_instance_extension
#undef VK_device_extension
#undef VK_layer



#endif//VALKYRIE_GRAPHICS_OPTION_HPP
