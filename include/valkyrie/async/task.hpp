//
// Created by Maxwell on 2021-04-02.
//

#ifndef VALKYRIE_ADT_TASK_HPP
#define VALKYRIE_ADT_TASK_HPP

#include <valkyrie/traits.hpp>
#include <valkyrie/async/semaphore.hpp>
#include <valkyrie/utility/function_ref.hpp>
#include <valkyrie/status/generic_code.hpp>

#include <csetjmp>
#include <chrono>
#include <coroutine>


#define VK_stdcall_conv VK_if(VK_compiler_msvc(__stdcall))

namespace valkyrie{


  using clock = std::chrono::system_clock;


  class autonomous_task;

  template <std::derived_from<autonomous_task> T>
  class task;

  class autonomous_task{

    template <std::derived_from<autonomous_task> T>
    friend class task;

    using PFN_threadProc = unsigned(VK_stdcall_conv*)(void*) noexcept;

    static std::pair<void*, unsigned> launchThread_(size_t stackSize, PFN_threadProc threadProc, void* pParams) noexcept;
    static void                       setTaskDesc_(void* handle, autonomous_task* pTask) noexcept;


    status               threadStatus = code::not_ready;
    std::atomic_uint32_t refCount;

    virtual status           begin() = 0;
    virtual void             on_exit(const status_code<void>& exitStatus) noexcept { }


    enum {
      BeginTask,
      NormalExit,
      FastExit
    };


    template <typename T, typename ...Args>
    inline static VK_stdcall_conv unsigned threadProcess(void* pParams) noexcept {
      using ParamType = std::tuple<autonomous_task**, binary_semaphore*, std::tuple<Args&&...>>;
      auto& params = *static_cast<ParamType*>(pParams);
      T task{ std::make_from_tuple<T>(std::get<2>(params)) };
      (*std::get<0>(params)) = &task;
      autonomous_task* pTask = &task;
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

      pTask->on_exit(pTask->threadStatus);

      return 0;
    }

    /*template <typename T, typename ...Args>
    inline static VK_stdcall_conv unsigned threadProcessNonBlocking(void* pParams) noexcept{
      using ParamType = std::tuple<autonomous_task**, binary_semaphore*, std::tuple<Args&&...>>;
      auto& params = *static_cast<ParamType*>(pParams);
      T task{ std::make_from_tuple<T>(std::get<2>(params)) };
      (*std::get<0>(params)) = &task;
      autonomous_task* pTask = &task;
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

      pTask->on_exit(pTask->threadStatus);

      return 0;
    }*/

    static thread_local autonomous_task* pThisTask;
    static thread_local std::jmp_buf     ExitBuffer;


  protected:

    ~autonomous_task() = default;



    template <typename Rep, typename Period>
    void sleep_for(std::chrono::duration<Rep, Period> duration) noexcept {
      const std::chrono::time_point timePoint = duration + clock::now();
      do {
        this->yield();
      } while( clock::now() < timePoint );
    }
    template <typename Clk, typename Duration>
    void sleep_until(std::chrono::time_point<Clk, Duration> timePoint) noexcept {
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

    void yield();

    virtual string_view get_description() const noexcept {
      return "Anonymous Task";
    }

  public:

    template <typename T, typename ...Args>
    inline constexpr static auto ThreadProcess = &threadProcess<T, Args...>;
  };

  template <std::derived_from<autonomous_task> T>
  class task{
  public:
    task() = default;
    explicit task(T* pTask, void* handle, u32 id) noexcept
        : pTask(pTask),
        threadHandle(handle),
        threadId(id){}



  private:
    T*    pTask        = nullptr;
    void* threadHandle = nullptr;
    u32   threadId     = 0;
    /*template <std::derived_from<AutonomousTask> T, typename ...Args> requires(ConstructibleFrom<T, Args...>)
    friend Task<T> launch(Args&& ...args) noexcept;*/
  };

  template <std::derived_from<autonomous_task> T, typename ...Args>
  task<T> launch(Args&& ...args) noexcept requires(ConstructibleFrom<T, Args...>) {

    T*    pTask;
    void* handle;
    u32   id;

    binary_semaphore isReady{0};

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

    std::tie(handle, id) = launchThread_(stackSize, autonomous_task::ThreadProcess<T, Args...>, &paramTuple);

    isReady.acquire();

    return Task<T>(pTask, handle, id);
  }
}

#endif//VALKYRIE_ADT_TASK_HPP
