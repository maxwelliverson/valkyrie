//
// Created by Maxwell on 2020-11-30.
//

#include "internal_structures.hpp"

#include <valkyrie/gui/window.hpp>

namespace {
  inline constexpr decltype(auto) WindowClassName = "Valkyrie Class Window";

  inline void setLastResult(LPARAM val) noexcept {
    reinterpret_cast<Internal::SurfaceImpl*>(((CREATESTRUCT*)val)->lpCreateParams)->lastResult = valkyrie::sys::win32::get_last_error();
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