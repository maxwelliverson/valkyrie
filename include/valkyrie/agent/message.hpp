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
  enum class message_id    : u64;
  enum class message_state : u32 {
    invalid,
    writing,
    enqueued,
    reading,
    vestigial,
  };


  VK_constant agent_id   bad_agent   = static_cast<agent_id>(0);
  VK_constant message_id bad_message = static_cast<message_id>(0);


  namespace impl{
    struct message_info{
      u32           nextOffset = 0;
      message_state state      = message_state::invalid;
    };
  }


  struct alignas(16) message{
    impl::message_info info;
    message_id         messageId;
    agent_id           senderId;
  };
}

#endif //VALKYRIE_AGENT_MESSAGE_HPP
