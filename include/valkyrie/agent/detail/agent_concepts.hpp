//
// Created by maxwe on 2021-04-25.
//

#ifndef VALKYRIE_AGENT_DETAIL_AGENT_CONCEPTS_HPP
#define VALKYRIE_AGENT_DETAIL_AGENT_CONCEPTS_HPP

#include <valkyrie/traits.hpp>

namespace valkyrie{
  inline namespace concepts{
    template <typename T>
    concept mailbox_descriptor = requires{
      { T::message_size } -> extent_type;
    };
  }
}

#endif  //VALKYRIE_AGENT_DETAIL_AGENT_CONCEPTS_HPP
