//
// Created by Maxwell on 2021-02-11.
//

#ifndef VALKYRIE_CORE_AGENT_AGENT_HPP
#define VALKYRIE_CORE_AGENT_AGENT_HPP

#include <valkyrie/Core/Traits.hpp>
#include <valkyrie/Core/Utility/Uuid.hpp>
#include <valkyrie/Core/Utility/Casting.hpp>



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


  class Agent{

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
  };

  template <std::derived_from<Agent> A>
  class agent_ptr<A>{
  public:




  private:
    A* pAgent;
  };




}

#endif //VALKYRIE_CORE_AGENT_AGENT_HPP
