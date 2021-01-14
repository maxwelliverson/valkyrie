//
// Created by Maxwell on 2020-11-16.
//

#ifndef VALKYRIE_SHADERS_HPP
#define VALKYRIE_SHADERS_HPP

#include <valkyrie/Graphics/API/ShaderModule.hpp>

#include <valkyrie/Core/ADT/ArrayRef.hpp>

namespace valkyrie::Graphics{

  class Shader{

  public:
    virtual Core::Span<const u32> compiledCode() const noexcept = 0;
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
