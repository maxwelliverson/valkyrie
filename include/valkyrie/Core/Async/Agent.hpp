//
// Created by Maxwell on 2020-12-04.
//

#ifndef VALKYRIE_ASYNC_AGENT_HPP
#define VALKYRIE_ASYNC_AGENT_HPP


#include <valkyrie/Core/Utility/StringView.hpp>
#include <valkyrie/Core/Utility/Arrays.hpp>
#include <valkyrie/Core/Utility/FlatMap.hpp>
#include <valkyrie/Core/Utility/Enums.hpp>
#include <valkyrie/Core/Error/Maybe.hpp>

#include <valkyrie/Core/Async/Atomic.hpp>

#include <coroutine>
#include <concepts>
#include <utility>
#include <tuple>



namespace valkyrie::Core{

  //class Agent;
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

  using AgentStatus = typename enum_traits<AgentStatusCode>::status_type;

  namespace Detail{
    class MessageDomainBase{

    };
  }




  enum class MessageState : u32{
    Invalid,
    Written,
    Locked,
    Read,
    Completed
  };

  enum class MessageAction : u32{
    Advance  = 0x1,
    Preserve = 0x2,
    Jump     = 0x4
  };

  /*enum class MessageCmd : u32{
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
    TriggerEvent,

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
  };*/


  struct Message{
    Atomic<MessageState> state;
    Atomic<u32>          nextOffset;
  };


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

  namespace Detail{
    template <typename T>
    inline constexpr static auto emplaceMsgDefaultCtor = [](void* pMsgAddress) noexcept {
      new(pMsgAddress) T{};
    };
    template <typename T, typename ...Args>
    inline constexpr static auto emplaceMsgFunc = [](void* pMsgAddress, void* pArgs) noexcept {
      new(pMsgAddress) T(std::make_from_tuple<T>(std::move(*static_cast<std::tuple<Args&&...>*>(pArgs))));
    };

    struct AnyType{
      template <typename T>
      operator T() const noexcept;
    };

    template <typename T>
    struct HasProtectedMembers : T{
      auto writeMessageDefault() const -> decltype(this->writeMessage(emplaceMsgDefaultCtor<AnyType>));
      auto writeMessageNonDefault() const -> decltype(this->writeMessage(emplaceMsgFunc<std::tuple<int, int, int>, int, int, char>));
    };
  }

  template <typename T>
  concept Polymorphic = requires(T* pValue){
    { dynamic_cast<const volatile void*>(pValue) };
  };
  template <typename T>
  concept Abstract    = Polymorphic<T> && requires{
    { sizeof(T) };
  } && !requires(Detail::AnyType a){
    { T(a) };
  };

  template <typename T>
  concept AgentLike = /*Polymorphic<T> && */requires(T& v, const T& cv, const Detail::HasProtectedMembers<T>& pv){
      typename T::message_type;
      typename T::status_type;
    { pv.writeMessageDefault() } -> SameAs<typename T::status_type>;
    { pv.writeMessageNonDefault() } -> SameAs<typename T::status_type>;
  };

  class AgentBase{
  public:
    using message_type = Message;
    using status_type = typename enum_traits<AgentStatusCode>::status_type;
  protected:

    status_type writeMessage(void(*pConstructor)(void*) noexcept) noexcept;
    status_type writeMessage(void(*pConstructor)(void*, void*) noexcept, void* pArgs) noexcept;
    status_type readMessage() noexcept;

  public:

    template <typename T, typename ...Args>
    inline AgentStatus write(Args&& ...args) noexcept {
      if constexpr (sizeof...(Args) > 0) {
        std::tuple<Args&&...> argTuple{ std::forward<Args>(args)... };
        return this->writeMessage(Detail::template emplaceMsgFunc<T, Args...>, &argTuple);
      }
      else {
        return this->writeMessage(Detail::template emplaceMsgDefaultCtor<T>);
      }
    }

    virtual ~AgentBase() = default;
  };

  template <typename Derived, std::derived_from<AgentBase> Base = AgentBase>
  class Agent : public Base{
  public:
    using typename Base::message_type;
    using typename Base::status_type;
    using base_type = Base;
    using process_fn_type = status_type(Derived::*)(message_type& msg) noexcept;

    using Base::Base;

  private:

    status_type doReadMessage(message_type& msg) noexcept {

    }
    status_type doWriteMessage(message_type& msg) noexcept {

    }
  };


  /*template <typename T>
  class agent_ptr{
    static_assert(std::derived_from<T, Agent>,
        "For all agent_ptr<T>, T must be a subtype of valkyrie::Core::Agent");
  };*/

  class AbstractExecutor : public Agent<AbstractExecutor>{};

  class Thread     : public Agent<Thread, AbstractExecutor>{};
  class ThreadPool : public Agent<ThreadPool, AbstractExecutor>{
    using Base_ = Agent<ThreadPool, AbstractExecutor>;
  public:

    using typename Base_::status_type;
    using typename Base_::message_type;


    template <typename Ret, typename ...Args>
    status_type schedule(Ret(*pFunction)(Args...), std::type_identity_t<Args>... args) noexcept;
  };


  namespace Detail{}


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


  namespace Detail{
    struct MessageHookKey;
  }

  using MessageHookHandle = Detail::MessageHookKey*;

  /*struct IMessageProcessor{
    virtual AgentStatus doKill(Message& message) noexcept;
    virtual AgentStatus doCloseQueue(Message& message) noexcept;
    virtual AgentStatus doInvalidateQueue(Message& msg) noexcept;
    virtual AgentStatus doSkip(Message& msg) noexcept;

    virtual AgentStatus doBarrierArrive(Message& msg) noexcept;
    virtual AgentStatus doBarrierWait(Message& msg) noexcept;
    virtual AgentStatus doBarrierArriveAndWait(Message& msg) noexcept;
    virtual AgentStatus doBarrierDrop(Message& msg) noexcept;
    virtual AgentStatus doAcquireSemaphore(Message& msg) noexcept;
    virtual AgentStatus doReleaseSemaphore(Message& msg) noexcept;

    virtual AgentStatus doQueryUuid(Message& msg) noexcept;
    virtual AgentStatus doQueryName(Message& msg) noexcept;
    virtual AgentStatus doQueryDescription(Message& msg) noexcept;
    virtual AgentStatus doQueryProducerCount(Message& msg) noexcept;
    virtual AgentStatus doQueryMethod(Message& msg) noexcept;
    virtual AgentStatus doQueryProperty(Message& msg) noexcept;
    virtual AgentStatus doQuerySupport(Message& msg) noexcept;
    virtual AgentStatus doSetProperty(Message& msg) noexcept;

    virtual AgentStatus doWrite(Message& msg) noexcept;
    virtual AgentStatus doRead(Message& msg) noexcept;
    virtual AgentStatus doFlush(Message& msg) noexcept;

    virtual AgentStatus doInvokeMethod(Message& msg) noexcept;
    virtual AgentStatus doRegisterMethod(Message& msg) noexcept;
    virtual AgentStatus doUnregisterMethod(Message& msg) noexcept;
    virtual AgentStatus doInvokeCallback(Message& msg) noexcept;
    virtual AgentStatus doInvokeCoroutine(Message& msg) noexcept;
    virtual AgentStatus doRegisterHook(Message& msg) noexcept;
    virtual AgentStatus doUnregisterHook(Message& msg) noexcept;

    virtual AgentStatus doUserDefined(Message& msg) noexcept;
  };

  struct IAgentIOSupport : virtual IMessageProcessor{
    AgentStatus doRead(Message &msg) noexcept override  = 0;
    AgentStatus doWrite(Message &msg) noexcept override = 0;
    AgentStatus doFlush(Message &msg) noexcept override = 0;
  };
  struct IAgentBarrierSupport : virtual IMessageProcessor{
    AgentStatus doBarrierArrive(Message &msg) noexcept override = 0;
    AgentStatus doBarrierDrop(Message &msg) noexcept override = 0;
    AgentStatus doBarrierArriveAndWait(Message &msg) noexcept override = 0;
    AgentStatus doBarrierWait(Message &msg) noexcept override = 0;
  };*/

  /*struct ProcessorChain{
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
  };*/

  /*using MessageProcessKey = AgentStatus(IMessageProcessor::*)(Message&) noexcept;*/

  /*using PMFN_messageProcessorChainLink = PFN_processMessage ProcessorChain::*;


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

    virtual void noOp( MessageProcessor&,             Message&) const noexcept;
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
    virtual void unregisterHook( MessageProcessor&,   Message&) const noexcept;


    bool dispatchMessage(Message& msg) noexcept {
      if (auto processPtr = dispatchMsgCmd(msg.mCmd)) {
        return processMessage(msg, &processorChain, processPtr);
      }
      return false;
    }
  };*/
}

#endif//VALKYRIE_ASYNC_AGENT_HPP
