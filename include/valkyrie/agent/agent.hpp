//
// Created by Maxwell on 2021-02-11.
//

#ifndef VALKYRIE_AGENT_AGENT_HPP
#define VALKYRIE_AGENT_AGENT_HPP

#include <valkyrie/async/semaphore.hpp>
#include <valkyrie/utility/uuid.hpp>
#include <valkyrie/utility/casting.hpp>
#include <valkyrie/traits.hpp>


namespace valkyrie{





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

  /*class Agent{
  public:
    virtual AgentSender*   createSender() noexcept = 0;
    virtual AgentReceiver* createReceiver() noexcept = 0;
    virtual bool           destroySender(AgentSender* sender) noexcept = 0;
    virtual bool           destroyReceiver(AgentReceiver* pReceiver) noexcept = 0;
  };*/

  template <typename T>
  class Proxy{};


  class ChannelReader;
  class ChannelWriter;

  class Channel{
  public:
    virtual ~Channel() = default;
  };

  class Agent{
  public:
    virtual ~Agent() = default;

    virtual ChannelReader* acquireReader() noexcept = 0;
    virtual ChannelWriter* acquireWriter() noexcept = 0;
    virtual void releaseReader(ChannelReader* pReader) noexcept = 0;
    virtual void releaseWriter(ChannelWriter* pWriter) noexcept = 0;
  };

  class ChannelBroker{ };

  class ChannelReader{
  public:
    virtual ~ChannelReader() = default;
  };
  class ChannelWriter{
  public:
    virtual ~ChannelWriter() = default;
  };

  template <typename T>
  class Reader{
    using proxy_type  = Proxy<T>;
    using reader_type = typename agent_traits<proxy_type>::reader_type;
  public:

    ~Reader() {

    }

  private:
    reader_type* pReader;
  };
  template <typename T>
  class Writer{
    using proxy_type  = Proxy<T>;
    using writer_type = typename agent_traits<proxy_type>::writer_type;


  public:

  private:
    writer_type* pWriter;
  };

  class AutonomousEntityRoot{
  public:
    virtual ~AutonomousEntityRoot() = default;
  };

  template <typename Derived, typename Base = AutonomousEntityRoot>
  class AutonomousEntity{
  public:
  };




  /*
   *
   * class RandomGenerator{
   * public:
   *   virtual ~RandomGenerator() = default;
   *
   *   virtual void generateBatch(Channel* channel, u64 genCount) noexcept = 0;
   * };
   *
   * class CurandRandomGenerator{};
   * class OpenCLRandomGenerator{};
   * class CPURandomGenerator{};
   *
   *
   * class RandomEngineState{
   *   Channel                          channel;
   *   struct State {
   *     std::atomic_uint64_t           countConsumed;
   *     u32                            typeSize;
   *   } state;
   *
   *   std::atomic_uint32_t             refCount;
   * };
   *
   *
   *
   *
   * class RandomEngine{
   *   RandomEngineState* pState;
   *
   *   explicit RandomEngine(RandomEngineState*);
   *
   *   public:
   *   RandomEngine() = default;
   *
   *   static RandomEngine create(u64 batchSize, u64 totalBufferSize) noexcept{
   *     std::thread         adminThread;
   *     binary_semaphore    isReady;
   *     RandomEngineState* pState;
   *
   *     isReady.acquire();
   *
   *     adminThread = std::thread{[batchSize, totalBufferSize](binary_semaphore* isReady, RandomEngineState** ppState){
   *
   *       using clock = std::chrono::high_precision_clock;
   *       using self  = std::this_thread;
   *
   *       RandomEngineState                state;
   *       CudaRandomGenerator              generator;
   *       std::chrono::nanoseconds         sleepTime   = 50us;
   *       std::chrono::nanoseconds         lastSleepTime = sleepTime;
   *       uint64_t                         countConsumedDuringPreviousSleep = batchSize;
   *       std::unique_ptr<RandomGenerator> generator;
   *
   *       int64_t countConsumed = batchSize;
   *
   *       *ppState = &state;
   *       state.refCount += 1;
   *
   *       generator.generateBatch(&state.channel, batchSize);
   *
   *       while ((self::sleep_for(state.sleepTime), true) && state.refCount.load() != 0) {
   *         auto countConsumed = state.countConsumed.exchange(0);
   *         generator.generateBatch(&state.channel, batchSize);
   *       }
   *     }, &isReady, &pState};
   *
   *     threadIsReady.acquire();
   *
   *     adminThread.detach();
   *     return RandomEngine(pState);
   *   }
   * };
   *
   * template <typename T, typename Dist = UniformDistribution<T>>
   * class RNG{
   * public:
   *   T get() const noexcept {
   *     T ret;
   *     pEngine->channel.readMessage([&ret](void* pMsg, void* pState) mutable { ret = *(T*)pMsg; ++((RandomEngine::State*)pState)->countConsumed; });
   *     return ret;
   *   }
   *   inline T operator()() const noexcept {
   *     return this->get();
   *   }
   *   template <size_t N>
   *   void get(std::span<T, N> output) const noexcept {
   *     pEngine->channel.readMessage([n = output.size()](void* pMsg){ return n * sizeof(T); }, [output](void* pMsg, auto* pState){ std::memcpy(output.data(), pMsg, output.size() * sizeof(T)); pState->countConsumed += output.size(); });
   *   }
   *   template <size_t N>
   *   void operator()(std::span<T, N> output) const noexcept {
   *     this->get(output);
   *   }
   *   void get(T* pArray, size_t desiredCount) const noexcept {
   *     this->get(std::span{ pArray, desiredCount });
   *   }
   *   void operator()(T* pArray, size_t desiredCount) const noexcept {
   *     this->get(std::span{ pArray, desiredCount });
   *   }
   *
   *
   *
   * private:
   *   RandomEngine*              pEngine;
   *   [[no_unique_address]] Dist distribution;
   * };
   *
   *   Channel {  }
   *
   *   State { }
   *
   *
   *   Agent {
   *     Set<Channel>,
   *     Set<State>
   *   }
   *
   *   Administrator { }
   *
   *   Entity {
   *     Set<Agent>,
   *     Administrator
   *   }
   *
   *
   *
   *
   *
   * */





  class InstructorAgent{

    binary_semaphore availSenders_;
    binary_semaphore availReceivers_;

  public:
    // One -> One
  };
  class BroadcasterAgent{

    binary_semaphore availSenders_;
    semaphore       availReceivers_;

  public:
    // One -> Many
  };
  class CollectorAgent{

    semaphore availSenders_;
    binary_semaphore availReceivers_;

  public:
    // Many -> One
  };
  class DispatcherAgent{

    semaphore availSenders_;
    semaphore availReceivers_;

  public:
    // Many -> Many
  };



  class UnsafeAgent{
    u64                  typeId_;
    u32                  propertyBits_;
    std::atomic_int32_t  refCount_;
    semaphore            availSenders_;
    semaphore            availReceivers_;

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

    uuid typeID_;
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

#endif //VALKYRIE_AGENT_AGENT_HPP
