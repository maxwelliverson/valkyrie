//
// Created by Maxwell on 2020-11-16.
//

#ifndef VALKYRIE_GRAPHICS_SHADER_HPP
#define VALKYRIE_GRAPHICS_SHADER_HPP

#include <valkyrie/graphics/api/shader_module.hpp>

#include <valkyrie/adt/array_ref.hpp>

namespace valkyrie::graphics{

  class Shader{

  public:
    virtual span<const u32> compiledCode() const noexcept = 0;
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

#endif//VALKYRIE_GRAPHICS_SHADER_HPP
