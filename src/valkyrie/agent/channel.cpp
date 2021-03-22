//
// Created by maxwe on 2021-03-20.
//

#include <valkyrie/async/semaphore.hpp>
#include <valkyrie/agent/channel.hpp>


using namespace valkyrie;


namespace {

  template <size_t N>
  struct unsigned_integer_impl;
  template <>
  struct unsigned_integer_impl<1>{
    using type = u8;
  };
  template <>
  struct unsigned_integer_impl<2>{
    using type = u16;
  };
  template <>
  struct unsigned_integer_impl<4>{
    using type = u32;
  };
  template <>
  struct unsigned_integer_impl<8>{
    using type = u64;
  };
  template <size_t N>
  struct unsigned_integer_impl : unsigned_integer_impl<8>{};



  template <typename T>
  struct object_pool_entry{

    using index_type = typename unsigned_integer_impl<sizeof(T)>::type;
    using value_type = T;

    inline constexpr static index_type max_index = std::numeric_limits<index_type>::max();

    union{
      index_type index;
      value_type value;
    };
  };

  class private_sender : public channel_sender{
    void * begin_send(u64 msgLength, bool block) noexcept override;
    void end_send(void *addr, u64 msgLength) noexcept override;
  };
  class private_receiver : public channel_receiver{
    message * begin_receive(u64 receiveCount, bool block) noexcept override;
    void end_receive(message *pMsg, u64 receiveCount) noexcept override;
  };
  class broadcast_sender : public channel_sender{
    void * begin_send(u64 msgLength, bool block) noexcept override;
    void end_send(void *addr, u64 msgLength) noexcept override;
  };
  class broadcast_receiver : public channel_receiver{
    message * begin_receive(u64 receiveCount, bool block) noexcept override;
    void end_receive(message *pMsg, u64 receiveCount) noexcept override;
  };
  class collector_sender  : public channel_sender{
    void * begin_send(u64 msgLength, bool block) noexcept override;
    void end_send(void *addr, u64 msgLength) noexcept override;
  };
  class collector_receiver : public channel_receiver{
    message * begin_receive(u64 receiveCount, bool block) noexcept override;
    void end_receive(message *pMsg, u64 receiveCount) noexcept override;
  };
  class random_access_sender : public channel_sender{
    void * begin_send(u64 msgLength, bool block) noexcept override;
    void end_send(void *addr, u64 msgLength) noexcept override;
  };
  class random_access_receiver : public channel_receiver{
    message * begin_receive(u64 receiveCount, bool block) noexcept override;
    void end_receive(message *pMsg, u64 receiveCount) noexcept override;
  };



  class private_channel : public channel_base{
    private_sender   sender_;
    private_receiver receiver_;

  public:

  };
  class broadcast_channel : public channel_base{
    private_sender   sender_;

  public:
  };
  class collector_channel{};
  class random_access_channel{};
}