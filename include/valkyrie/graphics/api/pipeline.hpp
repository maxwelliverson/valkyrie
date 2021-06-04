//
// Created by Maxwell on 2020-11-12.
//

#ifndef VALKYRIE_GRAPHICS_API_PIPELINE_HPP
#define VALKYRIE_GRAPHICS_API_PIPELINE_HPP

#include <valkyrie/graphics/api/vulkan.hpp>


#include <optional>

namespace valkyrie::graphics::api {
  class pipeline : public vulkan_object {};


  /*class VertexInputState{
    struct Info{
      u32 location;
      u32 offset;
      u32 stride;
      //Format
      bool vertexOrInstanceIndex;
    };
    flat_map<u32, Info> bindings;
  };
  class InputAssemblyState{
    struct Info{
      PrimitiveTopology topology;
      bool enableRestart;
    };
    small_vector<Info, 8> state;
  };
  class ViewportState{
    struct Info{
      float x;
      float y;
      float width;
      float height;
      interval<f32> depth;
      struct {
        u32 x;
        u32 y;
        u32 width;
        u32 height;
      } scissors;
    };
    small_vector<Info, 1> viewports;
  };
  class RasterizerState{
    bool enableDepthClamp;
    bool enableDiscard;
    PolygonMode polygonMode;
    CullMode cullMode;
    FrontFace frontFace;
    struct {
      bool enable;
      float constantFactor;
      float clamp;
      float slopeFactor;
    } depthBias;
    float lineWidth;
  };
  class MultisamplingState{
    SampleCount sampleCount; // REQUIRED
    u64 sampleMask; // OPTIONAL
    interval<f32, UpperBound<0xFFFF'FFFF'FFFF'FFFFULL>> sampleShading; //OPTIONAL
    bool enableAlphaToCoverage; // OPTIONAL
    bool enableAlphaToOne;    // OPTIONAL
    bool enableSampleShading; // REQUIRED
  };
  class DepthStencilTestingState{
    struct {
      bool enableTest;
      bool enableWrite;
      bool enableBoundsTest;
      CompareOp compareOp;
      interval<float> bounds;
    } depth;
    struct {
      bool enableTest;
      struct {
        StencilOp failOp;
        StencilOp passOp;
        StencilOp depthFailOp;
        CompareOp compareOp;
        u32       compareMask;
        u32       writeMask;
        u32       reference;
      } front, back;
    } stencil;
  }; // OPTIONAL
  class ColorBlendingState{
    struct Attachment{
      bool enableBlend;
      struct {
        BlendOp blendOp;
        struct {
          BlendFactor src;
          BlendFactor dst;
        } blendFactor;
      } color, alpha;
      bitflag<ColorComponent> componentMask;
    };

    // Provided by VK_EXT_blend_operation_advanced
    BlendOverlap blendOverlap;
    bool srcPremultiplied;
    bool dstPremultiplied;

    // Basic
    bool enableLogicOp;
    LogicOp logicOp;
    struct {
      float r;
      float g;
      float b;
      float a;
    } blendConstant;
    small_vector<Attachment, 2> inputAttachmentStates;
  };
  class DynamicState{
    small_vector<DynamicStateFlags> dynamicStates;
  };

  struct DescriptorSetLayoutBinding{
    u32 binding;

  };

  struct PipelineLayout{

  };

  struct PipelineVertexInfo{
    VertexInputState input;
    InputAssemblyState inputAssembly;
    VertexShader shader;
  };

  struct FixedFunctions{
    VertexInputState vertexInput;
    InputAssemblyState inputAssembly;
    ViewportState viewport;
    RasterizerState rasterizer;
    MultisamplingState multisampling;
    DepthStencilTestingState depthStencilTesting;
    ColorBlendingState colorBlending;
    DynamicState dynamicState;
  };

  class ComputePipeline : public pipeline {};
  class GraphicsPipeline : public pipeline {
    std::optional<PipelineVertexInfo> vertex;
    ViewportState viewport;
    RasterizerState rasterizer;
    MultisamplingState multisampling;
    DepthStencilTestingState depthStencilTesting;
    ColorBlendingState colorBlending;
    DynamicState dynamicState;
  };
  class RayTracingPipeline : public pipeline {};
  class PipelineLibrary : public VulkanType {};



  class DepthStencilPipeline : public GraphicsPipeline{

  };*/
}

#endif//VALKYRIE_GRAPHICS_API_PIPELINE_HPP
