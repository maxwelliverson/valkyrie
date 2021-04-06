//
// Created by maxwe on 2021-04-05.
//

#include <valkyrie/status/domains/std.hpp>
#include <valkyrie/adt/symbol.hpp>
#include <valkyrie/adt/small_vector.hpp>

#include <valkyrie/async/mutex.hpp>

using namespace valkyrie;


namespace {
  symbol_map<u32>                          id_map        {};
  small_vector<const std::error_category*> id_to_category{};

  shared_mutex                             id_map_mutex{};


  const std::error_category* get_error_category(u32 lookup) noexcept {
    lock outer(id_map_mutex, read_access);

    if (lookup < id_to_category.size()) {
      return id_to_category[lookup];
    }
    return nullptr;
  }
  u32                        get_index(const std::error_category* cat) noexcept {
    string_view category_name(cat->name());
    lock        outer(id_map_mutex, read_access);

    auto result = id_map.find(category_name);
    if (result == id_map.end()) {
      lock inner(*outer.take(), write_access);

      auto new_id = narrow_cast<u32>(id_to_category.size());

      id_map.insert({category_name, new_id});
      id_to_category.push_back(cat);

      VK_assert(new_id == id_to_category.size() - 1);
      VK_assert(id_map[category_name] == new_id);
      VK_assert(id_to_category[new_id] == cat);

      return new_id;
    }
    return result->get();
  }


  class std_string_ref : public string_ref{

    static void std_string_thunk(string_ref* This_, const string_ref* Other_, _thunk_op op) noexcept {
      auto* const       This = static_cast<std_string_ref *>(This_);
      const auto* const Other = static_cast<const std_string_ref *>(Other_);
      switch (op) {
        case _thunk_op::copy: {
          This->emplace_string(Other->get_string());
          break;
        }
        case _thunk_op::move: {
          //TODO: Fix this, undefined behaviour here from moving a const reference
          This->emplace_string((std::string&&)Other->get_string());
          break;
        }
        case _thunk_op::destruct: {
          VK_assert(!Other_);
          This->get_string().~basic_string();
        }
        VK_no_default;
      }
    }

    void*              state() const noexcept {
      return (void*)&this->_state[0];
    }

    std::string&       get_string()       noexcept {
      return *static_cast<std::string*>(state());
    }
    const std::string& get_string() const noexcept {
      return *static_cast<const std::string*>(state());
    }

    template <typename ...Args>
    void        emplace_string(Args&& ...args) noexcept {
      new(state()) std::string(std::forward<Args>(args)...);
      set_begin_end();
    }
    inline void set_begin_end() noexcept {
      auto& str = get_string();
      _begin = (utf8*)std::to_address(str.begin());
      _end   = (utf8*)std::to_address(str.end());
    }

  public:
    explicit std_string_ref(std::string&& str) noexcept : string_ref(std_string_thunk){
      this->emplace_string(std::move(str));
    }
  };

}

#define TO_CODE(stat) static_cast<const std_status&>(stat).value().code
#define TO_CAT(stat) get_error_category(static_cast<const std_status&>(stat).value().id)
#define TO_VALUE(stat) static_cast<const std_status&>(stat).value()



std::error_code std_error_category_domain::to_error_code(value_type val) noexcept {
  return std::error_code{ val.code, *get_error_category(val.id) };
}
std_error_category_domain::value_type std_error_category_domain::to_value_type(std::error_code std_err) noexcept {
  return value_type{ .id = get_index(&std_err.category()), .code = std_err.value() };
}
string_ref std_error_category_domain::do_message(const status_code<void> &status) const noexcept {
  auto Val = TO_VALUE(status);
  const auto* ErrCat = get_error_category(Val.id);
  return std_string_ref("[" + std::string(ErrCat->name()) + "] :: " + ErrCat->message(Val.code));
}
bool std_error_category_domain::do_failure(const status_code<void> &status) const noexcept {
  return TO_CODE(status) != 0;
}
bool std_error_category_domain::do_equivalent(const status_code<void> &statusA, const status_code<void> &statusB) const noexcept {
  return to_error_code(TO_VALUE(statusA)) == to_error_code(TO_VALUE(statusB));
}
code std_error_category_domain::do_generic_code(const status_code<void> &status) const noexcept {
  //TODO: Implement std_error_category_domain::do_generic_code
  VK_unreachable;
  return {};
}
severity std_error_category_domain::do_severity(const status_code<void> &status) const noexcept {
  //TODO: Implement std_error_category_domain::do_severity
  VK_unreachable;
  return {};
}
void std_error_category_domain::do_throw_exception(const status_code<void> &code) const {
#if VK_exceptions_enabled
  throw std::system_error(to_error_code(TO_VALUE(code)));
#else
  panic(code);
#endif
}
string_ref std_error_category_domain::name() const noexcept {
  return VK_string("std::error_category");
}


std_status make_status_code(std::error_code err) noexcept {
  return std_status(std::in_place, std_error_category_domain::to_value_type(err));
}