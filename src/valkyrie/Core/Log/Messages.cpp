//
// Created by Maxwell on 2020-11-17.
//
#include <valkyrie/Core/IO/Messages.hpp>

//#define _CRTDBG_MAP_ALLOC
#define NOMINMAX

#include <valkyrie/Core/Async/SharedResource.hpp>
#include <windows.h>
//#include <crtdbg.h>
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

  using PFN_VirtualAlloc       = decltype(&VirtualAlloc);
  using PFN_VirtualAlloc2      = decltype(&VirtualAlloc2);
  using PFN_VirtualAllocEx     = decltype(&VirtualAllocEx);
  using PFN_MapViewOfFile      = decltype(&MapViewOfFile);
  using PFN_MapViewOfFile2     = decltype(&MapViewOfFile2);
  using PFN_MapViewOfFile3     = decltype(&MapViewOfFile3);
  using PFN_VirtualFree        = decltype(&VirtualFree);
  using PFN_VirtualFreeEx      = decltype(&VirtualFreeEx);
  using PFN_VirtualQuery       = decltype(&VirtualQuery);
  using PFN_VirtualQueryEx     = decltype(&VirtualQueryEx);
  using PFN_VirtualLock        = decltype(&VirtualLock);
  using PFN_VirtualUnlock      = decltype(&VirtualUnlock);
  using PFN_VirtualProtect     = decltype(&VirtualProtect);
  using PFN_VirtualProtectEx   = decltype(&VirtualProtectEx);
  using PFN_UnmapViewOfFile    = decltype(&UnmapViewOfFile);
  using PFN_UnmapViewOfFileEx  = decltype(&UnmapViewOfFileEx);
  using PFN_UnmapViewOfFile2   = decltype(&UnmapViewOfFile2);
  using PFN_CreateFileMapping  = decltype(&CreateFileMapping);
  using PFN_CreateFileMapping2 = decltype(&CreateFileMapping2);
  using PFN_CloseHandle        = decltype(&CloseHandle);

  struct VirtualMemoryApi{

    HMODULE                KernelLib;

    PFN_VirtualAlloc       VirtualAlloc;
    PFN_VirtualAlloc2      VirtualAlloc2;
    PFN_VirtualAllocEx     VirtualAllocEx;
    PFN_MapViewOfFile      MapViewOfFile;
    PFN_MapViewOfFile2     MapViewOfFile2;
    PFN_MapViewOfFile3     MapViewOfFile3;
    PFN_VirtualFree        VirtualFree;
    PFN_VirtualFreeEx      VirtualFreeEx;
    PFN_VirtualQuery       VirtualQuery;
    PFN_VirtualQueryEx     VirtualQueryEx;
    PFN_VirtualLock        VirtualLock;
    PFN_VirtualUnlock      VirtualUnlock;
    PFN_VirtualProtect     VirtualProtect;
    PFN_VirtualProtectEx   VirtualProtectEx;
    PFN_UnmapViewOfFile    UnmapViewOfFile;
    PFN_UnmapViewOfFileEx  UnmapViewOfFileEx;
    PFN_UnmapViewOfFile2   UnmapViewOfFile2;
    PFN_CreateFileMapping  CreateFileMapping;
    PFN_CreateFileMapping2 CreateFileMapping2;
    PFN_CloseHandle        CloseHandle;

    explicit VirtualMemoryApi(const char* pLibraryName = "KernelBase.dll") noexcept
        : KernelLib(LoadLibrary(pLibraryName)){
      if (!KernelLib)
        panic(Core::System::Win32::getLastError());

      VirtualAlloc       = (PFN_VirtualAlloc)      GetProcAddress(KernelLib, "VirtualAlloc");
      VirtualAlloc2      = (PFN_VirtualAlloc2)     GetProcAddress(KernelLib, "VirtualAlloc2");
      VirtualAllocEx     = (PFN_VirtualAllocEx)    GetProcAddress(KernelLib, "VirtualAllocEx");
      MapViewOfFile      = (PFN_MapViewOfFile)     GetProcAddress(KernelLib, "MapViewOfFile");
      MapViewOfFile2     = (PFN_MapViewOfFile2)    GetProcAddress(KernelLib, "MapViewOfFile2");
      MapViewOfFile3     = (PFN_MapViewOfFile3)    GetProcAddress(KernelLib, "MapViewOfFile3");
      VirtualFree        = (PFN_VirtualFree)       GetProcAddress(KernelLib, "VirtualFree");
      VirtualFreeEx      = (PFN_VirtualFreeEx)     GetProcAddress(KernelLib, "VirtualFreeEx");
      VirtualQuery       = (PFN_VirtualQuery)      GetProcAddress(KernelLib, "VirtualQuery");
      VirtualQueryEx     = (PFN_VirtualQueryEx)    GetProcAddress(KernelLib, "VirtualQueryEx");
      VirtualLock        = (PFN_VirtualLock)       GetProcAddress(KernelLib, "VirtualLock");
      VirtualUnlock      = (PFN_VirtualUnlock)     GetProcAddress(KernelLib, "VirtualUnlock");
      VirtualProtect     = (PFN_VirtualProtect)    GetProcAddress(KernelLib, "VirtualProtect");
      VirtualProtectEx   = (PFN_VirtualProtectEx)  GetProcAddress(KernelLib, "VirtualProtectEx");
      UnmapViewOfFile    = (PFN_UnmapViewOfFile)   GetProcAddress(KernelLib, "UnmapViewOfFile");
      UnmapViewOfFileEx  = (PFN_UnmapViewOfFileEx) GetProcAddress(KernelLib, "UnmapViewOfFileEx");
      UnmapViewOfFile2   = (PFN_UnmapViewOfFile2)  GetProcAddress(KernelLib, "UnmapViewOfFile2");
      CreateFileMapping  = (PFN_CreateFileMapping) GetProcAddress(KernelLib, "CreateFileMapping");
      CreateFileMapping2 = (PFN_CreateFileMapping2)GetProcAddress(KernelLib, "CreateFileMapping2");
      CloseHandle        = (PFN_CloseHandle)       GetProcAddress(KernelLib, "CloseHandle");
    }
    ~VirtualMemoryApi(){
      if (!FreeLibrary(KernelLib))
        panic(Core::System::Win32::getLastError());
    }
  };
  inline const VirtualMemoryApi memoryApi{};

  inline auto getLastError() noexcept{
    return Core::System::Win32::getLastError();
  }

  using SystemDomain = VK_if(VK_debug_build(Core::StatusValidationDomain<Core::System::Win32StatusDomain>)VK_else(Core::System::Win32StatusDomain));

  using SystemStatus = Core::StatusCode<SystemDomain>;


  template <typename T>
  using Win32Maybe = Core::Maybe<T, SystemDomain>;

  template <typename T>
  class Win32Result : public Win32Maybe<T>{
  public:

    Win32Result() : Win32Maybe<T>(getLastError()){}
    Win32Result(const T& value) : Win32Maybe<T>(value){}
    Win32Result(T&& value) noexcept : Win32Maybe<T>(std::move(value)){}
    Win32Result(const SystemStatus& stat) : Win32Maybe<T>(stat){}
    Win32Result(SystemStatus&& stat) : Win32Maybe<T>(std::move(stat)){}
    template <typename ...Args> requires(std::constructible_from<T, Args...>)
    Win32Result(std::in_place_t, Args&& ...args) : Win32Maybe<T>(std::forward<Args>(args)...){}
  };

  class VirtualPlaceholder{
    valkyrie::byte* address_ = nullptr;
    u64             size_    = 0;
  public:
    VirtualPlaceholder() = default;
    VirtualPlaceholder(const VirtualPlaceholder&) = delete;
    VirtualPlaceholder(VirtualPlaceholder&& other) noexcept{
      std::swap(address_, other.address_);
      std::swap(size_, other.size_);
    }
    VirtualPlaceholder& operator=(const VirtualPlaceholder&) = delete;
    VirtualPlaceholder& operator=(VirtualPlaceholder&& other) noexcept {
      this->~VirtualPlaceholder();
      new(this) VirtualPlaceholder(std::move(other));
      return *this;
    }


    VirtualPlaceholder(void* address, u64 size) noexcept : address_((valkyrie::byte*)address), size_(size){}
    ~VirtualPlaceholder(){
      if (address_ && !memoryApi.VirtualFree(address_, 0, MEM_RELEASE))
        panic(getLastError());
    }

    inline u64             size() const noexcept {
      return size_;
    }
    inline void*           address() const noexcept {
      return address_;
    }
    inline Win32Result<VirtualPlaceholder> takeFront(u64 bytes) noexcept {
      if (!memoryApi.VirtualFree(address_, bytes, MEM_RELEASE | MEM_PRESERVE_PLACEHOLDER))
        return {};
      auto* const oldAddress = address_;
      address_ += bytes;
      size_    -= bytes;
      return VirtualPlaceholder(oldAddress, bytes);
    }
    inline valkyrie::byte* release() noexcept{
      auto tmp = address_;
      address_ = nullptr;
      size_ = 0;
      return tmp;
    }
  };
  class MemoryFile{
    HANDLE fileHandle = nullptr;
  public:
    MemoryFile() = default;
    MemoryFile(const MemoryFile&) = delete;
    MemoryFile(MemoryFile&& other) noexcept : fileHandle(other.fileHandle){
      other.fileHandle = nullptr;
    }
    MemoryFile& operator=(const MemoryFile&) = delete;
    MemoryFile& operator=(MemoryFile&& other) noexcept{
      this->~MemoryFile();
      new(this) MemoryFile(std::move(other));
      return *this;
    }


    explicit MemoryFile(HANDLE handle) noexcept : fileHandle(handle){}
    ~MemoryFile(){
      if (fileHandle && !memoryApi.CloseHandle(fileHandle))
        panic(getLastError());
    }

    inline HANDLE       release() noexcept{
      HANDLE tmp = fileHandle;
      fileHandle = nullptr;
      return tmp;
    }
    inline HANDLE       handle() const noexcept {
      return fileHandle;
    }
  };
  class MappedMemory{
    void* address_ = nullptr;
    u64   size_    = 0;
  public:
    MappedMemory() = default;
    MappedMemory(const MappedMemory&) = delete;
    MappedMemory(MappedMemory&& other) noexcept {
      std::swap(address_, other.address_);
      std::swap(size_, other.size_);
    }

    MappedMemory& operator=(const MappedMemory&) = delete;
    MappedMemory& operator=(MappedMemory&& other) noexcept {
      this->~MappedMemory();
      new(this) MappedMemory(std::move(other));
      return *this;
    }
    MappedMemory(void* pMessages, u64 bufferLength) noexcept
        : address_(pMessages), size_(bufferLength){}
    ~MappedMemory(){
      auto result = unmap();
      if (result.failure())
        panic(result);
    }

    inline void* address() const noexcept {
      return address_;
    }
    inline u64 size() const noexcept {
      return size_;
    }
    inline std::pair<void*, u64> release() noexcept {
      std::pair<void*, u64> result{nullptr, 0};
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
        0
    );
    if (!result)
      return {};
    return VirtualPlaceholder{(valkyrie::byte*)result, placeholderSize};
  }
  inline Win32Result<MemoryFile> createFileInMemory(u64 fileSize) noexcept {
    auto result = memoryApi.CreateFileMapping(
        nullptr,
        nullptr,
        PAGE_READWRITE,
        0,
        (DWORD)fileSize,
        nullptr
    );
    if (!result)
      return {};
    return MemoryFile(result);
  }
  inline Win32Result<MappedMemory> mapPlaceholder(MemoryFile& file, VirtualPlaceholder& vmLocation) noexcept {
    const u64 size = vmLocation.size();
    void* tmp = memoryApi.MapViewOfFile3(
      file.handle(),
      nullptr,
      vmLocation.address(),
      0,
      size,
      MEM_REPLACE_PLACEHOLDER,
      PAGE_READWRITE,
      nullptr,
      0
    );
    if (!tmp)
      return {};
    VK_assert(tmp == vmLocation.address());
    return MappedMemory(vmLocation.release(), size);
  }

  inline Win32Result<std::pair<MappedMemory, MappedMemory>> allocateCircularBuffer(u64 length) noexcept{
    std::pair<VirtualPlaceholder, VirtualPlaceholder> placeholder;
    MemoryFile                            physicalMemory;
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

  inline SystemStatus unmapView(void* pView) noexcept{
    if(!memoryApi.UnmapViewOfFile(pView))
      return getLastError();
    return {};
  }

  /*Core::SystemStatus   allocateMessageBufferWithGuards(u32& bufferSize, _Outptr_opt_ Core::Message** __restrict ppMessage) noexcept {

    using valkyrie::byte;

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
  }*/


  struct AgentMessageBuffer{
    //Core::Agent*                pReceiver;
    Core::Message*              pMessages;
    std::atomic<Core::Message*> pFront;
    std::atomic<Core::Message*> pBack;
    uint32_t                    bufferSize;
  };

  enum class AgentMessageKind : uint32_t;

  inline constexpr AgentMessageKind MessageFullyProcessed = static_cast<AgentMessageKind>(-1);

  struct AgentMessage{
    AgentMessageKind mKind;
    uint32_t         mSize;
    valkyrie::byte   pData[];
  };

  struct AgentMailbox{
    valkyrie::byte*      pBuffer;
    uint32_t             bufferSize;
    std::atomic_uint32_t fullyProcessedOffset;
    std::atomic_uint32_t nextReadOffset;
    std::atomic_uint32_t nextWriteOffset;
  };

  struct AgentInner{
    AgentMailbox* pMailbox;
    std::atomic_uint32_t nextReadOffset;
  };

  void readFrom(AgentMailbox* pMailbox /*, Args&& ...args*/) noexcept{

    uint32_t readOffset;
    uint32_t nextOffset;
    AgentMessage* pMessage;

    readOffset = pMailbox->nextReadOffset.load(std::memory_order_acquire);
    do {
      pMessage = reinterpret_cast<AgentMessage*>(pMailbox->pBuffer + readOffset);
    } while(!pMailbox->nextReadOffset.compare_exchange_weak(readOffset, readOffset + pMessage->mSize));

    nextOffset = readOffset;
    const uint32_t readOffsetCopy = readOffset;

    // Process message here...

    do {
      ((valkyrie::byte*&)pMessage) += pMessage->mSize;
      nextOffset += pMessage->mSize;
    } while(pMessage->mKind == MessageFullyProcessed);

    pMailbox->fullyProcessedOffset.compare_exchange_strong(readOffset, nextOffset);

    reinterpret_cast<AgentMessage*>(pMailbox->pBuffer + readOffsetCopy)->mKind = MessageFullyProcessed;
  }

  struct MessageResult{

  };


  using PFN_msgProc = Core::AgentStatus(*)(Core::Message* pMessage, void* pUserData);


  inline Core::AgentStatus readNext(AgentMessageBuffer* pBuffer, PFN_msgProc proc, void* pUserData) noexcept {
    auto status = proc(pBuffer->pFront.load(std::memory_order_acquire), pUserData);

  }

  inline void writeNext(AgentMessageBuffer* pBuffer, PFN_msgProc proc, void* pUserData) noexcept {
    auto status = proc(pBuffer->pFront.load(std::memory_order_acquire), pUserData);
  }



}

class Core::MessageQueue{

protected:
  struct ProducerMemo{
    MessageProducer* address = nullptr;
    std::string      name{};            // For debugging
  };
  struct ConsumerMemo{
    MessageConsumer* address = nullptr;
    std::string      name{};            // For debugging
  };

  using ProducerRegistry = FlatMap<u32, ProducerMemo>;
  using ConsumerRegistry = FlatMap<u32, ConsumerMemo>;

  using PFN_delete = void(*)(MessageQueue* pQueue, void* pUserData);
  using PFN_consumeMessage = void(*)(Message* pMessage, void* pUserData);

public:
  Message* pQueue;

  u32 bufferLength;

  u32 nextWriteOffset = 0;
  u32 nextReadOffset = 0;

  std::atomic_uint32_t referenceCount = 0;

  std::atomic_uint32_t producerId = 0;
  std::atomic_uint32_t consumerId = 0;

  ProducerRegistry producers{};
  ConsumerRegistry consumers{};

  PFN_delete pDelete = [](MessageQueue* pThis, void*){ delete pThis; };
  void* pDeleteUserData = nullptr;

  PFN_consumeMessage pConsumeMessage = nullptr;
  void* pConsumeMessageUserData = nullptr;

  /*MessageQueue(u32 bufferSize, u32 guardSize, void(*pDeleter)(MessageQueue*, void*), void* pUserData, Core::SystemStatus& status)
      : pQueue(),
        bufferLength(bufferSize),
        guardSize(guardSize),
        pDeleter(pDeleter),
        pUserData(pUserData){
    status = allocateMessageBuffer(bufferLength, this->guardSize, &pQueue);
  }*/

  MessageQueue(Message* pMessages, u32 bufferLength) noexcept : pQueue(pMessages), bufferLength(bufferLength) { }
  MessageQueue(const MessageQueue&) = delete;
  virtual ~MessageQueue() = default;


  inline static SystemStatus makeNew(MessageQueue& queue, u64 bufferSize) noexcept {
    std::pair<VirtualPlaceholder, VirtualPlaceholder> placeholder;
    MemoryFile   physicalMemory;
    MappedMemory mappedBuffer;

    if (auto result = allocatePlaceholder(bufferSize * 2))
      placeholder.first = result.take();
    else
      return result.status();

    if (auto result = placeholder.first.takeFront(bufferSize))
      placeholder.second = result.take();
    else
      return result.status();

    if (auto result = createFileInMemory(bufferSize))
      physicalMemory = result.take();
    else
      return result.status();

    if (auto result = mapPlaceholder(physicalMemory, placeholder.first))
      mappedBuffer = result.take();
    else
      return result.status();

    if (auto result = mapPlaceholder(physicalMemory, placeholder.second))
      result.take().release();
    else
      return result.status();

    std::tie(reinterpret_cast<void*&>(queue.pQueue), queue.bufferLength) = mappedBuffer.release();
    return {};
  }


  inline virtual u32          acquireProducer(MessageProducer* pProducer) noexcept = 0;
  inline virtual u32          acquireConsumer(MessageConsumer* pConsumer) noexcept = 0;
  inline virtual SystemStatus releaseProducer(u32 prodId) noexcept = 0;
  inline virtual SystemStatus releaseConsumer(u32 consId) noexcept = 0;


  SystemStatus destroy() noexcept {
    Core::SystemStatus result{NO_ERROR};
    auto deleter  = pDelete;
    auto userData = pDeleteUserData;
    if (pQueue) {
      result = unmapView(pQueue);
      if (result.success())
        result = unmapView(pQueue + bufferLength);
      else
        unmapView(pQueue + bufferLength);
    }
    deleter(this, userData);
    return std::move(result);
  }
};



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