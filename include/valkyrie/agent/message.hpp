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



  VK_constant agent_id   bad_agent   = static_cast<agent_id>(0);
  VK_constant message_id bad_message = static_cast<message_id>(0);


  namespace impl{

    enum class message_state32 : u32 {
      invalid,
      written,
      enqueued,
      reading,
      vestigial
    };
    enum class message_state64 : u64 {
      invalid,
      written,
      enqueued,
      reading,
      vestigial
    };

    struct message_info32{
      u32 nextOffset        = 0;
      message_state32 state = message_state32::invalid;
    };
    struct message_info64{
      u64 nextOffset        = 0;
      message_state64 state = message_state64::invalid;
    };
  }


  struct alignas(16) message{
    union{
      u64                  nextOffset = 0;
      impl::message_info32 info32;
      impl::message_info64 info64;
    };
    message_id         messageId;
    agent_id           senderId;
  };
}

#endif //VALKYRIE_AGENT_MESSAGE_HPP
