//
// Created by Maxwell on 2020-11-15.
//

#include "FunctionPointers.hpp"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <utility>

/*class Library{
public:
  Library(){
    library_ = LoadLibraryA("vulkan-1.dll");
  }
  ~Library(){
    FreeLibrary(library_);
  }

  template <typename T>
  T getProcAddress( const char* function ) const noexcept {
    return (T)::GetProcAddress(library_, function );
  }


private:
  HINSTANCE library_;
};

const static Library lib{};*/

FreeFunctionLoader::FreeFunctionLoader() {
  libraryHandle = LoadLibrary("vulkan-1.dll");
  this->getProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(::GetProcAddress((HMODULE)libraryHandle, "vkGetInstanceProcAddr"));
}
FreeFunctionLoader::FreeFunctionLoader(FreeFunctionLoader&& other) noexcept : libraryHandle(nullptr), getProcAddr(nullptr){
  std::swap(libraryHandle, other.libraryHandle);
  std::swap(getProcAddr, other.getProcAddr);
}
FreeFunctionLoader::~FreeFunctionLoader() {
  if (libraryHandle) {
    auto freeResult = FreeLibrary((HMODULE) libraryHandle);
    assert(freeResult);
  }
}

namespace {
  inline FreeFunctions* getFreeFunctions() noexcept {
    static FreeFunctions instance{};
    return &instance;
  }
}
FreeFunctions* FreeFunctions::pInstance = getFreeFunctions();