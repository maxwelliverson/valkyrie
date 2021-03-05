//
// Created by Maxwell on 2020-12-01.
//

#ifndef VALKYRIE_INTERNAL_DXGI_WRAPPERS_HPP
#define VALKYRIE_INTERNAL_DXGI_WRAPPERS_HPP

#include <valkyrie/adt/flat_map.hpp>
#include <valkyrie/adt/small_array.hpp>
#include <valkyrie/adt/static_array.hpp>
#include <valkyrie/graphics/api/vulkan.hpp>

VK_msvc_warning(push)
VK_msvc_warning(disable:5104)
VK_msvc_warning(disable:5105)
#include <dxgi1_6.h>
#include <wrl.h>
VK_msvc_warning(pop)

#undef interface

namespace valkyrie::GUI::Internal{
  
  using Format = graphics::api::Format;
  using ImageAspectFlags = graphics::api::ImageAspectFlags;
  using ComponentMapping = graphics::api::ComponentMapping;
  using ImageAspect = graphics::api::Flags::ImageAspect;
  using Swizzle = graphics::api::ComponentSwizzle;


  
  /*
   * #define VK_return_format(case_, ret_) case DXGI_FORMAT_##case_: return Format::ret_;
#define PP_VK_impl_RET_FORMAT(x) VK_return_format x
#define VK_enumerate_formats(...) VK_foreach(PP_VK_impl_RET_FORMAT, __VA_ARGS__)
   * inline constexpr Format translateFormat(DXGI_FORMAT dxgiFormat) noexcept {
    DXGI_FORMAT_R32G32B32A32_TYPELESS;
    DXGI_FORMAT_R32G32B32_TYPELESS;
    DXGI_FORMAT_R16G16B16A16_TYPELESS;
    DXGI_FORMAT_R32G32_TYPELESS;
    DXGI_FORMAT_R32G8X24_TYPELESS;
    DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
    DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;
    DXGI_FORMAT_R10G10B10A2_TYPELESS;
    DXGI_FORMAT_R8G8B8A8_TYPELESS;
    DXGI_FORMAT_R16G16_TYPELESS;
    DXGI_FORMAT_R32_TYPELESS;
    DXGI_FORMAT_R24G8_TYPELESS;
    DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    DXGI_FORMAT_X24_TYPELESS_G8_UINT;
    DXGI_FORMAT_R8G8_TYPELESS;
    DXGI_FORMAT_R16_TYPELESS;
    DXGI_FORMAT_R8_TYPELESS;
    DXGI_FORMAT_BC1_TYPELESS;
    DXGI_FORMAT_BC2_TYPELESS;
    DXGI_FORMAT_BC3_TYPELESS;
    DXGI_FORMAT_BC4_TYPELESS;
    DXGI_FORMAT_BC5_TYPELESS;
    DXGI_FORMAT_B8G8R8A8_TYPELESS;
    DXGI_FORMAT_B8G8R8X8_TYPELESS;
    DXGI_FORMAT_BC6H_TYPELESS;
    DXGI_FORMAT_BC7_TYPELESS;
    DXGI_FORMAT_A8_UNORM;
    DXGI_FORMAT_R1_UNORM;
    case DXGI_FORMAT_AYUV: -> A8R8G8B8UInt
    case DXGI_FORMAT_Y410: -> A8R8G8B8UInt
    case DXGI_FORMAT_Y416: -> R16G16B16A16UInt
    case DXGI_FORMAT_P010:
    case DXGI_FORMAT_P016:
    case DXGI_FORMAT_420_OPAQUE:
    case DXGI_FORMAT_YUY2:
    case DXGI_FORMAT_Y210:
    case DXGI_FORMAT_Y216:
    case DXGI_FORMAT_NV11:
    case DXGI_FORMAT_AI44:
    case DXGI_FORMAT_IA44:
    case DXGI_FORMAT_P8:
    case DXGI_FORMAT_A8P8:
    case DXGI_FORMAT_P208:
    case DXGI_FORMAT_V208:
    case DXGI_FORMAT_V408:
    switch (dxgiFormat) {
      VK_enumerate_formats(
          (R32G32B32A32_FLOAT,   R32G32B32A32SFloat),
          (R32G32B32A32_UINT,    R32G32B32A32UInt),
          (R32G32B32A32_SINT,    R32G32B32A32SInt),
          (R32G32B32_FLOAT,      R32G32B32SFloat),
          (R32G32B32_UINT,       R32G32B32UInt),
          (R32G32B32_SINT,       R32G32B32SInt),
          (R16G16B16A16_FLOAT,   R16G16B16A16SFloat),
          (R16G16B16A16_UNORM,   R16G16B16A16UNorm),
          (R16G16B16A16_UINT,    R16G16B16A16UInt),
          (R16G16B16A16_SNORM,   R16G16B16A16SNorm),
          (R16G16B16A16_SINT,    R16G16B16A16SInt),
          (R32G32_FLOAT,         R32G32SFloat),
          (R32G32_UINT,          R32G32UInt),
          (R32G32_SINT,          R32G32SInt),
          (D32_FLOAT_S8X24_UINT, D32SFloatS8UInt),
          (R10G10B10A2_UNORM,    A2R10G10B10UNormPack32),
          (R10G10B10A2_UINT,     A2R10G10B10UIntPack32),
          (R11G11B10_FLOAT,      B10G11R11UFloatPack32),
          (R8G8B8A8_UNORM, R8G8B8A8UNorm),
          (R8G8B8A8_UNORM_SRGB, R8G8B8A8SRGB),
          (R8G8B8A8_UINT, R8G8B8A8UInt),
          (R8G8B8A8_SNORM, R8G8B8A8SNorm),
          (R8G8B8A8_SINT, R8G8B8A8SInt),
          (R16G16_FLOAT, R16G16SFloat),
          (R16G16_UNORM, R16G16UNorm),
          (R16G16_UINT, R16G16UInt),
          (R16G16_SNORM, R16G16SNorm),
          (R16G16_SINT, R16G16SInt),
          (D32_FLOAT, D32SFloat),
          (R32_FLOAT, R32SFloat),
          (R32_UINT, R32UInt),
          (R32_SINT, R32SInt),
          (D24_UNORM_S8_UINT, D24UNormS8UInt),
          (R8G8_UNORM, R8G8UNorm),
          (R8G8_UINT, R8G8UInt),
          (R8G8_SNORM, R8G8SNorm),
          (R8G8_SINT, R8G8SInt),
          (R16_FLOAT, R16SFloat),
          (D16_UNORM, D16UNorm),
          (R16_UNORM, R16UNorm),
          (R16_UINT, R16UInt),
          (R16_SNORM, R16SNorm),
          (R16_SINT, R16SInt),
          (R8_UNORM, R8UNorm),
          (R8_UINT, R8UInt),
          (R8_SNORM, R8SNorm),
          (R8_SINT, R8SInt),
          (R9G9B9E5_SHAREDEXP, E5B9G9R9UFloatPack32),
          (R8G8_B8G8_UNORM, B8G8R8G8_422UNorm),
          (G8R8_G8B8_UNORM, G8B8G8R8_422UNorm),
          (BC1_UNORM, Bc1RGBAUNormBlock),
          (BC1_UNORM_SRGB, Bc1RGBASRGBBlock),
          (BC2_UNORM, Bc2UNormBlock),
          (BC2_UNORM_SRGB, Bc2SRGBBlock),
          (BC3_UNORM, Bc3UNormBlock),
          (BC3_UNORM_SRGB, Bc3SRGBBlock),
          (BC4_UNORM, Bc4UNormBlock),
          (BC4_SNORM, Bc4SNormBlock),
          (BC5_UNORM, Bc5UNormBlock),
          (BC5_SNORM, Bc5SNormBlock),
          (B5G6R5_UNORM, B5G6R5UNormPack16),
          (B5G5R5A1_UNORM, A1R5G5B5UNormPack16),
          (B8G8R8A8_UNORM, B8G8R8A8UNorm),
          (B8G8R8X8_UNORM, B8G8R8UNorm),
          (R10G10B10_XR_BIAS_A2_UNORM, A2R10G10B10UNormPack32),
          (B8G8R8A8_UNORM_SRGB, B8G8R8A8SRGB),
          (B8G8R8X8_UNORM_SRGB, B8G8R8SRGB),
          (BC6H_UF16, Bc6hUFloatBlock),
          (BC6H_SF16, Bc6hSFloatBlock),
          (BC7_UNORM, Bc7UNormBlock),
          (BC7_UNORM_SRGB, Bc7SRGBBlock),
          (B4G4R4A4_UNORM, B4G4R4A4UNormPack16),
          (NV12, G8_B8R8_2PLANE_420UNorm),
          (P010, G16_B16R16_2PLANE_420UNorm)
      )
      default:
        return Format::Undefined;
    }
  }*/

  namespace DXGI{

    struct FormatMapping{
      Format           colorCompatible = Format::Undefined;
      Format           depthCompatible = Format::Undefined;
      Format           bitCompatible   = Format::Undefined;
      ImageAspectFlags aspectColor{};
      ImageAspectFlags aspectDepth{};
      ComponentMapping swizzle    {};
    };
    struct FormatInfo{
      Format           format = Format::Undefined;
      ImageAspectFlags aspect {};
      ComponentMapping swizzle{};
    };

    class FormatFamily{
      inline constexpr static u32 MaxSize = 5;

    public:
      constexpr FormatFamily() = default;
      constexpr FormatFamily(const FormatFamily&) = default;
      constexpr FormatFamily(std::initializer_list<Format> list) noexcept{
        u32 i = 0;
        for (Format f : list)
          formats[i] = f;
      }

      using iterator = Format*;
      using const_iterator = const Format*;



      size_t         size() const noexcept {
        for (u32 size = 0; size < MaxSize; ++size) {
          if (formats[size] == Format::Undefined)
            return size;
        }
        return MaxSize;
      }

      iterator       begin()        noexcept {
        return formats;
      }
      const_iterator begin()  const noexcept {
        return formats;
      }
      const_iterator cbegin() const noexcept {
        return formats;
      }

      iterator       end()        noexcept {
        return &formats[size() - 1];
      }
      const_iterator end()  const noexcept {
        return &formats[size() - 1];
      }
      const_iterator cend() const noexcept {
        return &formats[size() - 1];
      }

    private:
      Format formats[MaxSize]{
          Format::Undefined,
          Format::Undefined,
          Format::Undefined,
          Format::Undefined,
          Format::Undefined
      };
    };

    enum class FormatMode{
      Any,
      Color,
      Depth,
      Raw
    };


    /**
     * \brief Retrieves info for a given DXGI format
     *
     * \param [in] Format The DXGI format to look up
     * \param [in] Mode the format lookup mode
     * \returns Format info
     */
    FormatInfo getFormatInfo(DXGI_FORMAT Format, FormatMode Mode) noexcept;

    /**
     * \brief Retrieves original info for a given DXGI format
     *
     * Doesn't perform any format adjustment, so this
     * can be used to determine the packed data format
     * of a DXGI format for things like data uploads.
     * \param [in] Format The DXGI format to look up
     * \param [in] Mode the format lookup mode
     * \returns Format info
     */
    FormatInfo getPackedFormatInfo(DXGI_FORMAT Format, FormatMode Mode) noexcept;

    /**
     * \brief Retrieves a format family
     *
     * \param [in] Format The format to query
     * \param [in] Mode Image format mode
     * \returns Image format family
     */
    FormatFamily getFormatFamily(DXGI_FORMAT Format, FormatMode Mode) noexcept;


    enum RawResult : HRESULT;

    class status_domain : public status_domain{
      inline constexpr static uuid class_id{"a16442cf-17f2-4180-a14a-f66f71d63792"};
    public:
      using value_type = RawResult;
      using typename status_domain::string_ref;
      using status_type = status_code<status_domain>;

      string_view name() const noexcept override{
        return VK_string("DirectX Graphics Infrastructure");
      }
      string_ref doMessage(const status_code<void>& status) const noexcept override {
        switch (static_cast<const status_type&>(status).value()) {
          case S_OK:
            return VK_string("Success");
          case DXGI_ERROR_ACCESS_DENIED:
            return VK_string("You tried to use a resource to which you did not have the required access privileges. This error is most typically caused when you write to a shared resource with read-only access.");
          case DXGI_ERROR_ACCESS_LOST:
            return VK_string("The desktop duplication interface is invalid. The desktop duplication interface typically becomes invalid when a different type of image is displayed on the desktop.");
          case DXGI_ERROR_ALREADY_EXISTS:
            return VK_string("The desired element already exists. This is returned by DXGIDeclareAdapterRemovalSupport if it is not the first time that the function is called.");
          case DXGI_ERROR_CANNOT_PROTECT_CONTENT:
            return VK_string("DXGI can't provide content protection on the swap chain. This error is typically caused by an older driver, or when you use a swap chain that is incompatible with content protection.");
          case DXGI_ERROR_DEVICE_HUNG:
            return VK_string("The application's device failed due to badly formed commands sent by the application. This is an design-time issue that should be investigated and fixed.");
          case DXGI_ERROR_DEVICE_REMOVED:
            return VK_string("The video card has been physically removed from the system, or a driver upgrade for the video card has occurred. The application should destroy and recreate the device. For help debugging the problem, call ID3D10Device::GetDeviceRemovedReason.");
          case DXGI_ERROR_DEVICE_RESET:
            return VK_string("The device failed due to a badly formed command. This is a run-time issue; The application should destroy and recreate the device.");
          case DXGI_ERROR_DRIVER_INTERNAL_ERROR:
            return VK_string("The driver encountered a problem and was put into the device removed state.");
          case DXGI_ERROR_FRAME_STATISTICS_DISJOINT:
            return VK_string("An event (for example, a power cycle) interrupted the gathering of presentation statistics.");
          case DXGI_ERROR_GRAPHICS_VIDPN_SOURCE_IN_USE:
            return VK_string("The application attempted to acquire exclusive ownership of an output, but failed because some other application (or device within the application) already acquired ownership.");
          case DXGI_ERROR_INVALID_CALL:
            return VK_string("The application provided invalid parameter data; this must be debugged and fixed before the application is released.");
          case DXGI_ERROR_MORE_DATA:
            return VK_string("The buffer supplied by the application is not big enough to hold the requested data.");
          case DXGI_ERROR_NAME_ALREADY_EXISTS:
            return VK_string("The supplied name of a resource in a call to IDXGIResource1::CreateSharedHandle is already associated with some other resource.");
          case DXGI_ERROR_NONEXCLUSIVE:
            return VK_string("A global counter resource is in use, and the Direct3D device can't currently use the counter resource.");
          case DXGI_ERROR_NOT_CURRENTLY_AVAILABLE:
            return VK_string("The resource or request is not currently available, but it might become available later.");
          case DXGI_ERROR_NOT_FOUND:
            return VK_string("When calling IDXGIObject::GetPrivateData, the GUID passed in is not recognized as one previously passed to IDXGIObject::SetPrivateData or IDXGIObject::SetPrivateDataInterface. When calling IDXGIFactory::EnumAdapters or IDXGIAdapter::EnumOutputs, the enumerated ordinal is out of range.");
          case DXGI_ERROR_REMOTE_CLIENT_DISCONNECTED:
            return VK_string("Remote client disconnected");
          case DXGI_ERROR_REMOTE_OUTOFMEMORY:
            return VK_string("Remote client is out of memory.");
          case DXGI_ERROR_RESTRICT_TO_OUTPUT_STALE:
            return VK_string("The DXGI output (monitor) to which the swap chain content was restricted is now disconnected or changed.");
          case DXGI_ERROR_SDK_COMPONENT_MISSING:
            return VK_string("The operation depends on an SDK component that is missing or mismatched.");
          case DXGI_ERROR_SESSION_DISCONNECTED:
            return VK_string("The Remote Desktop Services session is currently disconnected.");
          case DXGI_ERROR_UNSUPPORTED:
            return VK_string("The requested functionality is not supported by the device or the driver.");
          case DXGI_ERROR_WAIT_TIMEOUT:
            return VK_string("The time-out interval elapsed before the next desktop frame was available.");
          case DXGI_ERROR_WAS_STILL_DRAWING:
            return VK_string("The GPU was busy at the moment when a call was made to perform an operation, and did not execute or schedule the operation.");
          case DXGI_ERROR_NOT_CURRENT:
            return VK_string("The DXGI objects that the application has created are no longer current & need to be recreated for this operation to be performed.");
          case DXGI_ERROR_HW_PROTECTION_OUTOFMEMORY:
            return VK_string("Insufficient HW protected memory exits for proper function.");
          case DXGI_ERROR_DYNAMIC_CODE_POLICY_VIOLATION:
            return VK_string("Creating this device would violate the process's dynamic code policy.");
          case DXGI_ERROR_NON_COMPOSITED_UI:
            return VK_string("The operation failed because the compositor is not in control of the output.");
          case DXGI_STATUS_OCCLUDED:
            return VK_string("The window content is not visible. When receiving this status, an application can stop rendering and use DXGI_PRESENT_TEST to determine when to resume rendering.");
          case DXGI_STATUS_CLIPPED:
            return VK_string("The Present operation was partially invisible to the user.");
          case DXGI_STATUS_MODE_CHANGED:
            return VK_string("The desktop display mode has been changed, there might be color conversion/stretching. The application should call IDXGISwapChain::ResizeBuffers to match the new display mode.");
          case DXGI_STATUS_MODE_CHANGE_IN_PROGRESS:
            return VK_string("IDXGISwapChain::ResizeTarget and IDXGISwapChain::SetFullscreenState will return DXGI_STATUS_MODE_CHANGE_IN_PROGRESS if a fullscreen/windowed mode transition is occurring when either api is called.");
          case DXGI_STATUS_NO_REDIRECTION:
            return VK_string("The driver is requesting that the DXGI runtime not use shared resources to communicate with the Desktop Window Manager.");
          case DXGI_STATUS_NO_DESKTOP_ACCESS:
            return VK_string("The Present operation was not visible because the Windows session has switched to another desktop (for example, ctrl-alt-del).");
          case DXGI_STATUS_GRAPHICS_VIDPN_SOURCE_IN_USE:
            return VK_string("The Present operation was not visible because the target monitor was being used for some other purpose.");
          default:
            return VK_string("Unknown DXGI driver error.");
        }
      }
      code doCode(const status_code<void>& status) const noexcept override{

        using C = code;

        switch(static_cast<const status_type&>(status).value()) {
          case S_OK:
            return C::Success;
          case DXGI_ERROR_ACCESS_DENIED:
            return C::PermissionDenied;
          case DXGI_ERROR_ACCESS_LOST:
            return C::LostResource;
          case DXGI_ERROR_ALREADY_EXISTS:
            return C::ResourceAlreadyExists;
          case DXGI_ERROR_CANNOT_PROTECT_CONTENT:
            return C::NotSupported;
          case DXGI_ERROR_DEVICE_HUNG:
            return C::DeviceUnrecoverableState;
          case DXGI_ERROR_DEVICE_REMOVED:
            return C::LostDevice;
          case DXGI_ERROR_DEVICE_RESET:
            return C::DeviceUnknown;
          case DXGI_ERROR_DRIVER_INTERNAL_ERROR:
            return C::UnrecoverableState;
          case DXGI_ERROR_FRAME_STATISTICS_DISJOINT:
            return C::InvalidResource;
          case DXGI_ERROR_INVALID_CALL:
            return C::InvalidArgument;
          case DXGI_ERROR_MORE_DATA:
            return C::InsufficientSize;
          case DXGI_ERROR_NAME_ALREADY_EXISTS:
            return C::ResourceAlreadyExists;
          case DXGI_ERROR_NONEXCLUSIVE:
            return C::ResourceNotOwned;
          case DXGI_ERROR_NOT_CURRENTLY_AVAILABLE:
            return C::Busy;
          case DXGI_ERROR_NOT_FOUND:
            return C::ResourceNotFound;
          case DXGI_ERROR_REMOTE_CLIENT_DISCONNECTED:
            return C::LostConnection;
          case DXGI_ERROR_REMOTE_OUTOFMEMORY:
            return C::DeviceOutOfMemory;
          case DXGI_ERROR_RESTRICT_TO_OUTPUT_STALE:
            return C::LostDevice;
          case DXGI_ERROR_SDK_COMPONENT_MISSING:
            return C::NotImplemented;
          case DXGI_ERROR_SESSION_DISCONNECTED:
            return C::LostConnection;
          case DXGI_ERROR_UNSUPPORTED:
            return C::NotSupported;
          case DXGI_ERROR_WAIT_TIMEOUT:
            return C::TimedOut;
          case DXGI_ERROR_NOT_CURRENT:
            return C::ResourceOutOfDate;
          case DXGI_ERROR_HW_PROTECTION_OUTOFMEMORY:
            return C::DeviceOutOfMemory;
          case DXGI_ERROR_DYNAMIC_CODE_POLICY_VIOLATION:
            return C::PermissionDenied;
          case DXGI_ERROR_NON_COMPOSITED_UI:
            return C::ResourceNotOwned;
          case DXGI_STATUS_OCCLUDED:
            return C::Unnecessary;
          case DXGI_STATUS_MODE_CHANGED:
            return C::Changed;
          case DXGI_STATUS_MODE_CHANGE_IN_PROGRESS:
            return C::Cancelled;
          case DXGI_ERROR_WAS_STILL_DRAWING:
          case DXGI_STATUS_NO_DESKTOP_ACCESS:
          case DXGI_STATUS_GRAPHICS_VIDPN_SOURCE_IN_USE:
          case DXGI_ERROR_GRAPHICS_VIDPN_SOURCE_IN_USE:
            return C::DeviceBusy;
          default:
            return C::Unknown;
        }
      }
      bool doFailure(const status_code<void>& status) const noexcept override{
        return FAILED(static_cast<const status_type&>(status).value());
      }
      Severity doSeverity(const status_code<void>& status) const noexcept override{

        using S = Severity;

        switch(static_cast<const status_type&>(status).value()){
          case DXGI_ERROR_NOT_CURRENTLY_AVAILABLE:
          case DXGI_ERROR_WAIT_TIMEOUT:
            return S::Error;
          case DXGI_STATUS_OCCLUDED:
            return S::Performance;
          case DXGI_ERROR_WAS_STILL_DRAWING:
          case DXGI_STATUS_NO_DESKTOP_ACCESS:
          case DXGI_STATUS_GRAPHICS_VIDPN_SOURCE_IN_USE:
          case DXGI_ERROR_GRAPHICS_VIDPN_SOURCE_IN_USE:
            return S::Info;
          default:
            return getDefaultSeverity(this->doCode(status));
        }
      }
      bool doEquivalent(const status_code<void>& a, const status_code<void>& b) const noexcept override{
        VK_assert(a.domain() == *this);

        if (b.domain() == *this)
          return static_cast<const status_type&>(a).value() == static_cast<const status_type&>(b).value();
        return false;
      }
      const uuid & id() const noexcept override {
        return class_id;
      }

      inline constexpr static const status_domain& get() noexcept;
    };

    inline constexpr static status_domain domainInstance{};

    inline constexpr const status_domain& status_domain::get() noexcept {
      return domainInstance;
    }

    using Status = status_code<status_domain>;
    using Error  = error_code<status_domain>;

    inline Status makeStatusCode(RawResult result) noexcept {
      return Status{std::in_place, result};
    }

    template <typename T>
    using Result = maybe<T, status_domain>;

    class Output{

      using FormatMap = flat_map<DXGI_FORMAT, small_array<DXGI_MODE_DESC>>;

      Microsoft::WRL::ComPtr<IDXGIOutput6> pOutput;
      HMONITOR win32Handle;
      graphics::Box2D<i32> displayRectangle;
      std::string name{"UNKNOWN MONITOR"};
      FormatMap formatSupport{};
      interval<float> luminance;
      float maxFullFrameLuminance;
    public:
      VK_msvc_warning(push)
      VK_msvc_warning(disable:4244)
      Output() = default;
      Output(IDXGIOutput6* pOutput, const DXGI_OUTPUT_DESC1& desc) noexcept
          : pOutput(pOutput),
            win32Handle(desc.Monitor),
            displayRectangle{
                .offsets = { desc.DesktopCoordinates.left, desc.DesktopCoordinates.top },
                .extents = { desc.DesktopCoordinates.right - desc.DesktopCoordinates.left, desc.DesktopCoordinates.top - desc.DesktopCoordinates.bottom }
            },
            name(&desc.DeviceName[0], &desc.DeviceName[std::wcslen(desc.DeviceName)]),
            luminance{ desc.MinLuminance, desc.MaxLuminance },
            maxFullFrameLuminance(desc.MaxFullFrameLuminance){}
      VK_msvc_warning(pop)
      Output(const Output&) = default;
      Output(Output&&) noexcept = default;


      Status waitForVBlank() const noexcept {
        return pOutput ? (RawResult)pOutput->WaitForVBlank() : (RawResult)DXGI_ERROR_INVALID_CALL;
      }
      Status getGammaControlCapabilities(DXGI_GAMMA_CONTROL_CAPABILITIES& capabilities) const noexcept {
        return (RawResult)pOutput->GetGammaControlCapabilities(&capabilities);
      }
      Status getGammaControl(DXGI_GAMMA_CONTROL& gammaControl) const noexcept{
        return (RawResult)pOutput->GetGammaControl(&gammaControl);
      }
      Status setGammaControl(const DXGI_GAMMA_CONTROL& gammaControl) const noexcept {
        return (RawResult)pOutput->SetGammaControl(&gammaControl);
      }
      Status getFrameStatistics(DXGI_FRAME_STATISTICS& frameStatistics) const noexcept {
        return (RawResult)pOutput->GetFrameStatistics(&frameStatistics);
      }
      std::string_view          getName() const noexcept {
        return name;
      }
      graphics::Position2D<i32> getPosition() const noexcept {
        return displayRectangle.offsets;
      }
      graphics::Extent2D<i32>   getExtents() const noexcept {
        return displayRectangle.extents;
      }
      interval<float>     getLuminance() const noexcept {
        return luminance;
      }
      interval<float>     getFullFrameLuminance() const noexcept {
        return { luminance.min, maxFullFrameLuminance };
      }
      HMONITOR                  getMonitorHandle() const noexcept {
        return win32Handle;
      }
    };
    class Adapter{
      Microsoft::WRL::ComPtr<IDXGIAdapter4> pAdapter;
      std::string name{"UNKNOWN ADAPTER"};
      size_t DedicatedVideoMemory{};
      size_t DedicatedSystemMemory{};
      size_t SharedSystemMemory{};
      LUID AdapterLUID{};
      Status lastResult{};
      /*DXGI_ADAPTER_FLAG3 Flags;
      DXGI_GRAPHICS_PREEMPTION_GRANULARITY GraphicsPreemptionGranularity;
      DXGI_COMPUTE_PREEMPTION_GRANULARITY ComputePreemptionGranularity;*/
    public:
      Adapter() = default;
      Adapter(IDXGIAdapter4* pAdapter) noexcept
          : pAdapter(pAdapter){
        DXGI_ADAPTER_DESC3 desc;
        lastResult = (RawResult)this->pAdapter->GetDesc3(&desc);
        if (lastResult.success()) {
          const wchar_t* descName = &desc.Description[0];
          VK_msvc_warning(push)
          VK_msvc_warning(disable:4244)
          name.assign(descName, descName + std::wcslen(descName));
          VK_msvc_warning(pop)
          DedicatedVideoMemory = desc.DedicatedVideoMemory;
          DedicatedSystemMemory = desc.DedicatedSystemMemory;
          SharedSystemMemory = desc.SharedSystemMemory;
          AdapterLUID = desc.AdapterLuid;
        }
      }
      Adapter(const Adapter&) = default;

      std::string_view getName() const noexcept {
        return name;
      }
      size_t dedicatedVideoMemory() const noexcept {
        return DedicatedVideoMemory;
      }
      size_t dedicatedSystemMemory() const noexcept {
        return DedicatedSystemMemory;
      }
      size_t sharedSystemMemory() const noexcept {
        return SharedSystemMemory;
      }
      const LUID& getLUID() const noexcept {
        return AdapterLUID;
      }
      Status getLastError() const noexcept {
        return lastResult;
      }
      Result<Output> getMonitor(u32 monitorID) const noexcept {
        RawResult result;
        IDXGIOutput* pOutput;
        IDXGIOutput6* pOutput6;
        if ((result = (RawResult)pAdapter->EnumOutputs(monitorID, &pOutput)))
          return result;
        if ((result = (RawResult)pOutput->QueryInterface(&pOutput6)))
          return result;
        DXGI_OUTPUT_DESC1 outputDesc;
        if ((result = (RawResult)pOutput6->GetDesc1(&outputDesc)))
          return result;
        return { pOutput6, outputDesc };
      }
    };

    class Factory{
      Microsoft::WRL::ComPtr<IDXGIFactory7> pFactory;
    public:
      Factory() = default;
      Factory(IDXGIFactory7* pFactory) noexcept : pFactory(pFactory){}
      Factory(const Factory& other) : pFactory(other.pFactory){}
      Factory(Factory&& other) noexcept : pFactory(std::move(other.pFactory)){}

      Factory& operator=(const Factory&) = default;
      Factory& operator=(Factory&&) noexcept = default;

      Status          makeWindowAssociation(HWND hWindow) const noexcept {
        return (RawResult)pFactory->MakeWindowAssociation(hWindow, 0);
      }
      Result<HWND>    getWindowAssociation() const noexcept {
        HWND hWindow;
        auto result = (RawResult)pFactory->GetWindowAssociation(&hWindow);
        if (result == S_OK)
          return hWindow;
        return Status(result);
      }
      Result<Adapter> getAdapter(u32 adapterIndex) const noexcept {
        using AdapterResult = Result<Adapter>;
        IDXGIAdapter4* pAdapter;
        IDXGIFactory7* pFac = pFactory.Get();
        auto&& uuid = __uuidof(IDXGIAdapter4);
        void** ppAdapter = (void**)&pAdapter;
        auto result = (RawResult)pFac->EnumAdapterByGpuPreference(adapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, uuid, ppAdapter);
        if (result == (RawResult)S_OK)
          return pAdapter;
        return result;
      }
    };

    inline Result<Factory> createFactory() noexcept {
      IDXGIFactory7* pFactory;
      auto result = (RawResult)CreateDXGIFactory2(VK_if(VK_debug_build(DXGI_CREATE_FACTORY_DEBUG)VK_else(0x0)), __uuidof(IDXGIFactory7), &(void*&)pFactory);
      if (result == S_OK)
        return pFactory;
      return result;
    }
  }
}


#endif //VALKYRIE_INTERNAL_DXGI_WRAPPERS_HPP
