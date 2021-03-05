//
// Created by Maxwell on 2021-01-12.
//

#ifndef VALKYRIE_GRAPHICS_ENGINE_SHADER_INTERFACE_HPP
#define VALKYRIE_GRAPHICS_ENGINE_SHADER_INTERFACE_HPP

#include <valkyrie/adt/array_ref.hpp>
#include <valkyrie/adt/small_array.hpp>



namespace valkyrie::graphics::engine{

  class ShaderInterface;
  class ShaderInterfaceNode;
  class ShaderInterfaceNodeIn;
  class ShaderInterfaceNodeOut;



  class ShaderInterfaceNode{ };
  class ShaderInterfaceNodeIn : public ShaderInterfaceNode{ };
  class ShaderInterfaceNodeOut : public ShaderInterfaceNode{ };
  class ShaderInterface{

  };
}

#endif //VALKYRIE_GRAPHICS_ENGINE_SHADER_INTERFACE_HPP
