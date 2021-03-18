//
// Created by Maxwell on 2020-11-19.
//

#ifndef VALKYRIE_STATUS_SEVERITY_HPP
#define VALKYRIE_STATUS_SEVERITY_HPP

#include "../primitives.hpp"

namespace valkyrie{
  enum class severity : i32{
    debug          = 0x1,
    performance    = 0x2,
    success        = 0x4,
    info           = 0x8,
    warning        = 0x10,
    error          = 0x20,
    internal_error = 0x40,
    fatal          = 0x80
  };
}

#endif//VALKYRIE_STATUS_SEVERITY_HPP
