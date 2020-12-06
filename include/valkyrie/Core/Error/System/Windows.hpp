//
// Created by Maxwell on 2020-11-21.
//

#ifndef VALKYRIE_WINDOWS_ERROR_HPP
#define VALKYRIE_WINDOWS_ERROR_HPP

#include <valkyrie/Core/Config.hpp>
#include <valkyrie/Core/Error/Severity.hpp>
#include <valkyrie/Core/Types.hpp>

#include <valkyrie/Core/Error/Status.hpp>

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
#include <Windows.h>
VK_msvc_warning(pop)

namespace valkyrie::Core::System::Win32{
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
    Severity severity() const noexcept {
      switch(severity_) {
        case 0:
          return Severity::Success;
        case 1:
          return Severity::Info;
        case 2:
          return isNStatus ? Severity::Warning : Severity::Error;
        case 3:
          return Severity::Error;
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

#include <valkyrie/Core/Error/System/Win32.hpp>
#include <valkyrie/Core/Error/System/WinNT.hpp>
#include <valkyrie/Core/Error/System/COM.hpp>

#endif//VALKYRIE_WINDOWS_ERROR_HPP
