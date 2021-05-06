//
// Created by Maxwell on 2021-02-08.
//

#ifndef VALKYRIE_AGENT_MAILBOX_HPP
#define VALKYRIE_AGENT_MAILBOX_HPP


#include "detail/mailbox_impl.hpp"
#include "detail/mailbox_traits.hpp"


#include <valkyrie/adt/tags.hpp>
#include <valkyrie/status/result.hpp>
#include <valkyrie/memory/literals.hpp>


namespace valkyrie{

  inline constexpr static u64 no_limit = static_cast<u64>(-1);


  namespace impl{
    template <typename F>
    concept msg_write_functor = requires(F&& functor, void* address){
      { (std::forward<F>(functor))(address) } -> std::convertible_to<message*>;
    };
    template <typename F>
    concept msg_read_functor = requires(F&& functor, agent_id agent, message* msg){
      (std::forward<F>(functor))(agent, msg);
    };
  }


  template <mailbox_descriptor Desc>
  class basic_mailbox : protected impl::mailbox_impl<Desc>::type {
    using base = typename impl::mailbox_impl<Desc>::type;
    using traits = impl::mailbox_traits<Desc>;
    
    VK_constant u64 max_writers = traits::max_writers;
    VK_constant u64 max_readers = traits::max_readers;

    class writer{
      basic_mailbox* mailbox_;
      agent_id       agent_;
    public:

      writer() noexcept : mailbox_(nullptr), agent_(bad_agent){}
      writer(agent_id agent, basic_mailbox* mailbox) noexcept
          : mailbox_( mailbox->acquire_write_perms() ? mailbox : nullptr ),
            agent_(agent){ }
      writer(const writer&) = delete;
      writer(writer&& other) noexcept
          : mailbox_(std::exchange(other.mailbox_, nullptr)),
            agent_(std::exchange(other.agent_, bad_agent)){ }
      ~writer() {
        if ( mailbox_ )
          mailbox_->release_write_perms();
      }

      template <impl::msg_write_functor F>
      inline void write(u64 size, F&& functor){
        VK_assert( mailbox_ );
        mailbox_->unsafe_write(agent_, size, std::forward<F>(functor));
      }
      template <impl::msg_write_functor F>
      inline bool try_write(u64 size, F&& functor){
        VK_assert( mailbox_ );
        return mailbox_->unsafe_try_write(agent_, size, std::forward<F>(functor));
      }

      explicit operator bool() const noexcept {
        return mailbox_;
      }
    };
    class reader{
      basic_mailbox* mailbox_;
      agent_id       agent_;
    public:

      reader() noexcept : mailbox_(nullptr), agent_(bad_agent){}
      reader(agent_id agent, basic_mailbox* mailbox) noexcept
          : mailbox_( mailbox->acquire_read_perms() ? mailbox : nullptr ),
            agent_(agent){ }
      reader(const reader&) = delete;
      reader(reader&& other) noexcept
          : mailbox_(std::exchange(other.mailbox_, nullptr)),
            agent_(std::exchange(other.agent_, bad_agent)){ }

      ~reader() {
        if ( mailbox_ )
          mailbox_->release_read_perms();
      }

      template <impl::msg_read_functor F>
      inline void read(F&& functor) noexcept {
        VK_assert( mailbox_ );
        mailbox_->unsafe_read(agent_, std::forward<F>(functor));
      }
      template <impl::msg_read_functor F>
      inline bool try_read(F&& functor) noexcept {
        VK_assert( mailbox_ );
        return mailbox_->unsafe_try_read(agent_, std::forward<F>(functor));
      }

      explicit operator bool() const noexcept {
        return mailbox_;
      }
    };
    
    struct pipeline{
      writer in;
      reader out;
    };


    explicit basic_mailbox(u64 size, status& status) noexcept
        : base(max_writers, max_readers, size, status){ }

    template <raw_allocator Alloc> requires (!traits::message_size_is_dynamic)
    basic_mailbox(u64 size, Alloc&& allocator, status& status) noexcept
        : base(max_writers, max_readers, size, std::forward<Alloc>(allocator), status){}
    template <raw_allocator Alloc> requires (!traits::message_size_is_dynamic)
    explicit basic_mailbox(Alloc&& allocator, status& status) noexcept
        : base(max_writers, max_readers, std::forward<Alloc>(allocator), status){}

  public:

    basic_mailbox() : base(max_writers, max_readers){ }
    basic_mailbox(const basic_mailbox&) = delete;

    basic_mailbox(basic_mailbox&& other) noexcept;


    static result<basic_mailbox> open(u64 size) noexcept {
      status status;
      basic_mailbox mailbox(size, status);
      if ( status.failure() )
        return std::move(status);
      return std::move(mailbox);
    }
    
    

    
    writer open_writer(agent_id agent) noexcept {
      return writer(agent, this);
    }
    reader open_reader(agent_id agent) noexcept {
      return reader(agent, this);
    }
    
    pipeline open_pipe(agent_id from, agent_id to) noexcept {
      if ( auto w = open_writer(from) ) {
        if ( auto r = open_reader(to) ) {
          return pipeline{
              .in = std::move(w),
              .out = std::move(r)
          };
        }
      }
    }  


    template <impl::msg_write_functor F>
    void unsafe_write(agent_id agent, u64 size, F&& functor) noexcept {
      u64 nextOffset;
      message* msg = (std::forward<F>(functor))(this->do_begin_write(size, nextOffset));
      msg->senderId = agent;
      this->do_end_write(msg, nextOffset);
    }
    template <impl::msg_write_functor F>
    bool unsafe_try_write(agent_id agent, u64 size, F&& functor) noexcept {
      
      u64 nextOffset;
      if ( void* address = this->do_try_begin_write(size, nextOffset)) VK_likely {
        message* msg = (std::forward<F>(functor))(address);
        msg->senderId = agent;
        this->do_end_write(msg, nextOffset);
        return true;
      }
      
      return false;
    }
    
    template <impl::msg_read_functor F>
    void unsafe_read(agent_id agent, F&& functor) noexcept {
      message* msg = this->do_begin_read();
      (std::forward<F>(functor))(agent, msg);
      this->do_end_read(msg);
    }
    template <impl::msg_read_functor F>
    bool unsafe_try_read(agent_id agent, F&& functor) noexcept {
      if (message* msg = this->do_try_begin_read()) {
        (std::forward<F>(functor))(agent, msg);
        this->do_end_read(msg);
        return true;
      }
      return false;
    }
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
    struct outbox{
      VK_constant bool single_writer    = true;
      VK_constant bool many_readers     = true;
      VK_constant bool is_read_coherent = false;
    };

    template <auto Size>
    struct inbox{
      VK_constant auto message_size = Size;
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
  using outbox           = basic_mailbox<policy::outbox<MsgSize>>;
  template <auto MsgSize>
  using inbox            = basic_mailbox<policy::inbox<MsgSize>>;
  template <auto MsgSize>
  using communal_mailbox = basic_mailbox<policy::communal_mailbox<MsgSize>>;



  /*using my_mailbox    = private_mailbox<dynamic>;
  using other_mailbox = private_mailbox<32>;
  
  
  void hello(message*(* ctor)(void*), void(* proc)(agent_id, message*), u64 size){
    my_mailbox mb_0{};    // a to b
    other_mailbox mb_1{}; // b to c
    agent_id agent_a = (agent_id)1;
    agent_id agent_b = (agent_id)2;
    agent_id agent_c = (agent_id)3;

    auto [in_0, out_0] = mb_0.open_pipe(agent_a, agent_b);
    auto [in_1, out_1] = mb_1.open_pipe(agent_b, agent_c);

    auto in_1_ = std::move(in_1);

    out_1.try_read(proc); // returns false
    out_0.try_read(proc); // returns false
    in_0.write(size, ctor);
    out_0.read([&](agent_id agent, message* msg){
      // ...
      in_1_.write(size, [&](void* addr){
        return static_cast<message*>(std::memcpy(addr, msg, size));
      });
    });
    out_1.read(proc);
  }*/


  enum class log_level{
    debug,
    info,
    warning,
    error
  };
  struct log_message : message, variable_size {
    log_level level;
    u32       message_length;
    utf8      string[];

    VK_constant uuid class_id{"24434bef-f3ea-4bc7-a0ec-96f57753a15d"};
    VK_constant message_id message_id{to_integer(class_id)};


    log_message(agent_id sender_id, log_level level, string_view str) noexcept
        : message(message_id, sender_id),
          level(level),
          message_length(narrow_cast<u32>(str.length())) {
      std::memcpy(string, str.data(), message_length);
    }

    inline static u64 object_size(const log_message& msg) noexcept {
      return align_to<alignof(message)>(offsetof(log_message, string) + msg.message_length);
    }
  };


  void hello() {

    using namespace memory_literals;

    using dynamic_inbox = inbox<dynamic>;
    using chunk_inbox   = inbox<128>;


    dynamic_inbox thisInbox{dynamic_inbox::open(64_KiB).value()};
    chunk_inbox   chunkInbox{chunk_inbox::open(32).value()};

    agent_id a{1}, b{2}, c{3}, d{4};


    auto&& [ sender, reader ] = thisInbox.open_pipe(a, b);

    //sender.write();
  }
}

#endif //VALKYRIE_AGENT_MAILBOX_HPP
