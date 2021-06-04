//
// Created by maxwe on 2021-03-04.
//

#ifndef VALKYRIE_UTILITY_LIBRARY_HPP
#define VALKYRIE_UTILITY_LIBRARY_HPP

#include <valkyrie/adt/string.hpp>
#include <valkyrie/status/result.hpp>
#include <valkyrie/memory/default_allocator.hpp>
#include <valkyrie/memory/new_delete.hpp>
#include <valkyrie/memory/malloc.hpp>
#include <valkyrie/memory/smart_ptr.hpp>
#include <valkyrie/utility/filesystem.hpp>

namespace valkyrie{

  enum class abi_type : u8 {
    c,
    itanium,
    msvc_x86,
    msvc_x64,
    
    native_cxx = VK_if(VK_and(VK_system_windows, VK_64bit)(msvc_x64)VK_else_if(VK_and(VK_system_windows, VK_32bit)(msvc_x86)VK_else(itanium)))
  };


  template <allocator_c Alloc = default_allocator>
  class basic_library;

  namespace impl{
    class library;
  }
  
  class demangler final {
    
    abi_type abi;
    
    struct demangle_state;
    
    u64          get_state_size() const noexcept;

    void         do_begin_demangle(demangle_state* state, string_view mangled_name, std::span<utf8> buffer) const noexcept;
    bool         do_demangle(demangle_state* state, status& s) const noexcept;
    u64          get_next_buffer_size(demangle_state* state) const noexcept;
    void         do_update_buffer(demangle_state* state, std::span<utf8> buffer) const noexcept;
    u64          do_finish_demangle(demangle_state* state) const noexcept;
    
    
  public:
    demangler() noexcept : abi(abi_type::native_cxx){}
    explicit demangler(abi_type abi) noexcept : abi(abi){}


    result<string_view> operator()(string_view mangled_name, std::span<utf8> buffer) const noexcept {

      status      current_status;
      auto* state = reinterpret_cast<demangle_state*>(alloca(get_state_size()));

      do_begin_demangle(state, mangled_name, buffer);

      if ( !do_demangle(state, current_status) )
        return std::move(current_status);
      return string_view(buffer.data(), do_finish_demangle(state));
    }

    template <typename Alloc> requires allocator_c<Alloc>
    result<basic_string<Alloc>> operator()(string_view mangled_name, Alloc& allocator) const noexcept {

      using string_type = basic_string<Alloc>;

      u64   buffer_size = mangled_name.size() + 1;
      utf8* buffer_ptr  = reinterpret_cast<utf8*>(raw_malloc(allocator, buffer_size));
      raw_memzero(allocator, buffer_ptr, buffer_size);

      status      current_status;

      auto* state = reinterpret_cast<demangle_state*>(alloca(get_state_size()));

      do_begin_demangle(state, mangled_name, { buffer_ptr, buffer_size });

      while ( !do_demangle(state, current_status) ) {
        if ( current_status.generic() != code::in_progress )
          return std::move(current_status);

        u64   new_buffer_size = get_next_buffer_size(state);
        utf8* new_buffer_ptr  = reinterpret_cast<utf8*>(raw_realloc(allocator, buffer_ptr, new_buffer_ptr, buffer_size));

        if ( !new_buffer_ptr ) VK_unlikely {
          new_buffer_ptr = reinterpret_cast<utf8*>(raw_malloc(allocator, new_buffer_size));
          if ( !new_buffer_ptr ) VK_unlikely {
            raw_free(allocator, buffer_ptr, buffer_size);
            return code::out_of_memory;
          }
          raw_memcpy(allocator, new_buffer_ptr, buffer_ptr, buffer_size);
          raw_memzero(allocator, new_buffer_ptr + buffer_size, new_buffer_size - buffer_size);
          raw_free(allocator, std::exchange(buffer_ptr, new_buffer_ptr), std::exchange(buffer_size, new_buffer_size));
        }
        else {
          raw_memzero(allocator, new_buffer_ptr + buffer_size, new_buffer_size - buffer_size);
          buffer_ptr = new_buffer_ptr;
          buffer_size = new_buffer_size;
        }

        do_update_buffer(state, { buffer_ptr, buffer_size });
      }

      const u64 final_size = do_finish_demangle(state);

      return basic_string<Alloc>();
    }
  };
  
  class mangler final {
    
    abi_type abi;
    
    
    struct mangle_state;

    u64          get_state_size() const noexcept;

    void         do_begin_mangle(mangle_state* state, string_view mangled_name, std::span<utf8> buffer) const noexcept;
    bool         do_mangle(mangle_state* state, status& s) const noexcept;
    u64          get_next_buffer_size(mangle_state* state) const noexcept;
    void         do_update_buffer(mangle_state* state, std::span<utf8> buffer) const noexcept;
    u64          do_finish_mangle(mangle_state* state) const noexcept;
    
  public:
    mangler() noexcept : abi(abi_type::native_cxx){}
    mangler(abi_type abi) noexcept : abi(abi){}
  };
  
  

  class library_function{
    function_ptr<void> pfn;
    string_view        fn_name;
    abi_type           fn_abi;
    // some kind of type_id



    /*struct demangle_state;

    u64          demangle_state_size() const noexcept;

    void         do_begin_demangle(demangle_state* state, span<utf8> buffer) const noexcept;
    bool         do_demangle(demangle_state* state, status& s) const noexcept;
    u64          get_next_growth_size(demangle_state* state) const noexcept;
    void         do_update_demangle(demangle_state* state, span<utf8> buffer) const noexcept;
    u64          do_finish_demangle(demangle_state* state) const noexcept;*/


  public:
    library_function(function_ptr<void> pfn, string_view name, abi_type abi) noexcept
        : pfn(pfn), fn_name(name), fn_abi(abi){ }

    template <typename Ret, typename ...Args>
    operator function_ptr<Ret, Args...>() const noexcept {
      return static_cast<function_ptr<Ret, Args...>>(pfn);
    }

    VK_nodiscard string_view name() const noexcept {
      return fn_name;
    }
    VK_nodiscard abi_type    abi() const noexcept {
      return fn_abi;
    }

    /*template <typename Alloc = default_allocator> requires allocator_c<remove_ref_t<Alloc>>
    result<basic_string<Alloc>> demangled_name(Alloc&& allocator = {}) const noexcept {

      using string_type = basic_string<Alloc>;

      string_type demangled_buffer{ make_std_allocator(allocator) };
      status      current_status;
      demangled_buffer.resize(fn_name.size(), u8'\0');

      auto* state = reinterpret_cast<demangle_state*>(alloca(demangle_state_size()));

      do_begin_demangle(state, demangled_buffer);

      while ( !do_demangle(state, current_status) ) {
        if ( current_status.generic() != code::in_progress )
          return std::move(current_status);

        demangled_buffer.resize(get_next_growth_size(state));
        do_grow_buffer(state, demangled_buffer);
      }

      demangled_buffer.resize(do_finish_demangle(state), {});
      return std::move(demangled_buffer);
    }*/
  };

  /*class library_iterator{
  public:

  private:
    impl::library*           lib;
    mutable library_function fn;
  };

  class library_sentinel{

  };*/

  namespace impl{
    VK_interface library {

      // TODO: Implement iteration?

      virtual ~library() = default;


      virtual string_view        do_get_name() const noexcept = 0;
      virtual bool               do_get_filepath(fs::path& path, status& s) const noexcept = 0;

      virtual library*           do_clone(void* memory, status& s) const noexcept = 0;

      virtual function_ptr<void> do_load(utf8_string name, abi_type& abi, status& s) noexcept = 0;

      virtual status             do_pin() noexcept = 0;

      virtual void               do_close() VK_throws = 0;


      virtual u64                do_get_type_size() const noexcept = 0;
      virtual u64                do_get_type_align() const noexcept = 0;



      virtual void anchor();
    };

    extern const u64 system_library_size;
    extern const u64 system_library_align;

    library* system_library_open(void* memory, string_view name, const fs::path* path, system_status& s) noexcept;
  }

  template <allocator_c Alloc>
  class basic_library {

    using trt = allocator_traits<Alloc>;
    using allocator_type = typename allocator_traits<Alloc>::allocator_type;

    result<impl::library*> clone_instance(const impl::library* other) noexcept {
      const u64 size = other->do_get_type_size();
      const u64 align = other->do_get_type_align();
      status s;
      void* memory = trt::allocate_node(allocator, size, align);
      auto* obj = other->do_clone(memory, s);

      if ( obj )
        return obj;
      return std::move(s);
    }

  public:

    basic_library() = default;

    basic_library(impl::library* VK_param(nonnull) obj, allocator_type&& alloc) noexcept
        : allocator(std::move(alloc)),
          instance(obj){ }

    basic_library(const basic_library& other)
        : allocator(trt::select_on_container_copy_construct(other.allocator)),
          instance(clone_instance(other.instance).value()){ }

    basic_library(basic_library&& other) noexcept
        : allocator(std::move(other.allocator)),
          instance(std::exchange(other.instance, nullptr)){ }

    ~basic_library() {
      if ( instance ) {
        instance->do_close();
        const u64 size = instance->do_get_type_size();
        const u64 align = instance->do_get_type_align();
        instance->~library();
        trt::deallocate_node(allocator, instance, size, align);
      }
    }




    string_view name() const noexcept {
      VK_assert( instance != nullptr );
      VK_assume_nonnull( instance );

      return instance->do_get_name();
    }

    result<library_function> load(utf8_string name) const noexcept {
      abi_type abi;
      status   s;
      function_ptr<void> pfn = instance->do_load(name, abi, s);

      if ( !pfn )
        return std::move(s);

      return { pfn, string_view(name), abi };
    }

    void close() noexcept {
      instance->do_close();
    }

    result<fs::path> filepath() const noexcept {
      fs::path path;
      status   s;

      if ( instance->do_get_filepath(path, s) )
        return path;
      return std::move(s);
    }


    status pin() noexcept {
      VK_assert( instance );
      VK_assume_nonnull( instance );
      return instance->do_pin();
    }


    static system_result<basic_library> open(allocator_type alloc, string_view libname, const fs::path& path = {}) noexcept {
      const u64 size = impl::system_library_size;
      const u64 align = impl::system_library_align;
      void* memory = trt::allocate_node(alloc, size, align);
      system_status s;

      if (auto* lib = impl::system_library_open(memory, libname, path.empty() ? nullptr : &path, s))
        return { lib, std::move(alloc) };
      return std::move(s);
    }
    static system_result<basic_library> open(string_view libname, const fs::path& path = {}) noexcept /*requires(std::is_default_constructible_v<allocator_type>) */{
      return open(allocator_type{}, libname, path);
    }

  private:
    [[no_unique_address]] allocator_type allocator;
    impl::library*                       instance;
  };

  using library = basic_library<>;


  template <typename Lib, typename Alloc = default_allocator, typename ...Args>
  requires ( std::derived_from<Lib, impl::library> && raw_allocator_c<Alloc> && constructible_from<Lib, Args...>)
  basic_library<Alloc> make_library(Args&& ...args) noexcept {}
}

#endif //VALKYRIE_UTILITY_LIBRARY_HPP
