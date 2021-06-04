//
// Created by Maxwell on 2020-11-12.
//

#ifndef VALKYRIE_GRAPHICS_API_INSTANCE_HPP
#define VALKYRIE_GRAPHICS_API_INSTANCE_HPP


#include "vulkan.hpp"

#include <valkyrie/adt/arrays.hpp>

#include <valkyrie/utility/version.hpp>
#include <valkyrie/adt/string_view.hpp>

namespace valkyrie::graphics::api{

  /*enum class InstanceExtensionID : u32;
  enum class LayerID : u32;
  enum class ValidationFeatureID : u32;

  struct VersionedIdentifier{
    string_view name;
    version version;
  };

  class instance : public vulkan_object{
    class Impl;
    Impl* pImpl;
  public:
    //Instance();
    instance(const VersionedIdentifier& application, const VersionedIdentifier& engine);
    instance(const instance &);
    instance(instance &&) noexcept;
    instance & operator=(const instance &);
    instance & operator=(instance &&) noexcept;
    ~instance();


    static InstanceExtensionID parseExtension(std::string_view extName) noexcept;
    static LayerID parseLayer(std::string_view layerName) noexcept;


    surface createSurface(extent2d<> extents) const noexcept;


    status enable(InstanceExtensionID extensionId) noexcept;
    status disable(InstanceExtensionID extensionId) noexcept;
    status enable(LayerID layerId) noexcept;
    status disable(LayerID layerId) noexcept;
    status enable(ValidationFeatureID validationFeatureId) noexcept;
    status disable(ValidationFeatureID validationFeatureId) noexcept;

    bool isEnabled(InstanceExtensionID extensionId) const noexcept;
    bool isAvailable(InstanceExtensionID extensionId) const noexcept;
    bool isEnabled(LayerID extensionId) const noexcept;
    bool isAvailable(LayerID extensionId) const noexcept;
    bool isEnabled(ValidationFeatureID validationFeatureId) const noexcept;
    bool isAvailable(ValidationFeatureID validationFeatureId) const noexcept;

    span<const physical_device>   physicalDevices() const noexcept;
  };*/

  VK_declare_struct(instance_create_info);
  VK_declare_struct(application_info);
  VK_declare_struct(debug_utils_label_EXT);
  VK_declare_struct(debug_utils_object_name_info_EXT);
  VK_declare_struct(debug_utils_messenger_create_info_EXT);
  VK_declare_struct(validation_features_EXT);
  VK_declare_struct(debug_utils_messenger_callback_data_EXT);

  VK_define_in_struct(application_info){

    application_info() = default;
    application_info(utf8_string appName, version appVersion, utf8_string engName, version engVersion) noexcept
        : applicationName(appName),
          applicationVersion(appVersion),
          engineName(engName),
          engineVersion(engVersion)
    {}

    application_info& set_application_name(utf8_string name) noexcept {
      applicationName = name;
      return *this;
    }
    application_info& set_application_version(version appVersion) noexcept {
      applicationVersion = appVersion;
      return *this;
    }
    application_info& set_engine_name(utf8_string name) noexcept {
      engineName = name;
      return *this;
    }
    application_info& set_engine_version(version engVersion) noexcept {
      engineVersion = engVersion;
      return *this;
    }

  private:
    utf8_string        applicationName;
    version            applicationVersion;
    utf8_string        engineName;
    version            engineVersion;
    version            apiVersion = version(1, 2, 0);
  };

  VK_define_in_struct(instance_create_info){
    instance_create_info() = default;

    instance_create_info(const application_info& appInfo,
                         std::span<const cstring> enabledLayers,
                         std::span<const cstring> enabledExtensions) noexcept
        : pApplicationInfo(&appInfo),
          enabledLayerCount(enabledLayers.size()),
          ppEnabledLayerNames(enabledLayers.data()),
          enabledExtensionCount(enabledExtensions.size()),
          ppEnabledExtensionNames(enabledExtensions.data()){}

  private:
    u32                     flags = 0;
    const application_info* pApplicationInfo;
    u32                     enabledLayerCount;
    const cstring*          ppEnabledLayerNames;
    u32                     enabledExtensionCount;
    const cstring*          ppEnabledExtensionNames;
  };




  using PFN_debug_utils_messenger_callback_EXT = bool32(VK_stdcall*)(flag_bits::debug_message_severity_EXT severity,
                                                                     flag_bits::debug_message_type_EXT type,
                                                                     const debug_utils_messenger_callback_data_EXT* callbackData,
                                                                     void* userData);


  VK_define_in_struct(validation_features_EXT, instance_create_info){

    validation_features_EXT() = default;
    validation_features_EXT(std::span<const validation_feature_enable_EXT> enabledFeatures,
                            std::span<const validation_feature_disable_EXT> disabledFeatures) noexcept
        : enabledValidationFeatureCount(enabledFeatures.size()),
          pEnabledValidationFeatures(enabledFeatures.data()),
          disabledValidationFeatureCount(disabledFeatures.size()),
          pDisabledValidationFeatures(disabledFeatures.data())
    {}

    validation_features_EXT& set_enabled_features(std::span<const validation_feature_enable_EXT> enabledFeatures) noexcept {
      enabledValidationFeatureCount = enabledFeatures.size();
      pEnabledValidationFeatures = enabledFeatures.data();
      return *this;
    }
    validation_features_EXT& set_disabled_features(std::span<const validation_feature_disable_EXT> disabledFeatures) noexcept {
      disabledValidationFeatureCount = disabledFeatures.size();
      pDisabledValidationFeatures = disabledFeatures.data();
      return *this;
    }

  private:
    u32 enabledValidationFeatureCount = 0;
    const validation_feature_enable_EXT* pEnabledValidationFeatures = nullptr;
    u32 disabledValidationFeatureCount = 0;
    const validation_feature_disable_EXT* pDisabledValidationFeatures = nullptr;
  };
  VK_define_in_struct(debug_utils_messenger_create_info_EXT, instance_create_info){


    debug_utils_messenger_create_info_EXT() = default;
    debug_utils_messenger_create_info_EXT(debug_message_severity_flags_EXT       severities,
                                          debug_message_type_flags_EXT           types,
                                          PFN_debug_utils_messenger_callback_EXT callback,
                                          void*                                  userData) noexcept
        : messageSeverities(severities),
          messageTypes(types),
          callback(callback),
          pUserData(userData)
    {}

    debug_utils_messenger_create_info_EXT& set_severity_flags(debug_message_severity_flags_EXT severityFlags) noexcept {
      messageSeverities = severityFlags;
      return *this;
    }
    debug_utils_messenger_create_info_EXT& set_type_flags(debug_message_type_flags_EXT typeFlags) noexcept {
      messageTypes = typeFlags;
      return *this;
    }
    debug_utils_messenger_create_info_EXT& set_callback(PFN_debug_utils_messenger_callback_EXT pfnCallback, void* userData) noexcept {
      callback = pfnCallback;
      pUserData = userData;
      return *this;
    }

  private:
    u32                                    flags = 0;
    debug_message_severity_flags_EXT       messageSeverities;
    debug_message_type_flags_EXT           messageTypes;
    PFN_debug_utils_messenger_callback_EXT callback;
    void*                                  pUserData;
  };

  VK_define_in_struct(debug_utils_label_EXT){
    utf8_string label_name;
    struct {
      f32 r;
      f32 g;
      f32 b;
      f32 a;
    } color;
  };
  VK_define_in_struct(debug_utils_object_name_info_EXT){
    object_type objectType;
    u64         objectHandle;
    utf8_string name;
  };

  VK_define_in_struct(debug_utils_messenger_callback_data_EXT){
    u32                                     flags;
    utf8_string                             pMessageIdName;
    i32                                     messageIdNumber;
    utf8_string                             pMessage;
    u32                                     queueLabelCount;
    const debug_utils_label_EXT*            pQueueLabels;
    u32                                     cmdBufLabelCount;
    const debug_utils_label_EXT*            pCmdBufLabels;
    u32                                     objectCount;
    const debug_utils_object_name_info_EXT* pObjects;
  };

}

#endif//VALKYRIE_GRAPHICS_API_INSTANCE_HPP
