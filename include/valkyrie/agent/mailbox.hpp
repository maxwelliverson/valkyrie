//
// Created by Maxwell on 2021-02-08.
//

#ifndef VALKYRIE_AGENT_MAILBOX_HPP
#define VALKYRIE_AGENT_MAILBOX_HPP


#include <valkyrie/traits.hpp>
#include <valkyrie/async/atomic.hpp>
#include <valkyrie/status/result.hpp>

namespace valkyrie{

  inline constexpr static u32 NoLimit = static_cast<u32>(-1);


  template <
      u32 MaxWriterCount = NoLimit,
      u32 MaxReaderCount = NoLimit>
  class mailbox;

  template <
      u32 MaxWriterCount = NoLimit,
      u32 MaxReaderCount = NoLimit>
  class basic_mailbox;

  namespace impl{
    class mailbox_base{
    protected:

      mailbox_base() noexcept : pMsgQueue(nullptr), queueLength(0){}
      mailbox_base(u32 length, system_status& status) noexcept;
      mailbox_base(const mailbox_base&) = delete;
      mailbox_base(mailbox_base&& ) noexcept;


      ~mailbox_base();


      void* pMsgQueue;
      u32   queueLength;
    };
  }
  
  template <u32 MaxWriterCount, u32 MaxReaderCount>
  class mailbox{
  protected:
    inline constexpr static bool SingleWriter = MaxWriterCount == 1;
    inline constexpr static bool SingleReader = MaxReaderCount == 1;
    inline constexpr static bool PointToPoint = SingleWriter && SingleReader;


    explicit mailbox() noexcept : pMessageQueue(nullptr), queueSize(0){}
    //explicit mailbox(AgentConcurrency concurrency, Status& status) noexcept;
    //explicit mailbox(u32 queueSize, AgentConcurrency concurrency, Status& status) noexcept;

  public:

    mailbox(const mailbox&) = delete;
    mailbox(mailbox&&) noexcept;
    mailbox& operator=(const mailbox&) = delete;
    mailbox& operator=(mailbox&&) noexcept;
    ~mailbox();


  private:

    void*       pMessageQueue;
    u32         queueSize;


    atomic<u32> nextReadOffset = 0;
    atomic<u32> nextWriteOffset = 0;
    atomic<u32> syncMarker = 0;
  };


  using private_mailbox    = mailbox<1, 1>;
  using broadcast_mailbox  = mailbox<1>;
  using collection_mailbox = mailbox<static_cast<size_t>(-1), 1>;
  using dispatch_mailbox   = mailbox<>;

}

#endif //VALKYRIE_AGENT_MAILBOX_HPP
