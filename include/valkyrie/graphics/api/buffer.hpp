//
// Created by Maxwell on 2020-11-11.
//

#ifndef VALKYRIE_RESOURCE_BUFFER_HPP
#define VALKYRIE_RESOURCE_BUFFER_HPP

#include <valkyrie/graphics/api/vulkan.hpp>

namespace valkyrie::graphics::api{
  class Buffer : public VulkanResource{};
  class BufferView : public VulkanResource{};
}

#endif//VALKYRIE_RESOURCE_BUFFER_HPP
