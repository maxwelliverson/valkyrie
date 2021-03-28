//
// Created by maxwe on 2021-03-01.
//

#include <valkyrie/agent/agent.hpp>
#include <valkyrie/agent/mailbox.hpp>
#include <valkyrie/utility/function_ref.hpp>
#include <valkyrie/status/generic_code.hpp>
#include <valkyrie/adt/list.hpp>

#include <thread>
#include <csetjmp>

#include <curand_kernel.h>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <processthreadsapi.h>
#include <synchapi.h>


using namespace valkyrie;



namespace {

  using clock = std::chrono::high_resolution_clock;

  class AutonomousTask;
  template <std::derived_from<AutonomousTask> T>
  class Task;

#define STD_CALLCONV VK_if(VK_compiler_msvc(__stdcall))

  using PFN_threadProc = unsigned(STD_CALLCONV *)(void*) noexcept;

  class AutonomousTask{

    template <std::derived_from<AutonomousTask> T>
    friend class Task;

    status threadStatus = code::NotReady;
    std::atomic_uint32_t refCount;

    virtual status           begin() = 0;
    virtual void             onExit(const status_code<void>& exitStatus) noexcept {
      //return exitStatus;
    }


    enum {
      BeginTask,
      NormalExit,
      FastExit
    };


    template <typename T, typename ...Args>
    inline static __stdcall unsigned threadProcess(void* pParams) noexcept {
      using ParamType = std::tuple<AutonomousTask**, BinarySemaphore*, std::tuple<Args&&...>>;
      auto& params = *static_cast<ParamType*>(pParams);
      T task{ std::make_from_tuple<T>(std::get<2>(params)) };
      (*std::get<0>(params)) = &task;
      AutonomousTask* pTask = &task;
      pThisTask = pTask;
      std::get<1>(params)->release();

      switch (setjmp(ExitBuffer)) {
        case BeginTask:
          pTask->threadStatus = pTask->begin();
          break;
        case NormalExit: {
          uint32_t refCount_;
          while ((refCount_ = pTask->refCount.load(std::memory_order_acquire)))
            pTask->refCount.wait(refCount_);
        }
        case FastExit:
          break;

        VK_no_default;
      }

      pTask->onExit(pTask->threadStatus);

      return 0;
    }

    static thread_local AutonomousTask* pThisTask;
    static thread_local std::jmp_buf    ExitBuffer;


  protected:

    ~AutonomousTask() = default;



    template <typename Rep, typename Period>
    void sleepFor(std::chrono::duration<Rep, Period> duration) noexcept {
      const std::chrono::time_point timePoint = duration + clock::now();
      do {
        this->yield();
      } while( clock::now() < timePoint );
    }
    template <typename Clk, typename Duration>
    void sleepUntil(std::chrono::time_point<Clk, Duration> timePoint) noexcept {
      while( clock::now() < timePoint )
        this->yield();
    }

    template <typename Dom>
    VK_noreturn void kill(status_code<Dom> status) noexcept {
      pThisTask->threadStatus = std::move(status);
      std::longjmp(ExitBuffer, FastExit);
    }
    template <typename Dom>
    VK_noreturn void exit(status_code<Dom> status) noexcept {
      pThisTask->threadStatus = std::move(status);
      std::longjmp(ExitBuffer, NormalExit);
    }

    void yield() {
      SwitchToThread();
    }

  public:

    virtual string_view getDescription() const noexcept {
      return "Anonymous Task";
    }

    template <typename T, typename ...Args>
    inline constexpr static auto ThreadProcess = &threadProcess<T, Args...>;

  };

  template <std::derived_from<AutonomousTask> T>
  class Task{
  public:
    Task() = default;
    explicit Task(T* pTask, void* handle, u32 id) noexcept
        : pTask(pTask),
          threadHandle(handle),
          threadId(id){}



  private:
    T*    pTask        = nullptr;
    void* threadHandle = nullptr;
    u32   threadId     = 0;

    template <std::derived_from<AutonomousTask> T, typename ...Args> requires(ConstructibleFrom<T, Args...>)
    friend Task<T> launch(Args&& ...args) noexcept;
  };

  std::pair<void*, unsigned> launchThread_(size_t stackSize, PFN_threadProc threadProc, void* pParams) noexcept {
    SECURITY_ATTRIBUTES secAttr{
        .nLength = sizeof(SECURITY_ATTRIBUTES),
        .lpSecurityDescriptor = nullptr,
        .bInheritHandle = true
    };
    std::pair<void*, unsigned> results;

    results.first = reinterpret_cast<void*>(_beginthreadex(
        &secAttr,
        static_cast<unsigned>(stackSize),
        threadProc,
        pParams,
        0,
        &results.second));

    return results;
  }
  void                       setTaskDesc_(void* handle, AutonomousTask* pTask) noexcept {

    static constexpr int DescBufferLength = 4096 * 2;
    wchar_t descriptionBuffer[DescBufferLength];

    auto desc = pTask->getDescription();
    int result = MultiByteToWideChar(CP_UTF8,
                                     0,
                                     desc.c_str(),
                                     (int)desc.size(),
                                     descriptionBuffer,
                                     DescBufferLength);
    assert(result);
    descriptionBuffer[result] = L'\0';

    auto descResult = SetThreadDescription(handle, descriptionBuffer);

    assert(SUCCEEDED(descResult));
  }

  template <std::derived_from<AutonomousTask> T, typename ...Args>
  Task<T> launch(Args&& ...args) noexcept requires(ConstructibleFrom<T, Args...>) {

    T*    pTask;
    void* handle;
    u32   id;

    BinarySemaphore isReady{0};

    auto paramTuple = std::tuple{
      &pTask,
      &isReady,
      std::forward_as_tuple(std::forward<Args>(args)...)
    };

    size_t stackSize;
    if constexpr (requires{ { T::stackSize() } -> std::convertible_to<size_t>; })
      stackSize = T::stackSize();
    else
      stackSize = 0;

    std::tie(handle, id) = launchThread_(stackSize, AutonomousTask::ThreadProcess<T, Args...>, &paramTuple);

    isReady.acquire();

    return Task<T>(pTask, handle, id);
  }


  using function_ref;

  class Channel{

  };

  class BroadcastChannel{
    void* pBuffer;
    u32   bufferSize;

    std::atomic_uint32_t writeOffset;
    std::atomic_uint32_t nextReadOffset;
    std::atomic_uint32_t writeEndOffset;

  public:

    inline status writeMessage(function_ref<void(void*)> fnCtor, u64 msgSize) noexcept;
    inline status readMessage(function_ref<status(void*)> msgProc)       noexcept;
    inline status tryReadMessage(function_ref<status(void*)> msgProc)    noexcept;
  };

  class RandomGenerator{
  public:
    virtual ~RandomGenerator() = default;

    virtual void generateBatch(BroadcastChannel& channel, u64 genCount) noexcept = 0;
  };

  /*template <typename T>
  struct AtomicSListNode{
    atomic<AtomicSListNode<T>*> pNext;
    T value;
  };
  template <typename T, typename Alloc = std::allocator<AtomicSListNode<T>>>
  class AtomicSList{

  public:

    using node_type = AtomicSListNode<T>;
    using atomic_node_type = atomic<node_type*>;
    using value_type = T;
    using allocator_type = typename std::allocator_traits<Alloc>::template rebind_alloc<node_type>;

    using reference = value_type&;


    AtomicSList() = default;
    AtomicSList(const AtomicSList& other);
    AtomicSList(AtomicSList&& other) noexcept;

    template <std::ranges::range Rng> requires(std::constructible_from<T, std::ranges::range_reference_t<Rng>>)
    explicit AtomicSList(Rng&& rng) noexcept {
      auto beg = std::ranges::begin(rng);
      const auto end = std::ranges::end(rng);

      node_type* nextNode;

      if ( beg != end ) {

        nextNode = makeNode(*beg);
        pHead.store(nextNode);
        pTail.store(nextNode);

        ++beg;
      }

      for ( ; beg != end; ++beg ) {
        nextNode = makeNode(*beg);
        pTail.exchange(nextNode)->pNext.store(nextNode);
      }
    }


    template<typename ...Args>
    T& emplaceBack(Args&& ...args) noexcept {

    }

  private:

    using alloc_traits = std::allocator_traits<allocator_type>;


    inline void removeNextNode(atomic_node_type* node) noexcept {
      node_type* lastHead = node->load( std::memory_order_acquire );
      node_type* nextNode;
      do {
        nextNode = lastHead->pNext.load( std::memory_order_acquire );
      } while( !node->compare_exchange_weak( lastHead, nextNode, std::memory_order_acq_rel ) );

      destroyNode(lastHead);
    }

    template <typename ...Args>
    node_type* makeNode(Args&& ...args) noexcept {
      auto* pObj = alloc_traits::allocate(alloc, 1);
      alloc_traits::construct(alloc, pObj, std::forward<Args>(args)... );
      return pObj;
    }
    void destroyNode(node_type* pNode) noexcept {

    }

    atomic<AtomicSListNode<T>*>    pHead  = nullptr;
    atomic<AtomicSListNode<T>*>    pTail  = nullptr;
    u64                                  length = 0;
    [[no_unique_address]] allocator_type alloc;

  };*/

  enum class cmd{
    destroy,

  };

  class task_list;

  class task_node{

    friend class task_list;

    task_list*         list;
    atomic<task_node*> next;

    virtual void destroy() noexcept { delete this; }
    virtual void process() noexcept = 0;

  protected:



    inline void proceed() noexcept;
    inline void finish() noexcept;
    inline void cycle() noexcept;
    inline void bump() noexcept;



  public:
    virtual ~task_node() = default;

    virtual void anchor();
  };

  class task_list{

    friend class task_node;

  public:

    void enqueue(task_node* task) noexcept {
      task_node* tail_task = tail.load(std::memory_order_acquire);
      while (!tail.compare_exchange_weak(tail_task, task, std::memory_order_acq_rel));
      if (tail_task) {
        task_node* node = nullptr;
        while ( !tail_task->next.compare_exchange_strong(node, task, std::memory_order_acq_rel) );
      } else {
        head.compare_exchange_strong(tail_task, task, std::memory_order_acq_rel);
        //assert( result );
      }
      task->list = this;
      ++length;
    }

    void run() noexcept {
      task_node* node = head.load(std::memory_order_acquire);
      if ( node )
        node->process();
    }

    u64 size() const noexcept {
      return length.load(std::memory_order_acquire);
    }
    bool empty() const noexcept {
      return length == 0;
    }


  private:
    atomic<task_node*> head        = nullptr;
    atomic<task_node*> tail        = nullptr;
    atomic<u64>        length = 0;
  };


  class CudaRingBuffer{
    u64                          bufferLength = 0;
    CUmemGenericAllocationHandle memHandle    = 0;
    CUdeviceptr                  memAddr      = 0;

    std::atomic_uint64_t         nextReadOffset  = 0;
    std::atomic_uint64_t         lastReadOffset  = 0;
    std::atomic_uint64_t         nextWriteOffset = 0;
    std::atomic_uint64_t         lastWriteOffset = 0;



    struct offsets {
      std::atomic_uint64_t next_offset = 0;
      std::atomic_uint64_t last_offset = 0;
    } read, write;


    void createHandle(const CUmemAllocationProp& props) noexcept {
      auto result = cuMemCreate(&memHandle, bufferLength, &props, 0);
      assert(result == CUDA_SUCCESS);
    }
    void reserveRange(size_t align) noexcept {
      auto result = cuMemAddressReserve(&memAddr, bufferLength * 2, align, 0, 0);
      assert( result == CUDA_SUCCESS);
    }
    void mapRange() const noexcept {
      auto result = cuMemMap(memAddr, bufferLength, 0, memHandle, 0);
      assert( result == CUDA_SUCCESS );
      result      = cuMemMap(memAddr + bufferLength, bufferLength, 0, memHandle, 0);
      assert( result == CUDA_SUCCESS );
    }
    void setMemAccess(const CUmemAccessDesc& accessDesc) const noexcept {
      auto result = cuMemSetAccess(memAddr, bufferLength * 2, &accessDesc, 1);
      assert( result == CUDA_SUCCESS );
    }

    void destroyHandle() noexcept {
      if ( memHandle ) {
        auto result = cuMemRelease(memHandle);
        assert( result == CUDA_SUCCESS );
        memHandle = 0;
      }
    }
    void freeRange() noexcept {
      if ( memAddr ) {
        auto result = cuMemAddressFree(memAddr, bufferLength * 2);
        assert( result == CUDA_SUCCESS );
        memAddr = 0;
      }
    }
    void unmapRange() const noexcept {
      if ( memAddr ) {
        auto result = cuMemUnmap(memAddr, bufferLength * 2);
        assert( result == CUDA_SUCCESS );
      }
    }

    static void fillAccessDesc(CUmemAccessDesc& accessDesc, CUdevice dev) noexcept {
      accessDesc.flags = CU_MEM_ACCESS_FLAGS_PROT_READWRITE;
      accessDesc.location = {
          .type = CU_MEM_LOCATION_TYPE_DEVICE,
          .id = dev
      };
    }
    static void fillProperties(CUmemAllocationProp& props, CUdevice dev) noexcept {
      props.type = CU_MEM_ALLOCATION_TYPE_PINNED;
      props.requestedHandleTypes = CU_MEM_HANDLE_TYPE_WIN32;
      props.location = {
          .type = CU_MEM_LOCATION_TYPE_DEVICE,
          .id   = dev
      };
      props.win32HandleMetaData = nullptr;
      props.allocFlags = {};
    }
    static size_t getMinGranularity(const CUmemAllocationProp& props) noexcept {
      size_t allocGranularity;

      auto allocGranResult = cuMemGetAllocationGranularity(&allocGranularity,
                                                           &props,
                                                           CU_MEM_ALLOC_GRANULARITY_MINIMUM);
      assert(allocGranResult == CUDA_SUCCESS);
      return allocGranularity;
    }
    static size_t alignLength(u64 length, u64 align) noexcept {
      const size_t allocGranMask = align - 1;
      const size_t maskedLength = allocGranMask & length;
      return length + (static_cast<bool>(maskedLength) * ((allocGranMask & ~maskedLength ) + 1));
    }

    CudaRingBuffer() = default;

    inline CUdeviceptr ptr(u64 offset) const noexcept {
      return memAddr + offset;
    }


    struct task_list;
    struct task_list_node;
    using pfn_task_process = task_list(*)(task_list_node*) noexcept;

    struct callback_data;
    using pfn_callback = callback_data*(*)(callback_data*) noexcept;

    struct op_data{
      u64 offset;
      u64 length;
    };

    struct callback_data{
      pfn_callback           callback;
      op_data                data;
      atomic<callback_data*> p_next;
    };

    static op_data begin(u64 length, u64 bufferLength, offsets& o) noexcept {
      op_data info{
          .offset = o.next_offset.load(std::memory_order_acquire),
          .length = length
      };
      u64 next_offset;

      do {
        const auto availLength = [&]{
          const i64 diff = static_cast<i64>(o.last_offset.load(std::memory_order_acquire)) -
                           static_cast<i64>(info.offset);
          if ( diff < 0 )
            return bufferLength + diff;
          return static_cast<u64>(diff);
        }();
        info.length = std::max(info.length, availLength);
        next_offset = info.offset + info.length;
        next_offset -= static_cast<bool>(next_offset >= bufferLength) * bufferLength;
      } while (!o.next_offset.compare_exchange_strong(info.offset, next_offset, std::memory_order_acq_rel));

      return info;
    }
    static op_data end(op_data info, offsets& o) noexcept {

    }

    op_data begin_read(u64 readLength) noexcept {
      return CudaRingBuffer::begin(readLength, bufferLength, read);
    }
    void    end_read(op_data info) noexcept {
      end();
    }
    op_data begin_write(u64 writeLength) noexcept {}
    void    end_write(op_data info) noexcept {}

  public:

    explicit CudaRingBuffer(CUdevice dev, u64 length) noexcept{

      CUmemAccessDesc accessDesc;
      CUmemAllocationProp properties;

      fillAccessDesc(accessDesc, dev);
      fillProperties(properties, dev);


      const size_t allocGranularity = getMinGranularity(properties);
      bufferLength                  = alignLength(length, allocGranularity);

      createHandle(properties);
      reserveRange(allocGranularity);
      mapRange();
      setMemAccess(accessDesc);


    }
    CudaRingBuffer(const CudaRingBuffer& other) = delete;
    CudaRingBuffer(CudaRingBuffer&& other) noexcept
        : nextReadOffset(other.nextReadOffset.load()),
          lastReadOffset(other.lastReadOffset.load()),
          nextWriteOffset(other.nextWriteOffset.load()),
          lastWriteOffset(other.lastWriteOffset.load()){
      std::swap(bufferLength, other.bufferLength);
      std::swap(memHandle, other.memHandle);
      std::swap(memAddr, other.memAddr);
    }


    ~CudaRingBuffer() {
      unmapRange();
      freeRange();
      destroyHandle();
    }


    std::span<std::byte> read(std::span<std::byte>& byteSpan, CUstream stream) noexcept {

      const auto spanLength = byteSpan.size();
      u64 readLength = spanLength;
      u64 readOffset = nextReadOffset.load(std::memory_order_acquire);
      u64 nextOffset;

      do {
        const auto availLength = [&]{
          const i64 diff = static_cast<i64>(lastReadOffset.load(std::memory_order_acquire)) -
                           static_cast<i64>(readOffset);
          if ( diff < 0 )
            return bufferLength + diff;
          return static_cast<u64>(diff);
        }();
        readLength = std::max(readLength, availLength);
        nextOffset = readOffset + readLength;
        nextOffset -= static_cast<bool>(nextOffset >= bufferLength) * bufferLength;
      } while (!nextReadOffset.compare_exchange_strong(readOffset, nextOffset, std::memory_order_acq_rel));

      if ( !readLength )
        return {};

      using params_t = std::tuple<CudaRingBuffer*, u64, u64>;

      cuMemcpyDtoHAsync(byteSpan.data(), ptr(readOffset), readLength, stream);
      cuLaunchHostFunc(stream, [](void* pParams){
            auto* params = static_cast<params_t*>(pParams);
            auto [ pThis, readLength, readOffset ] = *params;
            u64 writeOffset = readOffset;
            const u64 nextOffset = readOffset + readLength;
            while ( !pThis->lastWriteOffset.compare_exchange_strong(writeOffset, nextOffset) ) {
              pThis->lastWriteOffset.wait(writeOffset);
              writeOffset = readOffset;
            }
            pThis->lastWriteOffset.notify_all();
            delete params;
          }, new params_t{ this, readLength, readOffset });

      byteSpan = byteSpan.subspan(readLength);
      return { byteSpan.data(), readLength };
    }
    void write(std::span<const std::byte> byteSpan, CUstream stream) noexcept {

      const auto spanLength = byteSpan.size();
      u64 writeLength = spanLength;
      u64 writeOffset = nextWriteOffset.load(std::memory_order_acquire);
      u64 nextOffset;

      do {
        const auto availLength = [&]{
          const i64 diff = static_cast<i64>(lastWriteOffset.load(std::memory_order_acquire)) -
                           static_cast<i64>(writeOffset);
          if ( diff < 0 )
            return bufferLength + diff;
          return static_cast<u64>(diff);
        }();
      } while ();

      cuMemcpyHtoDAsync()
    }
    void write(void(* pWriteMsgFn)(void*), CUstream stream) noexcept {

    }
    void write(void(* pWriteMsgFn)(void* addr, void*), void* pUserData, CUstream stream) noexcept {

    }
  };

  class CurandRandomGenerator : public RandomGenerator{
    CudaRingBuffer   deviceBuffer;
    CUstream         stream;
    CUgraphExec      graphExec;
    std::atomic_bool isExecuting;

    void initStream() noexcept {
      auto strResult = cuStreamCreate(&stream, CU_STREAM_NON_BLOCKING);
      assert(strResult == CUDA_SUCCESS);
    }
    void initGraph()  noexcept {

      CUgraph tmpGraph;

      auto captureBeginResult = cuStreamBeginCapture(stream, CU_STREAM_CAPTURE_MODE_RELAXED);


      auto captureEndResult = cuStreamEndCapture(stream, &tmpGraph);


    }

    void init() noexcept {
      initStream();
      initGraph();
    }
    void destroy() noexcept {
      auto execDestroy = cuGraphExecDestroy(graphExec);
      auto streamDestroy = cuStreamDestroy(stream);
    }




  public:

    CurandRandomGenerator(CUdevice device, u64 maxConsumeAtOnce)
        : deviceBuffer(device, maxConsumeAtOnce * sizeof(u64)){}

    void generateBatch(BroadcastChannel& channel, u64 genCount) noexcept override {
      isExecuting.wait(true);
      auto result = cuGraphLaunch(graphExec, stream);
      assert(result == CUDA_SUCCESS);
    }
  };



  class RandomEngineState{
    Channel              channel;
    std::atomic_uint64_t countConsumed;
    u32                  typeSize;
    std::atomic_uint32_t refCount;
  };

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