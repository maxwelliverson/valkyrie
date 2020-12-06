//
// Created by Maxwell on 2020-11-09.
//

#ifndef VALKYRIE_COMMAND_BUFFER_HPP
#define VALKYRIE_COMMAND_BUFFER_HPP

#include <valkyrie/Graphics/API/Vulkan.hpp>
#include <valkyrie/Graphics/API/CommandPool.hpp>

namespace valkyrie::Graphics::API{
  class CommandBuffer : public VulkanObject{};

  class PrimaryCommandBuffer : public CommandBuffer{};
  class SecondaryCommandBuffer : public CommandBuffer{};
}

#endif//VALKYRIE_COMMAND_BUFFER_HPP
