//
// Created by maxwe on 2021-05-14.
//

#include <valkyrie/adt/twine.hpp>

#include <charconv>

using namespace valkyrie;

namespace {

  template <typename T, typename ...Args>
  inline std::errc print_number(function_ref<void(const utf8 *, u64)> printer, T val, Args ...args) noexcept {
    constexpr static u64 BufferSize = 32;
    char buffer[BufferSize];
    auto [a, b] = std::to_chars(buffer, std::end(buffer), val, args...);
    if ( b != std::errc{})
      return b;
    printer((const utf8*)buffer, a - buffer);
    return std::errc{};
  }

  inline void print_mem(function_ref<void(const utf8*, u64)> printer, const mem& m) noexcept {

  }
}

void twine::print_child(function_ref<void(const utf8 *, u64)> printer, child c, type child_type) {
  switch ( child_type ) {

    case type::null:
    case type::empty: break;

    case type::twine:
      c.m_twine->print(printer);
      break;
    case type::string:
      printer(c.m_string->data(), c.m_string->size());
      break;
    case type::std_string:
      printer((const utf8*)c.m_std_string->data(), c.m_std_string->size());
      break;
    case type::string_view:
      printer(c.m_string_view->data(), c.m_string_view->size());
      break;
    case type::mutable_string_view:
      printer(c.m_mutable_string_view->data(), c.m_mutable_string_view->size());
      break;
    case type::string_ref:
      printer(c.m_string_ref->data(), c.m_string_ref->size());
      break;
    case type::small_string:
      printer(static_cast<const impl::small_string_ref_accessor*>(c.m_small_string)->data(),
              c.m_small_string->size());
      break;
    case type::utf8_string:
      printer(c.m_utf8_string, std::char_traits<utf8>::length(c.m_utf8_string));
      break;
    case type::utf8_char:
      printer(&c.m_utf8, 1);
      break;
      //TODO: Handle errc values returned by print_number
    case type::u32_decimal:
      print_number(printer, c.m_u32);
      break;
    case type::i32_decimal:
      print_number(printer, c.m_i32);
      break;
    case type::u64_decimal:
      print_number(printer, c.m_u64);
      break;
    case type::i64_decimal:
      print_number(printer, c.m_i64);
      break;
    case type::u64_hex:
      print_number(printer, c.m_u64, 16);
      break;
    case type::f32:
      print_number(printer, c.m_f32);
      break;
    case type::f64:
      print_number(printer, c.m_f64);
      break;
    case type::mem:
      print_mem(printer, *c.m_mem);
      break;
    VK_no_default;
  }
}
void twine::print_child_repr(function_ref<void(const utf8 *, u64)> printer, child c, type child_type) {
  switch ( child_type ) {

#define do_print(str) printer(u8##str, sizeof(str) - 1)

    case type::null:
      do_print("null");
      break;
    case type::empty:
      do_print("empty");
      break;

      // TODO: Continue
    case type::twine:
      do_print("rope:");
      c.m_twine->print_raw_repr(printer);
      break;
    case type::string:
      do_print("string:\"");
      printer(c.m_string->data(), c.m_string->size());
      do_print("\"");
      break;
    case type::std_string:
      do_print("std::string:\"");
      printer((const utf8*)c.m_std_string->data(), c.m_std_string->size());
      do_print("\"");
      break;
    case type::string_view:
      do_print("string_view:\"");
      printer(c.m_string_view->data(), c.m_string_view->size());
      do_print("\"");
      break;
    case type::mutable_string_view:
      do_print("mutable_string_view:\"");
      printer(c.m_mutable_string_view->data(), c.m_mutable_string_view->size());
      do_print("\"");
      break;
    case type::string_ref:
      do_print("string_ref:\"");
      printer(c.m_string_ref->data(), c.m_string_ref->size());
      do_print("\"");
      break;
    case type::small_string:
      do_print("small_string:\"");
      printer(static_cast<const impl::small_string_ref_accessor*>(c.m_small_string)->data(),
              c.m_small_string->size());
      do_print("\"");
      break;
    case type::utf8_string:
      do_print("cstring:\"");
      printer(c.m_utf8_string, std::char_traits<utf8>::length(c.m_utf8_string));
      do_print("\"");
      break;
    case type::utf8_char:
      do_print("char8_t:\"");
      printer(&c.m_utf8, 1);
      do_print("\"");
      break;
      //TODO: Handle errc values returned by print_number
    case type::u32_decimal:
      do_print("u32:\"");
      print_number(printer, c.m_u32);
      do_print("\"");
      break;
    case type::i32_decimal:
      do_print("i32:\"");
      print_number(printer, c.m_i32);
      do_print("\"");
      break;
    case type::u64_decimal:
      do_print("u64:\"");
      print_number(printer, c.m_u64);
      do_print("\"");
      break;
    case type::i64_decimal:
      do_print("i64:\"");
      print_number(printer, c.m_i64);
      do_print("\"");
      break;
    case type::u64_hex:
      do_print("u64_hex:\"");
      print_number(printer, c.m_u64, 16);
      do_print("\"");
      break;
    case type::f32:
      do_print("f32:\"");
      print_number(printer, c.m_f32);
      do_print("\"");
      break;
    case type::f64:
      do_print("f64:\"");
      print_number(printer, c.m_f64);
      do_print("\"");
      break;
    case type::mem:
      do_print("mem:\"");
      print_mem(printer, *c.m_mem);
      do_print("\"");
      break;
    VK_no_default;
  }
}

void twine::print(function_ref<void(const utf8 *, u64)> printer) const {
  print_child(printer, left_child, left_type);
  print_child(printer, right_child, right_type);
}

void twine::print_raw_repr(function_ref<void(const utf8 *, u64)> printer) const {
  do_print("(twine");
  print_child_repr(printer, left_child, left_type);
  do_print(" ");
  print_child_repr(printer, right_child, right_type);
  do_print(")");
}