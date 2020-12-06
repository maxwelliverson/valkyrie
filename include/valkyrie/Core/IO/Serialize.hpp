//
// Created by Maxwell on 2020-11-17.
//

#ifndef VALKYRIE_SERIALIZE_HPP
#define VALKYRIE_SERIALIZE_HPP

#include <valkyrie/Core/IO/Messages.hpp>
#include <valkyrie/Core/Traits.hpp>
#include <valkyrie/Core/Utility/Uuid.hpp>

#include <charconv>

namespace valkyrie{



  template <std::integral I>
  struct Traits::Serializable<I>{

  };
  template <>
  struct Traits::Serializable<Core::Uuid>{

  };
}

#endif//VALKYRIE_SERIALIZE_HPP
