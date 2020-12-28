//
// Created by Maxwell on 2020-11-17.
//

#ifndef VALKYRIE_STRING_BUFFER_HPP
#define VALKYRIE_STRING_BUFFER_HPP

#include <valkyrie/Core/Utility/StringView.hpp>
#include <valkyrie/Core/Utility/Arrays.hpp>
#include <valkyrie/Core/Utility/FlatMap.hpp>
#include <valkyrie/Core/Utility/Enums.hpp>
#include <valkyrie/Core/Error/Maybe.hpp>

#if VK_compiler_clang && !VK_compiler_msvc

#endif
#include <coroutine>

namespace valkyrie::Core{

  class Agent;
  struct Message;
  enum class MessageCmd : u32;


  namespace Detail{
    enum class AgentStatusDomainSubtype{
      None,
      Channel,
      Command,
      Property,
      Message,
      External
    };
    enum class AgentStatusSubtypeNone     : u32{
      Success,          // Operation was successful
      InternalError     // Unknown error, likely a symptom of some library bug
    };
    enum class AgentStatusSubtypeChannel  : u32{
      Blocked,          // Consumption of messages from this channel is currently blocked for some unspecified reason (Eg. an AcquireSemaphore command)
      TooManyProducers, // The number of agents attempting to write to this channel exceeded some maximum (Default: 1)
      NoConsumer,       // No agent is presently reading messages from this channel
      BufferOverflow,   // Mailbox does not currently have enough space to write message and the calling agent must either perform a blocking wait or try again later.
      MessageTooLarge   // Message size exceeds maximum size (By default the max message size is equal to the size of the mailbox memory buffer)
    };
    enum class AgentStatusSubtypeCommand  : u32{
      NotSupported,       // This agent is not able to process the supplied command
      DomainNotSupported, // This agent is not able to process commands from the specified user defined domain.
      InvalidValue        // The supplied command does not correspond to any valid MessageCmd value.
    };
    enum class AgentStatusSubtypeProperty : u32{
      NotFound,           // No property matching the provided identifier was found
      //NotFoundCloseMatch, // No property matching the provided identifier was found, but a potential close match was found (ie. potential typo was detected)
      InvalidFormat       // Type incompatibility error between the stored data type and the requested data type.
    };
    enum class AgentStatusSubtypeMessage  : u32 {
      InvalidContents, // Payload is in some invalid state (likely was provided invalid arguments)
      InvalidSize      // Payload is an invalid size for the given command
    };

    struct AgentStatusValue{
      AgentStatusDomainSubtype subType;
      u32                      code;
    };


    class AgentStatusDomain         : public StatusDomain{};
    class AgentChannelStatusDomain  : public AgentStatusDomain{};
    class AgentCommandStatusDomain  : public AgentStatusDomain{};
    class AgentPropertyStatusDomain : public AgentStatusDomain{};
    class AgentMessageStatusDomain  : public AgentStatusDomain{};
    class AgentExternalStatusDomain : public AgentStatusDomain{};
  }
  enum class AgentStatusCode {
    Success,
    Queued,
    UnknownError,
    ChannelBlocked,
    ChannelTooManyProducers,
    ChannelNoConsumer,
    ChannelBufferOverflow,
    ChannelMessageTooLarge,
    CommandNotSupported,
    CommandDomainNotSupported,
    CommandInvalidValue,
    PropertyNotFound,
    PropertyNotFoundCloseMatch,
    PropertyInvalidFormat,
    MessageInvalidContents,
    MessageInvalidSize,
    FailedToProcessExternal
  };
}

namespace valkyrie::Traits{
  template <>
  struct StatusEnum<Core::AgentStatusCode>{

    using enum_type = Core::AgentStatusCode;

    inline constexpr static Core::StringView name{VK_string("AgentStatusCode")};
    inline constexpr static Core::StringView scoped_name{VK_string("valkyrie::Core::AgentStatusCode")};

    inline constexpr static Core::StringView domain_name{VK_string("Agent Status")};
    inline constexpr static Core::Uuid       domain_uuid{"1204717e-359a-4b77-9e7f-e0ab787e8e55"};

    inline const/*expr*/ static Core::StatusEnumMap<enum_type> info{
        { Core::Severity::Success,       Core::AgentStatusCode::Success,                   VK_string("Success"),                                                          { Core::Code::Success } },
        { Core::Severity::Info,          Core::AgentStatusCode::Queued,                    VK_string("Message is queued but has not yet been read"),                      { Core::Code::InProgress, Core::Code::Success } },
        { Core::Severity::InternalError, Core::AgentStatusCode::UnknownError,              VK_string("Unknown Internal Error occurred in the Valkyrie Agents subsystem"), { } },
        { Core::Severity::Warning,       Core::AgentStatusCode::ChannelBlocked,            VK_string("Channel is currently blocked (Eg. by waiting on a semaphore)"),     { Core::Code::Busy } },
        {                                Core::AgentStatusCode::ChannelTooManyProducers,   VK_string("Channel has too many producers"),                                   { Core::Code::ConnectionLimitReached } },
        {                                Core::AgentStatusCode::ChannelNoConsumer,         VK_string("Channel has no consumer"),                                          { Core::Code::NotConnected } },
        {                                Core::AgentStatusCode::ChannelBufferOverflow,     VK_string("Core::AgentStatusCode::ChannelBufferOverflow"),                     { Core::Code::InsufficientSize, Core::Code::OutOfBoundsAccess } },
        {                                Core::AgentStatusCode::ChannelMessageTooLarge,    VK_string("Core::AgentStatusCode::ChannelMessageTooLarge"),                    { Core::Code::ResourceTooLarge, Core::Code::InvalidArgument } },
        {                                Core::AgentStatusCode::CommandNotSupported,       VK_string("Core::AgentStatusCode::CommandNotSupported"),                       { Core::Code::NotSupported } },
        {                                Core::AgentStatusCode::CommandDomainNotSupported, VK_string("Core::AgentStatusCode::CommandDomainNotSupported"),                 { Core::Code::NotSupported } },
        {                                Core::AgentStatusCode::CommandInvalidValue,       VK_string("Core::AgentStatusCode::CommandInvalidValue"),                       { Core::Code::OutOfDomain, Core::Code::InvalidArgument } },
        {                                Core::AgentStatusCode::PropertyNotFound,          VK_string("Core::AgentStatusCode::PropertyNotFound"),                          { Core::Code::ResourceNotFound } },
        {                                Core::AgentStatusCode::PropertyInvalidFormat,     VK_string("Core::AgentStatusCode::PropertyInvalidFormat"),                     { Core::Code::InvalidFormat } },
        {                                Core::AgentStatusCode::MessageInvalidContents,    VK_string("Core::AgentStatusCode::MessageInvalidContents"),                    { Core::Code::InvalidArgument } },
        {                                Core::AgentStatusCode::MessageInvalidSize,        VK_string("Core::AgentStatusCode::MessageInvalidSize"),                        { Core::Code::InvalidArgument } },
        {                                Core::AgentStatusCode::FailedToProcessExternal,   VK_string("Core::AgentStatusCode::FailedToProcessExternal"),                   { Core::Code::ExternalError } }
    };
  };
}

namespace valkyrie::Core{


  using AgentStatus = enum_traits<AgentStatusCode>::status_type;



  /*enum class MessageDomain    : u32 {
    Cmd,                 // CmdKillConsumer, CmdCloseQueue, CmdInvalidateQueue, CmdInvalidateMessages, CmdBlockFor, CmdBlockUntil, CmdSignalEvent
    Sync,                // BarrierArrive, BarrierWait, BarrierArriveAndWait, BarrierDrop, AcquireSemaphore, ReleaseSemaphore
    Query,               // QueryUUID, QueryName, QueryDescription, QueryProducerCount, QueryMethod, QueryHasProperty, QueryProperty
    IO,                  // Write, Read, ReadUntil, ReadN, ReadNextToken, Flush
    Exec,                // CallMethod, RegisterMethod, ExecCallback, ExecCoroutine, UnregisterMethod, RegisterHook, UnregisterHook
    UserDefined          // Etc.
  };

  enum class CmdMessageKind   : u32 {
    NoOp,
    Kill,
    CloseQueue,
    InvalidateQueue,
    InvalidateMessages
  };
  enum class SyncMessageKind  : u32 {
    BarrierArrive,
    BarrierWait,
    BarrierArriveAndWait,
    BarrierDrop,
    AcquireSemaphore,
    ReleaseSemaphore
  };
  enum class QueryMessageKind : u32 {
    Uuid,
    Name,
    Description,
    ProducerCount,
    Method,
    Property
  };
  enum class IOMessageKind    : u32 {
    Write,
    Read,
    ReadUntil,
    ReadNext,
    Flush
  };
  enum class ExecMessageKind  : u32 {
    CallMethod,
    RegisterMethod,
    UnregisterMethod,
    Callback,
    Coroutine,
    RegisterHook,
    UnregisterHook
  };
  enum class UserMessageKind  : u32;*/

  enum class MessageCmd : u32{
    NoOp,
    Kill,
    CloseQueue,
    InvalidateQueue,
    Skip,
    BarrierArrive,
    BarrierWait,
    BarrierArriveAndWait,
    BarrierDrop,
    AcquireSemaphore,
    ReleaseSemaphore,
    QueryUuid,
    QueryName,
    QueryDescription,
    QueryProducerCount,
    QueryMethod,
    QuerySupport,
    QueryProperty,
    SetProperty,
    Write,
    Read,
    Flush,
    InvokeMethod,
    RegisterMethod,
    UnregisterMethod,
    InvokeCallback,
    InvokeCoroutine,
    RegisterHook,
    UnregisterHook,
    UserDefined
  };


  struct Message{
    Message*      pNext;
    MessageCmd    mCmd;
  };

  /*template <MessageType Type>
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
  using QueueEndMessage = SignalMessage<MessageType::QueueEnd>;*/
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

  class Agent{



    AgentStatus doSend() const noexcept;

  public:

    virtual ~Agent() = default;
  };

  template <typename T>
  class agent_ptr{

  };

  class Thread     : public Agent{};
  class ThreadPool : public Agent{};







  class MessageProcessor;

  /*template <auto BufferSize = dynamicExtent>
  class AgentQuery;

  template <>
  class AgentQuery<>{
    template <typename Derived, QueryMessageKind messageKind>
    friend struct QueryMessage;
    std::u8string resultBuffer;
  public:

  };
  template <size_t N>
  class AgentQuery<N>{
    template <typename Derived, QueryMessageKind messageKind>
    friend struct QueryMessage;
    utf8 resultBuffer[N];
  public:
  };

  struct AgentQueryResult{
    Status status{ GenericStatus(Code::NotReady) };
    utf8*  pBuffer;
    u64    bufferLength;
  };

  template <typename Derived, QueryMessageKind messageKind>
  struct QueryMessage : Message{
    template <size_t N>
    QueryMessage(Message* pPrevious, const AgentQuery<N>& query) noexcept
        : Message{ .mDomain = MessageDomain::Query, .mInstruction = (u32)messageKind, .pNext = (Message*)(((byte*)this) + sizeof(Derived)) }{
      pPrevious->pNext = this;
    }
    QueryMessage(Message* pPrevious, const AgentQuery<>&  query) noexcept
        : Message{ .mDomain = MessageDomain::Query, .mInstruction = (u32)messageKind }{
      pPrevious->pNext = this;
    }
  };*/

  using PFN_processMessage = void(*)(MessageProcessor&, Message&, void*);

  struct MessageHook{
    MessageHook* pNext;
    void* pUserData;
    PFN_processMessage msgHook;
  };

  struct IMessageProcessor{
    virtual Message* msgNoOp(Message* message) noexcept;
    virtual Message* msgKill(Message* message) noexcept;
    virtual Message* msgCloseQueue(Message* message) noexcept;
  };

  struct ProcessorChain{
    ProcessorChain*    pNext                 = nullptr;
    void*              pUserData             = nullptr;


    PFN_processMessage msgNoOp               = nullptr;
    PFN_processMessage msgKill               = nullptr;
    PFN_processMessage msgCloseQueue         = nullptr;
    PFN_processMessage msgInvalidateQueue    = nullptr;
    PFN_processMessage msgSkip               = nullptr;

    PFN_processMessage msgBarrierArrive      = nullptr;
    PFN_processMessage msgBarrierWait        = nullptr;
    PFN_processMessage msgBarrierArriveAndWait = nullptr;
    PFN_processMessage msgBarrierDrop        = nullptr;
    PFN_processMessage msgAcquireSemaphore   = nullptr;
    PFN_processMessage msgReleaseSemaphore   = nullptr;

    PFN_processMessage msgQueryUuid          = nullptr;
    PFN_processMessage msgQueryName          = nullptr;
    PFN_processMessage msgQueryDescription   = nullptr;
    PFN_processMessage msgQueryProducerCount = nullptr;
    PFN_processMessage msgQueryMethod        = nullptr;
    PFN_processMessage msgQueryProperty      = nullptr;
    PFN_processMessage msgQuerySupport       = nullptr;
    PFN_processMessage msgSetProperty        = nullptr;

    PFN_processMessage msgWrite              = nullptr;
    PFN_processMessage msgRead               = nullptr;
    PFN_processMessage msgFlush              = nullptr;

    PFN_processMessage msgInvokeMethod       = nullptr;
    PFN_processMessage msgRegisterMethod     = nullptr;
    PFN_processMessage msgUnregisterMethod   = nullptr;
    PFN_processMessage msgInvokeCallback     = nullptr;
    PFN_processMessage msgInvokeCoroutine    = nullptr;
    PFN_processMessage msgRegisterHook       = nullptr;
    PFN_processMessage msgUnregisterHook     = nullptr;

    PFN_processMessage msgUserDefined        = nullptr;
  };

  using PMFN_messageProcessorChainLink = PFN_processMessage ProcessorChain::*;




  class MessageProcessor{
    ProcessorChain processorChain;
    bool processMessage(Message& msg, const ProcessorChain* pChain, PMFN_messageProcessorChainLink chainLink) noexcept {
      bool result = false;
      while (pChain) {
        if (auto pProcessMessage = pChain->*chainLink) {
          pProcessMessage(*this, msg, pChain->pUserData);
          result = true;
        }
        pChain = pChain->pNext;
      }
      return result;
    }
    inline static PMFN_messageProcessorChainLink dispatchMsgCmd(MessageCmd msgCmd) noexcept {

#define PP_VK_impl_GET_CHAIN_PTR(Cmd) case MessageCmd::Cmd: return &ProcessorChain::msg##Cmd;

      switch (msgCmd) {
        VK_foreach(PP_VK_impl_GET_CHAIN_PTR,
                   NoOp,
                   Kill,
                   CloseQueue,
                   InvalidateQueue,
                   Skip,
                   BarrierArrive,
                   BarrierWait,
                   BarrierArriveAndWait,
                   BarrierDrop,
                   AcquireSemaphore,
                   ReleaseSemaphore,
                   QueryUuid,
                   QueryName,
                   QueryDescription,
                   QueryProducerCount,
                   QueryMethod,
                   QuerySupport,
                   QueryProperty,
                   SetProperty,
                   Write,
                   Read,
                   //ReadUntil,
                   //ReadNext,
                   Flush,
                   InvokeMethod,
                   RegisterMethod,
                   UnregisterMethod,
                   InvokeCallback,
                   InvokeCoroutine,
                   RegisterHook,
                   UnregisterHook,
                   UserDefined)
        default:
          return nullptr;
      }
    }
  public:

    /*virtual void noOp( MessageProcessor&,             Message&) const noexcept;
    virtual void kill( MessageProcessor&,             Message&) const noexcept;
    virtual void closeQueue( MessageProcessor&,       Message&) const noexcept;
    virtual void invalidateQueue( MessageProcessor&,  Message&) const noexcept;
    virtual void skip( MessageProcessor&,             Message&) const noexcept;

    virtual void barrierArrive( MessageProcessor&,    Message&) const noexcept;
    virtual void barrierWait( MessageProcessor&,      Message&) const noexcept;
    virtual void barrierArriveAndWait( MessageProcessor&, Message&) const noexcept;
    virtual void barrierDrop( MessageProcessor&,      Message&) const noexcept;
    virtual void acquireSemaphore( MessageProcessor&, Message&) const noexcept;
    virtual void releaseSemaphore( MessageProcessor&, Message&) const noexcept;

    virtual void write( MessageProcessor&,            Message&) const noexcept;
    virtual void read( MessageProcessor&,             Message&) const noexcept;
    virtual void readUntil( MessageProcessor&,        Message&) const noexcept;
    virtual void readNext( MessageProcessor&,         Message&) const noexcept;
    virtual void flush( MessageProcessor&,            Message&) const noexcept;

    virtual void callMethod( MessageProcessor&,       Message&) const noexcept;
    virtual void registerMethod( MessageProcessor&,   Message&) const noexcept;
    virtual void unregisterMethod( MessageProcessor&, Message&) const noexcept;
    virtual void callback( MessageProcessor&,         Message&) const noexcept;
    virtual void coroutine( MessageProcessor&,        Message&) const noexcept;
    virtual void registerHook( MessageProcessor&,     Message&) const noexcept;
    virtual void unregisterHook( MessageProcessor&,   Message&) const noexcept;*/


    bool dispatchMessage(Message& msg) noexcept {
      if (auto processPtr = dispatchMsgCmd(msg.mCmd)) {
        return processMessage(msg, &processorChain, processPtr);
      }
      return false;
    }
  };

  /*class MessageProcessor{
    CmdMessageProcessor*   pCmdProcessor;
    SyncMessageProcessor*  pSyncProcessor;
    QueryMessageProcessor* pQueryProcessor;
    IOMessageProcessor*    pIOProcessor;
    ExecMessageProcessor*  pExecProcessor;
    UserMessageProcessor*  pUserProcessor;
  public:

  };*/

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
