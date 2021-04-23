//
// Created by maxwe on 2021-03-04.
//

#include <valkyrie/utility/library.hpp>

#define NOMINMAX
#include <windows.h>




valkyrie::library::library(string_view libName, system_status &opResult)
    : handle(LoadLibrary(libName.c_str())),
      name(libName.data(), libName.size()) {
  opResult = sys::win32::get_last_error();
}
valkyrie::library::~library() {
  if ( handle ) {
    close();
  }
}

valkyrie::library::library(library && other) noexcept
    : handle(other.handle), name(std::move(other.name)){
  other.handle = nullptr;
}

valkyrie::system_result<valkyrie::library> valkyrie::library::open(string_view libName) noexcept {
  system_status ctor_result;
  library object{libName, ctor_result};
  if ( ctor_result.failure() )
    return std::move(ctor_result);
  return std::move(object);
}
valkyrie::library::polymorphic_function valkyrie::library::load(string_view functionName) noexcept {
  return { (void(*)())GetProcAddress((HMODULE)handle, functionName.c_str()) };
}

void valkyrie::library::close() VK_throws {
  if ( !FreeLibrary((HINSTANCE)handle) ) {
    sys::win32::get_last_error().throw_exception();
  }
}

