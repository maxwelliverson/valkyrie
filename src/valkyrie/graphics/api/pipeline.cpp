//
// Created by Maxwell on 2020-11-16.
//

#include <valkyrie/graphics/api/device.hpp>
#include <valkyrie/graphics/api/pipeline.hpp>

#include "function_pointers.hpp"

using namespace valkyrie::graphics::api;

class pipeline::Impl{
  device device;
};

namespace valkyrie{
  namespace {
    struct PipelineInfo{
      VkGraphicsPipelineCreateInfo graphicsPipeline;
    };
  }
}
