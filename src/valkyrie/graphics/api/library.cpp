//
// Created by maxwe on 2021-05-26.
//

#include <valkyrie/graphics/api/library.hpp>



#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>



using namespace valkyrie;

namespace {

  inline constexpr char vulkan_lib_name[] = "vulkan-1";

  class vulkan_core_library final : public impl::library {

    using PFN_getInstanceProcAddr = function_ptr<void(__stdcall*)(void), void*, utf8_string>;

    HMODULE libraryHandle;
    PFN_getInstanceProcAddr getInstProcAddress;




  public:

    vulkan_core_library(const vulkan_core_library& other) noexcept : getInstProcAddress(other.getInstProcAddress){}

    vulkan_core_library(status& s) noexcept{
      HMODULE libraryHandle = LoadLibrary(vulkan_lib_name);
      if ( !libraryHandle ) {
        s = sys::win32::get_last_error();
        return;
      }

      HMODULE cloned_handle;
      if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_PIN, vulkan_lib_name, &cloned_handle)) {

      }

      getInstProcAddress = (PFN_getInstanceProcAddr)GetProcAddress(libraryHandle, "vkGetInstanceProcAddr");
      if ( getInstProcAddress ) {
        s = sys::win32::get_last_error();
        return;
      }

      s = {};
    }


    string_view do_get_name() const noexcept override {
      return u8"vulkan_free_functions";
    }
    bool do_get_filepath(fs::path& path, status& s) const noexcept override {
      s = code::resource_not_found;
      return false;
    }
    vulkan_core_library* do_clone(void* memory, status& s) const noexcept override {
      return new(memory) vulkan_core_library(*this, s);
    }

    function_ptr<void> do_load(utf8_string name, abi_type& abi, status& s) noexcept override {
      abi = abi_type::c;
      function_ptr<void> ptr = getInstProcAddress(nullptr, name);
      if ( !ptr )
        s = system_status(ERROR_NOT_FOUND);
      return ptr;
    }

    u64 do_get_type_size() const noexcept override {
      return sizeof(vulkan_core_library);
    }
    u64 do_get_type_align() const noexcept override {
      return alignof(vulkan_core_library);
    }

    status do_pin() noexcept override {
      return {};
    }

    void do_close() VK_throws override { }

    void anchor() override { }
  };
}