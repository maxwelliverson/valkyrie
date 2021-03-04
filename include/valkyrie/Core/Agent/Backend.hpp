//
// Created by maxwe on 2021-03-03.
//

#ifndef VALKYRIE_CORE_AGENT_BACKEND_HPP
#define VALKYRIE_CORE_AGENT_BACKEND_HPP

#include <valkyrie/Core/Traits.hpp>

namespace valkyrie::Core{
  class Backend{};

  class AutonomousTask : public Backend{};

  template <std::derived_from<AutonomousTask> T>
  class Task{};

  template <typename T>
  class Autonomous{




  };

}

#endif //VALKYRIE_CORE_AGENT_BACKEND_HPP
