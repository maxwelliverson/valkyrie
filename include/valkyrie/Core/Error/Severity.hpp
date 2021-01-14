//
// Created by Maxwell on 2020-11-19.
//

#ifndef VALKYRIE_SEVERITY_HPP
#define VALKYRIE_SEVERITY_HPP

#include <valkyrie/Core/Types.hpp>

namespace valkyrie::Core{
  enum class Severity : i32{
    Debug         = 0x1,
    Performance   = 0x2,
    Success       = 0x4,
    Info          = 0x8,
    Warning       = 0x10,
    Error         = 0x20,
    InternalError = 0x40,
    Fatal         = 0x80
  };
}

#endif//VALKYRIE_SEVERITY_HPP
