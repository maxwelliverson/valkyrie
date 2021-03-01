//
// Created by Maxwell on 2021-02-11.
//

#ifndef VALKYRIE_CORE_AGENT_AGENT_HPP
#define VALKYRIE_CORE_AGENT_AGENT_HPP

#include <valkyrie/Core/Traits.hpp>
#include <valkyrie/Core/Utility/Uuid.hpp>
#include <valkyrie/Core/Utility/Casting.hpp>
#include <valkyrie/Core/Async/Semaphore.hpp>



namespace valkyrie::Core{


  template <typename A>
  class agent_ptr;
  
  class AgentSender;
  class AgentReceiver;
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
  };


  class AgentMessageProcessor{

  };

  class AgentEntity{
    /*enum class Scope{ Private, Local, Shared };
    Scope scope;*/

    u64 typeId_;

  public:

  };


  //class Agent;

  /*namespace Agent{

    template <typename Msg>
    class message_traits *//* {
      inline static u32 length(const Msg& message) noexcept;


    }*//*;

    enum class Scope{
      Private,
      Local,
      Shared
    };

    class Entity{

    };

    class Registry{};
    class TypeRegistryEntry{
      i64   typeName;
      void* libraryHandle;
    };
    class TypeRegistry : public Registry{


    };
    class SharedRegistry : public Registry{};
    class LocalRegistry  : public Registry{};

    class SharedEntity{
      u64 agentId_;

    };
    class LocalEntity{};
    class PrivateEntity{};

    class Channel{

    };
    class SenderBase{};
    class ReceiverBase{};



    class SharedChannel : public Channel{};
    class LocalChannel  : public Channel{};
    class PrivateChannel : public Channel{};
  }*/

  class Agent{
  public:
    virtual AgentSender*   createSender() noexcept = 0;
    virtual AgentReceiver* createReceiver() noexcept = 0;
    virtual bool           destroySender(AgentSender* sender) noexcept = 0;
    virtual bool           destroyReceiver(AgentReceiver* pReceiver) noexcept = 0;
  };

  class AgentProxy{
  public:

  };



  class AgentInstance{
  public:
    virtual AgentSender*   createSender() noexcept = 0;
    virtual AgentReceiver* createReceiver() noexcept = 0;
    virtual bool           destroySender(AgentSender* sender) noexcept = 0;
    virtual bool           destroyReceiver(AgentReceiver* pReceiver) noexcept = 0;
  };

  template <typename MsgDomain>
  class Instance : public AgentInstance{};

  template <typename MsgDomain>
  class SharedInstance  : public Instance<MsgDomain>{};
  template <typename MsgDomain>
  class LocalInstance   : public Instance<MsgDomain>{};
  template <typename MsgDomain>
  class PrivateInstance : public Instance<MsgDomain>{};


  class InstructorAgent{

    BinarySemaphore availSenders_;
    BinarySemaphore availReceivers_;

  public:
    // One -> One
  };
  class BroadcasterAgent{

    BinarySemaphore availSenders_;
    Semaphore       availReceivers_;

  public:
    // One -> Many
  };
  class CollectorAgent{

    Semaphore availSenders_;
    BinarySemaphore availReceivers_;

  public:
    // Many -> One
  };
  class DispatcherAgent{

    Semaphore availSenders_;
    Semaphore availReceivers_;

  public:
    // Many -> Many
  };



  class UnsafeAgent{
    u64                  typeId_;
    u32                  propertyBits_;
    std::atomic_int32_t  refCount_;
    Semaphore            availSenders_;
    Semaphore            availReceivers_;

    ~UnsafeAgent();

  public:

    explicit UnsafeAgent(u64 id_, u32 maxSenders, u32 maxReceivers) noexcept;


    bool makeSender() noexcept;
    bool makeReceiver() noexcept;
    void destroySender() noexcept;
    void destroyReceiver() noexcept;

  };

  class UnsafeSender;
  class UnsafeReceiver;

  class UnsafeAgentProxy{

    friend class UnsafeSender;
    friend class UnsafeReceiver;

    UnsafeAgent* pAgent;

    inline bool sendMessage(bool(*pfnSendMessage)(UnsafeAgent* pAgent, UnsafeSender* pSender, void* pUserData) noexcept, UnsafeSender* pSender, void* pUserData) noexcept {
      return pfnSendMessage(this->pAgent, pSender, pUserData);
    }
    inline bool receiveMessage(bool(*pfnReceiveMessage)(UnsafeAgent* pAgent, UnsafeReceiver* pReceiver, void* pUserData) noexcept, UnsafeReceiver* pReceiver, void* pUserData) noexcept {
      return pfnReceiveMessage(pAgent, pReceiver, pUserData);
    }
    inline bool sendMessage(bool(*pfnSendMessage)(UnsafeAgent* pAgent, UnsafeSender* pSender) noexcept, UnsafeSender* pSender) noexcept {
      return pfnSendMessage(pAgent, pSender);
    }
    inline bool receiveMessage(bool(*pfnReceiveMessage)(UnsafeAgent* pAgent, UnsafeReceiver* pReceiver) noexcept, UnsafeReceiver* pReceiver) noexcept {
      return pfnReceiveMessage(pAgent, pReceiver);
    }

  public:

    virtual ~UnsafeAgentProxy();

    virtual UnsafeSender*   createSender() noexcept = 0;
    virtual UnsafeReceiver* createReceiver() noexcept = 0;
    virtual bool            destroySender(UnsafeSender* sender) noexcept = 0;
    virtual bool            destroyReceiver(UnsafeReceiver* pReceiver) noexcept = 0;


  };



  class UnsafeSender{
    UnsafeAgent* pAgent;
  public:
    virtual ~UnsafeSender();
  };
  class UnsafeReceiver{

  public:
    virtual ~UnsafeReceiver();
  };



  template <typename MsgDomain>
  auto make_sender(Instance<MsgDomain>& instance) noexcept {
    auto* senderInstance = instance.createSender();

  }







  /*class Agent{

    friend class AgentSender;
    friend class AgentReceiver;
    
    enum class Scope{
      Private,
      Local,
      Shared
    };

    Uuid typeID_;
    u64  agentID_;

  public:

    virtual ~Agent() = default;
    
    virtual void acquireHandle() const noexcept = 0;
    virtual void releaseHandle() const noexcept = 0;
    
    virtual AgentSender*   createSender() noexcept = 0;
    virtual AgentReceiver* createReceiver() noexcept = 0;
    virtual bool           destroySender(AgentSender* sender) noexcept = 0;
    virtual bool           destroyReceiver(AgentReceiver* pReceiver) noexcept = 0;


    u64 hash() const noexcept {
      return agentID_;
    }
  };*/

  /*template <std::derived_from<Agent> A>
  class agent_ptr<A>{
  public:




  private:
    A* pAgent;
  };*/




}

#endif //VALKYRIE_CORE_AGENT_AGENT_HPP
