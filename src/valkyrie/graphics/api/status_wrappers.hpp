//
// Created by Maxwell on 2020-11-18.
//

#ifndef VALKYRIE_STATUS_WRAPPERS_HPP
#define VALKYRIE_STATUS_WRAPPERS_HPP

#include <valkyrie/status/generic_code.hpp>
//#include <valkyrie/utility/Enums.hpp>

#include <vulkan/vulkan_core.h>
//#include <spirv-tools/libspirv.hpp>
//#include <shaderc/shaderc.hpp>
//#include <optix.h>
/*#include <cuda_runtime.h>
#include <cuda.h>*/

//#include "InternalStructures.hpp"

namespace valkyrie::graphics{
  class VkResultStatusDomain final : public status_domain{
  public:
    using value_type = VkResult;

    constexpr VkResultStatusDomain() noexcept : status_domain(uuid("a3138144-1344-43ea-bded-dd0385a12e4c")){}

    string_ref name() const noexcept override {
      return string_ref("Vulkan");
    }

    bool do_failure(const status_code<void>& status) const noexcept override {
      VK_assert(status.domain() == *this);
      return static_cast<const status_code<VkResultStatusDomain>&>(status).value() < 0;
    }
    bool do_equivalent(const status_code<void>& statusA, const status_code<void>& statusB) const noexcept override {
      VK_assert(statusA.domain() == *this);
        return (statusB.domain() == *this)
             && static_cast<const status_code<VkResultStatusDomain>&>(statusA).value() ==
                static_cast<const status_code<VkResultStatusDomain>&>(statusB).value();
    }
    code do_generic_code(const status_code<void>& status) const noexcept override {

      using code = code;

      VK_assert(status.domain() == *this);
      switch (static_cast<const status_code<VkResultStatusDomain>&>(status).value()) {
        case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS: return code::InvalidArgument;
        case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT: return code::LostResource;
        case VK_ERROR_NOT_PERMITTED_EXT: return code::PermissionDenied;
        case VK_ERROR_FRAGMENTATION: return code::TooFragmented;
        case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT: return code::IncompatibleResource;
        case VK_ERROR_INVALID_EXTERNAL_HANDLE: return code::InvalidResource;
        case VK_ERROR_OUT_OF_POOL_MEMORY: return code::DeviceOutOfMemory;
        case VK_ERROR_INVALID_SHADER_NV: return code::InvalidResource;
        case VK_ERROR_VALIDATION_FAILED_EXT: return code::FailedAssertion;
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: return code::IncompatibleDevice;
        case VK_ERROR_OUT_OF_DATE_KHR: return code::ResourceOutOfDate;
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: return code::ResourceUnavailable;
        case VK_ERROR_SURFACE_LOST_KHR: return code::LostResource;
        case VK_ERROR_FRAGMENTED_POOL: return code::TooFragmented;
        case VK_ERROR_FORMAT_NOT_SUPPORTED: return code::InvalidFormat;
        case VK_ERROR_TOO_MANY_OBJECTS: return code::ResourceLimitReached;
        case VK_ERROR_INCOMPATIBLE_DRIVER: return code::BadDriver;
        case VK_ERROR_FEATURE_NOT_PRESENT: return code::ResourceUnavailable;
        case VK_ERROR_EXTENSION_NOT_PRESENT: return code::ResourceUnavailable;
        case VK_ERROR_LAYER_NOT_PRESENT: return code::ResourceUnavailable;
        case VK_ERROR_MEMORY_MAP_FAILED: return code::DeviceBadTransfer;
        case VK_ERROR_DEVICE_LOST: return code::LostDevice;
        case VK_ERROR_INITIALIZATION_FAILED: return code::InitializationFailure;
        case VK_ERROR_OUT_OF_DEVICE_MEMORY: return code::DeviceOutOfMemory;
        case VK_ERROR_OUT_OF_HOST_MEMORY: return code::OutOfMemory;
        case VK_SUCCESS: return code::Success;
        case VK_NOT_READY: return code::NotReady;
        case VK_TIMEOUT: return code::TimedOut;
        case VK_EVENT_SET: return code::Set;
        case VK_EVENT_RESET: return code::Unset;
        case VK_INCOMPLETE: return code::InProgress;
        case VK_SUBOPTIMAL_KHR: return code::InvalidatedState;
        case VK_THREAD_IDLE_KHR: return code::Idle;
        case VK_THREAD_DONE_KHR: return code::Done;
        case VK_OPERATION_DEFERRED_KHR: return code::InProgress;
        case VK_OPERATION_NOT_DEFERRED_KHR: return code::Done;
        case VK_PIPELINE_COMPILE_REQUIRED_EXT: return code::FurtherActionsRequired;
        default: return code::Unknown;
      }
    }
    string_ref do_message(const status_code<void>& status) const noexcept override {
      VK_assert(status.domain() == *this);
      switch (static_cast<const status_code<VkResultStatusDomain>&>(status).value()) {
          case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS: return "Requested invalid opaque capture address";
          case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT: return "Lost fullscreen exclusive mode";
          case VK_ERROR_NOT_PERMITTED_EXT: return "Insufficient permission";
          case VK_ERROR_FRAGMENTATION: return "Allocation failed due to fragmentation";
          case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT: return "Plane layout incompatible with DRM format modifier";
          case VK_ERROR_INVALID_EXTERNAL_HANDLE: return "Invalid handle to external memory";
          case VK_ERROR_OUT_OF_POOL_MEMORY: return "Pool is out of memory";
          case VK_ERROR_INVALID_SHADER_NV: return "GLSL syntax error";
          case VK_ERROR_VALIDATION_FAILED_EXT: return "Validation failure";
          case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: return "Display is incompatible with current GPU";
          case VK_ERROR_OUT_OF_DATE_KHR: return "Resources are out of date";
          case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: return "Requested native window is already in use";
          case VK_ERROR_SURFACE_LOST_KHR: return "Lost presentation surface";
          case VK_ERROR_FRAGMENTED_POOL: return "Pool is too fragmented";
          case VK_ERROR_FORMAT_NOT_SUPPORTED: return "Format not supported by current GPU";
          case VK_ERROR_TOO_MANY_OBJECTS: return "Too many Vulkan objects";
          case VK_ERROR_INCOMPATIBLE_DRIVER: return "Incompatible driver";
          case VK_ERROR_FEATURE_NOT_PRESENT: return "Requested feature not supported";
          case VK_ERROR_EXTENSION_NOT_PRESENT: return "Requested extension not available";
          case VK_ERROR_LAYER_NOT_PRESENT: return "Requested layer not found";
          case VK_ERROR_MEMORY_MAP_FAILED: return "Memory map operation failed";
          case VK_ERROR_DEVICE_LOST: return "Lost GPU";
          case VK_ERROR_INITIALIZATION_FAILED: return "Vulkan failed to initialize properly";
          case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "Out of GPU memory";
          case VK_ERROR_OUT_OF_HOST_MEMORY: return "Out of system memory";
          case VK_SUCCESS: return "Success";
          case VK_NOT_READY: return "Not ready";
          case VK_TIMEOUT: return "Timed out";
          case VK_EVENT_SET: return "Event set";
          case VK_EVENT_RESET: return "Event reset";
          case VK_INCOMPLETE: return "Property enumeration incomplete";
          case VK_SUBOPTIMAL_KHR: return "Current swapchain is suboptimal";
          case VK_THREAD_IDLE_KHR: return "Idle";
          case VK_THREAD_DONE_KHR: return "Done";
          case VK_OPERATION_DEFERRED_KHR: return "Operation deferred";
          case VK_OPERATION_NOT_DEFERRED_KHR: return "Operation completed, not deferred";
          case VK_PIPELINE_COMPILE_REQUIRED_EXT: return "Pipeline compilation is required";
          default: return "Unknown error";
      }
    }

    severity do_severity(const status_code<void>& status) const noexcept override {
      VK_assert(status.domain() == *this);

      using Severity = Severity;

      switch (static_cast<const status_code<VkResultStatusDomain>&>(status).value()) {
        case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
        case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
        case VK_ERROR_NOT_PERMITTED_EXT:
        case VK_ERROR_FRAGMENTATION:
        case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
        case VK_ERROR_INVALID_EXTERNAL_HANDLE:
        case VK_ERROR_OUT_OF_POOL_MEMORY:
        case VK_ERROR_INVALID_SHADER_NV:
        case VK_ERROR_VALIDATION_FAILED_EXT:
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
        case VK_ERROR_OUT_OF_DATE_KHR:
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
        case VK_ERROR_SURFACE_LOST_KHR:
        case VK_ERROR_FRAGMENTED_POOL:
        case VK_ERROR_FORMAT_NOT_SUPPORTED:
        case VK_ERROR_TOO_MANY_OBJECTS:
        case VK_ERROR_INCOMPATIBLE_DRIVER:
        case VK_ERROR_FEATURE_NOT_PRESENT:
        case VK_ERROR_EXTENSION_NOT_PRESENT:
        case VK_ERROR_LAYER_NOT_PRESENT:
        case VK_ERROR_MEMORY_MAP_FAILED: return Severity::Error;



        case VK_NOT_READY:
        case VK_TIMEOUT:
        case VK_EVENT_SET:
        case VK_EVENT_RESET:
        case VK_INCOMPLETE:
        case VK_THREAD_IDLE_KHR:
        case VK_THREAD_DONE_KHR:
        case VK_OPERATION_DEFERRED_KHR:
        case VK_OPERATION_NOT_DEFERRED_KHR:
        case VK_PIPELINE_COMPILE_REQUIRED_EXT: return Severity::Info;

        case VK_SUBOPTIMAL_KHR: return Severity::Performance;

        case VK_SUCCESS: return Severity::Success;

        case VK_ERROR_DEVICE_LOST:
        case VK_ERROR_INITIALIZATION_FAILED:
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
        case VK_ERROR_OUT_OF_HOST_MEMORY: return Severity::Fatal;

        default: return Severity::InternalError;
      }
    }

    static constexpr const VkResultStatusDomain& get() noexcept;
  };

  namespace detail{
    inline constexpr static VkResultStatusDomain vulkanResultDomainInstance{};
  }

  constexpr const VkResultStatusDomain& VkResultStatusDomain::get() {
    return detail::vulkanResultDomainInstance;
  }
}

namespace valkyrie{

  /*template <>
  struct traits::StatusEnum<VkResult>{

    using enum_type = VkResult;

    inline constexpr static string_view name{VK_string("VkResult")};

    inline constexpr static string_view domain_name{VK_string("Vulkan")};
    inline constexpr static uuid       domain_uuid{"a3138144-1344-43ea-bded-dd0385a12e4c"};




    inline const static StatusEnumMap<enum_type> info{
        {                          VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS,               VK_raw_string(Requested invalid opaque capture address),           { code::InvalidArgument } },
        {                          VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT,          VK_raw_string(Lost fullscreen exclusive mode),                     { code::LostResource } },
        {                          VK_ERROR_NOT_PERMITTED_EXT,                            VK_raw_string(Insufficient permission),                            { code::PermissionDenied } },
        {                          VK_ERROR_FRAGMENTATION,                                VK_raw_string(Allocation failed due to fragmentation),             { code::OutOfMemory } },
        {                          VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT, VK_raw_string(Plane layout incompatible with DRM format modifier), { code::InsufficientSize, code::IncompatibleResource } },
        {                          VK_ERROR_INVALID_EXTERNAL_HANDLE,                      VK_raw_string(Invalid handle to external memory),                  { code::InvalidResource } },
        {                          VK_ERROR_OUT_OF_POOL_MEMORY,                           VK_raw_string(Pool is out of memory),                              { code::OutOfMemory } },
        {                          VK_ERROR_INVALID_SHADER_NV,                            VK_raw_string(GLSL syntax error),                                  { code::InvalidResource } },
        {                          VK_ERROR_VALIDATION_FAILED_EXT,                        VK_raw_string(Validation failure),                                 { code::FailedAssertion } },
        {                          VK_ERROR_INCOMPATIBLE_DISPLAY_KHR,                     VK_raw_string(Display is incompatible with current GPU),           { code::IncompatibleDevice } },
        {                          VK_ERROR_OUT_OF_DATE_KHR,                              VK_raw_string(Resources are out of date) },
        {                          VK_ERROR_NATIVE_WINDOW_IN_USE_KHR,                     VK_raw_string(Requested native window is already in use),          { code::ResourceUnavailable, code::ResourceNotOwned } },
        {                          VK_ERROR_SURFACE_LOST_KHR,                             VK_raw_string(Lost presentation surface),                          { code::LostResource } },
        { severity::InternalError, VK_ERROR_UNKNOWN,                                      VK_raw_string(Unknown error) },
        {                          VK_ERROR_FRAGMENTED_POOL,                              VK_raw_string(Pool is too fragmented) },
        {                          VK_ERROR_FORMAT_NOT_SUPPORTED,                         VK_raw_string(Format not supported by current GPU) },
        {                          VK_ERROR_TOO_MANY_OBJECTS,                             VK_raw_string(Too many Vulkan objects) },
        {                          VK_ERROR_INCOMPATIBLE_DRIVER,                          VK_raw_string(Incompatible driver),                                { code::IncompatibleResource, code::BadVersion } },
        {                          VK_ERROR_FEATURE_NOT_PRESENT,                          VK_raw_string(Requested feature not supported),                    { code::ResourceUnavailable, code::NotImplemented } },
        {                          VK_ERROR_EXTENSION_NOT_PRESENT,                        VK_raw_string(Requested extension not available),                  { code::ResourceUnavailable, code::NotImplemented } },
        {                          VK_ERROR_LAYER_NOT_PRESENT,                            VK_raw_string(Requested layer not found),                          { code::ResourceUnavailable, code::NotImplemented, code::ResourceNotFound } },
        {                          VK_ERROR_MEMORY_MAP_FAILED,                            VK_raw_string(Memory map operation failed) },
        { severity::Fatal,         VK_ERROR_DEVICE_LOST,                                  VK_raw_string(Lost GPU),                                           { code::LostResource } },
        { severity::Fatal,         VK_ERROR_INITIALIZATION_FAILED,                        VK_raw_string(Vulkan failed to initialize properly),               { code::InitializationFailure } },
        { severity::Fatal,         VK_ERROR_OUT_OF_DEVICE_MEMORY,                         VK_raw_string(Out of GPU memory),                                  { code::OutOfMemory } },
        { severity::Fatal,         VK_ERROR_OUT_OF_HOST_MEMORY,                           VK_raw_string(Out of system memory),                               { code::OutOfMemory } },
        { severity::Success,       VK_SUCCESS,                                            VK_raw_string(Success),                                            { code::Success } },
        { severity::Info,          VK_NOT_READY,                                          VK_raw_string(Not ready),                                          { code::NotReady } },
        { severity::Info,          VK_TIMEOUT,                                            VK_raw_string(Timed out),                                          { code::TimedOut } },
        { severity::Info,          VK_EVENT_SET,                                          VK_raw_string(Event set),                                          { code::Set } },
        { severity::Info,          VK_EVENT_RESET,                                        VK_raw_string(Event reset),                                        { code::Unset } },
        { severity::Info,          VK_INCOMPLETE,                                         VK_raw_string(Property enumeration incomplete),                    { code::InProgress } },
        { severity::Performance,   VK_SUBOPTIMAL_KHR,                                     VK_raw_string(Current swapchain is suboptimal) },
        { severity::Info,          VK_THREAD_IDLE_KHR,                                    VK_raw_string(Idle),                                               { code::Idle } },
        { severity::Info,          VK_THREAD_DONE_KHR,                                    VK_raw_string(Done),                                               { code::Done } },
        { severity::Info,          VK_OPERATION_DEFERRED_KHR,                             VK_raw_string(Operation deferred),                                 { code::InProgress }},
        { severity::Info,          VK_OPERATION_NOT_DEFERRED_KHR,                         VK_raw_string(Operation completed, not deferred),                  { code::Done } },
        { severity::Info,          VK_PIPELINE_COMPILE_REQUIRED_EXT,                      VK_raw_string(Pipeline compilation is required),                   { code::FurtherActionsRequired } }
    };

  };*/



  /*template <>
  struct traits::StatusEnum<spv_result_t>{
    using enum_type = spv_result_t;

    inline constexpr static string_view domain_name{VK_string("Spirv-Tools")};
    inline constexpr static uuid       domain_uuid{"90a87156-5fae-4d2f-91d7-159ba760b22a"};



    inline const*//*expr*//* static StatusEnumMap<enum_type> info{
        { severity::Fatal,         SPV_ERROR_WRONG_VERSION,      VK_raw_string(Requested invalid SpirV version),   { code::BadVersion } },
        {                          SPV_ERROR_MISSING_EXTENSION,  VK_raw_string(Requested extension not available), { code::ResourceUnavailable, code::NotImplemented } },
        {                          SPV_ERROR_INVALID_DATA,       VK_raw_string(SpirV code failed data validation), { code::InvalidResource, code::FailedAssertion } },
        {                          SPV_ERROR_INVALID_CAPABILITY, VK_raw_string(Invalid capability),                { code::InvalidArgument } },
        {                          SPV_ERROR_INVALID_LAYOUT,     VK_raw_string(Invalid layout),                    { code::InvalidArgument } },
        {                          SPV_ERROR_INVALID_CFG,        VK_raw_string(Invalid CFG),                       { code::InvalidResource } },
        {                          SPV_ERROR_INVALID_ID,         VK_raw_string(Invalid resource ID),               { code::InvalidArgument } },
        {                          SPV_ERROR_INVALID_LOOKUP,     VK_raw_string(Invalid identifier),                { code::InvalidArgument } },
        {                          SPV_ERROR_INVALID_DIAGNOSTIC, VK_raw_string(Invalid diagnostic),                { code::InvalidResource } },
        {                          SPV_ERROR_INVALID_VALUE,      VK_raw_string(Invalid value),                     { code::InvalidArgument } },
        {                          SPV_ERROR_INVALID_TABLE,      VK_raw_string(Invalid table),                     { code::InvalidResource } },
        {                          SPV_ERROR_INVALID_TEXT,       VK_raw_string(Invalid text),                      { code::InvalidResource }  },
        {                          SPV_ERROR_INVALID_BINARY,     VK_raw_string(Invalid binary),                    { code::InvalidResource }  },
        { severity::Fatal,         SPV_ERROR_INVALID_POINTER,    VK_raw_string(Invalid pointer),                   { code::BadAddress, code::InvalidResource }  },
        { severity::Fatal,         SPV_ERROR_OUT_OF_MEMORY,      VK_raw_string(Out of memory),                     { code::OutOfMemory }  },
        { severity::InternalError, SPV_ERROR_INTERNAL,           VK_raw_string(Internal error) },
        { severity::Info,          SPV_SUCCESS,                  VK_raw_string(Success),                           { code::Success }  },
        { severity::Warning,       SPV_UNSUPPORTED,              VK_raw_string(Unsupported operation),             { code::NotImplemented }  },
        { severity::Info,          SPV_END_OF_STREAM,            VK_raw_string(Reached end of stream),             { code::Done }  },
        { severity::Warning,       SPV_WARNING,                  VK_raw_string(Warning) },
        {                          SPV_FAILED_MATCH,             VK_raw_string(Failed to match),                   { code::ResourceNotFound }  },
        { severity::Info,          SPV_REQUESTED_TERMINATION,    VK_raw_string(Cancelled by client),               { code::Cancelled } }
    };
  };

  //enum_traits<spv_result_t>::

  template <>
  struct traits::StatusEnum<shaderc_compilation_status>{
    using enum_type = shaderc_compilation_status;

    inline constexpr static string_view domain_name{VK_string("ShaderC Compilation")};
    inline constexpr static uuid       domain_uuid{"65a682d7-ce31-4b80-b606-514814c2cbac"};


    inline const*//*expr*//* static StatusEnumMap<enum_type> info{
        { severity::Info,          shaderc_compilation_status_success,              VK_raw_string(Success),                          { code::Success } },
        {                          shaderc_compilation_status_invalid_stage,        VK_raw_string(Unable to determine shader stage), { code::InferenceFailure, code::InvalidResource } },
        {                          shaderc_compilation_status_compilation_error,    VK_raw_string(Compilation error),                { code::ExternalError } },
        { severity::InternalError, shaderc_compilation_status_internal_error,       VK_raw_string(Internal error) },
        {                          shaderc_compilation_status_null_result_object,   VK_raw_string(Null result object),               { code::BadAddress, code::InvalidArgument } },
        {                          shaderc_compilation_status_invalid_assembly,     VK_raw_string(Invalid SpirV assembly),           { code::InvalidResource } },
        {                          shaderc_compilation_status_validation_error,     VK_raw_string(SpirV validation failed),          { code::InvalidResource, code::FailedAssertion } },
        {                          shaderc_compilation_status_transformation_error, VK_raw_string(Bad binary transformation) },
        {                          shaderc_compilation_status_configuration_error,  VK_raw_string(Invalid compiler configuration),   { code::InvalidConfiguration } }
    };
  };*/

  /*template <>
  struct traits::StatusEnum<OptixResult>{

    using enum_type = OptixResult;

    VK_status_enum_domain(NVidia OptiX, "91619b05-0b4a-4c7c-8d80-522daa39ce82");

    VK_status_enum_info(
        (OPTIX_SUCCESS,                  severity = severity::Success, message = "Success",              generic = { code::Success }),
        (OPTIX_ERROR_INVALID_VALUE,                                    message = "Invalid value",        generic = { code::InvalidArgument }),
        (OPTIX_ERROR_HOST_OUT_OF_MEMORY, severity = severity::Fatal,   message = "Out of system memory", generic = { code::OutOfMemory }),
        (OPTIX_ERROR_INVALID_OPERATION,                                message = "Invalid Operation"),
        (OPTIX_ERROR_FILE_IO_ERROR,                                    message = "File IO error",        generic = { code::IOError })
    );


        OPTIX_ERROR_INVALID_OPERATION               = 7003,
        OPTIX_ERROR_FILE_IO_ERROR                   = 7004,
        OPTIX_ERROR_INVALID_FILE_FORMAT             = 7005,
        OPTIX_ERROR_DISK_CACHE_INVALID_PATH         = 7010,
        OPTIX_ERROR_DISK_CACHE_PERMISSION_ERROR     = 7011,
        OPTIX_ERROR_DISK_CACHE_DATABASE_ERROR       = 7012,
        OPTIX_ERROR_DISK_CACHE_INVALID_DATA         = 7013,
        OPTIX_ERROR_LAUNCH_FAILURE                  = 7050,
        OPTIX_ERROR_INVALID_DEVICE_CONTEXT          = 7051,
        OPTIX_ERROR_CUDA_NOT_INITIALIZED            = 7052,
        OPTIX_ERROR_VALIDATION_FAILURE              = 7053,
        OPTIX_ERROR_INVALID_PTX                     = 7200,
        OPTIX_ERROR_INVALID_LAUNCH_PARAMETER        = 7201,
        OPTIX_ERROR_INVALID_PAYLOAD_ACCESS          = 7202,
        OPTIX_ERROR_INVALID_ATTRIBUTE_ACCESS        = 7203,
        OPTIX_ERROR_INVALID_FUNCTION_USE            = 7204,
        OPTIX_ERROR_INVALID_FUNCTION_ARGUMENTS      = 7205,
        OPTIX_ERROR_PIPELINE_OUT_OF_CONSTANT_MEMORY = 7250,
        OPTIX_ERROR_PIPELINE_LINK_ERROR             = 7251,
        OPTIX_ERROR_INTERNAL_COMPILER_ERROR         = 7299,
        OPTIX_ERROR_DENOISER_MODEL_NOT_SET          = 7300,
        OPTIX_ERROR_DENOISER_NOT_INITIALIZED        = 7301,
        OPTIX_ERROR_ACCEL_NOT_COMPATIBLE            = 7400,
        OPTIX_ERROR_NOT_SUPPORTED                   = 7800,
        OPTIX_ERROR_UNSUPPORTED_ABI_VERSION         = 7801,
        OPTIX_ERROR_FUNCTION_TABLE_SIZE_MISMATCH    = 7802,
        OPTIX_ERROR_INVALID_ENTRY_FUNCTION_OPTIONS  = 7803,
        OPTIX_ERROR_LIBRARY_NOT_FOUND               = 7804,
        OPTIX_ERROR_ENTRY_SYMBOL_NOT_FOUND          = 7805,
        OPTIX_ERROR_LIBRARY_UNLOAD_FAILURE          = 7806,
        OPTIX_ERROR_CUDA_ERROR                      = 7900,
        OPTIX_ERROR_INTERNAL_ERROR                  = 7990,
        OPTIX_ERROR_UNKNOWN                         = 7999,
  };

  template <>
  struct traits::Singleton<graphics::api::detail::OptiXExceptionDomain>{
    inline constexpr static string_view name{u8"OptiXExceptionDomain"};
    inline constexpr static uuid       class_id{"21a110fc-e91b-4b3e-a210-e01b3d40336d"};
    inline constexpr static const graphics::api::detail::OptiXExceptionDomain& get() noexcept;
  };*/

  //traits::probe<traits::Enum::StatusInfo, VkResult>::
}

namespace valkyrie::graphics{

  /*namespace detail{
    class OptiXExceptionDomain : status_domain{

    };
  }*/


  /*inline valkyrie::graphics::api::VulkanStatus make_status_code(VkResult result) noexcept {
    return valkyrie::graphics::api::VulkanStatus(result);
  }
  inline valkyrie::graphics::api::SpirVStatus make_status_code(spv_result_t result) noexcept {
    return valkyrie::graphics::api::SpirVStatus(result);
  }
  inline valkyrie::graphics::api::ShaderCStatus make_status_code(shaderc_compilation_status result) noexcept {
    return valkyrie::graphics::api::ShaderCStatus(result);
  }*/


  /*using VulkanStatus = typename enum_traits<VkResult>::status_type;
  using SpirVStatus  = typename enum_traits<spv_result_t>::status_type;
  using ShaderCStatus = typename enum_traits<shaderc_compilation_status>::status_type;*/
  //using VulkanStatus = status_code<status_enum_domain<VkResult>>;
  using VulkanStatus = status_code<VkResultStatusDomain>;
  //using SpirVStatus  = status_code<status_enum_domain<spv_result_t>>;
  //using ShaderCStatus = status_code<status_enum_domain<shaderc_compilation_status>>;
}




namespace {

  inline valkyrie::graphics::VulkanStatus make_status_code(VkResult result) noexcept {
    return valkyrie::graphics::VulkanStatus(std::in_place, result);
  }
  /*inline valkyrie::graphics::api::SpirVStatus make_status_code(spv_result_t result) noexcept {
    return valkyrie::graphics::api::SpirVStatus(result);
  }
  inline valkyrie::graphics::api::ShaderCStatus make_status_code(shaderc_compilation_status result) noexcept {
    return valkyrie::graphics::api::ShaderCStatus(result);
  }*/


  //valkyrie::traits::detail::DomainInfo<valkyrie::status_enum_domain<VkResult>>::
}

#endif //VALKYRIE_STATUS_WRAPPERS_HPP
