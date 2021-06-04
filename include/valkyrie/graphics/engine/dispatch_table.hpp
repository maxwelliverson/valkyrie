//
// Created by maxwe on 2021-05-10.
//

#ifndef VALKYRIE_GRAPHICS_ENGINE_DISPATCH_TABLE_HPP
#define VALKYRIE_GRAPHICS_ENGINE_DISPATCH_TABLE_HPP

#include <valkyrie/primitives.hpp>

#include <valkyrie/utility/bitflag.hpp>

namespace valkyrie{

  namespace impl{
    enum class dispatch : u32{
      has_state  = 0x1,
      owns_state = 0x2,
      dominates  = 0x4
    };
  }

  template <>
  struct traits::bitflag_enum<impl::dispatch>{

  };

  class symbol{};

  class command_handle;

  namespace impl{
    using any_function_ptr = function_ptr<void>;



    struct dispatch_command {
      dispatch_command*  next_command;
      any_function_ptr   function;
      void*              state;
      u32                flags;
      /*bitflags<dispatch> flags;*/
    };
    struct dispatch_entry {
      symbol            name;
      dispatch_command* command;
      dispatch_command* last_command;
    };




    class dispatch_table{
    protected:

      dispatch_command* insert_command(u32 index, void* memory, any_function_ptr function) noexcept;
      dispatch_command* insert_command(u32 index, void* memory, any_function_ptr function, void* state) noexcept;
      dispatch_command* insert_command_owning_state(u32 index, void* memory, any_function_ptr function, void* state) noexcept;

      dispatch_command* lookup_command(u32 index) const noexcept;

      void              push_front_command(u32 index, dispatch_command* command) noexcept;
      void              push_back_command(u32 index, dispatch_command* command) noexcept;
      dispatch_command* pop_front_command(u32 index) noexcept;
      dispatch_command* pop_back_command(u32 index) noexcept;


      dispatch_command* clear_commands(u32 index) noexcept;
      dispatch_command* clear_commands() noexcept;

    public:

      VK_nodiscard u32 size() const noexcept {
        return entry_count;
      }


    private:
      dispatch_table* parent_table;
      dispatch_entry* entry_table;
      u32             entry_count;
    };
  }


  class command_handle{
    impl::dispatch_table*   table;
    impl::dispatch_command* command;
  };

  template <typename CodeType, typename FuncType, typename Alloc>
  class basic_dispatch_table;

  template <typename CodeType, typename RetType, typename ...ArgTypes, typename Alloc>
  class basic_dispatch_table<CodeType, RetType(ArgTypes...), Alloc> : public impl::dispatch_table {
  public:



  private:

  };




  class dispatch_table_builder{
  public:

    void extend(dispatch_table* table) noexcept;

    void add_entry(unsigned code, symbol name) noexcept;


    dispatch_table build() noexcept;

    void reset() noexcept;
  };
  
  
}

#endif  //VALKYRIE_GRAPHICS_ENGINE_DISPATCH_TABLE_HPP
