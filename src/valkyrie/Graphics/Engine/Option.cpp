//
// Created by Maxwell on 2020-11-24.
//

#include <valkyrie/Graphics/Engine/Option.hpp>

#include "../API/FunctionPointers.hpp"

#include <map>
#include <memory_resource>

namespace Engine = valkyrie::Graphics::Engine;

namespace {
  struct InstanceInfo{

  };
  struct DeviceInfo{

  };
}

struct Engine::Configuration::InternalState{
  std::pmr::map<u32, DeviceInfo>
};