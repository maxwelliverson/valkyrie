//
// Created by maxwe on 2021-04-25.
//

#ifndef VALKYRIE_CONSTANTS_HPP
#define VALKYRIE_CONSTANTS_HPP

#include "preprocessor.hpp"
#include "primitives.hpp"

namespace valkyrie{
  enum class dynamic_t : u32;
  enum class nothing_t : u32;
  VK_constant nothing_t nothing{1};
  VK_constant dynamic_t dynamic{};
}

#endif  //VALKYRIE_CONSTANTS_HPP
