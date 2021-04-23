//
// Created by Maxwell on 2021-02-11.
//

#ifndef VALKYRIE_AGENT_MESSAGE_HPP
#define VALKYRIE_AGENT_MESSAGE_HPP

#include <valkyrie/traits.hpp>


namespace valkyrie{


  enum class agent_id : u32;


  class alignas(16) message{
    agent_id senderId;
    u32      nextMsgOffset;

  protected:
    message(u32 nextOffset) noexcept : nextMsgOffset(nextOffset){}
  public:

  };
}

#endif //VALKYRIE_AGENT_MESSAGE_HPP
