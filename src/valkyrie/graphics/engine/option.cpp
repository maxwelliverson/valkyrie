//
// Created by Maxwell on 2020-11-24.
//

#include <valkyrie/graphics/engine/option.hpp>

#include "../api/function_pointers.hpp"

#include <map>
#include <memory_resource>

namespace engine = valkyrie::graphics::engine;

#define PP_VK_impl_EXTENSION_EXPANDS_requires(...)

#define PP_VK_impl_WRAP_FEATURE_NAME(name) feature(VK_string(#name))

#define VK_expand_features(...) VK_if(VK_not(VK_pack_is_empty(__VA_ARGS__))(feature features_[VK_get_arg_count(__VA_ARGS__)]{ \
    VK_foreach_delimit(PP_VK_impl_WRAP_FEATURE_NAME, VK_comma_delimiter, __VA_ARGS__) };))


#define VK_instance_extension(name_, vendor_, version_, ...) inline constexpr static struct : instance_extension { \
  string_view name() const noexcept override { return "VK_"#vendor_"_"#name_; }                                   \
  version specVersion() const noexcept override { return version(version_); }                         \
  } name_
#define VK_device_extension(name_, vendor_, version_, ...) inline constexpr static struct : device_extension {  \
  string_view name() const noexcept override { return "VK_"#vendor_"_"#name_; }                                   \
  version specVersion() const noexcept override { return version(version_); }                         \
   } name_
//#define VK_layer(name_, vendor_, )

namespace valkyrie::graphics::engine::options{
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
      class EXTDescriptorIndexing : public device_extension{};
      class KHRBufferDeviceAddress : public device_extension{};
      class KHRDeferredHostOperations : public device_extension{};
      class KHRAccelerationStructure : public device_extension{
        VK_expand_features(
            accelerationStructure,
            captureReplay,
            indirectBuild,
            hostCommands,
            descriptorBindingUpdateAfterBind
        )

        VK_constant size_t MaxSize = std::max({ sizeof(field<MaxU64>), sizeof(field<MaxU32>), sizeof(field<MinU32>) });

        template <typename T>
        using PaddedField = Padded<field<T>, MaxSize>;

        using MaxU64Field = Padded<field<MaxU64>, MaxSize>;
        using MaxU32Field = Padded<field<MaxU32>, MaxSize>;
        using MinU32Field = Padded<field<MinU32>, MaxSize>;

        MaxU64Field geometryCount;
        MaxU64Field instanceCount;
        MaxU64Field primitiveCount;
        MaxU32Field perStageDescriptor;
        MaxU32Field perStageDescriptorUpdateAfterBind;
        MaxU32Field descriptorSet;
        MaxU32Field descriptorSetUpdateAfterBind;
        MinU32Field scratchOffsetAlignment;

      public:
        constexpr KHRAccelerationStructure() noexcept : device_extension(VK_raw_string(KHR_acceleration_structure), version(11)){}

        array_ref<feature>     features() noexcept override {
          return features_;
        }
        array_ref<field<void>> properties() noexcept override {
          return array_ref<field<void>, 8>(&geometryCount);
        }

        device_extension* doGetDependencies(u32& arraySize) const noexcept override{
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


struct engine::configuration::InternalState{
  //std::pmr::map<u32, DeviceInfo>
};