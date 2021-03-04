//
// Created by Maxwell on 2021-02-08.
//

#ifndef VALKYRIE_CORE_AGENT_MAILBOX_HPP
#define VALKYRIE_CORE_AGENT_MAILBOX_HPP

#include <valkyrie/Core/Traits.hpp>
#include <valkyrie/Core/Async/Atomic.hpp>

namespace valkyrie::Core{
  
  template <
      size_t MaxWriterCount = static_cast<size_t>(-1),
      size_t MaxReaderCount = static_cast<size_t>(-1)>
  class Channel{
  protected:
    inline constexpr static bool SingleWriter = MaxWriterCount == 1;
    inline constexpr static bool SingleReader = MaxReaderCount == 1;
    inline constexpr static bool PointToPoint = SingleWriter && SingleReader;


    explicit Channel() noexcept : pMessageQueue(nullptr), queueSize(0){}
    //explicit Channel(AgentConcurrency concurrency, Status& status) noexcept;
    //explicit Channel(u32 queueSize, AgentConcurrency concurrency, Status& status) noexcept;

  public:

    Channel(const Channel&) = delete;
    Channel(Channel&&) noexcept;
    Channel& operator=(const Channel&) = delete;
    Channel& operator=(Channel&&) noexcept;
    ~Channel();



    inline
    
    
    

  private:

    void* pMessageQueue;
    u32   queueSize;


    Atomic<u32> nextReadOffset = 0;
    Atomic<u32> nextWriteOffset = 0;
    Atomic<u32> syncMarker = 0;
    
  };


  using PrivateChannel    = Channel<1, 1>;
  using BroadcastChannel  = Channel<1>;
  using CollectionChannel = Channel<static_cast<size_t>(-1), 1>;
  using DispatchChannel   = Channel<>;

}

#endif //VALKYRIE_CORE_AGENT_MAILBOX_HPP
