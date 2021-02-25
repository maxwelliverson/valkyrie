//
// Created by Maxwell on 2021-01-12.
//

#ifndef VALKYRIE_ENGINE_PIPELINE_FACTORY_HPP
#define VALKYRIE_ENGINE_PIPELINE_FACTORY_HPP


#include <valkyrie/Graphics/API/Vulkan.hpp>
#include <valkyrie/Core/Async/Atomic.hpp>
//#include <valkyrie/Core/ADT/TempArray.hpp>

namespace valkyrie::Graphics::Engine{
  
  struct EngineViewportState{
    API::Viewport*                pViewports;                         // length: viewportCount
    Core::Rect2D*                 pScissors;                          // length: viewportCount
    Core::Rect2D*                 pExclusiveScissors;                 // length: viewportCount
    Core::Rect2D*                 pDiscardRectangles;                 // length: viewportCount
    API::ShadingRatePalette*      pViewportShadingRatePalettes;       // length: viewportCount
    Core::Position2D<f32>*        pViewportScalingFactors;            // length: viewportCount
    u32                           viewportCount;



    API::ShadingRatePaletteEntry* pShadingRateEntries;                // length: viewportCount * maxShadingRatePaletteEntries
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

    API::CullModeFlags                     cullMode;
    API::FrontFace                         frontFace;
    API::PrimitiveTopology                 primitiveTopology;

    API::SampleCountFlags                  sampleLocationsPerPixel;
    Core::Extent2D<u32>                    sampleLocationGrid;
    std::span<const API::SampleLocation>   sampleLocations;

    API::CoarseSampleOrderType                    coarseSampleOrderType;
    std::span<const API::CoarseSampleOrderCustom> customCoarseSampleOrders;

    f32                                    depthBiasConstantFactor;
    f32                                    depthBiasClamp;
    f32                                    depthBiasSlopeFactor;

    f32                                    blendConstants[4];

    alignas(u32) bool                      depthTestEnabled;
    alignas(u32) bool                      depthWriteEnabled;
    API::CompareOp                         depthCompareOp;
    alignas(u32) bool                      depthBoundsTestEnabled;
    alignas(u32) bool                      stencilTestEnabled;
    StencilOpState                         frontStencil;
    StencilOpState                         backStencil;
    Core::Interval<f32>                    depthBounds;
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
