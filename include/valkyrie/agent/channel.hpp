//
// Created by maxwe on 2021-03-17.
//

#ifndef VALKYRIE_AGENT_CHANNEL_HPP
#define VALKYRIE_AGENT_CHANNEL_HPP


#include <valkyrie/memory/default_allocator.hpp>

#include <type_traits>

namespace valkyrie{

  class message;


  /**
   * VK_constant bool is_variable_length =
   *         true: length of message is obtained by calling \code message_traits<Msg>::length(Args...) \endcode
   *               this allows for variable length data to be allocated inline in the message queue.
   *         false (default): length of message is obtained from the result of \code sizeof(Msg) \endcode
   *
   * template <typename ...Args>
   * inline static u64 length(Args&& ...args) noexcept;
   *         Must be defined by any specializations that define is_variable_length as true. Otherwise defaults
   *         to returning \code sizeof(Msg) \endcode
   *
   *
   * */
  template <typename Msg>
  struct message_traits{
    VK_constant bool is_variable_length = false;

    template <typename ...Args>
    inline static u64 length(Args&& ...) noexcept {
      return sizeof(Msg);
    }

    template <typename ...Args>
    inline static Msg* construct(void* addr, Args&& ...args) noexcept {
      new(addr) Msg(std::forward<Args>(args)...);
    }
  };




  class channel_sender;
  class channel_receiver;

  class channel_base{
  public:
    virtual ~channel_base() = default;

    virtual channel_sender*   acquire_sender()            noexcept = 0;
    virtual channel_receiver* acquire_receiver()          noexcept = 0;
    virtual void              release_sender(channel_sender*)     noexcept = 0;
    virtual void              release_receiver(channel_receiver*) noexcept = 0;
  };


  template <typename Ch>
  class sender;
  template <typename Ch>
  class receiver;

  class channel_sender{

    template <typename Ch>
    friend class sender;

  protected:

    virtual void* begin_send(u64 msgLength, bool block) noexcept = 0;
    virtual void  end_send(void* addr, u64 msgLength) noexcept = 0;


  public:
    virtual ~channel_sender() = default;
  };
  class channel_receiver{

    template <typename Ch>
    friend class receiver;

  public:
    virtual ~channel_receiver() = default;

    virtual message* begin_receive(u64 receiveCount, bool block) noexcept = 0;
    virtual void     end_receive(message* pMsg, u64 receiveCount) noexcept = 0;
  };



  template <>
  class sender<void>{



  public:
    using channel_type = channel_base;
    using sender_type  = channel_sender;
  protected:

    sender(channel_type* channel, sender_type* sender) noexcept : pChannel(channel), pSender(sender){}

  public:


    ~sender(){
      if ( pChannel )
        pChannel->release_sender(pSender);
    }


    template <typename Msg, typename ...Args>
    void send(Args&& ...args) const noexcept{

      const u64 msgSize = [&]{
        if constexpr (message_traits<Msg>::is_variable_length)
          return message_traits<Msg>::length(std::forward<Args>(args)...);
        else
          return sizeof(Msg);
      }();
      void* const address = pSender->begin_send(msgSize, true);
      message_traits<Msg>::construct(address, std::forward<Args>(args)...);
      pSender->end_send(address, msgSize);
    }


  private:
    channel_type* pChannel;
    sender_type*  pSender;
  };
  template <>
  class receiver<void>{
  public:
    using channel_type = channel_base;
    using receiver_type  = channel_receiver;


    ~receiver(){
      if ( pChannel )
        pChannel->release_receiver(pReceiver);
    }

  protected:
    receiver(channel_type* channel, receiver_type* receiver) noexcept : pChannel(channel), pReceiver(receiver){}



  private:
    channel_type*  pChannel;
    receiver_type* pReceiver;
  };

  template <typename Ch>
  class sender : public sender<void>{
    using channel_type = Ch;
    using sender_type = std::remove_pointer_t<decltype(std::declval<channel_type*>()->acquire_sender())>;

  public:

    explicit sender(channel_type* pChannel) noexcept : sender<void>(pChannel, pChannel->acquire_sender()){}

  };

  template <typename Ch>
  sender(Ch*) -> sender<Ch>;

  template <typename Ch>
  class receiver : public receiver<void>{

    using channel_type = Ch;
    using receiver_type = std::remove_pointer_t<decltype(std::declval<channel_type*>()->acquire_receiver())>;

  public:

    explicit receiver(channel_type* pChannel) noexcept : receiver<void>(pChannel, pChannel->acquire_receiver()){}

  };

  template <typename Ch>
  receiver(Ch*) -> receiver<Ch>;

}

#endif//VALKYRIE_AGENT_CHANNEL_HPP
