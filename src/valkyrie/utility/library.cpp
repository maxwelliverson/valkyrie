//
// Created by maxwe on 2021-03-04.
//

#include <valkyrie/utility/library.hpp>

#define NOMINMAX
#include <windows.h>




valkyrie::library::library(string_view libName, system_status &opResult)
    : handle(LoadLibrary(libName.c_str())),
      name(libName.data(), libName.size()) {
  opResult = sys::win32::getLastError();
}
valkyrie::library::~library() {
  const auto result = FreeLibrary((HINSTANCE)handle);
  INVARIANT( result );
}
valkyrie::system_result<valkyrie::library> valkyrie::library::open(string_view libName) noexcept {

}
valkyrie::library::polymorphic_function valkyrie::library::load(string_view functionName) noexcept {
  return { (void(*)())GetProcAddress((HMODULE)handle, functionName.c_str()) };
}

