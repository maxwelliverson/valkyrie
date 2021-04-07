//
// Created by maxwe on 2021-04-06.
//



#include <cstdint>
#include <atomic>
#include <type_traits>
#include <cassert>
#include <tuple>

#ifndef LFP_ALLOW_BLOCKING
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
}

/*
#include <iostream>

#include <windows.h>
#include <memoryapi.h>

int main(){
  std::cout << "Large Page Size: " << GetLargePageMinimum() << std::endl;
}*/
