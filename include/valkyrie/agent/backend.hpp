//
// Created by maxwe on 2021-03-03.
//

#ifndef VALKYRIE_AGENT_BACKEND_HPP
#define VALKYRIE_AGENT_BACKEND_HPP

#include <valkyrie/traits.hpp>

namespace valkyrie{
  class backend{};

  class thread_pool : public backend{};

  class autonomous_task : public backend{};

  template <std::derived_from<autonomous_task> T>
  class task{};

  template <typename T>
  class autonomous{




  };

}

#endif //VALKYRIE_AGENT_BACKEND_HPP
