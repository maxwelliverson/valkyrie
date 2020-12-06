//
// Created by Maxwell on 2020-11-09.
//

#ifndef VALKYRIE_TYPES_HPP
#define VALKYRIE_TYPES_HPP

#include <cstddef>

namespace valkyrie{
  using i8 = signed char;
  using u8 = unsigned char;
  using i16 = signed short;
  using u16 = unsigned short;
  using i32 = signed int;
  using u32 = unsigned int;
  using i64 = signed long long int;
  using u64 = unsigned long long int;
  using f32 = float;
  using f64 = double;

  using ascii = char;
  using utf8  = char8_t;
  using utf16 = char16_t;
  using utf32 = char32_t;

  using cstring = const ascii*;

  using byte = std::byte;
}

#endif//VALKYRIE_TYPES_HPP
