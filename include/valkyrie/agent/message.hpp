//
// Created by Maxwell on 2021-02-11.
//

#ifndef VALKYRIE_AGENT_MESSAGE_HPP
#define VALKYRIE_AGENT_MESSAGE_HPP

#include <valkyrie/traits.hpp>


namespace valkyrie{

  namespace impl{
    template <bool SingleWriter,
        bool SingleReader,
        bool IsCoherent>
    struct mailbox_ops;
  }


  enum class agent_id     : u32;
  enum class message_type : u64;

  inline constexpr static u32 NoLimit = static_cast<u32>(-1);

  template <
      u32 MaxWriterCount = NoLimit,
      u32 MaxReaderCount = 1,
      bool IsCoherent    = static_cast<bool>(MaxReaderCount - 1)>
  class basic_mailbox;


  class alignas(16) message{
    message_type messageType;
    agent_id     senderId;
    u32          nextMsgOffset;

    template <u32, u32, bool>
    friend class basic_mailbox;
    template <bool, bool, bool>
    friend class impl::mailbox_ops;

  protected:
    message(u32 nextOffset) noexcept : nextMsgOffset(nextOffset){}
  public:


  };
}

#endif //VALKYRIE_AGENT_MESSAGE_HPP
