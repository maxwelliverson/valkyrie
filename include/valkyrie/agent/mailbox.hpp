//
// Created by Maxwell on 2021-02-08.
//

#ifndef VALKYRIE_AGENT_MAILBOX_HPP
#define VALKYRIE_AGENT_MAILBOX_HPP


#include "detail/mailbox_impl.hpp"
#include "detail/mailbox_traits.hpp"


namespace valkyrie{

  inline constexpr static u64 no_limit = static_cast<u64>(-1);


  namespace impl{

    class mailbox_base{
    protected:

      mailbox_base() noexcept : msgQueue(nullptr), queueLength(0){}
      mailbox_base(u64 length, system_status& status) noexcept;
      mailbox_base(const mailbox_base&) = delete;
      mailbox_base(mailbox_base&& ) noexcept;


      ~mailbox_base();


      VK_nodiscard inline void*    to_address(const u64 offset) const noexcept {
        return msgQueue + offset;
      }
      VK_nodiscard inline message* to_message(const u64 offset) const noexcept {
        return static_cast<message*>(to_address(offset));
      }
      VK_nodiscard inline u64      to_offset(const void* addr) const noexcept {
        return static_cast<const byte*>(addr) - msgQueue;
      }


      byte* msgQueue;
      u64   queueLength;
    };




  }


  template <mailbox_descriptor Desc>
  class basic_mailbox{
    using traits = impl::mailbox_traits<Desc>;
  public:

    using descriptor = Desc;




  };


  namespace policy{

    template <auto Size>
    struct private_mailbox{
      VK_constant auto message_size = Size;
      VK_define_tag(single_writer);
    };

    struct dynamic_private_mailbox{
      VK_constant auto message_size = dynamic;
      VK_define_tag(single_reader); // Not strictly necessary, single reader is the default
      VK_define_tag(single_writer);
    };
    template <u64 Size>
    struct static_private_mailbox{
      VK_constant u64 message_size = Size;
      VK_define_tag(single_reader); // Not strictly necessary, single reader is the default
      VK_define_tag(single_writer);
    };

    template <auto Size>
    struct bottle_mailbox{
      VK_constant bool single_writer    = true;
      VK_constant bool many_readers     = true;
      VK_constant bool is_read_coherent = false;
    };

    template <auto Size>
    struct default_mailbox{
      VK_constant auto message_size = Size;
      VK_define_tag(single_writer);
    };

    template <auto Size>
    struct communal_mailbox{
      VK_define_tag(many_readers);
      VK_define_tag(many_writers);
      VK_constant bool is_read_coherent  = false;
      VK_constant bool is_write_coherent = false;
    };
  }



  template <auto MsgSize>
  using private_mailbox  = basic_mailbox<policy::private_mailbox<MsgSize>>;
  template <auto MsgSize>
  using bottle_mailbox   = basic_mailbox<policy::bottle_mailbox<MsgSize>>;
  template <auto MsgSize>
  using mailbox          = basic_mailbox<policy::default_mailbox<MsgSize>>;
  template <auto MsgSize>
  using communal_mailbox = basic_mailbox<policy::communal_mailbox<MsgSize>>;
}

#endif //VALKYRIE_AGENT_MAILBOX_HPP
