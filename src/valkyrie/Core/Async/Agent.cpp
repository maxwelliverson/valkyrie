//
// Created by Maxwell on 2020-12-14.
//

#include <valkyrie/Core/Async/Agent.hpp>

#define NOMINMAX

#include <windows.h>
#include <synchapi.h>
#include <memoryapi.h>

#include <atomic>
#include <deque>

using namespace valkyrie;

namespace {


  /*enum LockState{
    LockUnderflow = -2,
    InvalidLock   = -1,
    IsUnlocked    =  0,
    IsLocked      =  1,
    LOckOverflow  =  2
  };

  struct SpinLock{
    LockState state;

    void lock() {}
  };

  struct IntrusiveMutex{
    LockState outerLock;
    LockState innerLock;
    u64       ownerBits;
  };


  struct ThreadIdHelper{
    inline static thread_local char ID = '\0';
  };

  inline thread_local const u64 threadID = (u64)&ThreadIdHelper::ID;

  inline constexpr u64 noOwners = 0;
  inline constexpr u64 uniqueOwner = ~0ULL;

  struct Mutex{
    LockState state = IsUnlocked;
    LockState innerLock = IsUnlocked;
    u64       ownerBits = 0;

    void lock() noexcept {
      LockState comparer = IsLocked;
      while (state == comparer)
        WaitOnAddress(&state, &comparer, sizeof(LockState), 10000);

    }
    void lockShared() noexcept {}
    bool tryLock() noexcept {}
    bool tryLockShared() noexcept {}
    void unlock() noexcept {}
    void unlockShared() noexcept {}
  };*/

  inline constexpr size_t PageSize = 4096;
  inline constexpr size_t AllocationGranularity = VK_if(VK_system_windows(0x1 << 16)VK_else(PageSize));
  inline constexpr size_t PageAlign = PageSize;

  inline constexpr size_t ClobberGuardSize = PageSize;

  constexpr inline size_t alignSize(size_t from, size_t align) noexcept {
    return from + ((~((size_t) bool(from & (align - 1)) - 1)) & ((from ^ (align - 1)) & (align - 1)));
  }

  using PFN_VirtualAlloc = decltype(&VirtualAlloc);
  using PFN_VirtualAlloc2 = decltype(&VirtualAlloc2);
  using PFN_VirtualAllocEx = decltype(&VirtualAllocEx);
  using PFN_MapViewOfFile = decltype(&MapViewOfFile);
  using PFN_MapViewOfFile2 = decltype(&MapViewOfFile2);
  using PFN_MapViewOfFile3 = decltype(&MapViewOfFile3);
  using PFN_VirtualFree = decltype(&VirtualFree);
  using PFN_VirtualFreeEx = decltype(&VirtualFreeEx);
  using PFN_VirtualQuery = decltype(&VirtualQuery);
  using PFN_VirtualQueryEx = decltype(&VirtualQueryEx);
  using PFN_VirtualLock = decltype(&VirtualLock);
  using PFN_VirtualUnlock = decltype(&VirtualUnlock);
  using PFN_VirtualProtect = decltype(&VirtualProtect);
  using PFN_VirtualProtectEx = decltype(&VirtualProtectEx);
  using PFN_UnmapViewOfFile = decltype(&UnmapViewOfFile);
  using PFN_UnmapViewOfFileEx = decltype(&UnmapViewOfFileEx);
  using PFN_UnmapViewOfFile2 = decltype(&UnmapViewOfFile2);
  using PFN_CreateFileMapping = decltype(&CreateFileMapping);
  using PFN_CreateFileMapping2 = decltype(&CreateFileMapping2);
  using PFN_CloseHandle = decltype(&CloseHandle);

  struct VirtualMemoryApi {

    HMODULE KernelLib;

    PFN_VirtualAlloc VirtualAlloc;
    PFN_VirtualAlloc2 VirtualAlloc2;
    PFN_VirtualAllocEx VirtualAllocEx;
    PFN_MapViewOfFile MapViewOfFile;
    PFN_MapViewOfFile2 MapViewOfFile2;
    PFN_MapViewOfFile3 MapViewOfFile3;
    PFN_VirtualFree VirtualFree;
    PFN_VirtualFreeEx VirtualFreeEx;
    PFN_VirtualQuery VirtualQuery;
    PFN_VirtualQueryEx VirtualQueryEx;
    PFN_VirtualLock VirtualLock;
    PFN_VirtualUnlock VirtualUnlock;
    PFN_VirtualProtect VirtualProtect;
    PFN_VirtualProtectEx VirtualProtectEx;
    PFN_UnmapViewOfFile UnmapViewOfFile;
    PFN_UnmapViewOfFileEx UnmapViewOfFileEx;
    PFN_UnmapViewOfFile2 UnmapViewOfFile2;
    PFN_CreateFileMapping CreateFileMapping;
    PFN_CreateFileMapping2 CreateFileMapping2;
    PFN_CloseHandle CloseHandle;

    explicit VirtualMemoryApi(const char *pLibraryName = "KernelBase.dll") noexcept
        : KernelLib(LoadLibrary(pLibraryName)) {
      if (!KernelLib)
        panic(Core::System::Win32::getLastError());

      VirtualAlloc = (PFN_VirtualAlloc)   GetProcAddress(KernelLib, "VirtualAlloc");
      VirtualAlloc2 = (PFN_VirtualAlloc2) GetProcAddress(KernelLib, "VirtualAlloc2");
      VirtualAllocEx = (PFN_VirtualAllocEx) GetProcAddress(KernelLib, "VirtualAllocEx");
      MapViewOfFile = (PFN_MapViewOfFile)   GetProcAddress(KernelLib, "MapViewOfFile");
      MapViewOfFile2 = (PFN_MapViewOfFile2) GetProcAddress(KernelLib, "MapViewOfFile2");
      MapViewOfFile3 = (PFN_MapViewOfFile3) GetProcAddress(KernelLib, "MapViewOfFile3");
      VirtualFree = (PFN_VirtualFree) GetProcAddress(KernelLib, "VirtualFree");
      VirtualFreeEx = (PFN_VirtualFreeEx) GetProcAddress(KernelLib, "VirtualFreeEx");
      VirtualQuery = (PFN_VirtualQuery) GetProcAddress(KernelLib, "VirtualQuery");
      VirtualQueryEx = (PFN_VirtualQueryEx) GetProcAddress(KernelLib, "VirtualQueryEx");
      VirtualLock = (PFN_VirtualLock) GetProcAddress(KernelLib, "VirtualLock");
      VirtualUnlock = (PFN_VirtualUnlock) GetProcAddress(KernelLib, "VirtualUnlock");
      VirtualProtect = (PFN_VirtualProtect) GetProcAddress(KernelLib, "VirtualProtect");
      VirtualProtectEx = (PFN_VirtualProtectEx) GetProcAddress(KernelLib, "VirtualProtectEx");
      UnmapViewOfFile = (PFN_UnmapViewOfFile) GetProcAddress(KernelLib, "UnmapViewOfFile");
      UnmapViewOfFileEx = (PFN_UnmapViewOfFileEx) GetProcAddress(KernelLib, "UnmapViewOfFileEx");
      UnmapViewOfFile2 = (PFN_UnmapViewOfFile2) GetProcAddress(KernelLib, "UnmapViewOfFile2");
      CreateFileMapping = (PFN_CreateFileMapping) GetProcAddress(KernelLib, "CreateFileMapping");
      CreateFileMapping2 = (PFN_CreateFileMapping2) GetProcAddress(KernelLib, "CreateFileMapping2");
      CloseHandle = (PFN_CloseHandle) GetProcAddress(KernelLib, "CloseHandle");
    }
    ~VirtualMemoryApi() {
      if (!FreeLibrary(KernelLib))
        panic(Core::System::Win32::getLastError());
    }
  };
  inline const VirtualMemoryApi memoryApi{};

  inline auto getLastError() noexcept {
    return Core::System::Win32::getLastError();
  }

  //using SystemDomain = VK_if(VK_debug_build(Core::StatusValidationDomain<Core::System::Win32StatusDomain>) VK_else(Core::System::Win32StatusDomain));
  using SystemDomain = Core::System::Win32StatusDomain;
  using SystemStatus = Core::StatusCode<SystemDomain>;
  using SystemError  = Core::ErrorCode<SystemDomain>;


  template<typename T>
  using Win32Maybe = Core::Maybe<T, SystemDomain>;

  template<typename T>
  class Win32Result : public Win32Maybe<T> {
  public:
    Win32Result() : Win32Maybe<T>(getLastError()) {}
    Win32Result(const T &value) : Win32Maybe<T>(value) {}
    Win32Result(T &&value) noexcept : Win32Maybe<T>(std::move(value)) {}
    Win32Result(const SystemStatus &stat) : Win32Maybe<T>(stat) {}
    Win32Result(SystemStatus &&stat) : Win32Maybe<T>(std::move(stat)) {}
    template<typename... Args>
    requires(ConstructibleFrom<T, Args...>)
        Win32Result(std::in_place_t, Args &&...args) : Win32Maybe<T>(std::forward<Args>(args)...) {}
  };

  class VirtualPlaceholder {
    valkyrie::byte *address_ = nullptr;
    u64 size_ = 0;

  public:
    VirtualPlaceholder() = default;
    VirtualPlaceholder(const VirtualPlaceholder &) = delete;
    VirtualPlaceholder(VirtualPlaceholder &&other) noexcept {
      std::swap(address_, other.address_);
      std::swap(size_, other.size_);
    }
    VirtualPlaceholder &operator=(const VirtualPlaceholder &) = delete;
    VirtualPlaceholder &operator=(VirtualPlaceholder &&other) noexcept {
      this->~VirtualPlaceholder();
      new (this) VirtualPlaceholder(std::move(other));
      return *this;
    }


    VirtualPlaceholder(void *address, u64 size) noexcept : address_((valkyrie::byte *) address), size_(size) {}
    ~VirtualPlaceholder() {
      if (address_ && !memoryApi.VirtualFree(address_, 0, MEM_RELEASE))
        panic(getLastError());
    }

    inline u64 size() const noexcept {
      return size_;
    }
    inline void *address() const noexcept {
      return address_;
    }
    inline Win32Result<VirtualPlaceholder> takeFront(u64 bytes) noexcept {
      if (!memoryApi.VirtualFree(address_, bytes, MEM_RELEASE | MEM_PRESERVE_PLACEHOLDER))
        return {};
      auto *const oldAddress = address_;
      address_ += bytes;
      size_ -= bytes;
      return VirtualPlaceholder(oldAddress, bytes);
    }
    inline valkyrie::byte *release() noexcept {
      auto tmp = address_;
      address_ = nullptr;
      size_ = 0;
      return tmp;
    }
  };
  class MemoryFile {
    HANDLE fileHandle = nullptr;

  public:
    MemoryFile() = default;
    MemoryFile(const MemoryFile &) = delete;
    MemoryFile(MemoryFile &&other) noexcept : fileHandle(other.fileHandle) {
      other.fileHandle = nullptr;
    }
    MemoryFile &operator=(const MemoryFile &) = delete;
    MemoryFile &operator=(MemoryFile &&other) noexcept {
      this->~MemoryFile();
      new (this) MemoryFile(std::move(other));
      return *this;
    }


    explicit MemoryFile(HANDLE handle) noexcept : fileHandle(handle) {}
    ~MemoryFile() {
      if (fileHandle && !memoryApi.CloseHandle(fileHandle))
        panic(getLastError());
    }

    inline HANDLE release() noexcept {
      HANDLE tmp = fileHandle;
      fileHandle = nullptr;
      return tmp;
    }
    inline HANDLE handle() const noexcept {
      return fileHandle;
    }
  };
  class MappedMemory {
    void *address_ = nullptr;
    u64 size_ = 0;

  public:
    MappedMemory() = default;
    MappedMemory(const MappedMemory &) = delete;
    MappedMemory(MappedMemory &&other) noexcept {
      std::swap(address_, other.address_);
      std::swap(size_, other.size_);
    }

    MappedMemory &operator=(const MappedMemory &) = delete;
    MappedMemory &operator=(MappedMemory &&other) noexcept {
      this->~MappedMemory();
      new (this) MappedMemory(std::move(other));
      return *this;
    }
    MappedMemory(void *pMessages, u64 bufferLength) noexcept
        : address_(pMessages), size_(bufferLength) {}
    ~MappedMemory() {
      auto result = unmap();
      if (result.failure())
        panic(result);
    }

    inline void *address() const noexcept {
      return address_;
    }
    inline u64 size() const noexcept {
      return size_;
    }
    inline std::pair<void *, u64> release() noexcept {
      std::pair<void *, u64> result{nullptr, 0};
      std::swap(result.first, address_);
      std::swap(result.second, size_);
      return result;
    }
    inline SystemStatus unmap() noexcept {
      if (address_) {
        if (!memoryApi.UnmapViewOfFile(address_))
          return getLastError();
        address_ = nullptr;
        size_ = 0;
      }
      return {};
    }
  };


  inline Win32Result<VirtualPlaceholder> allocatePlaceholder(u64 placeholderSize) noexcept {
    auto result = memoryApi.VirtualAlloc2(
        nullptr,
        nullptr,
        placeholderSize,
        MEM_RESERVE | MEM_RESERVE_PLACEHOLDER,
        PAGE_NOACCESS,
        nullptr,
        0);
    if (!result)
      return {};
    return VirtualPlaceholder{(valkyrie::byte *) result, placeholderSize};
  }
  inline Win32Result<MemoryFile> createFileInMemory(u64 fileSize) noexcept {
    auto result = memoryApi.CreateFileMapping(
        nullptr,
        nullptr,
        PAGE_READWRITE,
        0,
        (DWORD) fileSize,
        nullptr);
    if (!result)
      return {};
    return MemoryFile(result);
  }
  inline Win32Result<MappedMemory> mapPlaceholder(MemoryFile &file, VirtualPlaceholder &vmLocation) noexcept {
    const u64 size = vmLocation.size();
    void *tmp = memoryApi.MapViewOfFile3(
        file.handle(),
        nullptr,
        vmLocation.address(),
        0,
        size,
        MEM_REPLACE_PLACEHOLDER,
        PAGE_READWRITE,
        nullptr,
        0);
    if (!tmp)
      return {};
    VK_assert(tmp == vmLocation.address());
    return MappedMemory(vmLocation.release(), size);
  }

  inline Win32Result<std::pair<MappedMemory, MappedMemory>> allocateCircularBuffer(u64 length) noexcept {
    std::pair<VirtualPlaceholder, VirtualPlaceholder> placeholder;
    MemoryFile physicalMemory;
    std::pair<MappedMemory, MappedMemory> mappedBuffer;

    if (auto result = allocatePlaceholder(length * 2))
      placeholder.first = result.take();
    else
      return result.status();

    if (auto result = placeholder.first.takeFront(length))
      placeholder.second = result.take();
    else
      return result.status();

    if (auto result = createFileInMemory(length))
      physicalMemory = result.take();
    else
      return result.status();

    if (auto result = mapPlaceholder(physicalMemory, placeholder.first))
      mappedBuffer.first = result.take();
    else
      return result.status();

    if (auto result = mapPlaceholder(physicalMemory, placeholder.second))
      mappedBuffer.second = result.take();
    else
      return result.status();

    return std::move(mappedBuffer);
  }

  inline SystemStatus unmapView(void *pView) noexcept {
    if (!memoryApi.UnmapViewOfFile(pView))
      return getLastError();
    return {};
  }






  // Messages

  /*struct MessageHook;

  inline constexpr size_t IteratorSize = 3 * sizeof(void*);

  struct IteratorPlaceholderSizer{
    char placeholderBuffer[IteratorSize];
    void* pUserData;
    Core::PFN_processMessage msgHook;
  };
  struct IteratorPlaceholder{
    char buffer[sizeof(typename Core::Colony<IteratorPlaceholderSizer>::iterator)]{};
  };

  using MessageHookContainer = Core::Colony<MessageHook>;

  struct MessageHook{
    void*                    pUserData;
    Core::PFN_processMessage msgHook;
    IteratorPlaceholder      nextIterator{};

    MessageHook() = default;
    MessageHook(Core::PFN_processMessage msgHook, void* pUserData) noexcept
        : pUserData(pUserData), msgHook(msgHook){}
  };

  using MessageHookPointer = typename MessageHookContainer::iterator;

  static_assert(sizeof(MessageHookPointer) == IteratorSize);

  struct MessageHookStorage{
    MessageHookPointer msgNoOp;
    MessageHookPointer msgKill;
    MessageHookPointer msgCloseQueue;
    MessageHookPointer msgInvalidateQueue;
    MessageHookPointer msgSkip;

    MessageHookPointer msgBarrierArrive;
    MessageHookPointer msgBarrierWait;
    MessageHookPointer msgBarrierArriveAndWait;
    MessageHookPointer msgBarrierDrop;
    MessageHookPointer msgAcquireSemaphore;
    MessageHookPointer msgReleaseSemaphore;

    MessageHookPointer msgQueryUuid;
    MessageHookPointer msgQueryName;
    MessageHookPointer msgQueryDescription;
    MessageHookPointer msgQueryProducerCount;
    MessageHookPointer msgQueryMethod;
    MessageHookPointer msgQueryProperty;
    MessageHookPointer msgQuerySupport;
    MessageHookPointer msgSetProperty;

    MessageHookPointer msgWrite;
    MessageHookPointer msgRead;
    MessageHookPointer msgFlush;

    MessageHookPointer msgInvokeMethod;
    MessageHookPointer msgRegisterMethod;
    MessageHookPointer msgUnregisterMethod;
    MessageHookPointer msgInvokeCallback;
    MessageHookPointer msgInvokeCoroutine;
    MessageHookPointer msgRegisterHook;
    MessageHookPointer msgUnregisterHook;

    MessageHookPointer msgUserDefined;
  };

  using HookKey = MessageHookPointer MessageHookStorage::*;

  static_assert(sizeof(HookKey) == sizeof(u32));

  struct MessageHooks{
    MessageHookStorage   preHooks;
    MessageHookStorage   postHooks;
    MessageHookContainer hookStorage;

    void addPreHook(HookKey hookKey, Core::PFN_processMessage pFunction, void* pUserData) {
      MessageHookPointer* hookPointer = &(preHooks.*hookKey);

      while (hookPointer->operator->())
        hookPointer = reinterpret_cast<MessageHookPointer*>(static_cast<void*>(&((**hookPointer).nextIterator)));

      *hookPointer = hookStorage.emplace(pFunction, pUserData);
    }
    void addPostHook(HookKey hookKey, Core::PFN_processMessage pFunction, void* pUserData) {
      MessageHookPointer* hookPointer = &(postHooks.*hookKey);

      while (hookPointer->operator->())
        hookPointer = reinterpret_cast<MessageHookPointer*>(static_cast<void*>(&((**hookPointer).nextIterator)));

      *hookPointer = hookStorage.emplace(pFunction, pUserData);
    }
  };*/






  struct VirtualRingBuffer{
    valkyrie::byte* pBuffer;
    u32             bufferSize;
  };


  inline Win32Result<VirtualRingBuffer> allocateRingBuffer(u32 bufferSize) noexcept {

  }



  enum class Priority{
    Low,
    Default,
    High
  };
  enum class Threading{
    Single,
    Dual,
    Multi
  };

  inline constexpr Priority LowPriority     = Priority::Low;
  inline constexpr Priority DefaultPriority = Priority::Default;
  inline constexpr Priority HighPriority    = Priority::High;

  inline constexpr Threading SingleThreaded = Threading::Single;
  inline constexpr Threading DualThreaded   = Threading::Dual;
  inline constexpr Threading MultiThreaded  = Threading::Multi;


  struct AgentPolicy{
    Threading readThreading = SingleThreaded;
    Threading writeThreading = SingleThreaded;
    Priority priority = DefaultPriority;
  };

  /*template <Threading readThreading,
            Threading writeThreading,
            Priority priority>
  struct AgentMailbox;


  using PFN_writeMessage = void(*)(void* pMessageAddress, void* pUserData);
  //using PFN_readMessage  =



  template <Priority priority>
  struct AgentMailbox<SingleThreaded, SingleThreaded, priority> : VirtualRingBuffer{
    std::atomic_uint32_t readOffset = 0;
    std::atomic_uint32_t writeOffset = 0;
  };
  template <Priority priority>
  struct AgentMailbox<MultiThreaded, MultiThreaded,  priority> : VirtualRingBuffer{
    *//*bool writeProc(std::string_view message){

      const uint32_t messageStructSize = nextOffset(message);

      uint32_t writeOffset = 0;
      uint32_t nextOffset;
      Message* pMessage;

      do {
        nextOffset = writeOffset + messageStructSize;
        if (buffer.bufferSize < (nextOffset + offsetof(Message, nextOffset)))
          return false;
      } while(!buffer.nextWriteOffset.compare_exchange_strong(writeOffset, nextOffset, std::memory_order_acq_rel));


      pMessage = std::launder(new(static_cast<std::byte*>(buffer.pBuffer) + writeOffset) Message);
      pMessage->nextOffset.store(nextOffset, std::memory_order_release);
      pMessage->msgLength = uint32_t(message.length());
      std::memcpy(pMessage->pData, message.data(), pMessage->msgLength);

      pMessage->state.store(MessageState::Written, std::memory_order_release);
      pMessage->state.notify_all();

      return true;
    }

    bool readProc(std::stringstream& outBuffer, Buffer& buffer, BasicTimer& timer){

      TIME_FUNCTION(timer);

      uint32_t readOffset;
      uint32_t nextOffset = 0;
      Message* pMessage;
      //MessageState readState = MessageState::Invalid;

      readOffset = static_cast<uint32_t>(-1);

      while (!buffer.nextReadOffset.compare_exchange_strong(readOffset, nextOffset, std::memory_order_acq_rel)) {
        if (buffer.bufferSize - readOffset <= offsetof(Message, pData))
          return false;

        pMessage   = reinterpret_cast<Message*>(static_cast<std::byte*>(buffer.pBuffer) + readOffset);
        //readState = pMessage->state.load();

        pMessage->state.wait(MessageState::Invalid);
        if (pMessage->state.load() == MessageState::Final)
          return false;
        //assert(readState != MessageState::Processed);

        nextOffset = pMessage->nextOffset.load(std::memory_order_acquire);
      }


      assert(readOffset != nextOffset);

      *//**//*do {
        if (buffer.bufferSize == readOffset)
          return false;
        buffer.writeCompleteOffset.wait(readOffset);

        pMessage = reinterpret_cast<Message*>(static_cast<std::byte*>(buffer.pBuffer) + readOffset);
        nextOffset = pMessage->nextOffset.load(std::memory_order_acquire);

        if (buffer.bufferSize < nextOffset)
          return false;

      } while(!buffer.nextReadOffset.compare_exchange_weak(readOffset, nextOffset, std::memory_order_acq_rel));*//**//*

      outBuffer.write(pMessage->pData, pMessage->msgLength);

      pMessage->state = MessageState::Read;

      assert(readOffset < nextOffset);

#if defined(NDEBUG)
#define ASSERT_EVAL(...) __VA_ARGS__
#else
#define ASSERT_EVAL(...) assert(__VA_ARGS__)
#endif


      constexpr static auto checkIfRead = [](auto&& state){
        MessageState readState = MessageState::Read;
#if defined(NDEBUG)
        return state.compare_exchange_strong(readState, MessageState::Processed);
#else
        const bool result = state.compare_exchange_strong(readState, MessageState::Processed);
    //assert(result || readState == MessageState::Written);
    return result;
#endif

      };


      if (buffer.readCompleteOffset.compare_exchange_strong(readOffset, AcquireUnique, std::memory_order_acq_rel)){

        while (checkIfRead(pMessage->state)) {
          //readOffset = nextOffset;
          pMessage = reinterpret_cast<Message*>(static_cast<std::byte*>(buffer.pBuffer) + nextOffset);
          nextOffset = pMessage->nextOffset.load(std::memory_order_acquire);
          //assert(readOffset != nextOffset);
        }

        uint32_t releaseUnique = AcquireUnique;

        //readOffset = AcquireUnique;

        ASSERT_EVAL(buffer.readCompleteOffset.compare_exchange_strong(releaseUnique, nextOffset, std::memory_order_acq_rel));
        buffer.readCompleteOffset.notify_all();
      }

      //assert(readOffset < nextOffset || readOffset == AcquireUnique);

      *//**//*if (buffer.readCompleteOffset.compare_exchange_strong(readOffset, nextOffset, std::memory_order_acq_rel)){
        while (checkIfValidState(pMessage->state)) {
          readOffset = nextOffset;
          pMessage = reinterpret_cast<Message*>(static_cast<std::byte*>(buffer.pBuffer) + readOffset);

          nextOffset = pMessage->nextOffset.load(std::memory_order_acquire);
          assert(readOffset != nextOffset);
          ASSERT_EVAL(buffer.readCompleteOffset.compare_exchange_strong(readOffset, nextOffset, std::memory_order_acq_rel));
        }
      }*//**//*while (checkIfValidState(pMessage->state) &&
             buffer.readCompleteOffset.compare_exchange_strong(readOffset, nextOffset, std::memory_order_acq_rel)) {

        pMessage = reinterpret_cast<Message*>(static_cast<std::byte*>(buffer.pBuffer) + readOffset);

        //readOffset = nextOffset;

        nextOffset = pMessage->nextOffset.load(std::memory_order_acquire);
        assert(readOffset != nextOffset);
      }*//**//*

      *//**//*do {
        ((std::byte*&)pMessage) += pMessage->mSize;
        nextOffset += pMessage->mSize;
      } while(pMessage->mKind == MessageFullyProcessed);

      pMailbox->writeCompleteOffset.compare_exchange_strong(readOffset, nextOffset);

      reinterpret_cast<AgentMessage*>(pMailbox->pBuffer + readOffsetCopy)->mKind = MessageFullyProcessed;*//**//*

      return true;
    }*//*
  };
  template <Priority priority>
  struct AgentMailbox<SingleThreaded, MultiThreaded, priority> : VirtualRingBuffer{

    *//*bool writeProc(std::string_view message){

      const uint32_t messageStructSize = nextOffset(message);

      uint32_t writeOffset = 0;
      uint32_t nextOffset;
      Message* pMessage;

      do {
        nextOffset = writeOffset + messageStructSize;
        if (buffer.bufferSize < (nextOffset + offsetof(Message, nextOffset)))
          return false;
      } while(!buffer.nextWriteOffset.compare_exchange_strong(writeOffset, nextOffset, std::memory_order_acq_rel));


      pMessage = std::launder(new(static_cast<std::byte*>(buffer.pBuffer) + writeOffset) Message);
      pMessage->nextOffset.store(nextOffset, std::memory_order_release);
      pMessage->msgLength = uint32_t(message.length());
      std::memcpy(pMessage->pData, message.data(), pMessage->msgLength);

      pMessage->state.store(MessageState::Written, std::memory_order_release);
      pMessage->state.notify_all();

      return true;
    }*//*
  };*/
}


valkyrie::Core::AgentMailbox::AgentMailbox(const u32 queueSize, const AgentConcurrency concurrency, Status &status) noexcept
    : pMessageQueue(nullptr), queueSize(queueSize), dynamicInterface(dispatchInterface(concurrency)){
  MappedMemory mainBuffer, overflowBuffer;
  if (auto result = allocateCircularBuffer(queueSize)) {
    std::tie(mainBuffer, overflowBuffer) = result.take();
    status = Code::Success;
  } else
    status = result.status();
  overflowBuffer.release();
  std::tie(pMessageQueue, this->queueSize) = mainBuffer.release();
}

valkyrie::Core::AgentMailbox::AgentMailbox(const AgentConcurrency concurrency, Status &status) noexcept
    : AgentMailbox(AllocationGranularity, concurrency, status){}

valkyrie::Core::AgentMailbox::AgentMailbox(AgentMailbox&& otherMailbox) noexcept
    : pMessageQueue(nullptr),
      queueSize(0),
      nextWriteOffset(otherMailbox.nextWriteOffset.load()),
      nextReadOffset(otherMailbox.nextWriteOffset.load()),
      dynamicInterface(otherMailbox.dynamicInterface){
  std::swap(pMessageQueue, otherMailbox.pMessageQueue);
  std::swap(queueSize, otherMailbox.queueSize);
}

valkyrie::Core::AgentMailbox & valkyrie::Core::AgentMailbox::operator=(AgentMailbox && other) noexcept {
  this->~AgentMailbox();
  new(this) AgentMailbox(std::move(other));
  return *this;
}
valkyrie::Core::AgentMailbox::~AgentMailbox() {
  MappedMemory mem{pMessageQueue, queueSize}; // memory is automatically unmapped/freed in MappedMemory destructor
}




valkyrie::Core::AgentResult<valkyrie::Core::AgentMessage*> valkyrie::Core::AgentMailbox::writeMessageSPSC(FunctionRef<AgentMessage*(void*)> fnCtor, u64 msgSize) noexcept {
  u32 writeOffset = nextWriteOffset.load(std::memory_order_acquire);
  u32 nextOffset;
  AgentMessage* pMessage;

  nextOffset = writeOffset + msgSize;
  if (!hasAvailableSpace(writeOffset, nextOffset))
    return nullptr;
  nextOffset -= queueSize <= nextOffset ? queueSize : 0;

  pMessage = fnCtor(toMessage(writeOffset));
  pMessage->nextOffset = nextOffset;
  nextWriteOffset.store(nextOffset, std::memory_order_release);
  nextWriteOffset.notify_one();
  return pMessage;
}
valkyrie::Core::AgentResult<valkyrie::Core::AgentMessage*> valkyrie::Core::AgentMailbox::writeMessageMPSC(FunctionRef<AgentMessage*(void*)> fnCtor, u64 msgSize) noexcept {
  u32 writeOffset = nextWriteOffset.load();
  u32 nextOffset;
  AgentMessage* pMessage;

  do {
    nextOffset = writeOffset + msgSize;
    if (!hasAvailableSpace(writeOffset, nextOffset))
      return nullptr;
    nextOffset -= queueSize <= nextOffset ? queueSize : 0;
  } while(!nextWriteOffset.compare_exchange_strong(writeOffset, nextOffset, std::memory_order_acq_rel));

  pMessage = fnCtor(toMessage(writeOffset));
  pMessage->nextOffset = nextOffset;
  nextWriteOffset.notify_one();
  return pMessage;
}
valkyrie::Core::AgentResult<valkyrie::Core::AgentMessage*> valkyrie::Core::AgentMailbox::writeMessageSPMC(FunctionRef<AgentMessage*(void*)> fnCtor, u64 msgSize) noexcept {
  u32 writeOffset = nextWriteOffset.load(std::memory_order_acquire);
  u32 nextOffset;
  AgentMessage* pMessage;

  const u32 syncOffset = syncMarker.load(std::memory_order_acquire);

  nextOffset = writeOffset + msgSize;
  if (nextOffset >= (syncOffset + (queueSize * bool(syncOffset < writeOffset))))
    return nullptr;
  nextOffset -= queueSize <= nextOffset ? queueSize : 0;

  pMessage = fnCtor(toMessage(writeOffset));
  pMessage->nextOffset = nextOffset;
  nextWriteOffset.store(nextOffset, std::memory_order_release);
  nextWriteOffset.notify_one();
  return pMessage;
}
valkyrie::Core::AgentResult<valkyrie::Core::AgentMessage*> valkyrie::Core::AgentMailbox::writeMessageSPMCNonCoherent(FunctionRef<AgentMessage*(void*)> fnCtor, u64 msgSize) noexcept {
  u32 writeOffset = nextWriteOffset.load();
  u32 firstReadOffset = syncMarker.load();
  u32 nextOffset;
  AgentMessage* pMessage;

  nextOffset = writeOffset + msgSize;
  if (nextOffset >= (firstReadOffset + (queueSize * bool(firstReadOffset < writeOffset))))
    return nullptr;
  nextOffset -= queueSize <= nextOffset ? queueSize : 0;

  nextWriteOffset.store(nextOffset, std::memory_order_release);

  pMessage = fnCtor(toMessage(writeOffset));
  pMessage->nextOffset = nextOffset;
  nextWriteOffset.notify_one();
  return pMessage;
}
valkyrie::Core::AgentResult<valkyrie::Core::AgentMessage*> valkyrie::Core::AgentMailbox::writeMessageMPMC(FunctionRef<AgentMessage*(void*)> fnCtor, u64 msgSize) noexcept {
  u32 writeOffset = nextWriteOffset.load();
  u32 firstReadOffset = syncMarker.load();
  u32 nextOffset;
  AgentMessage* pMessage;

  do {
    nextOffset = writeOffset + msgSize;
    if (nextOffset >= (firstReadOffset + (queueSize * bool(firstReadOffset < writeOffset))))
      return nullptr;
    nextOffset -= queueSize <= nextOffset ? queueSize : 0;
  } while(!nextWriteOffset.compare_exchange_strong(writeOffset, nextOffset, std::memory_order_acq_rel));

  pMessage = fnCtor(toMessage(writeOffset));
  pMessage->nextOffset = nextOffset;
  nextWriteOffset.notify_all();
  return pMessage;
}

valkyrie::Core::AgentStatus                valkyrie::Core::AgentMailbox::readMessageSC (FunctionRef<AgentStatus(AgentMessage&)> msgProc) noexcept {
  u32 readOffset = nextReadOffset.load();
  u32 nextOffset;
  AgentMessage* pMessage;

  waitForMessage(readOffset);

  pMessage   = toMessage(readOffset);
  nextOffset = pMessage->nextOffset.load(std::memory_order_acquire);

  VK_assert(readOffset != nextOffset);

  AgentStatus status = msgProc(*pMessage);

  pMessage->~AgentMessage();

  nextReadOffset.store(nextOffset, std::memory_order_release);

  return status;
}
valkyrie::Core::AgentStatus                valkyrie::Core::AgentMailbox::readMessageMC (FunctionRef<AgentStatus(AgentMessage&)> msgProc) noexcept {

  //FIXME: I believe that the synchronization method here is broken... not totally sure what to do though.

  u32 syncOffset = syncMarker.load(std::memory_order_acq_rel);
  u32 readOffset = nextReadOffset.load();
  u32 nextOffset = 0;
  AgentMessage* pMessage;

  waitForMessage(readOffset);

  pMessage   = toMessage(readOffset);
  nextOffset = pMessage->nextOffset.load(std::memory_order_acquire);
  readOffset = syncOffset;

  // Blocking...
  if (!nextReadOffset.compare_exchange_strong(readOffset, nextOffset, std::memory_order_acq_rel)) {
    syncMarker.wait(syncOffset, std::memory_order_release);
    waitForMessage(readOffset);
    readOffset = nextReadOffset.load(std::memory_order_acquire);
    VK_assert(syncMarker.load() == readOffset);
    pMessage   = toMessage(readOffset);
    nextOffset = pMessage->nextOffset.load(std::memory_order_acquire);
    const bool acquireResult = nextReadOffset.compare_exchange_strong(readOffset, nextOffset, std::memory_order_acq_rel);
    VK_assert(acquireResult);
  }

  VK_assert(readOffset != nextOffset);

  AgentStatus status = msgProc(*pMessage);

  pMessage->~AgentMessage();

  const bool syncUpdateResult = syncMarker.compare_exchange_strong(readOffset, nextOffset, std::memory_order_acq_rel);
  VK_assert(syncUpdateResult);
  syncMarker.notify_one();

  return status;
}
valkyrie::Core::AgentStatus                valkyrie::Core::AgentMailbox::readMessageNonCoherent (FunctionRef<AgentStatus(AgentMessage&)> msgProc) noexcept {
  u32 readOffset;
  u32 nextOffset = 0;
  AgentMessage* pMessage;

  readOffset = static_cast<u32>(-1);

  while (!nextReadOffset.compare_exchange_strong(readOffset, nextOffset, std::memory_order_acq_rel)) {
    waitForMessage(readOffset);
    pMessage   = toMessage(readOffset);
    nextOffset = pMessage->nextOffset.load(std::memory_order_acquire);
  }

  VK_assert(readOffset != nextOffset);

  AgentStatus status = msgProc(*pMessage);

  // Tag the message as having been condemned.
  pMessage->condemn();

  // FIXME: A simple comparison should work here...
  if (syncMarker.compare_exchange_strong(readOffset, UniqueLock, std::memory_order_acq_rel)) {

    u32 exileResult       = nextOffset;
    u32 writeOffset;
    u32 nextMessageOffset = nextOffset;

    do {
      nextMessageOffset = exileResult;
      writeOffset       = nextWriteOffset.load(std::memory_order_acquire);
      exileResult       = pMessage->exile(writeOffset);
      pMessage          = toMessage(nextMessageOffset);
    } while (exileResult != writeOffset);

    exileResult = UniqueLock;
    const bool unlockResult = syncMarker.compare_exchange_strong(exileResult, nextMessageOffset, std::memory_order_acq_rel);
    VK_assert(unlockResult);
  }

  return status;
}