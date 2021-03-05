//
// Created by Maxwell on 2020-11-11.
//

#ifndef VALKYRIE_RESOURCE_IMAGE_HPP
#define VALKYRIE_RESOURCE_IMAGE_HPP

#include <valkyrie/graphics/api/vulkan.hpp>

namespace valkyrie::graphics::api{
  class Image : public VulkanResource{};
  class ImageView : public VulkanResource{};
}

#endif//VALKYRIE_RESOURCE_IMAGE_HPP
