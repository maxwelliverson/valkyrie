//
// Created by Maxwell on 2021-02-12.
//

#ifndef VALKYRIE_SRC_CORE_DYNAMIC_LOADER_HPP
#define VALKYRIE_SRC_CORE_DYNAMIC_LOADER_HPP

#include <valkyrie/status/status_code.hpp>

#define NOMINMAX

#include <windows.h>

#include <sstream>

namespace valkyrie{

#define VK_load_function(loader, function) using PFN_##function = decltype(&function); PFN_##function function{ loader.load(VK_stringify(function)) };

  template <typename Ret, typename ...Args>
  using FunctionPointer = Ret(*)(Args...);
  template <typename Ret, typename ...Args>
  using FunctionPointerNoReturn = Ret(* __attribute__((noreturn)))(Args...);



  class dynamic_loader {
    HMODULE hLibrary;
    cstring libraryName;

    struct PolymorphicFunction{
      FARPROC pFunction;

      template <typename Ret, typename ...Args>
      explicit operator FunctionPointer<Ret, Args...>() const noexcept {
        return (FunctionPointer<Ret, Args...>)pFunction;
      }
      template <typename Ret, typename ...Args>
      explicit operator FunctionPointerNoReturn<Ret, Args...>() const noexcept {
        return (FunctionPointerNoReturn<Ret, Args...>)pFunction;
      }
    };


  public:
    explicit dynamic_loader(const char* libraryName) noexcept : hLibrary(LoadLibrary(libraryName)), libraryName(libraryName) {
      if (!hLibrary) VK_unlikely {
        auto lastError = sys::win32::get_last_error();
        std::basic_stringstream<utf8> errMsg;
        errMsg << "In Function: LoadLibrary(\"" << libraryName << "\")";
        panic(lastError, errMsg.str().c_str());
      }
    }
    ~dynamic_loader(){
      if (!FreeLibrary(hLibrary)) VK_unlikely {
        auto lastError = sys::win32::get_last_error();
        std::basic_stringstream<utf8> errMsg;
        errMsg << "In Function: FreeLibrary(" << hLibrary << ")\n";
        errMsg << "Library Name: " << libraryName << "\n";
        panic(lastError, errMsg.str().c_str());
      }
    }

    PolymorphicFunction load(const char* functionName) const noexcept {
      auto result = GetProcAddress(hLibrary, functionName);
      if (!result) {
        auto lastError = sys::win32::get_last_error();
        std::basic_stringstream<utf8> errMsg;
        errMsg << "In Function: GetProcAddress(" << hLibrary << ", \"" << functionName << "\")\n";
        errMsg << "Library Name: " << libraryName << "\n";
        panic(lastError, errMsg.str().c_str());
      }
      return { .pFunction = result };
    }
  };

}

#endif //VALKYRIE_SRC_CORE_DYNAMIC_LOADER_HPP
