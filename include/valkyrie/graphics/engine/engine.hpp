//
// Created by Maxwell on 2020-11-15.
//

#ifndef VALKYRIE_GRAPHICS_ENGINE_ENGINE_HPP
#define VALKYRIE_GRAPHICS_ENGINE_ENGINE_HPP

#include <valkyrie/graphics/engine/option.hpp>
#include <valkyrie/utility/version.hpp>

namespace valkyrie::graphics::engine{
  class engine {
    void autoInitialize();
    virtual bool customInitialize() { return false; }
  public:
    engine();
  };
}

#endif//VALKYRIE_GRAPHICS_ENGINE_ENGINE_HPP
