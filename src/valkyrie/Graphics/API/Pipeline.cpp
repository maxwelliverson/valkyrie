//
// Created by Maxwell on 2020-11-16.
//

#include <valkyrie/Graphics/API/Device.hpp>
#include <valkyrie/Graphics/API/Pipeline.hpp>

#include "FunctionPointers.hpp"

using namespace valkyrie::Graphics::API;

class Pipeline::Impl{
  Device device;
};

namespace valkyrie{
  namespace {
    struct PipelineInfo{
      VkGraphicsPipelineCreateInfo graphicsPipeline;
    };
  }
}
