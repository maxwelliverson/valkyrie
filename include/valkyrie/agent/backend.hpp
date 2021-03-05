//
// Created by maxwe on 2021-03-03.
//

#ifndef VALKYRIE_AGENT_BACKEND_HPP
#define VALKYRIE_AGENT_BACKEND_HPP

#include <valkyrie/traits.hpp>

namespace valkyrie{
  class Backend{};

  class AutonomousTask : public Backend{};

  template <std::derived_from<AutonomousTask> T>
  class Task{};

  template <typename T>
  class Autonomous{




  };

}

#endif //VALKYRIE_AGENT_BACKEND_HPP
