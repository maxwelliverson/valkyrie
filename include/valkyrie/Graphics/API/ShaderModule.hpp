//
// Created by Maxwell on 2020-11-12.
//

#ifndef VALKYRIE_SHADER_MODULE_HPP
#define VALKYRIE_SHADER_MODULE_HPP

#include <valkyrie/Graphics/API/Vulkan.hpp>

namespace valkyrie::Graphics::API{

  enum class ShaderStage{
    Vertex = 0x1,
    TessellationControl = 0x2,
    TessellationEvaluation = 0x4,
    Geometry = 0x8,
    Fragment = 0x10,
    Compute = 0x20,
    Task = 0x40,
    Mesh = 0x80,
    RayGen  = 0x100,
    AnyHit  = 0x200,
    ClosestHit = 0x400,
    Miss    = 0x800,
    Intersection = 0x1000,
    Callable = 0x2000
  };


  class Shader{};



  class ShaderModule : public VulkanType{};



  class ShaderCompiler{

  };
}

#endif//VALKYRIE_SHADER_MODULE_HPP
