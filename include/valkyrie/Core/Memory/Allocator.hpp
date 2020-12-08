//
// Created by Maxwell on 2020-11-09.
//

#ifndef VALKYRIE_ALLOCATOR_HPP
#define VALKYRIE_ALLOCATOR_HPP

#include <valkyrie/Core/Config.hpp>
#include <valkyrie/Core/Error/Maybe.hpp>
#include <valkyrie/Core/Memory/Address.hpp>

#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

namespace valkyrie::Core{

  class MemorySize{
    u64 total_bytes_;
  public:
    constexpr MemorySize() = default;
    constexpr MemorySize(const MemorySize&) = default;
    constexpr explicit MemorySize(u64 bytes) noexcept : total_bytes_(bytes){}


    constexpr MemorySize& operator+=(const MemorySize other) noexcept{
      total_bytes_ += other.total_bytes_;
      return *this;
    }
    constexpr MemorySize& operator-=(const MemorySize other) noexcept{
      total_bytes_ -= other.total_bytes_;
      return *this;
    }
    constexpr MemorySize& operator+=(const i64 other) noexcept{
      total_bytes_ += other;
      return *this;
    }
    constexpr MemorySize& operator-=(const i64 other) noexcept{
      total_bytes_ -= other;
      return *this;
    }

    friend constexpr MemorySize operator+(MemorySize A, MemorySize B) noexcept{
      return MemorySize(A.total_bytes_ + B.total_bytes_);
    }
    friend constexpr MemorySize operator+(MemorySize A, i64 i) noexcept{
      return MemorySize(A.total_bytes_ + i);
    }
    friend constexpr MemorySize operator-(MemorySize A, MemorySize B) noexcept{
      return MemorySize(A.total_bytes_ - B.total_bytes_);
    }
    friend constexpr MemorySize operator-(MemorySize A, i64 i) noexcept{
      return MemorySize(A.total_bytes_ - i);
    }

    friend constexpr u64 toInteger(const MemorySize mem) noexcept{
      return mem.total_bytes_;
    }

    friend constexpr bool operator==(const MemorySize A, const MemorySize B) noexcept{
      return A.total_bytes_ == B.total_bytes_;
    }
    friend constexpr std::strong_ordering operator<=>(const MemorySize A, const MemorySize B) noexcept{
      return A.total_bytes_ <=> B.total_bytes_;
    }
  };

  inline constexpr static MemorySize operator""_B(unsigned long long int bytes) noexcept {
    return MemorySize(bytes);
  }
  inline constexpr static MemorySize operator""_KiB(unsigned long long int bytes) noexcept {
    return MemorySize(bytes << 10);
  }
  inline constexpr static MemorySize operator""_MiB(unsigned long long int bytes) noexcept {
    return MemorySize(bytes << 20);
  }
  inline constexpr static MemorySize operator""_GiB(unsigned long long int bytes) noexcept {
    return MemorySize(bytes << 30);
  }
  inline constexpr static MemorySize operator""_KiB(long double bytes) noexcept {
    return MemorySize(u64(bytes * (1ULL << 10)));
  }
  inline constexpr static MemorySize operator""_MiB(long double bytes) noexcept {
    return MemorySize(u64(bytes * (1ULL << 20)));
  }
  inline constexpr static MemorySize operator""_GiB(long double bytes) noexcept {
    return MemorySize(u64(bytes * (1ULL << 30)));
  }

  namespace System{
    inline constexpr static auto PageSize = 4_KiB; // Default on most systems, change if necessary
    inline constexpr static auto AllocationGranularity = VK_if(VK_system_windows(6))4_KiB;
    inline constexpr static auto LargePageSize = 4_MiB;


    enum PageKind{
      PageKindPhysical,
      PageKindLargePhysical,
      PageKindVirtual,
      PageKindLargeVirtual
    };
    inline constexpr static u32 LargeMask    = 0x1;
    inline constexpr static u32 VirtualMask = 0x2;

    class Page{};

    class alignas(toInteger(PageSize)) PhysicalPage : public Page{
    public:
      inline constexpr static MemorySize size() noexcept {
        return PageSize;
      }
    };
    class alignas(toInteger(AllocationGranularity)) VirtualPage : public Page{
    public:
      inline constexpr static MemorySize size() noexcept {
        return AllocationGranularity;
      }
    };
    class alignas(toInteger(LargePageSize)) LargePhysicalPage : public Page{
    public:
      inline constexpr static MemorySize size() noexcept {
        return LargePageSize;
      }
    };
    class alignas(toInteger(LargePageSize)) LargeVirtualPage : public Page{
    public:
      inline constexpr static MemorySize size() noexcept {
        return LargePageSize;
      }
    };

    class MemoryHandle{
      int fd;
      MemorySize bytes;
    public:
      explicit MemoryHandle(int fd, MemorySize bytes) : fd(fd), bytes(bytes){}
    };
    class VirtualMemory{
      void* pAddress;
      MemorySize bytes;
    public:
      explicit VirtualMemory(void* pAddress, MemorySize bytes) : pAddress(pAddress), bytes(bytes){}
    };

    VirtualMemory virtualAlloc(MemorySize bytes) noexcept {
      void* pAddress = mmap64(nullptr, toInteger(bytes), PROT_READ | PROT_WRITE, );
    }

    MemoryHandle createMemory(MemorySize bytes) noexcept {

      if (ftruncate(fd, toInteger(bytes))) {
        // bleeehhhh
      }
      if (fcntl(fd, F_ADD_SEALS, F_SEAL_GROW | F_SEAL_SHRINK)) {
        // bleeeeehhhhhh
      }
      return MemoryHandle(fd, bytes);
    }



    class CircularBuffer{

      void* pBuffer;
      MemorySize size;
      int backingFile;


      CircularBuffer(void* pBuffer, MemorySize size, int backingFile) noexcept
          : pBuffer(pBuffer), size(size), backingFile(backingFile){}

    public:
      static Maybe<CircularBuffer> allocate(MemorySize size) noexcept {
        int fd = memfd_create("valkyrie::Core::System::CircularBuffer", MFD_ALLOW_SEALING);
        if (fd == -1)

      }
      static MemorySize requiredGranularity() noexcept {
        return AllocationGranularity;
      }
    };



    /*class PageRange{
      union{
        PhysicalPage*      pPhysical;
        VirtualPage*       pVirtual;
        LargePhysicalPage* pLargePhysical;
        LargeVirtualPage*  pLargeVirtual;
      } page;
      PageKind kind  : 2;
      u32      count : 30;
      int      fd;
    public:



      [[nodiscard]] bool isLarge() const noexcept {
        return kind & LargeMask;
      }
      [[nodiscard]] bool isVirtual() const noexcept {
        return kind & VirtualMask;
      }
    };*/
  }


  class MemoryResource;
  template <typename T>
  class MemoryAllocator;
  template <typename T>
  class MemoryAllocation;


  template <typename T>
  class MemoryPlaceholder;









  struct MapParameters{};
  struct ReserveParameters{};
  struct CommitParameters{};
  struct AllocateParameters{};

  struct ReallocateParameters{};

  struct UnmapParameters{};
  struct ReleaseParameters{};
  struct DecommitParameters{};
  struct DeallocateParameters{};
}

#endif//VALKYRIE_ALLOCATOR_HPP
