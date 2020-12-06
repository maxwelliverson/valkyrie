//
// Created by Maxwell on 2020-11-11.
//

#ifndef VALKYRIE_RESOURCE_IMAGE_HPP
#define VALKYRIE_RESOURCE_IMAGE_HPP

#include <valkyrie/Graphics/API/Vulkan.hpp>

namespace valkyrie::Graphics::API{
  class Image : public VulkanResource{};
  class ImageView : public VulkanResource{};
}

#endif//VALKYRIE_RESOURCE_IMAGE_HPP
