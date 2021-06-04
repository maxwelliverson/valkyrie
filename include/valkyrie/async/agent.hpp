//
// Created by Maxwell on 2020-12-04.
//

#ifndef VALKYRIE_ASYNC_AGENT_HPP
#define VALKYRIE_ASYNC_AGENT_HPP

#include "atomic.hpp"
#include <valkyrie/adt/function_ref.hpp>
#include <valkyrie/status/result.hpp>

#include <concepts>
#include <coroutine>
#include <tuple>
#include <utility>


/*namespace valkyrie{

  //class Agent;
  struct Message;
  enum class MessageCmd : u32;


  namespace detail{
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


    class AgentStatusDomain         : public status_domain{};
    class AgentChannelStatusDomain  : public AgentStatusDomain{};
    class AgentCommandStatusDomain  : public AgentStatusDomain{};
    class AgentPropertyStatusDomain : public AgentStatusDomain{};
    class AgentMessageStatusDomain  : public AgentStatusDomain{};
    class AgentExternalStatusDomain : public AgentStatusDomain{};
  }
  enum class AgentCode {
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
  struct StatusEnum<AgentCode>{

    using enum_type = AgentCode;

    inline constexpr static string_view name{VK_string("AgentCode")};
    inline constexpr static string_view scoped_name{VK_string("valkyrie::AgentCode")};

    inline constexpr static string_view domain_name{VK_string("Agent Status")};
    inline constexpr static uuid       domain_uuid{"1204717e-359a-4b77-9e7f-e0ab787e8e55"};

    inline const*//*expr*//* static StatusEnumMap<enum_type> info{
        { severity::Success,       AgentCode::Success,                   VK_string("Success"),                                                          { code::success } },
        { severity::Info,          AgentCode::Queued,                    VK_string("Message is queued but has not yet been read"),                      { code::in_progress, code::success } },
        { severity::InternalError, AgentCode::UnknownError,              VK_string("Unknown Internal Error occurred in the Valkyrie Agents subsystem"), { } },
        { severity::Warning,       AgentCode::ChannelBlocked,            VK_string("Channel is currently blocked (Eg. by waiting on a semaphore)"),     { code::busy } },
        {                                AgentCode::ChannelTooManyProducers,   VK_string("Channel has too many producers"),                                   { code::connection_limit_reached } },
        {                                AgentCode::ChannelNoConsumer,         VK_string("Channel has no consumer"),                                          { code::not_connected } },
        {                                AgentCode::ChannelBufferOverflow,     VK_string("AgentCode::ChannelBufferOverflow"),                     { code::insufficient_size, code::out_of_bounds_access } },
        {                                AgentCode::ChannelMessageTooLarge,    VK_string("AgentCode::ChannelMessageTooLarge"),                    { code::resource_too_large, code::invalid_argument } },
        {                                AgentCode::CommandNotSupported,       VK_string("AgentCode::CommandNotSupported"),                       { code::not_supported } },
        {                                AgentCode::CommandDomainNotSupported, VK_string("AgentCode::CommandDomainNotSupported"),                 { code::not_supported } },
        {                                AgentCode::CommandInvalidValue,       VK_string("AgentCode::CommandInvalidValue"),                       { code::out_of_domain, code::invalid_argument } },
        {                                AgentCode::PropertyNotFound,          VK_string("AgentCode::PropertyNotFound"),                          { code::resource_not_found } },
        {                                AgentCode::PropertyInvalidFormat,     VK_string("AgentCode::PropertyInvalidFormat"),                     { code::invalid_format } },
        {                                AgentCode::MessageInvalidContents,    VK_string("AgentCode::MessageInvalidContents"),                    { code::invalid_argument } },
        {                                AgentCode::MessageInvalidSize,        VK_string("AgentCode::MessageInvalidSize"),                        { code::invalid_argument } },
        {                                AgentCode::FailedToProcessExternal,   VK_string("AgentCode::FailedToProcessExternal"),                   { code::external_error } }
    };
  };
}*/

namespace valkyrie{


  struct message{};

  template <typename T>
  concept message_type = std::derived_from<T, message>;



  template <typename ReaderType>
  class message_reader{
  public:
    using reader_type = ReaderType;
  };
  template <typename WriterType>
  class message_writer{
  public:
    using writer_type = WriterType;
  };
  template <typename ReaderType, typename WriterType>
  class agent{
  public:
    using reader_type = typename ReaderType::reader_type;
    using writer_type = typename WriterType::writer_type;
  };




  struct message{};

  template <typename Derived>
  class agent_reader{};
  template <typename Derived>
  class agent_sender{};

  class mailbox{
    void* pMailbox;
    u32   mailboxSize;
  };

  template <typename Derived>
  class agent{


  };





  enum class MessageCmd : u32;
  enum class MessageState : u32{
    Dead,
    Alive,
    Condemned
  };


  enum class AgentCode : u32{
    Success,
    NotReady,
    Blocked,
    MailboxIsEmpty,
    MailboxIsFull,
    MailboxCorrupted,
    BadMessage,
    UnsupportedMessage
  };

  class AgentStatusDomain : public status_domain{
  public:
    using value_type = u64;

    constexpr AgentStatusDomain(const uuid& id) noexcept : status_domain(id){}
  };

  class AgentStatusCodeDomain : public AgentStatusDomain{
  public:
    using value_type = AgentCode;

    constexpr AgentStatusCodeDomain() noexcept : AgentStatusDomain(uuid("3e0b5eaf-aa7c-43ed-86dd-6fbbb0ba117d")){}

    string_ref name() const noexcept override {
      return "Valkyrie Agent code";
    }
    bool do_failure(const status_code<void>& status) const noexcept override { }
    bool do_equivalent(const status_code<void>& statusA, const status_code<void>& statusB) const noexcept override { }
    code do_generic_code(const status_code<void>& status) const noexcept override { }
    string_ref do_message(const status_code<void>& status) const noexcept override { }

    inline static const AgentStatusCodeDomain& get() noexcept {
      constexpr static AgentStatusCodeDomain domainInstance{};
      return domainInstance;
    }
  };
  class AgentExceptionPayload{
    friend class AgentExceptionDomain;
    atomic<u32> refCount;

    virtual void doDestroy() noexcept = 0;
    void acquireRef() noexcept {
      ++refCount;
    }
    void releaseRef() noexcept {
      if (!--refCount) {
        this->doDestroy();
      }
    }
  protected:
    ~AgentExceptionPayload() = default;
  public:
    virtual string_ref message()   const noexcept = 0;
    virtual code      toGeneric() const noexcept = 0;
    virtual severity severity() const noexcept = 0;
    virtual bool      equivalentTo(const AgentExceptionPayload& other) const noexcept = 0;
  };
  class AgentExceptionDomain : public AgentStatusDomain{
  public:
    using value_type = AgentExceptionPayload*;

    bool do_failure(const status_code<void>& status) const noexcept { }
    bool do_equivalent(const status_code<void>& statusA, const status_code<void>& statusB) const noexcept { }
    code do_generic_code(const status_code<void>& status) const noexcept { }
    string_ref do_message(const status_code<void>& status) const noexcept { }

    virtual severity do_severity(const status_code<void>& status) const noexcept { return get_default_severity(this->do_generic_code(status)); }
    virtual void do_erased_copy(status_code<void>& To, const status_code<void>& From, size_t Bytes) const noexcept { std::memcpy(&To, &From, Bytes); }
    virtual void do_erased_destroy(status_code<void>& code, size_t Bytes) const noexcept { }

    VK_noreturn virtual void do_throw_exception(const status_code<void>& code) const VK_throws;


  };







  using AgentStatus = status_code<AgentStatusDomain>;
  using AgentStatusCode = status_code<AgentStatusCodeDomain>;
  using AgentException = status_code<AgentExceptionDomain>;

  AgentStatusCode make_status_code(AgentCode code) noexcept;
  AgentException  make_status_code(AgentExceptionPayload* pPayload) noexcept;

  template <typename T>
  using AgentResult = result<T, AgentStatusDomain>;

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

  class AgentMailbox;

  template <typename Msg>
  struct MessageTraits{};

  class AgentMessage;

  template <>
  struct MessageTraits<AgentMessage>{
    inline static u32  nextOffset(const AgentMessage& msg) noexcept;
    inline static void condemn(AgentMessage& msg) noexcept;
    inline static u32  exile(AgentMessage& msg, u32) noexcept;
    inline static void destroy(AgentMessage& msg) noexcept;
  };

  class AgentMessage{

    friend class AgentMailbox;
    class CondemnedMessage;

    atomic<u32>          nextOffset = 0;
    atomic<MessageState> state      = MessageState::Alive;

    inline void condemn() noexcept;
    inline u32  exile(u32) noexcept;

  public:
    virtual ~AgentMessage() = default;
  };



  class AgentMessage::CondemnedMessage final : public AgentMessage{
  public:
    CondemnedMessage(u32 nextOffset) noexcept{
      this->nextOffset = nextOffset;
      this->state      = MessageState::Condemned;
    }
  };


  inline void AgentMessage::condemn() noexcept {
    const u32 cacheNextOffset = this->nextOffset;
    VK_assert(this->state.load() == MessageState::Alive);
    this->~AgentMessage();
    new(this) CondemnedMessage(cacheNextOffset);
  }
  inline u32  AgentMessage::exile(const u32 writeOffset) noexcept {
    MessageState condemnedState = MessageState::Condemned;
    return state.compare_exchange_strong(condemnedState, MessageState::Dead, std::memory_order_acquire) ? nextOffset.load(std::memory_order_acquire) : writeOffset;
  }

  namespace detail{
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

  enum class AgentConcurrency : u32{
    MultiProducer = 0x1,
    MultiConsumer = 0x2,
    OutOfOrderConsumption = 0x4 | MultiConsumer
  };

  class AgentMailbox{


    friend class Agent;

    using PFN_writeMessage = AgentResult<AgentMessage*>(AgentMailbox::*)(function_ref<AgentMessage*(void*)> fnCtor, u64 msgSize) noexcept;
    using PFN_readMessage  = AgentStatus(AgentMailbox::*)(function_ref<AgentStatus(AgentMessage&)> msgProc) noexcept;

    AgentResult<AgentMessage*> writeMessageSPSC(function_ref<AgentMessage*(void*)> fnCtor, u64 msgSize) noexcept;
    AgentResult<AgentMessage*> writeMessageMPSC(function_ref<AgentMessage*(void*)> fnCtor, u64 msgSize) noexcept;
    AgentResult<AgentMessage*> writeMessageSPMC(function_ref<AgentMessage*(void*)> fnCtor, u64 msgSize) noexcept;
    AgentResult<AgentMessage*> writeMessageSPMCNonCoherent(function_ref<AgentMessage*(void*)> fnCtor, u64 msgSize) noexcept;
    AgentResult<AgentMessage*> writeMessageMPMC(function_ref<AgentMessage*(void*)> fnCtor, u64 msgSize) noexcept;

    AgentStatus                readMessageSC (function_ref<AgentStatus(AgentMessage&)> msgProc) noexcept;
    AgentStatus                readMessageMC (function_ref<AgentStatus(AgentMessage&)> msgProc) noexcept;
    AgentStatus                readMessageNonCoherent (function_ref<AgentStatus(AgentMessage&)> msgProc) noexcept;

    AgentStatus                tryReadMessageSC(function_ref<AgentStatus(AgentMessage&)> msgProc) noexcept;
    AgentStatus                tryReadMessageMC(function_ref<AgentStatus(AgentMessage&)> msgProc) noexcept;
    AgentStatus                tryReadMessageNonCoherent(function_ref<AgentStatus(AgentMessage&)> msgProc) noexcept;

  public:
    struct Interface{
      PFN_writeMessage pfnWriteMessage;
      PFN_readMessage  pfnReadMessage;
      PFN_readMessage  pfnTryReadMessage;
    };

  private:
    static Interface dispatchInterface(AgentConcurrency concurrency) noexcept;

    /*inline constexpr static u32              UniqueLock = u32(-1);

    inline constexpr static AgentConcurrency ConcurrencySPSC{ 0 };
    inline constexpr static AgentConcurrency ConcurrencyMPSC{ AgentConcurrency::MultiProducer };
    inline constexpr static AgentConcurrency ConcurrencySPMC{ AgentConcurrency::MultiConsumer };
    inline constexpr static AgentConcurrency ConcurrencyMPMC{ (u32)AgentConcurrency::MultiProducer | (u32)AgentConcurrency::MultiConsumer };
    inline constexpr static AgentConcurrency ConcurrencySPMCNonCoherent{ AgentConcurrency::OutOfOrderConsumption };
    inline constexpr static AgentConcurrency ConcurrencyMPMCNonCoherent{ (u32)AgentConcurrency::MultiProducer | (u32)AgentConcurrency::OutOfOrderConsumption };


    AgentResult<AgentMessage*> writeMessageSPSC(function_ref<AgentMessage*(void*)> fnCtor, u64 msgSize) noexcept;
    AgentResult<AgentMessage*> writeMessageMPSC(function_ref<AgentMessage*(void*)> fnCtor, u64 msgSize) noexcept;
    AgentResult<AgentMessage*> writeMessageSPMC(function_ref<AgentMessage*(void*)> fnCtor, u64 msgSize) noexcept;
    AgentResult<AgentMessage*> writeMessageSPMCNonCoherent(function_ref<AgentMessage*(void*)> fnCtor, u64 msgSize) noexcept;
    AgentResult<AgentMessage*> writeMessageMPMC(function_ref<AgentMessage*(void*)> fnCtor, u64 msgSize) noexcept;

    AgentStatus                readMessageSC (function_ref<AgentStatus(AgentMessage&)> msgProc) noexcept;
    AgentStatus                readMessageMC (function_ref<AgentStatus(AgentMessage&)> msgProc) noexcept;
    AgentStatus                readMessageNonCoherent (function_ref<AgentStatus(AgentMessage&)> msgProc) noexcept;


    inline static Interface dispatchInterface(AgentConcurrency concurrency) noexcept {
      switch (concurrency) {
        case ConcurrencySPSC:
          return { &AgentMailbox::writeMessageSPSC, &AgentMailbox::readMessageSC };
        case ConcurrencyMPSC:
          return { &AgentMailbox::writeMessageMPSC, &AgentMailbox::readMessageSC };
        case ConcurrencySPMC:
          return { &AgentMailbox::writeMessageSPMC, &AgentMailbox::readMessageMC };
        case ConcurrencyMPMC:
          return { &AgentMailbox::writeMessageMPMC, &AgentMailbox::readMessageMC };
        case ConcurrencySPMCNonCoherent:
          return { &AgentMailbox::writeMessageSPMCNonCoherent, &AgentMailbox::readMessageNonCoherent };
        case ConcurrencyMPMCNonCoherent:
          return { &AgentMailbox::writeMessageMPMC, &AgentMailbox::readMessageNonCoherent };
        VK_no_default;
      }
    }*/

  protected:
    void* pMessageQueue;
    u32   queueSize;


    atomic<u32> nextReadOffset = 0;
    atomic<u32> nextWriteOffset = 0;
    atomic<u32> syncMarker = 0;

    Interface dynamicInterface;

    inline AgentMessage* toMessage(const u32 offset) const noexcept {
      return reinterpret_cast<AgentMessage*>(static_cast<std::byte*>(pMessageQueue) + offset);
    }
    inline bool          hasAvailableSpace(const u32 writeOffset, const u32 nextWrite) const noexcept {
      const u32 readOffset = nextReadOffset.load();
      return nextWrite < (readOffset + (queueSize * bool(readOffset < writeOffset)));
    }

    explicit AgentMailbox() noexcept : pMessageQueue(nullptr), queueSize(0), dynamicInterface(){}
    explicit AgentMailbox(AgentConcurrency concurrency, status& status) noexcept;
    explicit AgentMailbox(u32 queueSize, AgentConcurrency concurrency, status& status) noexcept;

  public:

    AgentMailbox(const AgentMailbox&) = delete;
    AgentMailbox(AgentMailbox&&) noexcept;
    AgentMailbox& operator=(const AgentMailbox&) = delete;
    AgentMailbox& operator=(AgentMailbox&&) noexcept;
    ~AgentMailbox();

    inline static AgentMailbox        deferCreation() noexcept {
      return AgentMailbox{};
    }
    inline static result<AgentMailbox> create(AgentConcurrency concurrency) noexcept {
      status status;
      AgentMailbox mailbox{concurrency, status};
      if (status.failure())
        return std::move(status);
      return std::move(mailbox);
    }
    inline static result<AgentMailbox> create(u32 queueSize, AgentConcurrency concurrency) noexcept {
      status status;
      AgentMailbox mailbox{queueSize, concurrency, status};
      if (status.failure())
        return std::move(status);
      return std::move(mailbox);
    }

    inline bool empty() const noexcept {
      return nextWriteOffset.load() == nextReadOffset.load();
    }

    inline AgentResult<AgentMessage*> writeMessage(function_ref<AgentMessage*(void*)> fnCtor, u64 msgSize) noexcept {
      return (this->*dynamicInterface.pfnWriteMessage)(fnCtor, msgSize);
    }
    inline AgentStatus                readMessage(function_ref<AgentStatus(AgentMessage&)> msgProc)       noexcept {
      return (this->*dynamicInterface.pfnReadMessage)(msgProc);
    }
    inline AgentStatus                tryReadMessage(function_ref<AgentStatus(AgentMessage&)> msgProc)    noexcept {
      return (this->*dynamicInterface.pfnTryReadMessage)(msgProc);
    }
  };
  class AgentBackend{

    AgentBackend* pIndirectBackend = nullptr;
    u32  mailboxCount = 0;
    bool isRunning = false;
    bool isIndirect = false;

    virtual status doRegisterMailbox(AgentMailbox& mailbox) noexcept = 0;
    virtual status doUnregisterMailbox(const AgentMailbox& mailbox) noexcept = 0;
    virtual void   doUnregisterAll() noexcept = 0;
    virtual void   doDelete() noexcept = 0;
    virtual void   doRun() noexcept = 0;



  protected:

  public:
    status registerMailbox(AgentMailbox& mailbox) noexcept {
      ++mailboxCount;
      if (pIndirectBackend)
        return pIndirectBackend->registerMailbox(mailbox);
      else
        return doRegisterMailbox(mailbox);
    }
    status unregisterMailbox(const AgentMailbox& mailbox) noexcept {
      VK_assert(mailboxCount > 0);
      --mailboxCount;
      if (pIndirectBackend)
        return pIndirectBackend->unregisterMailbox(mailbox);
      else
        return doUnregisterMailbox(mailbox);
    }
    void   run() noexcept {
      VK_assert(!isRunning);
      isRunning = true;
      if (pIndirectBackend)
        pIndirectBackend->run();
      else
        doRun();
    }

    AgentBackend* getAddress() noexcept {
      if (pIndirectBackend)
        return pIndirectBackend;
      return this;
    }

    virtual ~AgentBackend() {
      if (pIndirectBackend) {

      }
    }

  };

  class Agent{

    enum class Flags{
      MultiProducer   = 0x1,
      MultiConsumer   = 0x2,
      NonBlocking     = 0x4,
      RelaxedOrdering = 0x8,
      Shared          = 0x10,
      MaxValuePlusOne
    };

    friend constexpr Flags operator|(Flags a, Flags b) noexcept {
      return static_cast<Flags>(static_cast<u32>(a) | static_cast<u32>(b));
    }
    friend constexpr Flags operator&(Flags a, Flags b) noexcept {
      return static_cast<Flags>(static_cast<u32>(a) & static_cast<u32>(b));
    }
    friend constexpr Flags operator^(Flags a, Flags b) noexcept {
      return static_cast<Flags>(static_cast<u32>(a) ^ static_cast<u32>(b));
    }

    inline constexpr static Flags flagMask =
        Flags{
            static_cast<u32>(Flags::MaxValuePlusOne) |
            (static_cast<u32>(Flags::MaxValuePlusOne) - 2)
        };



    friend constexpr Flags operator~(Flags a) noexcept {
      return flagMask & static_cast<Flags>(~static_cast<u32>(a));
    }

  protected:



    AgentMailbox  mailbox;
    AgentBackend* pBackend;

    explicit Agent(AgentConcurrency concurrency, AgentBackend* pBackend, status& status) noexcept
        : mailbox(concurrency, status),
          pBackend(pBackend->getAddress()) {
      if (status.success())
        status = this->pBackend->registerMailbox(mailbox);
    }
    explicit Agent(u32 queueSize, AgentConcurrency concurrency, AgentBackend* pBackend, status& status) noexcept
        : mailbox(queueSize, concurrency, status),
          pBackend(pBackend->getAddress()) {
      if (status.success())
        status = this->pBackend->registerMailbox(mailbox);
    }

  public:

    inline constexpr static Flags MultiProducer   = Flags::MultiProducer;
    inline constexpr static Flags MultiConsumer   = Flags::MultiConsumer;
    inline constexpr static Flags NonBlocking     = Flags::NonBlocking;
    inline constexpr static Flags RelaxedOrdering = Flags::RelaxedOrdering;
    inline constexpr static Flags Shared          = Flags::Shared;

    virtual ~Agent() {
      pBackend->unregisterMailbox(mailbox);
    }
  };


  template <typename A = Agent>
  class agent_ptr{
  public:
    using pointer = A*;
    using reference = A&;


    reference operator*() const noexcept {
      return *pObject;
    }
    pointer   operator->() const noexcept {
      return pObject;
    }



  private:
    pointer pObject;
  };


  class ExecutionQueue : public AgentBackend{

  };

  struct AgentPolicy{
    using status_type = AgentStatus;
    using backend_type = AgentBackend;
    inline constexpr static AgentConcurrency concurrency{};

    //inline static
  };

  /*class AbstractExecutor : public Agent<AbstractExecutor>{};

  class Thread     : public Agent<Thread, AbstractExecutor>{};
  class ThreadPool : public Agent<ThreadPool, AbstractExecutor>{
    using Base_ = Agent<ThreadPool, AbstractExecutor>;
  public:

    using typename Base_::status_type;
    using typename Base_::message_type;


    template <typename Ret, typename ...Args>
    status_type schedule(Ret(*pFunction)(Args...), std::type_identity_t<Args>... args) noexcept;
  };


  namespace detail{}


  class MessageProcessor;

  using PFN_processMessage = void(*)(MessageProcessor&, Message&, void*);


  namespace detail{
    struct MessageHookKey;
  }

  using MessageHookHandle = detail::MessageHookKey*;*/

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

  template <std::derived_from<AgentStatusDomain> Dom> requires(!std::same_as<Dom, AgentStatusDomain>)
  inline AgentStatus make_status_code(const status_code<Dom>& derivedCode) noexcept {
    return AgentStatus(std::in_place, erasure_cast<u64>(derivedCode.value()));
  }
}



#endif//VALKYRIE_ASYNC_AGENT_HPP
