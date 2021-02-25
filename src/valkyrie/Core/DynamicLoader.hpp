//
// Created by Maxwell on 2021-02-12.
//

#ifndef VALKYRIE_SRC_CORE_DYNAMIC_LOADER_HPP
#define VALKYRIE_SRC_CORE_DYNAMIC_LOADER_HPP

#include <valkyrie/Core/Error/Status.hpp>

#define NOMINMAX

#include <windows.h>

#include <sstream>

namespace valkyrie{

#define VK_load_function(loader, function) using PFN_##function = decltype(&function); PFN_##function function{ loader.load(VK_stringify(function)) };

  template <typename Ret, typename ...Args>
  using FunctionPointer = Ret(*)(Args...);
  template <typename Ret, typename ...Args>
  using FunctionPointerNoReturn = Ret(* __attribute__((noreturn)))(Args...);


  class DynamicLoader{
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
    explicit DynamicLoader(const char* libraryName) noexcept : hLibrary(LoadLibrary(libraryName)), libraryName(libraryName) {
      if (!hLibrary) [[unlikely]] {
        auto lastError = Core::System::Win32::getLastError();
        std::basic_stringstream<utf8> errMsg;
        errMsg << "In Function: LoadLibrary(\"" << libraryName << "\")";
        panic(lastError, errMsg.str().c_str());
      }
    }
    ~DynamicLoader(){
      if (!FreeLibrary(hLibrary)) [[unlikely]] {
        auto lastError = Core::System::Win32::getLastError();
        std::basic_stringstream<utf8> errMsg;
        errMsg << "In Function: FreeLibrary(" << hLibrary << ")\n";
        errMsg << "Library Name: " << libraryName << "\n";
        panic(lastError, errMsg.str().c_str());
      }
    }

    PolymorphicFunction load(const char* functionName) const noexcept {
      auto result = GetProcAddress(hLibrary, functionName);
      if (!result) {
        auto lastError = Core::System::Win32::getLastError();
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
