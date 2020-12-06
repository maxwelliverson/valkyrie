//
// Created by Maxwell on 2020-11-16.
//

#ifndef VALKYRIE_SHADERS_HPP
#define VALKYRIE_SHADERS_HPP

#include <valkyrie/Graphics/API/ShaderModule.hpp>

#include <valkyrie/Core/Utility/String.hpp>
#include <valkyrie/Core/Utility/Arrays.hpp>

namespace valkyrie::Graphics{

  class Shader{
    Core::DynamicArray<u32> compiledCode;
  };
  class VertexShader : public Shader{};
  class TessellationControlShader{};
  class TessellationEvaluationShader{};
  class GeometryShader{};
  class FragmentShader{};
  class ComputeShader{};
  class TaskShader{};
  class MeshShader{};


  class RayGenShader{};
  class AnyHitShader{};
  class ClosestHitShader{};
  class MissShader{};
  class IntersectionShader{};
  class CallableShader{};
}

#endif//VALKYRIE_SHADERS_HPP
