//
// Created by Maxwell on 2020-11-11.
//

#ifndef VALKYRIE_RESOURCE_IMAGE_HPP
#define VALKYRIE_RESOURCE_IMAGE_HPP

#include <valkyrie/graphics/api/vulkan.hpp>

namespace valkyrie::graphics::api{
  class image      : public vulkan_object{};
  class image_view : public vulkan_object{};
}

#endif//VALKYRIE_RESOURCE_IMAGE_HPP
