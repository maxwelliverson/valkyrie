//
// Created by maxwe on 2021-05-14.
//

#ifndef VALKYRIE_ADT_TWINE_HPP
#define VALKYRIE_ADT_TWINE_HPP

#include "string.hpp"
#include "small_string.hpp"
#include "function_ref.hpp"

namespace valkyrie{

  namespace impl{
    class small_string_ref_accessor : public small_array_base<u64>{
    public:
      VK_nodiscard const utf8* data() const noexcept {
        return static_cast<const utf8*>(this->begin_);
      }
    };
  }


  class  string_view;
  class  mutable_string_view;
  class  string_ref;
  struct mem;


  class twine{

    using small_string_base = impl::small_array_base<u64>;

    enum class type : u8{
      null,
      empty,
      twine,
      string,
      std_string,
      string_view,
      mutable_string_view,
      string_ref,
      small_string,
      // string_template_instance,
      utf8_string,
      utf8_char,
      u32_decimal,
      i32_decimal,
      u64_decimal,
      i64_decimal,
      u64_hex,
      f32,
      f64,
      mem
    };
    union child{
      const twine*               m_twine;
      const string*              m_string;
      const std::string*         m_std_string;
      const string_view*         m_string_view;
      const mutable_string_view* m_mutable_string_view;
      const small_string_base*   m_small_string;
      const string_ref*          m_string_ref;
      const utf8*                m_utf8_string;
      const mem*                 m_mem;
      utf8                       m_utf8;
      u32                        m_u32;
      i32                        m_i32;
      u64                        m_u64;
      i64                        m_i64;
      f32                        m_f32;
      f64                        m_f64;
    };

    child left_child;
    child right_child;

    type  left_type  = type::empty;
    type  right_type = type::empty;


    static void print_child(function_ref<void(const utf8*, u64)> printer, child c, type child_type);
    static void print_child_repr(function_ref<void(const utf8*, u64)> printer, child c, type child_type);


    explicit twine(type Kind) noexcept
        : left_type(Kind) {
      VK_assert_msg(is_nullary(), "Invalid twine kind!");
    }

    explicit twine(const twine& left_child, const twine &right_child) noexcept
        : left_child{ .m_twine = &left_child },
          right_child{ .m_twine = &right_child },
          left_type(type::twine),
          right_type(type::twine) {
      VK_assert_msg(is_valid(), "Invalid twine!");
    }

    explicit twine(child left_child, type left_type, child right_child, type right_type)
        : left_child(left_child),
          right_child(right_child),
          left_type(left_type),
          right_type(right_type) {
      VK_assert_msg(is_valid(), "Invalid twine!");
    }



    VK_nodiscard bool is_null() const noexcept {
      return get_left_type() == type::null;
    }
    VK_nodiscard bool is_empty() const noexcept {
      return get_left_type() == type::empty;
    }

    VK_nodiscard bool is_nullary() const noexcept {
      return is_null() || is_empty();
    }
    VK_nodiscard bool is_unary() const noexcept {
      return get_right_type() == type::empty && !is_nullary();
    }
    VK_nodiscard bool is_binary() const noexcept {
      return !is_null() && get_right_type() != type::empty;
    }

    VK_nodiscard type get_left_type() const noexcept { return left_type; }
    VK_nodiscard type get_right_type() const noexcept { return right_type; }

    VK_nodiscard bool is_valid() const noexcept {
      if ( is_nullary() && get_right_type() != type::empty )
        return false;

      if ( get_right_type() == type::null )
        return false;

      if (get_right_type() != type::empty && get_left_type() == type::empty)
        return false;

      if (get_left_type() == type::twine && !left_child.m_twine->is_binary())
        return false;
      if (get_right_type() == type::twine && !right_child.m_twine->is_binary())
        return false;

      return true;
    }

  public:

    twine() {
      VK_assert_msg(is_valid(), "invalid twine");
    }
    twine(const twine&) = default;

    twine(utf8_string str) noexcept{

      VK_assert_msg(str, "Tried to construct a twine from a nullptr");

      if ( str[0] != u8'\0') VK_likely {
          left_child.m_utf8_string = str;
          left_type = type::utf8_string;
      }
      else {
        left_type = type::empty;
      }

      VK_assert_msg(is_valid(), "invalid twine");
    }
    twine(cstring str) noexcept : twine(reinterpret_cast<utf8_string>(str)){}


    twine(const string& str) noexcept
        : left_child{ .m_string = &str },
          left_type(type::string){
      VK_assert_msg(is_valid(), "invalid twine");
    }
    twine(const std::string& str) noexcept
        : left_child{ .m_std_string = &str },
          left_type(type::std_string){
      VK_assert_msg(is_valid(), "invalid twine");
    }
    template <typename Alloc>
    twine(const any_small_string<Alloc>& str) noexcept
        : left_child{ .m_small_string = &str },
          left_type(type::small_string){}

    twine(const string_view& sv) noexcept
        : left_child{ .m_string_view = &sv },
          left_type(type::string_view){
      VK_assert_msg(is_valid(), "invalid twine");
    }
    twine(const mutable_string_view& msv) noexcept
        : left_child{ .m_mutable_string_view = &msv },
          left_type(type::mutable_string_view){
      VK_assert_msg(is_valid(), "invalid twine");
    }

    twine(const string_ref& str) noexcept
        : left_child{ .m_string_ref = &str },
          left_type(type::string_ref){
      VK_assert_msg(is_valid(), "invalid twine");
    }


    twine(const mem& memory) noexcept
        : left_child{ .m_mem = &memory },
          left_type(type::mem){
      VK_assert_msg(is_valid(), "invalid twine");
    }



    explicit twine(utf8 c) noexcept
        : left_child{ .m_utf8 = c },
          left_type(type::utf8_char){
      VK_assert_msg(is_valid(), "invalid twine");
    }
    explicit twine(ascii c) noexcept
        : left_child{ .m_utf8 = static_cast<utf8>(c) },
          left_type(type::utf8_char){
      VK_assert_msg(is_valid(), "invalid twine");
    }
    explicit twine(i8 c) noexcept
        : left_child{ .m_utf8 = static_cast<utf8>(c) },
          left_type(type::utf8_char){
      VK_assert_msg(is_valid(), "invalid twine");
    }
    explicit twine(u8 c) noexcept
        : left_child{ .m_utf8 = static_cast<utf8>(c) },
          left_type(type::utf8_char){
      VK_assert_msg(is_valid(), "invalid twine");
    }

    explicit twine(u32 u) noexcept
        : left_child{ .m_u32 = u },
          left_type(type::u32_decimal){
      VK_assert_msg(is_valid(), "invalid twine");
    }
    explicit twine(i32 i) noexcept
        : left_child{ .m_i32 = i },
          left_type(type::i32_decimal){
      VK_assert_msg(is_valid(), "invalid twine");
    }

    explicit twine(u64 u) noexcept
        : left_child{ .m_u64 = u },
          left_type(type::u64_decimal){
      VK_assert_msg(is_valid(), "invalid twine");
    }
    explicit twine(i64 i) noexcept
        : left_child{ .m_i64 = i },
          left_type(type::i64_decimal){
      VK_assert_msg(is_valid(), "invalid twine");
    }


    explicit twine(f32 f) noexcept
        : left_child{ .m_f32 = f },
          left_type(type::f32){
      VK_assert_msg(is_valid(), "invalid twine");
    }
    explicit twine(f64 f) noexcept
        : left_child{ .m_f64 = f },
          left_type(type::f64){
      VK_assert_msg(is_valid(), "invalid twine");
    }



    twine& operator=(const twine&) = delete;


    static twine null() noexcept {
      return twine(type::null);
    }


    /// Check if this twine is trivially empty; a false return value does not
    /// necessarily mean the twine is empty.
    VK_nodiscard VK_forceinline bool is_trivially_empty() const noexcept {
      return is_nullary();
    }

    VK_nodiscard VK_forceinline bool is_trivially_viewable() const noexcept {
      if (get_right_type() != type::empty)
        return false;

      switch (get_left_type()) {
        case type::empty:
        case type::utf8_string:
        case type::string:
        case type::std_string:
        case type::string_view:
          return true;
        default:
          return false;
      }
    }

    /// @}
    /// @name String Operations
    /// @{

    VK_nodiscard twine concat(const twine& suffix) const noexcept;

    /// @}
    /// @name Output & Conversion.
    /// @{

    /// Return the twine contents as a string.
    VK_nodiscard string str() const noexcept;

    template <typename Alloc>
    void write_to(any_small_string<Alloc>& out) const noexcept {
      this->print([&](const utf8* str, u64 size) mutable {
        out.append(str, str + size);
      });
    }


    /// This returns the twine as a single string_view.  This method is only valid
    /// if is_trivially_viewable() is true.
    VK_nodiscard string_view get_string_view() const noexcept {
      VK_assert_msg(is_trivially_viewable(), "This twine is not viewable");
      switch (get_left_type()) {
        case type::empty:
          return {};
        case type::utf8_string:
          return left_child.m_utf8_string;
        case type::std_string:
          return *left_child.m_std_string;
        case type::string:
          return *left_child.m_string;
        case type::string_view:
          return *left_child.m_string_view;
          VK_no_default;
      }
    }

    /// This returns the twine as a single string_view if it can be
    /// represented as such. Otherwise the twine is written into the given
    /// SmallVector and a string_view to the SmallVector's data is returned.
    template <typename Alloc>
    string_view get_string_view(any_small_string<Alloc>& out) const {
      if (is_trivially_viewable())
        return get_string_view();
      write_to(out);
      return string_view(out.data(), out.size());
    }

    /// This returns the twine as a single null terminated string_view if it
    /// can be represented as such. Otherwise the twine is written into the
    /// given SmallVector and a string_view to the SmallVector's data is returned.
    ///
    /// The returned string_view's size does not include the null terminator.
    template <typename Alloc>
    string_view write_to_null_terminated(any_small_string<Alloc>& out) const noexcept {
      if ( is_unary() ) {
        switch (get_left_type()) {
          case type::utf8_string:
            // Already null terminated, yay!
            return string_view(left_child.m_utf8_string);
          case type::std_string: {
            const std::string* str = left_child.m_std_string;
            return string_view(str->c_str(), str->size());
          }
          default:
            break;
        }
      }
      write_to(out);
      out.push_back(u8'\0');
      out.pop_back();
      return string_view(out.data(), out.size());
    }



    void print(function_ref<void(const utf8*, u64)> printer) const;
    void print_raw_repr(function_ref<void(const utf8*, u64)> printer) const;
    void dump() const;
    void dump_raw_repr() const;


    /*void print(raw_ostream &OS) const;
    void dump() const;
    void printRepr(raw_ostream &OS) const;
    void dumpRepr() const;*/
  };
  
  twine twine::concat(const twine &suffix) const noexcept {
    // Concatenation with null is null.
    if (is_null() || suffix.is_null())
      return twine(type::null);

    // Concatenation with empty yields the other side.
    if (is_empty())
      return suffix;
    if (suffix.is_empty())
      return *this;

    // Otherwise we need to create a new node, taking care to fold in unary
    // twines.
    child new_left_child{ .m_twine = this };
    child new_right_child{ .m_twine = &suffix };
    type  new_left_type  = type::twine;
    type  new_right_type = type::twine;
    if ( is_unary() ) {
      new_left_child = this->left_child;
      new_left_type  = this->left_type;
    }
    if ( suffix.is_unary() ) {
      new_right_child = suffix.left_child;
      new_right_type  = suffix.left_type;
    }

    return twine(new_left_child, new_left_type, new_right_child, new_right_type);
  }

  inline twine operator+(const twine& a, const twine& b) noexcept {
    return a.concat(b);
  }
}

#endif  //VALKYRIE_ADT_TWINE_HPP
