//
// Created by Maxwell on 2020-11-21.
//

#ifndef VALKYRIE_WINDOWS_ERROR_HPP
#define VALKYRIE_WINDOWS_ERROR_HPP

#include "../../preprocessor.hpp"
#include "../../primitives.hpp"
#include "../severity.hpp"
#include "../status_code.hpp"


VK_msvc_warning(push)
VK_msvc_warning(disable:5104)
VK_msvc_warning(disable:5105)
#define WIN32_LEAN_AND_MEAN
#define NOATOM
#define NOGDI
#define NOGDICAPMASKS
#define NOMETAFILE
#define NOMINMAX
#define NOMSG
#define NOOPENFILE
#define NORASTEROPS
#define NOSCROLL
#define NOSOUND
#define NOSYSMETRICS
#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOCRYPT
#define NOMCX
#include <windows.h>
            VK_msvc_warning(pop)

namespace valkyrie::sys::win32{
  class NativeHResult {
    u32 code_ : 16;
    u32 facility : 11;
    u32 reserved : 1;
    u32 isNStatus : 1;
    u32 customer : 1;
    u32 severity_ : 2;
  public:
    NativeHResult() noexcept = default;
    explicit NativeHResult(HRESULT value) noexcept : code_(), facility(), reserved(), isNStatus(), customer(), severity_(){
      ((HRESULT&)*this) = value;
    }

    u32 code() const noexcept {
      return code_;
    }
    severity severity() const noexcept {
      switch(severity_) {
        case 0:
          return severity::Success;
        case 1:
          return severity::Info;
        case 2:
          return isNStatus ? severity::Warning : severity::Error;
        case 3:
          return severity::Error;
        VK_no_default;
      }
    }

    explicit operator bool() const noexcept {
      return !(severity_ & 0x2);
    }
  };
  class NativeNTStatus {
    u32 severity_ : 2;
    u32 code_     : 30;
  public:
    NativeNTStatus() noexcept = default;
    explicit NativeNTStatus(u32 value) noexcept : severity_(), code_(){
      ((u32&)*this) = value;
    }

    explicit operator bool() const noexcept{
      return !severity_;
    }
  };
}

#include "COM.hpp"
#include "NT.hpp"
#include "win32.hpp"

#endif//VALKYRIE_WINDOWS_ERROR_HPP
