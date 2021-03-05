//
// Created by Maxwell on 2020-11-09.
//

#ifndef VALKYRIE_COMMAND_BUFFER_HPP
#define VALKYRIE_COMMAND_BUFFER_HPP

#include <valkyrie/graphics/api/command_pool.hpp>
#include <valkyrie/graphics/api/vulkan.hpp>

namespace valkyrie::graphics::api{
  class command_buffer : public VulkanObject{

  };

  class PrimaryCommandBuffer   : public command_buffer { };
  class SecondaryCommandBuffer : public command_buffer { };
}

#endif//VALKYRIE_COMMAND_BUFFER_HPP
