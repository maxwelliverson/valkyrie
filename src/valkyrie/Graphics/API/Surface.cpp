//
// Created by Maxwell on 2020-11-29.
//

#include <valkyrie/Graphics/API/Instance.hpp>
#include <valkyrie/Graphics/API/Surface.hpp>


#include "InternalStructures.hpp"
#include "FunctionPointers.hpp"

namespace Internal = valkyrie::Graphics::API::Internal;
namespace API = valkyrie::Graphics::API;

namespace {
  class Monitor{
    HMONITOR handle;
  };
  class Window{
    HWND handle;
  };

  inline constexpr decltype(auto) WindowClassName = "Valkyrie Class Window";

  inline void setLastResult(LPARAM val) noexcept {
    reinterpret_cast<Internal::SurfaceImpl*>(((CREATESTRUCT*)val)->lpCreateParams)->lastResult = valkyrie::Core::System::Win32::getLastError();
  }

  LRESULT valkyrieWinProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) noexcept{
    switch (message) {
      /*case WM_PAINT:

*/
      //TODO: Implement

      case WM_CREATE: {
        SetLastError(0);
        SetWindowLongPtr(window, 0, reinterpret_cast<valkyrie::i64>(((CREATESTRUCT *) lParam)->lpCreateParams));
        setLastResult(lParam);
      }
      default:
        return DefWindowProc(window, message, wParam, lParam);
    }
  }


  inline void registerValkyrieWindowClass(){
    static bool isRegistered = false;
    if (!isRegistered){
      WNDCLASSEX windowClass{
          .cbSize = sizeof(WNDCLASSEX),
          .style  = CS_OWNDC | CS_HREDRAW | CS_VREDRAW,
          .lpfnWndProc = valkyrieWinProc,
          .cbClsExtra  = 0,
          .cbWndExtra  = 8,
          .hInstance   = GetModuleHandle(nullptr),
          .hIcon       = LoadIcon(nullptr, IDI_APPLICATION),
          .hCursor     = LoadCursor(nullptr, IDC_ARROW),
          .hbrBackground = GetSysColorBrush(COLOR_BACKGROUND),
          .lpszMenuName  = nullptr,
          .lpszClassName = WindowClassName,
          .hIconSm       = nullptr
      };

      ATOM atom = RegisterClassEx(&windowClass);
      assert(atom);
      isRegistered = true;
    }
  }


}

class API::Surface::Impl : public Internal::SurfaceImpl{
public:
  Impl(Internal::InstanceImpl* pInstance, Core::Extent2D<> extent, VulkanStatus& result){
    pInstance = pInstance;
    allocator = &pInstance->allocationCallbacks;
    refCount  = 1;
    dims      = extent;

    registerValkyrieWindowClass();

    nativeProcess = GetModuleHandle(nullptr);
    nativeWindow = CreateWindowEx(
      WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW,
      WindowClassName,
      (const char*)pInstance->applicationName.c_str(),
      WS_VISIBLE,
      0,
      0,
      (int)extent.width,
      (int)extent.height,
      nullptr,
      nullptr,
      nativeProcess,
      this
    );

    VkWin32SurfaceCreateInfoKHR createInfo{
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .hinstance = nativeProcess,
        .hwnd      = nativeWindow
    };

    result = makeStatusCode(pInstance->instanceFunctions->createWin32Surface(pInstance->handle, &createInfo, allocator, &handle));
  }

  static Impl* create(Internal::InstanceImpl* pInstance, Core::Extent2D<> extent) noexcept {
    VulkanStatus status;
    auto* result = new Impl(pInstance, extent, status);
    if (status.failure())
      panic(status);
    return result;
  }
};

namespace valkyrie::Graphics::API{


  void Internal::destroySurfaceRef(Internal::SurfaceImpl* pImpl) noexcept {
    if (pImpl && !--pImpl->refCount) {
      pImpl->pInstance->instanceFunctions->destroySurface(pImpl->pInstance->handle, pImpl->handle, pImpl->allocator);
      destroyInstanceRef(pImpl->pInstance);
      delete pImpl;
    }
  }


  Surface::Surface(const Instance& instance, Core::Extent2D<> extent) noexcept
      : pImpl(Impl::create(*reinterpret_cast<Internal::InstanceImpl* const *>(&instance), extent)){}
  Surface::Surface(const Surface & other) : pImpl(other.pImpl){
    ++pImpl->refCount;
  }
  Surface::Surface(Surface && other) noexcept : pImpl(other.pImpl){
    other.pImpl = nullptr;
  }
  Surface& Surface::operator=(const Surface& other) {
    if (pImpl == other.pImpl)
      return *this;
    Internal::destroySurfaceRef(pImpl);
    pImpl = other.pImpl;
    if (pImpl)
      ++pImpl->refCount;
    return *this;
  }
  Surface& Surface::operator=(Surface&& other) noexcept {
    if (pImpl != other.pImpl) {
      Internal::destroySurfaceRef(pImpl);
      pImpl = other.pImpl;
      other.pImpl = nullptr;
    }
    return *this;
  }
  Surface::~Surface(){
    Internal::destroySurfaceRef(pImpl);
  }

  bool Surface::isValid() const noexcept {
    return pImpl;
  }
  Core::Extent2D<> Surface::getExtents() const noexcept {
    return pImpl ? pImpl->dims : Core::Extent2D<>{0, 0};
  }
}



