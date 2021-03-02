//
// Created by maxwe on 2021-03-01.
//

#include <valkyrie/Core/Error/GenericCode.hpp>
#include <valkyrie/Core/Agent/Agent.hpp>

#include <thread>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <synchapi.h>
#include <processthreadsapi.h>



using namespace valkyrie;



namespace {

  using Clock = std::chrono::high_resolution_clock;


  using Core::BinarySemaphore, Core::Semaphore, Core::CountingSemaphore;
  using Core::Status;

  class AutonomousTask;
  template <std::derived_from<AutonomousTask> T>
  class Task;

  template <std::derived_from<AutonomousTask> T, typename ...Args> requires(ConstructibleFrom<T, Args...>)
  Task<T> start(Args&& ...args) noexcept;

  using PFN_threadProc = unsigned long(__stdcall *)(void*) noexcept;

  class AutonomousTask{
    Status threadStatus = Core::Code::NotReady;
    std::atomic_uint32_t refCount;

    virtual Status           begin() = 0;
    virtual Status           onExit(Status exitStatus) noexcept {
      return exitStatus;
    }
    virtual Core::StringView getName() const noexcept {
      return "Anonymous Task";
    }




    template <typename T, typename ...Args>
    inline static __stdcall unsigned long threadProcess(void* pParams) noexcept {
      using ParamType = std::tuple<AutonomousTask**, BinarySemaphore*, std::tuple<Args...>>;
      auto& params = *static_cast<ParamType*>(pParams);
      T task{ std::make_from_tuple<T>(std::get<2>(params)) };
      (*std::get<0>(params)) = &task;
      std::get<1>(params)->release();
      pThisTask = &task;
      pThisTask->threadStatus = pThisTask->begin();

      uint32_t refCount;
      while ((refCount = pThisTask->refCount.load(std::memory_order_acquire)))
        pThisTask->refCount.wait(refCount);

      return 0;
    }

    static thread_local AutonomousTask* pThisTask;


  protected:



    template <typename Rep, typename Period>
    void sleepFor(std::chrono::duration<Rep, Period> duration) noexcept {
      this->sleepUntil(duration + Clock::now());
    }
    template <typename Clk, typename Duration>
    void sleepUntil(std::chrono::time_point<Clk, Duration> timePoint) noexcept {
      while(Clock::now() < timePoint)
        this->yield();
    }

    void yield() {
      SwitchToThread();
    }
    void suspend();

  public:

    template <std::derived_from<AutonomousTask> T, typename ...Args> requires(ConstructibleFrom<T, Args...>)
    friend Task<T> launch(Args&& ...args) noexcept;
  };

  template <std::derived_from<AutonomousTask> T>
  class Task{
  public:
  private:
    T*    pTask        = nullptr;
    void* threadHandle = nullptr;
    u32   threadId     = 0;

    template <std::derived_from<AutonomousTask> T, typename ...Args> requires(ConstructibleFrom<T, Args...>)
    friend Task<T> launch(Args&& ...args) noexcept;
  };

  template <std::derived_from<AutonomousTask> T, typename ...Args> requires(ConstructibleFrom<T, Args...>)
  Task<T> launch(Args&& ...args);





  void launchThread(PFN_threadProc threadProc, void* pParams){}

  template <std::derived_from<AutonomousTask> T, typename ...Args> requires(ConstructibleFrom<T, Args...>)
  Task<T> launch(Args&& ...args) noexcept {
    SECURITY_ATTRIBUTES secAttr{
      .nLength = sizeof(SECURITY_ATTRIBUTES),
      .lpSecurityDescriptor = nullptr,
      .bInheritHandle = true
    };
    size_t stackSize;
    if constexpr (requires{ { T::stackSize() } -> std::convertible_to<size_t>; })
      stackSize = T::stackSize();
    else
      stackSize = 0;

    Task<T> task;

    BinarySemaphore isReady{0};

    auto paramTuple = std::tuple{
        std::ref(task.pTask),
        std::ref(isReady),
        std::forward_as_tuple(std::forward<Args>(args)...)
    };

    using ParamType = decltype(paramTuple);

    constexpr static LPTHREAD_START_ROUTINE threadProcess = [](LPVOID pParams) -> DWORD {
      auto& params = *static_cast<ParamType*>(pParams);
      T task{ std::make_from_tuple<T>(std::get<2>(params)) };
      std::get<0>(params).get().pTask = &task;
      std::get<1>(params).get().release();
      AutonomousTask* pTask = &task;
      pTask->threadStatus = pTask->begin();
    };

    task.threadHandle = CreateThread(&secAttr,
                                     stackSize,
                                     threadProcess,
                                     &paramTuple,
                                     0,
                                     &task.threadId);

    isReady.acquire();
    return std::move(task);
  }



  class Channel{

  };

  class RandomGenerator{
  public:
    virtual ~RandomGenerator() = default;

    virtual void generateBatch(Channel* channel, u64 genCount) noexcept = 0;
  };

  class RandomEngineState{
    Channel              channel;
    std::atomic_uint64_t countConsumed;
    u32                  typeSize;
    std::atomic_uint32_t refCount;
  };

  class CurandRandomGenerator{};
  class OpenCLRandomGenerator{};
  class CPURandomGenerator{};

}











 class RandomEngine{
     RandomEngineState* pState;
  
     explicit RandomEngine(RandomEngineState*);
  
     public:
     RandomEngine() = default;
  
     static RandomEngine create(u64 batchSize, u64 totalBufferSize) noexcept{
         std::thread         adminThread;
         BinarySemaphore    isReady;
         RandomEngineState* pState;
    
             isReady.acquire();
    
             adminThread = std::thread{[batchSize, totalBufferSize](BinarySemaphore* isReady, RandomEngineState** ppState){
      
                 using clock = std::chrono::high_precision_clock;
             using self  = std::this_thread;
      
                 RandomEngineState                state;
             CudaRandomGenerator              generator;
             std::chrono::nanoseconds         sleepTime   = 50us;
             std::chrono::nanoseconds         lastSleepTime = sleepTime;
             uint64_t                         countConsumedDuringPreviousSleep = batchSize;
             std::unique_ptr<RandomGenerator> generator;
      
                 int64_t countConsumed = batchSize;
      
                 *ppState = &state;
             state.refCount += 1;
      
                 generator.generateBatch(&state.channel, batchSize);
      
                 while ((self::sleep_for(state.sleepTime), true) && state.refCount.load() != 0) {
               auto countConsumed = state.countConsumed.exchange(0);
               generator.generateBatch(&state.channel, batchSize);
             }
           }, &isReady, &pState};
    
             threadIsReady.acquire();
    
             adminThread.detach();
         return RandomEngine(pState);
       }
   };

 template <typename T>
 class RNG{
 public:
   T get() const noexcept {
     T ret;
     pEngine->channel.readMessage([&ret](void* pMsg, void* pState) mutable { ret = *(T*)pMsg; ++((RandomEngine::State*)pState)->countConsumed; });
     return ret;
   }
   inline T operator()() const noexcept {
     return this->get();
   }
   template <size_t N>
   void get(std::span<T, N> output) const noexcept {
     engine.channel.readMessage([n = output.size()](void* pMsg){ return n * sizeof(T); }, [output](void* pMsg, auto* pState){ std::memcpy(output.data(), pMsg, output.size() * sizeof(T)); pState->countConsumed += output.size(); });
   }
   template <size_t N>
   void operator()(std::span<T, N> output) const noexcept {
     this->get(output);
   }
   void get(T* pArray, size_t desiredCount) const noexcept {
     this->get(std::span{ pArray, desiredCount });
   }
   void operator()(T* pArray, size_t desiredCount) const noexcept {
     this->get(std::span{ pArray, desiredCount });
   }
  
  
  
   private:
     RandomEngine engine;
   };


   /*Agent {
     Set<Channel>,
     Set<State>
   }

   Administrator { }

   Entity {
     Set<Agent>,
     Administrator
   }*/