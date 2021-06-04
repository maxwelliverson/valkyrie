//
// Created by Maxwell on 2021-01-12.
//

#ifndef VALKYRIE_ENGINE_PIPELINE_FACTORY_HPP
#define VALKYRIE_ENGINE_PIPELINE_FACTORY_HPP


#include <valkyrie/graphics/api/vulkan.hpp>
#include <valkyrie/async/atomic.hpp>
//#include <valkyrie/adt/TempArray.hpp>

namespace valkyrie::graphics::engine{
  
  struct EngineViewportState{
    api::Viewport*           pViewports;                         // length: viewportCount
    Rect2D*                  pScissors;                          // length: viewportCount
    Rect2D*                  pExclusiveScissors;                 // length: viewportCount
    Rect2D*                  pDiscardRectangles;                 // length: viewportCount
    api::ShadingRatePalette* pViewportShadingRatePalettes;       // length: viewportCount
    Position2D<f32>*         pViewportScalingFactors;            // length: viewportCount
    u32                      viewportCount;



    api::ShadingRatePaletteEntry* pShadingRateEntries;                // length: viewportCount * maxShadingRatePaletteEntries
    u32                           maxShadingRatePaletteEntries;
  };

  struct EngineState{
    EngineViewportState viewports;
  };

  struct Viewports{
    EngineViewportState* pViewportState;
    u32                  viewportIndex;
    u32                  viewportCount;
  };



  struct GraphicsPipelineState{

    Viewports                              viewports;
    
    f32                                    lineWidth;
    u32                                    lineStippleFactor;
    u16                                    lineStipplePattern;

    api::CullModeFlags                     cullMode;
    api::FrontFace                         frontFace;
    api::PrimitiveTopology                 primitiveTopology;

    api::SampleCountFlags                  sampleLocationsPerPixel;
    Extent2D<u32>                    sampleLocationGrid;
    std::span<const api::SampleLocation>   sampleLocations;

    api::CoarseSampleOrderType                    coarseSampleOrderType;
    std::span<const api::CoarseSampleOrderCustom> customCoarseSampleOrders;

    f32                                    depthBiasConstantFactor;
    f32                                    depthBiasClamp;
    f32                                    depthBiasSlopeFactor;

    f32                                    blendConstants[4];

    alignas(u32) bool                      depthTestEnabled;
    alignas(u32) bool                      depthWriteEnabled;
    api::CompareOp                         depthCompareOp;
    alignas(u32) bool                      depthBoundsTestEnabled;
    alignas(u32) bool                      stencilTestEnabled;
    StencilOpState                         frontStencil;
    StencilOpState                         backStencil;
    Interval<f32>                    depthBounds;
  };

  template <typename T>
  class PipelineStateValue{
    union{
      const T constant;
      std::atomic_ref<const T> dynamic;
    } value;
    const bool isDynamic;


  public:
    PipelineStateValue()
  };

  template <typename T>
  class DynamicPipelineStateRef{
    std::atomic_ref<T> dynamicValue;
  public:

  };



  class PipelineFactory{
  public:
    virtual
  };
}


#endif//VALKYRIE_ENGINE_PIPELINE_FACTORY_HPP
