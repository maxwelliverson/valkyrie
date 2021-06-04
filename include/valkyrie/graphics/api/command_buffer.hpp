//
// Created by Maxwell on 2020-11-09.
//

#ifndef VALKYRIE_COMMAND_BUFFER_HPP
#define VALKYRIE_COMMAND_BUFFER_HPP

#include <valkyrie/graphics/api/command_pool.hpp>

namespace valkyrie::graphics::api{
  class command_buffer : public vulkan_object {

  };

  class primary_command_buffer   : public command_buffer { };
  class secondary_command_buffer : public command_buffer { };
}

#endif//VALKYRIE_COMMAND_BUFFER_HPP
