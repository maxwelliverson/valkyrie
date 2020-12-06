//
// Created by Maxwell on 2020-11-13.
//

#ifndef VALKYRIE_COMMAND_POOL_HPP
#define VALKYRIE_COMMAND_POOL_HPP

#include <valkyrie/Graphics/API/Vulkan.hpp>

namespace valkyrie::Graphics::API{
  class CommandPool{
    class Impl;
    Impl* pImpl;
  public:
    CommandPool(const Device& device);
  };
}

#endif//VALKYRIE_COMMAND_POOL_HPP
