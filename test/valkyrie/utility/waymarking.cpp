//
// Created by maxwe on 2021-05-01.
//

#include <valkyrie/adt/string.hpp>
#include <valkyrie/utility/waymarking.hpp>

#include <unordered_map>
#include <new>


namespace vlk = valkyrie;


class serializer{
public:
  template <typename T>
  bool apply(const T& obj) {

  }
};
class deserializer{
public:
  template <typename T>
  bool apply(T&);

  virtual bool apply(bool b) = 0;
};

namespace typename_impl{
  template <typename T>
  struct type_name_WRAPPER;

  VK_constant std::string_view type_name_WRAPPER_str = "type_name_WRAPPER<";

  template <typename T>
  VK_constant std::string_view get_typename_2() noexcept {
    return VK_function_name;
  }

  template <typename T>
  VK_constant std::string_view get_typename() noexcept {
    auto full_string = get_typename_2<type_name_WRAPPER<T>>();
    full_string.remove_prefix(full_string.find(type_name_WRAPPER_str) + type_name_WRAPPER_str.length());
    full_string.remove_suffix((full_string.length() - full_string.find(">>(void)")) + 1);
    return full_string;
  }
}

template <typename T>
VK_constant std::string_view type_name() noexcept {
  return typename_impl::get_typename<T>();
}


namespace hash_impl{

  VK_constant vlk::u64 hash_offset = 14695981039346656037ULL;
  VK_constant vlk::u64 hash_prime  = 1099511628211ULL;


  VK_constant vlk::u64 _hash_append_bytes(std::string_view string, vlk::u64 val = hash_offset) noexcept {
    for (const char c : string) {
      val ^= static_cast<const vlk::u64>(static_cast<const unsigned char>(c));
      val *= hash_prime;
    }
    return val;
  }


  VK_constant vlk::u64 hash_string(std::string_view type, vlk::u64 val = hash_offset) noexcept {
    return _hash_append_bytes(type, val);
  }

  template <typename T>
  VK_constant vlk::u64 hash_type(vlk::u64 val = hash_offset) noexcept {
    return hash_string(type_name<T>(), val);
  }

  template <vlk::typelist T>
  VK_constant vlk::u64 hash_typelist(vlk::u64 val = hash_offset) noexcept {
    return hash_typelist<vlk::meta::pop_front_t<T>>(hash_type<vlk::meta::front_t<T>>(val));
  }
  template <>
  inline constexpr vlk::u64 hash_typelist<vlk::meta::empty_list>(vlk::u64 val) noexcept {
    return val;
  }

  template <typename T>
  VK_constant vlk::u64 hash() noexcept {
    if constexpr (vlk::typelist<T>)
      return hash_typelist<T>();
    else
      return hash_type<T>();
  }
}


struct virtual_object_function_table{
  vlk::string_view name;
  vlk::u64         size;

  void(*pfn_destroy)(void*);
  void(*pfn_default_construct)(void*);
  void(*pfn_copy_construct)(void*, const void*);
  void(*pfn_move_construct)(void*, void*);

  bool(*pfn_serialize)(serializer&, const void*);
  bool(*pfn_deserialize)(deserializer&, void*);
};

namespace virtual_impl{

  VK_constant vlk::u64 virtual_align = 16;

  template <typename T>
  VK_constant virtual_object_function_table get_vtable() noexcept {
    return {
        .name                  = type_name<T>(),
        .size                  = (sizeof(T) + (virtual_align - 1)) & (~(virtual_align - 1)),
        .pfn_destroy           = [](void* p){ static_cast<T*>(p)->~T(); },
        .pfn_default_construct = [](void* p){ new(p) T(); },
        .pfn_copy_construct    = [](void* to, const void* from){ new(to) T(*static_cast<const T*>(from)); },
        .pfn_move_construct    = [](void* to, void* from){ new(to) T(std::move(*static_cast<T*>(from))); },
        .pfn_serialize         = [](serializer& ser, const void* obj) -> bool { return ser.apply(*static_cast<const T*>(obj)); },
        .pfn_deserialize       = [](deserializer& des, void* obj) -> bool { return des.apply(*static_cast<T*>(obj)); }
    };
  }

  template <typename T>
  VK_constant virtual_object_function_table vtable_for = get_vtable<T>();
}

using vtable_ptr = const virtual_object_function_table*;

class virtual_object_registry{
  std::unordered_map<vlk::u64, virtual_object_function_table> vtables;

public:

  virtual_object_registry() = default;

  template <typename T>
  bool register_type() noexcept {
    auto [iter, success] = vtables.try_emplace(hash_impl::hash<T>(), virtual_impl::vtable_for<T>);
    return success;
  }

  VK_nodiscard vtable_ptr lookup(vlk::u64 hash) const noexcept {
    auto result = vtables.find(hash);
    if ( result == vtables.cend() )
      return nullptr;
    return &result->second;
  }
};

class virtual_object{
  const virtual_object_function_table* vtable;
  void*                                object;

public:

};




#include <iostream>


int main(){

  std::cout << typename_impl::get_typename<typed_array<virtual_object>>() << std::endl;

  return 0;
}