//
// Created by Maxwell on 2020-11-18.
//

#ifndef VALKYRIE_STATUS_WRAPPERS_HPP
#define VALKYRIE_STATUS_WRAPPERS_HPP

#include <valkyrie/Core/Error/Status.hpp>
#include <valkyrie/Core/Utility/Enums.hpp>

#include <vulkan/vulkan_core.h>
#include <spirv-tools/libspirv.hpp>
#include <shaderc/shaderc.hpp>
#include <optix.h>
/*#include <cuda_runtime.h>
#include <cuda.h>*/

#include <initializer_list>


namespace valkyrie::Graphics::API{
  //class VulkanStatus;
  //class SpirvStatus;
  //class ShaderCStatus;
  //class CudaRuntimeStatus;
  //class CudaDriverStatus;
  namespace Detail{
    class OptiXExceptionDomain;
  }
  class OptiXException;
}

namespace valkyrie{

  using Core::Severity, Core::StringView, Core::Uuid, Core::Code;

  template <>
  struct Traits::StatusEnum<VkResult>{

    using enum_type = VkResult;

    inline constexpr static Core::StringView name{VK_string("VkResult")};

    inline constexpr static Core::StringView domain_name{VK_string("Vulkan")};
    inline constexpr static Core::Uuid       domain_uuid{"a3138144-1344-43ea-bded-dd0385a12e4c"};




    inline const/*expr*/ static Core::StatusEnumMap<enum_type> info{
        {                          VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS,               VK_raw_string(Requested invalid opaque capture address),           { Code::InvalidArgument } },
        {                          VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT,          VK_raw_string(Lost fullscreen exclusive mode),                     { Code::LostResource } },
        {                          VK_ERROR_NOT_PERMITTED_EXT,                            VK_raw_string(Insufficient permission),                            { Code::PermissionDenied } },
        {                          VK_ERROR_FRAGMENTATION,                                VK_raw_string(Allocation failed due to fragmentation),             { Code::OutOfMemory } },
        {                          VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT, VK_raw_string(Plane layout incompatible with DRM format modifier), { Code::InsufficientSize, Code::IncompatibleResource } },
        { Severity::Fatal,         VK_ERROR_INCOMPATIBLE_VERSION_KHR,                     VK_raw_string(Incompatible version),                               { Code::BadVersion, Code::IncompatibleResource } },
        {                          VK_ERROR_INVALID_EXTERNAL_HANDLE,                      VK_raw_string(Invalid handle to external memory),                  { Code::InvalidResource } },
        {                          VK_ERROR_OUT_OF_POOL_MEMORY,                           VK_raw_string(Pool is out of memory),                              { Code::OutOfMemory } },
        {                          VK_ERROR_INVALID_SHADER_NV,                            VK_raw_string(GLSL syntax error),                                  { Code::InvalidResource } },
        {                          VK_ERROR_VALIDATION_FAILED_EXT,                        VK_raw_string(Validation failure),                                 { Code::FailedAssertion } },
        {                          VK_ERROR_INCOMPATIBLE_DISPLAY_KHR,                     VK_raw_string(Display is incompatible with current GPU),           { Code::IncompatibleDevice } },
        {                          VK_ERROR_OUT_OF_DATE_KHR,                              VK_raw_string(Resources are out of date) },
        {                          VK_ERROR_NATIVE_WINDOW_IN_USE_KHR,                     VK_raw_string(Requested native window is already in use),          { Code::ResourceUnavailable, Code::ResourceNotOwned } },
        {                          VK_ERROR_SURFACE_LOST_KHR,                             VK_raw_string(Lost presentation surface),                          { Code::LostResource } },
        { Severity::InternalError, VK_ERROR_UNKNOWN,                                      VK_raw_string(Unknown error) },
        {                          VK_ERROR_FRAGMENTED_POOL,                              VK_raw_string(Pool is too fragmented) },
        {                          VK_ERROR_FORMAT_NOT_SUPPORTED,                         VK_raw_string(Format not supported by current GPU) },
        {                          VK_ERROR_TOO_MANY_OBJECTS,                             VK_raw_string(Too many Vulkan objects) },
        {                          VK_ERROR_INCOMPATIBLE_DRIVER,                          VK_raw_string(Incompatible driver),                                { Code::IncompatibleResource, Code::BadVersion } },
        {                          VK_ERROR_FEATURE_NOT_PRESENT,                          VK_raw_string(Requested feature not supported),                    { Code::ResourceUnavailable, Code::NotImplemented } },
        {                          VK_ERROR_EXTENSION_NOT_PRESENT,                        VK_raw_string(Requested extension not available),                  { Code::ResourceUnavailable, Code::NotImplemented } },
        {                          VK_ERROR_LAYER_NOT_PRESENT,                            VK_raw_string(Requested layer not found),                          { Code::ResourceUnavailable, Code::NotImplemented, Code::ResourceNotFound } },
        {                          VK_ERROR_MEMORY_MAP_FAILED,                            VK_raw_string(Memory map operation failed) },
        { Severity::Fatal,         VK_ERROR_DEVICE_LOST,                                  VK_raw_string(Lost GPU),                                           { Code::LostResource } },
        { Severity::Fatal,         VK_ERROR_INITIALIZATION_FAILED,                        VK_raw_string(Vulkan failed to initialize properly),               { Code::InitializationFailure } },
        { Severity::Fatal,         VK_ERROR_OUT_OF_DEVICE_MEMORY,                         VK_raw_string(Out of GPU memory),                                  { Code::OutOfMemory } },
        { Severity::Fatal,         VK_ERROR_OUT_OF_HOST_MEMORY,                           VK_raw_string(Out of system memory),                               { Code::OutOfMemory } },
        { Severity::Success,       VK_SUCCESS,                                            VK_raw_string(Success),                                            { Code::Success } },
        { Severity::Info,          VK_NOT_READY,                                          VK_raw_string(Not ready),                                          { Code::NotReady } },
        { Severity::Info,          VK_TIMEOUT,                                            VK_raw_string(Timed out),                                          { Code::TimedOut } },
        { Severity::Info,          VK_EVENT_SET,                                          VK_raw_string(Event set),                                          { Code::Set } },
        { Severity::Info,          VK_EVENT_RESET,                                        VK_raw_string(Event reset),                                        { Code::Unset } },
        { Severity::Info,          VK_INCOMPLETE,                                         VK_raw_string(Property enumeration incomplete),                    { Code::InProgress } },
        { Severity::Performance,   VK_SUBOPTIMAL_KHR,                                     VK_raw_string(Current swapchain is suboptimal) },
        { Severity::Info,          VK_THREAD_IDLE_KHR,                                    VK_raw_string(Idle),                                               { Code::Idle } },
        { Severity::Info,          VK_THREAD_DONE_KHR,                                    VK_raw_string(Done),                                               { Code::Done } },
        { Severity::Info,          VK_OPERATION_DEFERRED_KHR,                             VK_raw_string(Operation deferred),                                 { Code::InProgress }},
        { Severity::Info,          VK_OPERATION_NOT_DEFERRED_KHR,                         VK_raw_string(Operation completed, not deferred),                  { Code::Done } },
        { Severity::Info,          VK_PIPELINE_COMPILE_REQUIRED_EXT,                      VK_raw_string(Pipeline compilation is required),                   { Code::FurtherActionsRequired } }
    };

  };

  template <>
  struct Traits::StatusEnum<spv_result_t>{
    using enum_type = spv_result_t;

    inline constexpr static StringView domain_name{VK_string("Spirv-Tools")};
    inline constexpr static Uuid       domain_uuid{"90a87156-5fae-4d2f-91d7-159ba760b22a"};



    inline const/*expr*/ static Core::StatusEnumMap<enum_type> info{
        { Severity::Fatal,         SPV_ERROR_WRONG_VERSION,      VK_raw_string(Requested invalid SpirV version),   { Code::BadVersion } },
        {                          SPV_ERROR_MISSING_EXTENSION,  VK_raw_string(Requested extension not available), { Code::ResourceUnavailable, Code::NotImplemented } },
        {                          SPV_ERROR_INVALID_DATA,       VK_raw_string(SpirV code failed data validation), { Code::InvalidResource, Code::FailedAssertion } },
        {                          SPV_ERROR_INVALID_CAPABILITY, VK_raw_string(Invalid capability),                { Code::InvalidArgument } },
        {                          SPV_ERROR_INVALID_LAYOUT,     VK_raw_string(Invalid layout),                    { Code::InvalidArgument } },
        {                          SPV_ERROR_INVALID_CFG,        VK_raw_string(Invalid CFG),                       { Code::InvalidResource } },
        {                          SPV_ERROR_INVALID_ID,         VK_raw_string(Invalid resource ID),               { Code::InvalidArgument } },
        {                          SPV_ERROR_INVALID_LOOKUP,     VK_raw_string(Invalid identifier),                { Code::InvalidArgument } },
        {                          SPV_ERROR_INVALID_DIAGNOSTIC, VK_raw_string(Invalid diagnostic),                { Code::InvalidResource } },
        {                          SPV_ERROR_INVALID_VALUE,      VK_raw_string(Invalid value),                     { Code::InvalidArgument } },
        {                          SPV_ERROR_INVALID_TABLE,      VK_raw_string(Invalid table),                     { Code::InvalidResource } },
        {                          SPV_ERROR_INVALID_TEXT,       VK_raw_string(Invalid text),                      { Code::InvalidResource }  },
        {                          SPV_ERROR_INVALID_BINARY,     VK_raw_string(Invalid binary),                    { Code::InvalidResource }  },
        { Severity::Fatal,         SPV_ERROR_INVALID_POINTER,    VK_raw_string(Invalid pointer),                   { Code::BadAddress, Code::InvalidResource }  },
        { Severity::Fatal,         SPV_ERROR_OUT_OF_MEMORY,      VK_raw_string(Out of memory),                     { Code::OutOfMemory }  },
        { Severity::InternalError, SPV_ERROR_INTERNAL,           VK_raw_string(Internal error) },
        { Severity::Info,          SPV_SUCCESS,                  VK_raw_string(Success),                           { Code::Success }  },
        { Severity::Warning,       SPV_UNSUPPORTED,              VK_raw_string(Unsupported operation),             { Code::NotImplemented }  },
        { Severity::Info,          SPV_END_OF_STREAM,            VK_raw_string(Reached end of stream),             { Code::Done }  },
        { Severity::Warning,       SPV_WARNING,                  VK_raw_string(Warning) },
        {                          SPV_FAILED_MATCH,             VK_raw_string(Failed to match),                   { Code::ResourceNotFound }  },
        { Severity::Info,          SPV_REQUESTED_TERMINATION,    VK_raw_string(Cancelled by client),               { Code::Cancelled } }
    };
  };

  template <>
  struct Traits::StatusEnum<shaderc_compilation_status>{
    using enum_type = shaderc_compilation_status;

    inline constexpr static StringView domain_name{VK_string("ShaderC Compilation")};
    inline constexpr static Uuid       domain_uuid{"65a682d7-ce31-4b80-b606-514814c2cbac"};


    inline const/*expr*/ static Core::StatusEnumMap<enum_type> info{
        { Severity::Info,          shaderc_compilation_status_success,              VK_raw_string(Success),                          { Code::Success } },
        {                          shaderc_compilation_status_invalid_stage,        VK_raw_string(Unable to determine shader stage), { Code::InferenceFailure, Code::InvalidResource } },
        {                          shaderc_compilation_status_compilation_error,    VK_raw_string(Compilation error),                { Code::ExternalError } },
        { Severity::InternalError, shaderc_compilation_status_internal_error,       VK_raw_string(Internal error) },
        {                          shaderc_compilation_status_null_result_object,   VK_raw_string(Null result object),               { Code::BadAddress, Code::InvalidArgument } },
        {                          shaderc_compilation_status_invalid_assembly,     VK_raw_string(Invalid SpirV assembly),           { Code::InvalidResource } },
        {                          shaderc_compilation_status_validation_error,     VK_raw_string(SpirV validation failed),          { Code::InvalidResource, Code::FailedAssertion } },
        {                          shaderc_compilation_status_transformation_error, VK_raw_string(Bad binary transformation) },
        {                          shaderc_compilation_status_configuration_error,  VK_raw_string(Invalid compiler configuration),   { Code::InvalidConfiguration } }
    };
  };

  template <>
  struct Traits::StatusEnum<OptixResult>{

    using enum_type = OptixResult;

    VK_status_enum_domain(NVidia OptiX, "91619b05-0b4a-4c7c-8d80-522daa39ce82");

    VK_status_enum_info(
        (OPTIX_SUCCESS,                  severity = Severity::Success, message = "Success",              generic = { Code::Success }),
        (OPTIX_ERROR_INVALID_VALUE,                                    message = "Invalid value",        generic = { Code::InvalidArgument }),
        (OPTIX_ERROR_HOST_OUT_OF_MEMORY, severity = Severity::Fatal,   message = "Out of system memory", generic = { Code::OutOfMemory }),
        (OPTIX_ERROR_INVALID_OPERATION,                                message = "Invalid Operation"),
        (OPTIX_ERROR_FILE_IO_ERROR,                                    message = "File IO error",        generic = { Code::IOError })
    );


        /*OPTIX_ERROR_INVALID_OPERATION               = 7003,
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
        OPTIX_ERROR_UNKNOWN                         = 7999,*/
  };

  template <>
  struct Traits::Singleton<Graphics::API::Detail::OptiXExceptionDomain>{
    inline constexpr static StringView name{u8"OptiXExceptionDomain"};
    inline constexpr static Uuid       uuid{"21a110fc-e91b-4b3e-a210-e01b3d40336d"};
    inline constexpr static const Graphics::API::Detail::OptiXExceptionDomain& get() noexcept;
  };

  //Traits::Probe<Traits::Enum::StatusInfo, VkResult>::
}

namespace valkyrie::Graphics::API{

  namespace Detail{
    class OptiXExceptionDomain : Core::StatusDomain{

    };
  }



  using VulkanStatus = typename enum_traits<VkResult>::status_type;
  using SpirVStatus  = typename enum_traits<spv_result_t>::status_type;
  using ShaderCStatus = typename enum_traits<shaderc_compilation_status>::status_type;
}




namespace {
  inline valkyrie::Graphics::API::VulkanStatus makeStatusCode(VkResult result) noexcept {
    return valkyrie::Graphics::API::VulkanStatus(result);
  }
  inline valkyrie::Graphics::API::SpirVStatus makeStatusCode(spv_result_t result) noexcept {
    return valkyrie::Graphics::API::SpirVStatus(result);
  }
  inline valkyrie::Graphics::API::ShaderCStatus makeStatusCode(shaderc_compilation_status result) noexcept {
    return valkyrie::Graphics::API::ShaderCStatus(result);
  }
}

#endif //VALKYRIE_STATUS_WRAPPERS_HPP
