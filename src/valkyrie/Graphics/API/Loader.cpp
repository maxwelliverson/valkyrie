//
// Created by Maxwell on 2020-11-15.
//

#include "FunctionPointers.hpp"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

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
  HINSTANCE library = LoadLibraryA("vulkan-1.dll");
  this->getProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(::GetProcAddress(library, "vkGetInstanceProcAddr"));
  FreeLibrary(library);
}