//
// Created by maxwe on 2021-04-06.
//



/*#include <cstdint>
#include <atomic>
#include <type_traits>
#include <cassert>
#include <tuple>*/

/*#ifndef LFP_ALLOW_BLOCKING
static_assert(ATOMIC_POINTER_LOCK_FREE == 2,
              "Atomic pointer is not lock-free.");
#endif

#ifndef LFP_ALLOCATIONS_PER_CHUNK
#define LFP_ALLOCATIONS_PER_CHUNK (64 * 100)
#endif

namespace lfpAlloc {

  struct chunk{
    char* addr_;
    unsigned char
  };











  template <std::size_t Size>
  struct Cell {
    uint8_t val_[Size];
    Cell* next_ = this + 1;
  };

// For small types (less than the size of void*), no additional
// space is needed, so union val_ with next_ to avoid overhead.
  template <>
  struct Cell<0> {
    Cell() : next_{this + 1} {}
    union {
      uint8_t val_[sizeof(Cell*)];
      Cell* next_;
    };
  };

  template <std::size_t Size, std::size_t AllocationsPerChunk>
  struct Chunk {
    Chunk() noexcept {
      auto& last = memBlock_[AllocationsPerChunk - 1];
      last.next_ = nullptr;
    }
    Cell<Size> memBlock_[AllocationsPerChunk];
  };

  template <typename T>
  struct Node {
    Node() : val_(), next_(nullptr) {}
    Node(const T& val) : val_(val), next_(nullptr) {}
    T val_;
    std::atomic<Node<T>*> next_;
  };

  template <std::size_t Size, std::size_t AllocationsPerChunk>
  class ChunkList {
    static constexpr auto CellSize =
        (Size > sizeof(void*)) ? Size - sizeof(void*) : 0;
    using Chunk_t = Chunk<CellSize, AllocationsPerChunk>;
    using Cell_t = Cell<CellSize>;

    using ChunkNode = Node<Chunk_t>;
    using CellNode = Node<Cell_t*>;

  public:
    static ChunkList& getInstance() {
      static ChunkList c;
      return c;
    }

    Cell_t* allocateChain() {
      CellNode* recentHead = head_.load();
      CellNode* currentNext = nullptr;
      do {
        // If there are no available chains, allocate a new chunk
        if (!recentHead) {
          ChunkNode* currentHandle;

          // Make a new node
          auto newChunk = new ChunkNode();

          // Add the chunk to the chain
          do {
            currentHandle = handle_.load();
            newChunk->next_ = currentHandle;
          } while (
              !handle_.compare_exchange_weak(currentHandle, newChunk));
          return &newChunk->val_.memBlock_[0];
        }

        currentNext = recentHead->next_;
      } while (!head_.compare_exchange_weak(recentHead, currentNext));

      auto retnValue = recentHead->val_;
      delete recentHead;
      return retnValue;
    }

    void deallocateChain(Cell_t* newCell) {
      if (!newCell) {
        return;
      }
      CellNode* currentHead = head_.load();

      // Construct a new node to be added to the linked list
      CellNode* newHead = new CellNode(newCell);

      // Add the chain to the linked list
      do {
        newHead->next_.store(currentHead, std::memory_order_release);
      } while (!head_.compare_exchange_weak(currentHead, newHead));
    }

  private:
    ChunkList() : handle_(nullptr), head_(nullptr) {}

    std::atomic<ChunkNode*> handle_;
    std::atomic<CellNode*> head_;
  };

  template <std::size_t Size, std::size_t AllocationsPerChunk>
  class Pool {
    using ChunkList_t = ChunkList<Size, AllocationsPerChunk>;

  public:
    static constexpr auto CellSize =
        (Size > sizeof(void*)) ? Size - sizeof(void*) : 0;
    using Cell_t = Cell<CellSize>;

    Pool() : head_(nullptr) {}

    ~Pool() { ChunkList_t::getInstance().deallocateChain(head_); }

    void* allocate() {
      // Head loaded from head_
      Cell_t* currentHead = head_;
      Cell_t* next;

      // Out of cells to allocate
      if (!currentHead) {
        currentHead = ChunkList_t::getInstance().allocateChain();
      }

      next = currentHead->next_;
      head_ = next;
      return &currentHead->val_;
    }

    void deallocate(void* p) noexcept {
      auto newHead = reinterpret_cast<Cell_t*>(p);
      Cell_t* currentHead = head_;
      newHead->next_ = currentHead;
      head_ = newHead;
    }

  private:
    Cell_t* head_;
  };

  namespace detail {

    template <std::size_t Num, uint16_t... Ts>
    struct Pools : Pools<Num - 1, alignof(std::max_align_t) * Num, Ts...> {};

    template <uint16_t... Size>
    struct Pools<0, Size...> {
      using type = std::tuple<Pool<Size, LFP_ALLOCATIONS_PER_CHUNK>...>;
    };
  }

  template <std::size_t NumPools>
  class PoolDispatcher {
  public:
    void* allocate(std::size_t size) { return dispatchAllocate<0>(size); }

    void deallocate(void* p, std::size_t size) noexcept {
      dispatchDeallocate<0>(p, size);
    }

  private:
    thread_local static typename detail::Pools<NumPools>::type pools_;
    static_assert(NumPools > 0, "Invalid number of pools");

    template <std::size_t Index> requires (Index < NumPools)
    inline void* dispatchAllocate(uint64_t requestSize) {
      if (requestSize <= std::get<Index>(pools_).CellSize) {
        return std::get<Index>(pools_).allocate();
      } else {
        return dispatchAllocate<Index + 1>(requestSize);
      }
    }

    template <std::size_t Index>
    typename std::enable_if<!(Index < NumPools), void*>::type
    dispatchAllocate(std::size_t const&) {
      assert(false && "Invalid allocation size.");
      return nullptr;
    }

    template <std::size_t Index>
    typename std::enable_if <
        Index<NumPools>::type
    dispatchDeallocate(void* p, std::size_t const& requestSize) noexcept {
      if (requestSize <= std::get<Index>(pools_).CellSize) {
        std::get<Index>(pools_).deallocate(p);
      } else {
        dispatchDeallocate<Index + 1>(p, requestSize);
      }
    }

    template <std::size_t Index>
    typename std::enable_if<!(Index < NumPools)>::type
    dispatchDeallocate(void*, std::size_t const&) noexcept {
      assert(false && "Invalid deallocation size.");
    }
  };

  template <std::size_t NumPools>
  thread_local typename detail::Pools<NumPools>::type
      PoolDispatcher<NumPools>::pools_;
}*/

#include <valkyrie/status/result.hpp>


#include <iostream>

#include <windows.h>
#include <memoryapi.h>

#pragma comment(lib, "mincore")

#define COLOR(FGBG, CODE, BOLD) "\033[0;" BOLD FGBG CODE "m"

#define ALLCOLORS(FGBG,BOLD) {\
COLOR(FGBG, "0", BOLD),\
COLOR(FGBG, "1", BOLD),\
COLOR(FGBG, "2", BOLD),\
COLOR(FGBG, "3", BOLD),\
COLOR(FGBG, "4", BOLD),\
COLOR(FGBG, "5", BOLD),\
COLOR(FGBG, "6", BOLD),\
COLOR(FGBG, "7", BOLD)\
}

constexpr static const char colorcodes[2][2][8][10] = {
    { ALLCOLORS("3",""), ALLCOLORS("3","1;") },
    { ALLCOLORS("4",""), ALLCOLORS("4","1;") }
};

void setup_color_output() {
  HANDLE Console = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD Mode;
  GetConsoleMode(Console, &Mode);
  Mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  SetConsoleMode(Console, Mode);
}

enum class color {
  black = 0,
  red,
  green,
  yellow,
  blue,
  magenta,
  cyan,
  white,
  savedcolor,
  reset
  };

constexpr static struct bold_{} bold;

template <bool IsBold>
struct background{
  color colorCode;

  background(color c) : colorCode(c){}
  background(color c, bold_) : colorCode(c){}
};
template <bool IsBold>
struct foreground{
  color colorCode;

  foreground(color c) : colorCode(c){}
  foreground(color c, bold_) : colorCode(c){}
};

background(color)        -> background<false>;
background(color, bold_) -> background<true>;

foreground(color)        -> foreground<false>;
foreground(color, bold_) -> foreground<true>;



std::pair<color, bool> get_current_color(bool bg) noexcept {
  WORD colorWord = 0;
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
    colorWord = csbi.wAttributes;
  }
  WORD colors;
  bool isBold;
  if (bg) {
    colors = ((colorWord&BACKGROUND_RED) ? 0x1 : 0) |
             ((colorWord&BACKGROUND_GREEN) ? 0x2 : 0) |
             ((colorWord&BACKGROUND_BLUE) ? 0x4 : 0);
    isBold = (colorWord&BACKGROUND_INTENSITY);
  } else {
    colors = ((colorWord&1) ? FOREGROUND_RED : 0) |
        ((colorWord&2) ? FOREGROUND_GREEN : 0 ) |
        ((colorWord&4) ? FOREGROUND_BLUE : 0);
    isBold = (colorWord&FOREGROUND_INTENSITY);
  }
  return { static_cast<color>(colors), isBold };
}

class formatted_output{
  const char* originalFgColorCode;
  const char* originalBgColorCode;

  color foregroundColor;
  color backgroundColor;

  bool  foregroundIsBold;
  bool  backgroundIsBold;


  std::ostream& os;

  static const char* colorCode(color code, bool isBold, bool background) noexcept {
    return colorcodes[background][isBold][static_cast<std::underlying_type_t<color>>(code)&0x7];
  }

public:
  formatted_output(std::ostream& os)
      : os(os){
    std::tie(foregroundColor, foregroundIsBold) = get_current_color(false);
    std::tie(backgroundColor, backgroundIsBold) = get_current_color(true);
    originalFgColorCode = colorCode(foregroundColor, foregroundIsBold, false);
    originalBgColorCode = colorCode(backgroundColor, backgroundIsBold, true);
  }

  friend formatted_output& operator<<(formatted_output& OS, const valkyrie::string_ref& string) {
    OS.os.write(string.c_str(), string.size());
    return OS;
  }

  template <bool IsBold>
  friend formatted_output& operator<<(formatted_output& OS, background<IsBold> bg) noexcept {
    if ( bg.colorCode == color::reset )
      OS.os << OS.originalBgColorCode;
    else
      OS.os << colorCode(bg.colorCode, IsBold, true);
    return OS;
  }
  template <bool IsBold>
  friend formatted_output& operator<<(formatted_output& OS, foreground<IsBold> fg) noexcept {
    if ( fg.colorCode == color::reset )
      OS.os << OS.originalFgColorCode;
    else
      OS.os << colorCode(fg.colorCode, IsBold, false);
    return OS;
  }
  template <typename T> requires(requires(std::ostream& o, T&& t){ o << std::forward<T>(t); })
  friend formatted_output& operator<<(formatted_output& OS, T&& t) noexcept {
    OS.os << std::forward<T>(t);
    return OS;
  }
};


int main(){

  setup_color_output();

  formatted_output os(std::cout);

  /*HMODULE kernel = GetModuleHandle("KernelBase");
  auto* const VirtualAlloc2 = (PVOID(*)(HANDLE,PVOID,SIZE_T,ULONG,ULONG,PMEM_EXTENDED_PARAMETER,ULONG))GetProcAddress(kernel, "VirtualAlloc2");
  auto* const VirtualFree   = (BOOL(*)(PVOID, ULONG, SIZE_T))GetProcAddress(kernel, "VirtualFree");
*/

  /*LARGE_INTEGER freq;
  if (!QueryPerformanceFrequency(&freq))
    panic(valkyrie::sys::win32::get_last_error());

  std::cout << "PerfCounterFrequency => " << freq.QuadPart << std::endl;
  std::cout << "Large Page Size: " << GetLargePageMinimum() << std::endl;
  std::cout << "Hello World" << std::endl;*/

  using valkyrie::sys::win32::get_last_error;
  using valkyrie::system_status;

  system_status status;
  SYSTEM_INFO sysInfo;
  HANDLE procHandle;
  HANDLE mappedFile;
  size_t oneSize;
  size_t twoSize;
  size_t threeSize;
  void*  placeholderOne;
  void*  placeholderTwo;
  void*  placeholderThree;
  void*  committedOne;
  void*  committedTwo;
  void*  committedThree;
  size_t pageSize;
  size_t totalAllocSize;
  BOOL boolResult;

#define print_results(Func, b) Func; do {        \
              os << foreground(color::magenta, bold); \
              os << #Func ": ";                       \
                                                 \
              if ( b ) {                         \
                status = system_status(NO_ERROR);  \
                os << foreground(color::green);                                \
              }             \
              else {                             \
                status = get_last_error();                                  \
                os << foreground(color::red);                                 \
              }\
              os << status.message() << "\n";                                   \
              } while(false);
  


  committedOne = nullptr;
  committedTwo = nullptr;
  committedThree = nullptr;

  GetSystemInfo(&sysInfo);

  pageSize         = sysInfo.dwAllocationGranularity;
  totalAllocSize   = pageSize * 64;


  oneSize          = pageSize * 4;
  twoSize          = pageSize * 16;
  threeSize        = totalAllocSize - (oneSize + twoSize);
  placeholderOne   = print_results(VirtualAlloc2(nullptr,
                                      nullptr,
                                      totalAllocSize,
                                      MEM_RESERVE | MEM_RESERVE_PLACEHOLDER,
                                      PAGE_NOACCESS,
                                      nullptr,
                                      0), placeholderOne);

  mappedFile       = print_results(CreateFileMapping(GetCurrentProcess(), nullptr, PAGE_READWRITE, 0, totalAllocSize, nullptr), mappedFile);

  committedOne     = print_results(MapViewOfFile3(mappedFile, nullptr, placeholderOne, 0, totalAllocSize, MEM_REPLACE_PLACEHOLDER, PAGE_READWRITE, nullptr, 0), committedOne);

  boolResult       = print_results(UnmapViewOfFile2(GetCurrentProcess(), committedOne, MEM_PRESERVE_PLACEHOLDER), boolResult);

  boolResult       = print_results(VirtualFree(placeholderOne, oneSize, MEM_RELEASE | MEM_PRESERVE_PLACEHOLDER), boolResult);
  placeholderTwo   = static_cast<char*>(placeholderOne) + oneSize;
  boolResult       = print_results(VirtualFree(placeholderTwo, twoSize, MEM_RELEASE | MEM_PRESERVE_PLACEHOLDER), boolResult);
  placeholderThree = static_cast<char*>(placeholderTwo) + twoSize;

  committedOne     = print_results(VirtualAlloc2(nullptr, placeholderOne, oneSize,     MEM_RESERVE | MEM_COMMIT | MEM_REPLACE_PLACEHOLDER, PAGE_READWRITE, nullptr, 0), committedOne);
  committedTwo     = print_results(VirtualAlloc2(nullptr, placeholderTwo, twoSize,     MEM_RESERVE | MEM_COMMIT | MEM_REPLACE_PLACEHOLDER, PAGE_READWRITE, nullptr, 0), committedTwo);
  committedThree   = print_results(VirtualAlloc2(nullptr, placeholderThree, threeSize, MEM_RESERVE | MEM_COMMIT | MEM_REPLACE_PLACEHOLDER, PAGE_READWRITE, nullptr, 0), committedThree);

  boolResult       = print_results(VirtualFree(placeholderOne, oneSize + twoSize, MEM_RELEASE | MEM_COALESCE_PLACEHOLDERS), boolResult);
  boolResult       = print_results(VirtualFree(placeholderTwo, twoSize, MEM_RELEASE | MEM_PRESERVE_PLACEHOLDER), boolResult);
  boolResult       = print_results(VirtualFree(placeholderOne, oneSize + twoSize, MEM_RELEASE | MEM_COALESCE_PLACEHOLDERS), boolResult);
  boolResult       = print_results(VirtualFree(placeholderOne, oneSize, MEM_RELEASE | MEM_PRESERVE_PLACEHOLDER), boolResult);
  boolResult       = print_results(VirtualFree(placeholderOne, oneSize + twoSize, MEM_RELEASE | MEM_COALESCE_PLACEHOLDERS), boolResult);

  if ( committedOne )
    boolResult = print_results(VirtualFree(committedOne, oneSize, MEM_PRESERVE_PLACEHOLDER), boolResult);
  if ( committedTwo )
    boolResult = print_results(VirtualFree(committedTwo, twoSize, MEM_PRESERVE_PLACEHOLDER), boolResult);
  if ( committedThree )
    boolResult = print_results(VirtualFree(committedThree, threeSize, MEM_PRESERVE_PLACEHOLDER), boolResult);
  
  if ( placeholderOne )
    boolResult = print_results(VirtualFree(placeholderOne, 0, MEM_RELEASE), boolResult);
  if ( placeholderTwo )
    boolResult = print_results(VirtualFree(placeholderTwo, 0, MEM_RELEASE), boolResult);
  if ( placeholderThree )
    boolResult = print_results(VirtualFree(placeholderThree, 0, MEM_RELEASE), boolResult);


  return 0;
}
