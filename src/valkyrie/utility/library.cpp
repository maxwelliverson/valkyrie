//
// Created by maxwe on 2021-03-04.
//

#include <valkyrie/utility/library.hpp>
#include <valkyrie/status/domains/std.hpp>

#include <string>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

using namespace valkyrie;

namespace {

  class windows_dll final : public impl::library{
    string   name;
    HMODULE  handle;
    fs::path filepath;



  public:

    windows_dll(string_view name, HMODULE module, system_status& status) noexcept
        : name(name.data(), name.size()),
          handle(module),
          filepath() {
      constexpr static u32 MaxFilenameLen = 256;
      wchar_t buffer[MaxFilenameLen];
      auto result = GetModuleFileNameW(handle, buffer, MaxFilenameLen);
      status = sys::win32::get_last_error();
      wchar_t* filename_end   = wcsrchr(buffer, '\\');
      wchar_t* filename_begin = buffer;
      filepath = fs::path(std::wstring{filename_begin, u64(filename_end - filename_begin)});
    }

    windows_dll(string_view name, HMODULE module, const fs::path& p) noexcept
      : name(name.data(), name.size()),
        handle(module),
        filepath(p) { }


    ~windows_dll() override = default;


    windows_dll*    do_clone(void *memory, status &s) const noexcept override {
      HMODULE cloned_handle/* = handle*/;
      if ( !GetModuleHandleEx(0, (cstring)name.c_str(), &cloned_handle) ) {
        s = sys::win32::get_last_error();
        return nullptr;
      }
      return new(memory) windows_dll(name, cloned_handle, filepath);
    }

    string_view        do_get_name() const noexcept override {
      return name;
    }

    bool               do_get_filepath(fs::path& path, status& s) const noexcept override {
      std::error_code ec;
      path = fs::absolute(filepath, ec);
      if ( ec ) {
        s = make_status_code(ec);
        return false;
      }
      return true;
    }

    function_ptr<void> do_load(utf8_string fn_name, abi_type& abi, status& s) noexcept override{

      if ( !handle ) VK_unlikely {
        s = code::invalid_resource;
        return nullptr;
      }
      if ( !fn_name ) VK_unlikely {
        s = code::invalid_argument;
        return nullptr;
      }

      if ( fn_name[0] == '?' ) VK_unlikely
        abi = abi_type::msvc_x64;
      else
        abi = abi_type::c;

      auto result = GetProcAddress(handle, (cstring)fn_name);

      if ( !result )
        s = sys::win32::get_last_error();

      return function_ptr<void>(result);
    }

    status             do_pin() noexcept override {
      HMODULE cloned_handle;
      GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_PIN, (cstring)name.c_str(), &cloned_handle);
      return sys::win32::get_last_error();
    }

    void               do_close() VK_throws override {
      if (!FreeLibrary(handle)) {
        constexpr static u32 BufferSize = 1024;
        constexpr static char MessageFormat[] = "{ \"source\": { \"expression\": \"FreeLibrary(this->handle);\", \"function\": \"%s\", \"file\": \"%s\", \"line\": %d }, \"type\" = \"(anonymous)::windows_dll\", \"this\": { \"name\": \"%s\", \"handle\": %p, \"path\": \"%s\" }}";
        char buffer[BufferSize];
        auto path_str = filepath.string();
        sprintf_s(buffer, MessageFormat, VK_function_name, VK_filename, VK_line_number - 3, (cstring)name.c_str(), handle, path_str.c_str());
        panic(sys::win32::get_last_error(), (utf8(&)[BufferSize])buffer);
      }
    }

    u64                do_get_type_size() const noexcept override {
      return sizeof(windows_dll);
    }
    u64                do_get_type_align() const noexcept override {
      return alignof(windows_dll);
    }

    void               anchor() override {}
  };

}


const u64 impl::system_library_size = sizeof(windows_dll);
const u64 impl::system_library_align = alignof(windows_dll);

void impl::library::anchor(){ }


impl::library* impl::system_library_open(void *memory, string_view name, const fs::path *path, system_status &s) noexcept {
  constexpr static u32 BufferSize = 256;
  char buffer[BufferSize];
  HMODULE handle;

  if ( name.size() >= BufferSize ) {
    s = system_status(ERROR_FILENAME_EXCED_RANGE);
    return nullptr;
  }

  memcpy(buffer, name.data(), name.size());

  if ( path ) {

    std::error_code ec;
    fs::path abs_path = fs::absolute(*path, ec);
    if ( ec ) {
      s = system_status(ERROR_PATH_NOT_FOUND);
      return nullptr;
    }

    DLL_DIRECTORY_COOKIE searchPathCookie = AddDllDirectory(abs_path.c_str());
    if ( !searchPathCookie ) VK_likely {
      s = sys::win32::get_last_error();
      return nullptr;
    }

    handle = LoadLibraryEx(buffer, nullptr, LOAD_LIBRARY_SEARCH_USER_DIRS);

    if ( !handle ) {
      s = sys::win32::get_last_error();
      return nullptr;
    }

    if ( !RemoveDllDirectory(searchPathCookie) ) VK_likely {
      s = sys::win32::get_last_error();
      return nullptr;
    }

    return new(memory) windows_dll(name, handle, abs_path);
  }

  handle = LoadLibrary(buffer);

  if ( !handle ) {
    s = sys::win32::get_last_error();
    return nullptr;
  }

  return new(memory) windows_dll(name, handle, s);
}

