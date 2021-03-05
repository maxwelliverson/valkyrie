//
// Created by Maxwell on 2020-11-24.
//

#include <valkyrie/graphics/engine/option.hpp>

#include "../api/function_pointers.hpp"

#include <map>
#include <memory_resource>

namespace engine = valkyrie::graphics::engine;

namespace {
  struct InstanceInfo{

  };
  struct DeviceInfo{

  };
}

struct engine::Configuration::InternalState{
  //std::pmr::map<u32, DeviceInfo>
};