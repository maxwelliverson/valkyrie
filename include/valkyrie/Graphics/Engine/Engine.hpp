//
// Created by Maxwell on 2020-11-15.
//

#ifndef VALKYRIE_ENGINE_HPP
#define VALKYRIE_ENGINE_HPP

#include <valkyrie/Core/Utility/Version.hpp>
#include <valkyrie/Graphics/Engine/Option.hpp>
#include <valkyrie/Graphics/Engine/Setting.hpp>

namespace valkyrie::Graphics::Engine{
  class Engine{
    void autoInitialize();
    virtual bool customInitialize() { return false; }
  public:
    Engine();
  };
}

#endif//VALKYRIE_ENGINE_HPP
