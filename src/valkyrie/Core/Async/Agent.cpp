//
// Created by Maxwell on 2020-12-14.
//

#include <valkyrie/Core/Async/Agent.hpp>

#include "../Win32Functions.hpp"

#define NOMINMAX

#include <windows.h>
#include <synchapi.h>
#include <memoryapi.h>
#include <psapi.h>
#include <dbghelp.h>

#include <atomic>
#include <deque>
#include <sstream>

using namespace valkyrie;

#ifndef VALKYRIE_KERNEL_LIBRARY
#define VALKYRIE_KERNEL_LIBRARY "KernelBase.dll"
#endif
#ifndef VALKYRIE_DEBUG_HELP_LIBRARY
#define VALKYRIE_DEBUG_HELP_LIBRARY "DbgHelp.dll"
#endif

namespace {

  inline constexpr size_t PageSize              = 4096;
  inline constexpr size_t AllocationGranularity = VK_if(VK_system_windows(0x1 << 16)VK_else(PageSize));
  inline constexpr size_t PageAlign             = PageSize;

  inline constexpr size_t ClobberGuardSize      = PageSize;

  constexpr inline size_t alignSize(size_t from, size_t align) noexcept {
    return from + ((~((size_t) bool(from & (align - 1)) - 1)) & ((from ^ (align - 1)) & (align - 1)));
  }

  inline auto getLastError() noexcept {
    return Core::System::Win32::getLastError();
  }

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
    template<typename... Args> requires(ConstructibleFrom<T, Args...>)
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
      if (address_ && !kernelApi.VirtualFree(address_, 0, MEM_RELEASE))
        panic(getLastError());
    }

    inline u64 size() const noexcept {
      return size_;
    }
    inline void *address() const noexcept {
      return address_;
    }
    inline Win32Result<VirtualPlaceholder> takeFront(u64 bytes) noexcept {
      if (!kernelApi.VirtualFree(address_, bytes, MEM_RELEASE | MEM_PRESERVE_PLACEHOLDER))
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
      if (fileHandle && !kernelApi.CloseHandle(fileHandle))
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
        if (!kernelApi.UnmapViewOfFile(address_))
          return getLastError();
        address_ = nullptr;
        size_ = 0;
      }
      return {};
    }
  };


  inline Win32Result<VirtualPlaceholder>                    allocatePlaceholder(u64 placeholderSize) noexcept {
    auto result = kernelApi.VirtualAlloc2(
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
  inline Win32Result<MemoryFile>                            createFileInMemory(u64 fileSize) noexcept {
    auto result = kernelApi.CreateFileMapping(
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
  inline Win32Result<MappedMemory>                          mapPlaceholder(MemoryFile &file, VirtualPlaceholder &vmLocation) noexcept {
    const u64 size = vmLocation.size();
    void *tmp = kernelApi.MapViewOfFile3(
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
      placeholder.second = result.take();
    else
      return result.status();

    if (auto result = placeholder.second.takeFront(length))
      placeholder.first = result.take();
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
    /*if (!kernelApi.UnmapViewOfFile(pView))
      return getLastError();
    return {};*/
    kernelApi.UnmapViewOfFile(pView);
    return getLastError();
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
  /*enum class Priority{
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
  };*/

  inline constexpr u32              UniqueLock = u32(-1);

  inline constexpr Core::AgentConcurrency ConcurrencySPSC{ 0 };
  inline constexpr Core::AgentConcurrency ConcurrencyMPSC{ Core::AgentConcurrency::MultiProducer };
  inline constexpr Core::AgentConcurrency ConcurrencySPMC{ Core::AgentConcurrency::MultiConsumer };
  inline constexpr Core::AgentConcurrency ConcurrencyMPMC{ (u32)Core::AgentConcurrency::MultiProducer | (u32)Core::AgentConcurrency::MultiConsumer };
  inline constexpr Core::AgentConcurrency ConcurrencySPMCNonCoherent{ Core::AgentConcurrency::OutOfOrderConsumption };
  inline constexpr Core::AgentConcurrency ConcurrencyMPMCNonCoherent{ (u32)Core::AgentConcurrency::MultiProducer | (u32)Core::AgentConcurrency::OutOfOrderConsumption };
}

Core::AgentMailbox::Interface Core::AgentMailbox::dispatchInterface(Core::AgentConcurrency concurrency) noexcept {
  switch (concurrency) {
    case ConcurrencySPSC:
      return { &Core::AgentMailbox::writeMessageSPSC, &AgentMailbox::readMessageSC, &AgentMailbox::tryReadMessageSC };
    case ConcurrencyMPSC:
      return { &AgentMailbox::writeMessageMPSC, &AgentMailbox::readMessageSC, &AgentMailbox::tryReadMessageSC };
    case ConcurrencySPMC:
      return { &AgentMailbox::writeMessageSPMC, &AgentMailbox::readMessageMC, &AgentMailbox::tryReadMessageMC };
    case ConcurrencyMPMC:
      return { &AgentMailbox::writeMessageMPMC, &AgentMailbox::readMessageMC, &AgentMailbox::tryReadMessageMC };
    case ConcurrencySPMCNonCoherent:
      return { &AgentMailbox::writeMessageSPMCNonCoherent, &AgentMailbox::readMessageNonCoherent, &AgentMailbox::tryReadMessageNonCoherent };
    case ConcurrencyMPMCNonCoherent:
      return { &AgentMailbox::writeMessageMPMC, &AgentMailbox::readMessageNonCoherent, &AgentMailbox::tryReadMessageNonCoherent };
    VK_no_default;
  }
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
  //MappedMemory mem{pMessageQueue, queueSize}; // memory is automatically unmapped/freed in MappedMemory destructor
    SystemStatus results[2]{ unmapView(pMessageQueue), unmapView((byte*)pMessageQueue + queueSize) };
    if (results[0].failure() || results[1].failure()) [[unlikely]] {
      std::basic_stringstream<utf8> errMsg;
      errMsg << "\n\n\tIn Function: valkyrie::Core::AgentMailbox::~AgentMailbox()";
      errMsg << "\n\tUnmapViewOfFile(pMessageQueue) => " << results[0].message().c_str();
      errMsg << "\n\tUnmapViewOfFile(pMessageQueue + queueSize) => " << results[1].message().c_str();
      errMsg << "\n\tValues:\n\t\tpMessageQueue = " << pMessageQueue << "\n\t\tqueueSize = " << queueSize << "\n\n";
      panic(errMsg.str().c_str());
    }
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

  nextWriteOffset.wait(readOffset);

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

  nextWriteOffset.wait(readOffset);

  pMessage   = toMessage(readOffset);
  nextOffset = pMessage->nextOffset.load(std::memory_order_acquire);
  readOffset = syncOffset;

  // Blocking...
  if (!nextReadOffset.compare_exchange_strong(readOffset, nextOffset, std::memory_order_acq_rel)) {
    syncMarker.wait(syncOffset, std::memory_order_release);
    nextWriteOffset.wait(readOffset);
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
    nextWriteOffset.wait(readOffset);
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

valkyrie::Core::AgentStatus                valkyrie::Core::AgentMailbox::tryReadMessageSC (FunctionRef<AgentStatus(AgentMessage&)> msgProc) noexcept {
  u32 readOffset = nextReadOffset.load();
  u32 nextOffset;
  AgentMessage* pMessage;

  if (nextWriteOffset.load(std::memory_order_release) == readOffset)
    return StatusCode<AgentStatusCodeDomain>(AgentCode::MailboxIsEmpty);

  pMessage   = toMessage(readOffset);
  nextOffset = pMessage->nextOffset.load(std::memory_order_acquire);

  VK_assert(readOffset != nextOffset);

  AgentStatus status = msgProc(*pMessage);

  pMessage->~AgentMessage();

  nextReadOffset.store(nextOffset, std::memory_order_release);

  return status;
}
valkyrie::Core::AgentStatus                valkyrie::Core::AgentMailbox::tryReadMessageMC (FunctionRef<AgentStatus(AgentMessage&)> msgProc) noexcept {
  u32 syncOffset = syncMarker.load(std::memory_order_acq_rel);
  u32 readOffset = nextReadOffset.load();
  u32 nextOffset = 0;
  AgentMessage* pMessage;

  if (nextWriteOffset.load(std::memory_order_release) == readOffset)
    return StatusCode<AgentStatusCodeDomain>(AgentCode::MailboxIsEmpty);

  pMessage   = toMessage(readOffset);
  nextOffset = pMessage->nextOffset.load(std::memory_order_acquire);
  readOffset = syncOffset;

  // Blocking...
  if (!nextReadOffset.compare_exchange_strong(readOffset, nextOffset, std::memory_order_acq_rel)) {
    syncMarker.wait(syncOffset, std::memory_order_release);
    if (nextWriteOffset.load(std::memory_order_release) == readOffset)
      return StatusCode<AgentStatusCodeDomain>(AgentCode::MailboxIsEmpty);
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
valkyrie::Core::AgentStatus                valkyrie::Core::AgentMailbox::tryReadMessageNonCoherent(FunctionRef<AgentStatus(AgentMessage &)> msgProc) noexcept {
  u32 readOffset;
  u32 nextOffset = 0;
  AgentMessage* pMessage;

  readOffset = static_cast<u32>(-1);

  while (!nextReadOffset.compare_exchange_strong(readOffset, nextOffset, std::memory_order_acq_rel)) {
    if (nextWriteOffset.load(std::memory_order_release) == readOffset)
      return StatusCode<AgentStatusCodeDomain>(AgentCode::MailboxIsEmpty);
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