//
// Created by Maxwell on 2020-11-17.
//

#ifndef VALKYRIE_STRING_BUFFER_HPP
#define VALKYRIE_STRING_BUFFER_HPP

#include <valkyrie/Core/Utility/StringView.hpp>
#include <valkyrie/Core/Utility/Arrays.hpp>
#include <valkyrie/Core/Utility/FlatMap.hpp>
#include <valkyrie/Core/Error/Maybe.hpp>

#if VK_compiler_clang && !VK_compiler_msvc

#endif
#include <coroutine>

namespace valkyrie::Core{

  struct Message;
  enum class MessageType : u32;

  class MessageDomain{
  public:
    using message_type    = Message;
    using identifier_type = MessageType;
  };





  enum class MessageType : u32{
    InlineStringBuffer,
    CoroutineFrame,
    Callback,
    Fence,
    StringView,
    String,
    Virtual,

    Flush,
    Invalidate,
    Waiting,
    QueueEnd,

    SignalBegin = Flush,
    SignalEnd = QueueEnd
  };


  struct Message{
    MessageType mType;
    Message* pNext;
  };

  template <MessageType Type>
  struct SignalMessage : Message{
    constexpr SignalMessage() noexcept : Message{ .mType = Type, .pNext = nullptr }{}
    constexpr SignalMessage(Message* pNext) noexcept : Message{ .mType = Type, .pNext = pNext }{}
  };

  struct CoroutineFrameMessage : Message{
    std::coroutine_handle<void> handle;
  };

  struct InlineStringBufferMessage : Message{

    u32 length;
    utf8 buffer[];



    InlineStringBufferMessage(const utf8* pString, u32 Size, Message* pNext)
        : Message{ .mType = MessageType::InlineStringBuffer, .pNext = pNext }, length(Size){
      std::memcpy(&buffer[0], pString, length);
      buffer[length] = '\0';
    }
  };
  struct StringViewMessage : Message{
    StringView stringView;
    Message nextMessage{};
    StringViewMessage(StringView sv)
        : Message{ .mType = MessageType::StringView, .pNext = &nextMessage },
          stringView(sv),
          nextMessage{ .mType = MessageType::Waiting, .pNext = nullptr}{}
  };
  struct StringMessage : Message{
    u32   stringLength;
    utf8* pString;
    void(* pDeleter)(utf8*);
    Message nextMessage;

    StringMessage(u32 stringLength, utf8* pString, void(* pDeleter)(utf8*))
        : Message{ .mType = MessageType::String, .pNext = &nextMessage },
          stringLength(stringLength),
          pString(pString),
          pDeleter(pDeleter),
          nextMessage{ .mType = MessageType::Waiting, .pNext = nullptr }{}
    ~StringMessage(){
      if (pString && pDeleter) {
        pDeleter(pString);
      }
    }
  };
  struct FenceMessage : Message{

  };
  struct VirtualMessage : Message{

    VirtualMessage(Message* pNext) : Message{ .mType = MessageType::Virtual, .pNext = pNext }{}

    virtual ~VirtualMessage() = default;

    virtual u32 writeMessage(utf8* pBuffer, u32 bufferAvailableSize) const noexcept = 0;
  };

  using WaitingMessage = SignalMessage<MessageType::Waiting>;
  using FlushMessage   = SignalMessage<MessageType::Flush>;
  using InvalidateMessage = SignalMessage<MessageType::Invalidate>;
  using QueueEndMessage = SignalMessage<MessageType::QueueEnd>;


  /*template <typename T>
  class InlineMessage : public VirtualMessage{
  public:
    InlineMessage() : VirtualMessage(){}
  };*/


  /*class WriteRequest{ };
  class ReadRequest{ };
  class WriteAndForget{ };

  template <typename T>
  class ReadFuture{

  };*/

  class MessageQueue;





  class MessageProducer{
    MessageQueue* pQueue;
    u32 producerId;
  public:
    MessageProducer(MessageQueue* pQueue) noexcept;
    MessageProducer(const MessageProducer& other) = delete;
    MessageProducer(MessageProducer&& other) noexcept;

    MessageProducer& operator=(const MessageProducer& other) = delete;
    MessageProducer& operator=(MessageProducer&& other) noexcept;

    ~MessageProducer();

    Maybe<MessageProducer> clone() const noexcept;






    void setName(Core::StringView name) const noexcept;

    friend bool operator==(const MessageProducer& A, const MessageProducer& B) noexcept = default;
  };
  class MessageConsumer{
    MessageQueue* pQueue;
    u32 nextMessageOffset;
    u32 consumerId;
  public:
    MessageConsumer(MessageQueue* pQueue) noexcept;
    MessageConsumer(const MessageConsumer& other) = delete;
    MessageConsumer(MessageConsumer&& other) noexcept;

    MessageConsumer& operator=(const MessageConsumer& other) = delete;
    MessageConsumer& operator=(MessageConsumer&& other) noexcept;

    ~MessageConsumer();

    Maybe<MessageConsumer> clone() const noexcept;





    void setName(Core::StringView name) const noexcept;

    friend bool operator==(const MessageConsumer& A, const MessageConsumer& B) noexcept {
      return std::tie(A.pQueue, A.consumerId) == std::tie(B.pQueue, B.consumerId);
    }
  };

  class MessageChannel{
    MessageQueue* pQueue;
  public:
    MessageChannel();
    MessageChannel(u32 initialCapacity);

    MessageProducer makeProducer() noexcept;
    MessageConsumer makeConsumer() noexcept;
  };

  class Socket{

  };


  /*class StringBufferView{
    u32 bufferSize;
    u32 currentPos;
    utf8* pBuffer;



  public:

    virtual void flush() = 0;

    virtual u32 resize(u32 newSize) noexcept { return bufferSize; }
  };*/


}

#endif//VALKYRIE_STRING_BUFFER_HPP
