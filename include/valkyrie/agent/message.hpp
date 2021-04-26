//
// Created by Maxwell on 2021-02-11.
//

#ifndef VALKYRIE_AGENT_MESSAGE_HPP
#define VALKYRIE_AGENT_MESSAGE_HPP

#include <valkyrie/traits.hpp>
#include <valkyrie/async/atomic.hpp>

#include "detail/agent_concepts.hpp"

namespace valkyrie{

  namespace impl{
    template <bool SingleWriter,
        bool SingleReader,
        bool IsCoherent>
    struct mailbox_ops;
  }


  enum class agent_id      : u64;
  enum class message_type  : u64;
  enum class message_state : u32 {
    invalid,
    writing,
    enqueued,
    reading,
    vestigial,
  };


  namespace impl{
    struct message_info{
      u32           nextOffset = 0;
      message_state state      = message_state::invalid;
    };
  }




  template <mailbox_descriptor Desc>
  class basic_mailbox;


  class alignas(16) message{

  public:
    impl::message_info info;
    message_type       messageType;
    agent_id           senderId;


    template <mailbox_descriptor>
    friend class basic_mailbox;
    template <bool, bool, bool>
    friend class impl::mailbox_ops;

  protected:
    message(message_type msgType, agent_id agentId) noexcept
        : messageType(msgType), senderId(agentId){}

  public:


  };
}

#endif //VALKYRIE_AGENT_MESSAGE_HPP
