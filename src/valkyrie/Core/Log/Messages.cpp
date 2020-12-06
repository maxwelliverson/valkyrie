//
// Created by Maxwell on 2020-11-17.
//
#include <valkyrie/Core/IO/Messages.hpp>

#define _CRTDBG_MAP_ALLOC
#define NOMINMAX

#include <valkyrie/Core/Async/SharedResource.hpp>
#include <windows.h>
#include <crtdbg.h>
#include <synchapi.h>
#include <memoryapi.h>

#include <atomic>

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
  inline constexpr size_t PageAlign = PageSize;

  inline constexpr size_t ClobberGuardSize = PageSize;

  constexpr inline size_t alignSize(size_t from, size_t align) noexcept {
    return from + ((~((size_t)bool(from & (align - 1)) - 1)) & ((from ^ (align - 1)) & (align - 1)));
  }


  Core::SystemStatus   allocateMessageBufferWithGuards(u32& bufferSize, _Outptr_opt_ Core::Message** __restrict ppMessage) noexcept {

    using valkyrie::byte;
    using PFN_VirtualAlloc2 = void*(*)(HANDLE                 Process,
                                       PVOID                  BaseAddress,
                                       SIZE_T                 Size,
                                       ULONG                  AllocationType,
                                       ULONG                  PageProtection,
                                       MEM_EXTENDED_PARAMETER *ExtendedParameters,
                                       ULONG                  ParameterCount
    );
    using PFN_MapViewOfFile3 = void*(*)(HANDLE                 FileMapping,
                                        HANDLE                 Process,
                                        PVOID                  BaseAddress,
                                        ULONG64                Offset,
                                        SIZE_T                 ViewSize,
                                        ULONG                  AllocationType,
                                        ULONG                  PageProtection,
                                        MEM_EXTENDED_PARAMETER *ExtendedParameters,
                                        ULONG                  ParameterCount
    );

    PFN_VirtualAlloc2 pVirtualAlloc2;
    PFN_MapViewOfFile3 pMapViewOfFile3;
    HMODULE kernelDll = LoadLibrary("KernelBase.dll");
    pVirtualAlloc2 = (PFN_VirtualAlloc2)GetProcAddress(kernelDll, "VirtualAlloc2");
    pMapViewOfFile3 = (PFN_MapViewOfFile3)GetProcAddress(kernelDll, "MapViewOfFile3");

    byte* pAllocation;
    byte* pPlaceholderOne;
    byte* pPlaceholderTwo;
    byte* pViewOne;
    byte* pViewTwo;
    HANDLE hMappedFile;
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    bufferSize = alignSize(bufferSize, sysInfo.dwAllocationGranularity);
    const u32 guardSize = alignSize(sysInfo.dwPageSize, sysInfo.dwAllocationGranularity);


    const size_t allocationSize = (guardSize + bufferSize) * 2;

    pAllocation = (byte*)pVirtualAlloc2(nullptr,
                                       nullptr,
                                       allocationSize,
                                       MEM_RESERVE | MEM_RESERVE_PLACEHOLDER,
                                       PAGE_NOACCESS,
                                       nullptr,
                                       0);
    if (!pAllocation)
      return Core::System::Win32::getLastError();

    if (!VirtualFree(pAllocation, allocationSize, MEM_RELEASE | MEM_PRESERVE_PLACEHOLDER))
      return Core::System::Win32::getLastError();

    pPlaceholderOne = pAllocation + guardSize;
    pPlaceholderTwo = pPlaceholderOne + bufferSize;

    hMappedFile = CreateFileMapping(INVALID_HANDLE_VALUE,
                                    nullptr,
                                    PAGE_READWRITE,
                                    0,
                                    (DWORD)bufferSize,
                                    nullptr);
    if (!hMappedFile) {
      Core::SystemStatus result = Core::System::Win32::getLastError();
      VirtualFree(pAllocation, 0, MEM_RELEASE);
      VirtualFree(pPlaceholderTwo, 0, MEM_RELEASE);
      return std::move(result);
    }

    pViewOne = (byte*)pMapViewOfFile3(
        hMappedFile,
        nullptr,
        pPlaceholderOne,
        0,
        bufferSize,
        MEM_REPLACE_PLACEHOLDER,
        PAGE_READWRITE,
        nullptr,
        0);

    if (!pViewOne) {
      Core::SystemStatus result = Core::System::Win32::getLastError();
      CloseHandle(hMappedFile);
      VirtualFree(pAllocation, 0, MEM_RELEASE);
      VirtualFree(pPlaceholderTwo, 0, MEM_RELEASE);
      return std::move(result);
    }
    pPlaceholderOne = pViewOne;

    pViewTwo = (byte*)pMapViewOfFile3(
        hMappedFile,
        nullptr,
        pPlaceholderTwo,
        0,
        bufferSize,
        MEM_REPLACE_PLACEHOLDER,
        PAGE_READWRITE,
        nullptr,
        0
    );

    if (!pViewTwo) {
      Core::SystemStatus result = Core::System::Win32::getLastError();
      CloseHandle(hMappedFile);
      VirtualFree(pAllocation, 0, MEM_RELEASE);
      VirtualFree(pPlaceholderTwo, 0, MEM_RELEASE);
      return std::move(result);
    }

    *ppMessage = std::bit_cast<Core::Message*>(pViewOne);
    CloseHandle(hMappedFile);
    FreeLibrary(kernelDll);
    return Core::SystemStatus(NO_ERROR);
  }


  Core::SystemStatus   allocateMessageBuffer(u32& bufferSize, bool hasGuards, _Outptr_opt_ Core::Message** __restrict ppMessage) noexcept {

    if (hasGuards)
      return allocateMessageBufferWithGuards(bufferSize, ppMessage);

    using valkyrie::byte;

    using PFN_VirtualAlloc2 = void*(*)(HANDLE                 Process,
                                       PVOID                  BaseAddress,
                                       SIZE_T                 Size,
                                       ULONG                  AllocationType,
                                       ULONG                  PageProtection,
                                       MEM_EXTENDED_PARAMETER *ExtendedParameters,
                                       ULONG                  ParameterCount
    );
    using PFN_MapViewOfFile3 = void*(*)(HANDLE                 FileMapping,
                                        HANDLE                 Process,
                                        PVOID                  BaseAddress,
                                        ULONG64                Offset,
                                        SIZE_T                 ViewSize,
                                        ULONG                  AllocationType,
                                        ULONG                  PageProtection,
                                        MEM_EXTENDED_PARAMETER *ExtendedParameters,
                                        ULONG                  ParameterCount
    );

    PFN_VirtualAlloc2 pVirtualAlloc2;
    PFN_MapViewOfFile3 pMapViewOfFile3;
    HMODULE kernelDll = LoadLibrary("KernelBase.dll");
    pVirtualAlloc2 = (PFN_VirtualAlloc2)GetProcAddress(kernelDll, "VirtualAlloc2");
    pMapViewOfFile3 = (PFN_MapViewOfFile3)GetProcAddress(kernelDll, "MapViewOfFile3");

    byte* pPlaceholderOne;
    byte* pPlaceholderTwo;
    byte* pViewOne;
    byte* pViewTwo;
    HANDLE hMappedFile;
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    bufferSize = alignSize(bufferSize, sysInfo.dwAllocationGranularity);


    const size_t allocationSize = bufferSize * 2;

    pPlaceholderOne = (byte*)pVirtualAlloc2(nullptr,
                                    nullptr,
                                    allocationSize,
                                    MEM_RESERVE | MEM_RESERVE_PLACEHOLDER,
                                    PAGE_NOACCESS,
                                    nullptr,
                                    0);
    if (!pPlaceholderOne)
      return Core::System::Win32::getLastError();

    if (!VirtualFree(pPlaceholderOne, allocationSize, MEM_RELEASE | MEM_PRESERVE_PLACEHOLDER))
      return Core::System::Win32::getLastError();

    pPlaceholderTwo = pPlaceholderOne + bufferSize;

    hMappedFile = CreateFileMapping(INVALID_HANDLE_VALUE,
                                    nullptr,
                                    PAGE_READWRITE,
                                    0,
                                    (DWORD)bufferSize,
                                    nullptr);
    if (!hMappedFile) {
      Core::SystemStatus result = Core::System::Win32::getLastError();
      VirtualFree(pPlaceholderOne, 0, MEM_RELEASE);
      VirtualFree(pPlaceholderTwo, 0, MEM_RELEASE);
      return std::move(result);
    }

    pViewOne = (byte*)pMapViewOfFile3(
        hMappedFile,
        nullptr,
        pPlaceholderOne,
        0,
        bufferSize,
        MEM_REPLACE_PLACEHOLDER,
        PAGE_READWRITE,
        nullptr,
        0);

    if (!pViewOne) {
      Core::SystemStatus result = Core::System::Win32::getLastError();
      CloseHandle(hMappedFile);
      VirtualFree(pPlaceholderOne, 0, MEM_RELEASE);
      VirtualFree(pPlaceholderTwo, 0, MEM_RELEASE);
      return std::move(result);
    }
    pPlaceholderOne = pViewOne;

    pViewTwo = (byte*)pMapViewOfFile3(
      hMappedFile,
      nullptr,
      pPlaceholderTwo,
      0,
      bufferSize,
      MEM_REPLACE_PLACEHOLDER,
      PAGE_READWRITE,
      nullptr,
      0
    );

    if (!pViewTwo) {
      Core::SystemStatus result = Core::System::Win32::getLastError();
      CloseHandle(hMappedFile);
      VirtualFree(pPlaceholderOne, 0, MEM_RELEASE);
      VirtualFree(pPlaceholderTwo, 0, MEM_RELEASE);
      return std::move(result);
    }

    *ppMessage = std::bit_cast<Core::Message*>(pViewOne);
    CloseHandle(hMappedFile);
    FreeLibrary(kernelDll);
    return Core::SystemStatus(NO_ERROR);
  }
  Core::SystemStatus   deallocateMessageBufferWithGuards(Core::Message* __restrict pMessage, size_t bufferSize) noexcept{
    return Core::SystemStatus{ERROR_CALL_NOT_IMPLEMENTED};
  }
  Core::SystemStatus   deallocateMessageBuffer(Core::Message* __restrict pMessage, size_t bufferSize, bool hasGuards) noexcept {
    if (hasGuards)
      return deallocateMessageBufferWithGuards(pMessage, bufferSize);
    if (!UnmapViewOfFile(pMessage)) {
      auto result = Core::System::Win32::getLastError();
      UnmapViewOfFile(pMessage + bufferSize);
      return std::move(result);
    }
    UnmapViewOfFile(pMessage + bufferSize);
    return Core::System::Win32::getLastError();
  }
}

class Core::MessageQueue{

  struct ProducerMemo{
    MessageProducer* address = nullptr;
    std::string      name{};
  };
  struct ConsumerMemo{
    MessageConsumer* address = nullptr;
    std::string      name{};
  };

public:
  Message* pQueue;

  u32 queueEndOffset;
  u32 guardSize;
  u32 nextWriteOffset = 0;
  u32 nextReadOffset = 0;

  u32 directInstances = 0;

  u32 producerId = 0;
  u32 consumerId = 0;

  //LockState producerLock = IsUnlocked;
  //LockState consumerLock = IsUnlocked;

  FlatMap<u32, MessageProducer*> producers{};
  FlatMap<u32, MessageConsumer*> consumers{};

  void(* pDeleter)(MessageQueue* pThis, void* pUserData) = [](MessageQueue* pThis, void*){ delete pThis; };
  void* pUserData = nullptr;

  MessageQueue(u32 bufferSize, u32 guardSize, void(*pDeleter)(MessageQueue*, void*), void* pUserData, Core::SystemStatus& status)
      : pQueue(),
        queueEndOffset(bufferSize),
        guardSize(guardSize),
        pDeleter(pDeleter),
        pUserData(pUserData){
    status = allocateMessageBuffer(queueEndOffset, this->guardSize, &pQueue);
  }

  ~MessageQueue() = default;

  Core::SystemStatus destroy() noexcept {
    Core::SystemStatus result{NO_ERROR};
    auto deleter  = pDeleter;
    auto userData = pUserData;
    if (pQueue) {
      result = deallocateMessageBuffer(pQueue, queueEndOffset, guardSize);
    }
    deleter(this, userData);
    return std::move(result);
  }
};

namespace {


  inline bool producerLock(Core::MessageQueue* pQueue) noexcept {
    VK_assert(pQueue != nullptr);
    //LockStateEnum comparer = IsUnlocked;
    //WaitOnAddress(&pQueue->producerLock, &comparer, sizeof(LockStateEnum), );
  }

  inline void removeProducer(Core::MessageQueue* pQueue, Core::MessageProducer* pProducer) noexcept {
    if (!(pQueue || pProducer)) return;


  }
}

Core::MessageChannel::MessageChannel() : MessageChannel(2 * PageSize){}
Core::MessageChannel::MessageChannel(u32 initialCapacity) : pQueue(){}

Core::MessageConsumer Core::MessageChannel::makeConsumer() noexcept {
  return pQueue;
}
Core::MessageProducer Core::MessageChannel::makeProducer() noexcept {
  return pQueue;
}






Core::MessageProducer::MessageProducer(MessageQueue *pQueue) noexcept : pQueue(pQueue), producerId(pQueue->producerId++){
  this->pQueue->producers.emplace(producerId, this);
}
/*Core::MessageProducer::MessageProducer(const MessageProducer& other) : pQueue(other.pQueue), producerId(pQueue->producerId++){
  if (pQueue)
    pQueue->producers[producerId] = this;
}*/
Core::MessageProducer::MessageProducer(MessageProducer&& other) noexcept : pQueue(other.pQueue), producerId(other.producerId) {
  if (pQueue)
    pQueue->producers[producerId] = this;
  other.pQueue = nullptr;
}
/*Core::MessageProducer & Core::MessageProducer::operator=(const MessageProducer &other) {
  if (this == &other || other.pQueue == pQueue) return *this;

  return *this;
}*/
Core::MessageProducer & Core::MessageProducer::operator=(MessageProducer &&other) noexcept {

  return *this;
}
Core::MessageProducer::~MessageProducer() {
  if (pQueue)
    pQueue->producers.erase(producerId);
}

Core::MessageConsumer::MessageConsumer(MessageQueue *pQueue) noexcept : pQueue(pQueue), nextMessageOffset(pQueue->nextReadOffset), consumerId(pQueue->consumerId++){
  this->pQueue->consumers.emplace(consumerId, this);
}
/*Core::MessageConsumer::MessageConsumer(const MessageConsumer& other) {}*/
Core::MessageConsumer::MessageConsumer(MessageConsumer&& other) noexcept {

  //return *this;
}

/*Core::MessageConsumer& Core::MessageConsumer::operator=(const MessageConsumer& other) {}*/
Core::MessageConsumer& Core::MessageConsumer::operator=(MessageConsumer&& other) noexcept {

  return *this;
}
Core::MessageConsumer::~MessageConsumer() {
  if (pQueue)
    pQueue->consumers.erase(consumerId);
}