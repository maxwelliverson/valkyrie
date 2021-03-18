//
// Created by maxwe on 2021-03-17.
//

#ifndef VALKYRIE_AGENT_CHANNEL_HPP
#define VALKYRIE_AGENT_CHANNEL_HPP

namespace valkyrie{

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

  class channel_sender{
  public:
    virtual ~channel_sender() = default;
  };
  class channel_receiver{
    virtual ~channel_receiver() = default;
  };

  template <typename Ch>
  class sender;
  template <typename Ch>
  class receiver;

  template <>
  class sender<void>{
  public:
    using channel_type = channel_base;
    using sender_type  = channel_sender;


    ~sender(){
      if ( pChannel )
        pChannel->release_sender(pSender);
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
  private:
    channel_type*  pChannel;
    receiver_type* pReceiver;
  };

  template <typename Ch>
  class sender : public sender<void>{
    using channel_type = Ch;
    using sender_type = std::remove_pointer_t<decltype(std::declval<channel_type*>()->acquire_sender())>;

  public:

  };
  template <typename Ch>
  class receiver : public receiver<void>{

    using channel_type = Ch;
    using receiver_type = std::remove_pointer_t<decltype(std::declval<channel_type*>()->acquire_receiver())>;

  public:

  };

}

#endif//VALKYRIE_AGENT_CHANNEL_HPP
