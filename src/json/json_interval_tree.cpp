//
// Created by Maxwell on 2021-02-18.
//

#include <utility>

#include "jsonmm.h"
#include <json/json.h>

#include <any>
#include <bit>
#include <bitset>
#include <cassert>
#include <compare>
#include <cstddef>
#include <limits>
#include <memory_resource>
#include <optional>

#include <string>
#include <typeindex>
#include <concurrent_unordered_map.h>


#define this_is_null() (!(const volatile void*)this)

#define VALKYRIE_EXPENSIVE_CHECKS

#if defined(VALKYRIE_EXPENSIVE_CHECKS)
#define expensive_assert(...) assert(__VA_ARGS__)
#else
#define expensive_assert(...)
#endif



namespace json{


  using u8  = unsigned char;
  using u16 = unsigned short;
  using u32 = unsigned int;
  using u64 = unsigned long long;
  using std::byte;


  enum class ChunkHint : u32{};

  enum class Corruption{
    None,
    BlocksAvailableOutOfBounds,
    NextFreeBlockOutOfBounds,
    NextIndexOutOfBounds,
    EarlyLoopInBlockList,
    BadLoopLength,
    BadBookkeeping
  };

  /*struct Variable{
    const char* name;
    std::any    value;
  };
  class Invariant{

    using print_fn = void(*)(std::stringstream& out, const std::any& obj);

    template <typename T>
    inline constexpr static auto typed_print_fn = [](std::stringstream& out, const std::any& obj){
      if constexpr (requires(const T& t){ out << t; }) {
        out << *any_cast<T>(&obj);
      }
      else {
        out << std::to_string(*any_cast<T>(&obj));
      }
    };

  public:
    std::string                statement;
    std::optional<std::string> scope;
    std::vector<Variable>      variables;
    bool                       isValid;

    inline static concurrency::concurrent_unordered_map<std::type_index, print_fn> printingFunctions{};

    template <typename T>
    void insertVariable(const char* varName, T&& value) noexcept {
      using RealT = std::remove_cvref_t<T>;
      printingFunctions.insert({ std::type_index(typeid(RealT)), typed_print_fn<RealT> });
      variables.push_back({ varName, std::forward<T>(value) });
    }
  };
  class FunctionInvariants{
    std::string functionName;
    std::vector<Variable>      variables;
  };
  class ClassInvariants{

    using print_fn = void(*)(std::stringstream& out, const std::any& obj);

    template <typename T>
    inline constexpr static auto typed_print_fn = [](std::stringstream& out, const std::any& obj){
      if constexpr (requires(const T& t){ out << t; }) {
        out << *any_cast<T>(&obj);
      }
      else {
        out << std::to_string(*any_cast<T>(&obj));
      }
    };


    std::vector<std::pair<const char*, bool>> invariants;
    std::string                               className;
    std::vector<Variable>                     variables;

    inline static concurrency::concurrent_unordered_map<std::type_index, print_fn> printingFunctions{};
  public:
    template <typename T>
    void insertVariable(const char* varName, T&& value) noexcept {
      using RealT = std::remove_cvref_t<T>;
      printingFunctions.insert({ std::type_index(typeid(RealT)), typed_print_fn<RealT> });
      variables.push_back({ varName, std::forward<T>(value) });
    }
  };*/



  template <size_t BlockSize_, u8 BlocksPerChunk_ = std::numeric_limits<u8>::max()>
  class FixedSizeAllocator{
  public:

    inline constexpr static size_t BlockSize = BlockSize_;
    inline constexpr static u32    BlocksPerChunk = BlocksPerChunk_;

  private:

    static_assert(BlockSize && !(BlockSize & (BlockSize - 1)),
                  "FixedSizeAllocator<BlockSize, BlocksPerChunk> -> BlockSize must be a power of 2");


    inline constexpr static size_t ChunkMemorySize = BlockSize * BlocksPerChunk;
    inline constexpr static size_t ChunkMemoryAlignment = std::min(BlockSize, alignof(std::max_align_t));
    inline constexpr static size_t BlockSizeLog2 = std::countr_zero(BlockSize);

    struct Chunk{

      u8    availableBlocks = BlocksPerChunk;
      u8    nextFreeBlock   = 0;
      byte* pMemory         = nullptr;

      void init(std::pmr::memory_resource* allocator) noexcept {
        auto blocksPerChunk = BlocksPerChunk;
        pMemory = static_cast<std::byte*>(allocator->allocate(ChunkMemorySize, ChunkMemoryAlignment));
        u8 i{0x00};
        byte* p = pMemory;
        for(; i < BlocksPerChunk; p += BlockSize)
          *p = byte(++i);
      }
      void destroy(std::pmr::memory_resource* allocator) noexcept {

        assert(this->empty());

        this->release(allocator);
      }
      void release(std::pmr::memory_resource* allocator) noexcept {
        allocator->deallocate(pMemory, ChunkMemorySize, ChunkMemoryAlignment);
        pMemory = nullptr;
      }

      Chunk(){
        this->init(std::pmr::get_default_resource());
      }
      explicit Chunk(std::pmr::memory_resource* pResource) {
        this->init(pResource);
      }

      Chunk(const Chunk&) = delete;
      Chunk(Chunk&& other) noexcept : availableBlocks(0), nextFreeBlock(0), pMemory(nullptr){
        std::swap(availableBlocks, other.availableBlocks);
        std::swap(nextFreeBlock, other.nextFreeBlock);
        std::swap(pMemory, other.pMemory);
      }

      Chunk& operator=(const Chunk&) = delete;
      Chunk& operator=(Chunk&& other) noexcept {
        this->~Chunk();
        new(this) Chunk(std::move(other));
        return *this;
      }

      ~Chunk(){
        assert( !pMemory );

        /*if (pMemory)
          destroy(std::pmr::get_default_resource());*/

        //assert(!pMemory);
        /*if (pChunk) {
          allocator.deallocate(pChunk, ChunkAllocationSize);
        }*/
      }

      void* allocate() noexcept {
        if (full())
          return nullptr;
        auto* pResult = reinterpret_cast<u8*>(&pMemory[nextFreeBlock * BlockSize]);
        nextFreeBlock = *pResult;
        --availableBlocks;
        return pResult;
      }
      void  deallocate(void* p) noexcept {
        assert(contains(p));
        assert(!empty());
        auto* pRelease = static_cast<byte*>(p);
        assert((pRelease - pMemory) % BlockSize == 0);
        *pRelease = std::byte{nextFreeBlock};
        nextFreeBlock = static_cast<u8>((pRelease - pMemory) >> BlockSizeLog2);
        assert(nextFreeBlock == (pRelease - pMemory) >> BlockSizeLog2);
        ++availableBlocks;
      }
      bool  contains(void* p) const noexcept {
        const byte* const pComp = static_cast<const byte*>(p);
        return pMemory <= pComp && pComp < (pMemory + ChunkMemorySize);
      }
      bool  full() const noexcept {
        return !availableBlocks;
      }
      bool  empty() const noexcept {
        return availableBlocks == BlocksPerChunk;
      }

      Corruption checkCorruption() const noexcept {
        if (BlocksPerChunk < availableBlocks)
          return Corruption::BlocksAvailableOutOfBounds;

        if ( full() )
          return Corruption::None;

        if ( BlocksPerChunk <= nextFreeBlock )
          return Corruption::NextFreeBlockOutOfBounds;

        return checkDataCorruption();
      }
      Corruption checkDataCorruption() const noexcept {
        u8                          index = nextFreeBlock;
        std::bitset<BlocksPerChunk> foundBlocks;
        byte*                       nextBlock = nullptr;

        for (unsigned char count; ; ) {
          nextBlock = pMemory + (index * BlockSize);
          foundBlocks.set(index);
          ++count;
          if (count >= availableBlocks)
            break;

          index = u8(*nextBlock);
          if (BlocksPerChunk <= index)
            return Corruption::NextIndexOutOfBounds;
          if ( foundBlocks.test(index) )
            return Corruption::EarlyLoopInBlockList;
        }
        if ( foundBlocks.count() != availableBlocks )
          return Corruption::BadLoopLength;


        return Corruption::None;
      }
    };

    using ChunkVector = std::vector<Chunk>;

    ChunkVector chunks{};

    mutable Chunk* allocChunk   = nullptr;
    mutable Chunk* deallocChunk = nullptr;
    mutable Chunk* emptyChunk   = nullptr;

    std::pmr::memory_resource* pChunkResource = std::pmr::get_default_resource();

    /*ChunkHandle findChunk(void* p) const noexcept {
      if ( chunks.empty() )
        return nullptr;

      assert(deallocChunk != InvalidChunk);

      Chunk * lo = &chunks[deallocChunk];
      Chunk * hi = &chunks[deallocChunk + 1];
      const Chunk * loBound = &chunks.front();
      const Chunk * hiBound = &chunks.back() + 1;

      // Special case: deallocChunk_ is the last in the array
      if (hi == hiBound)
        hi = nullptr;

      for (;;)
      {
        if (lo)
        {
          if ( lo->contains( p ) )
            return lo;
          if ( lo == loBound ) {
            lo = nullptr;
            if ( !hi )
              break;
          }
          else
            --lo;
        }
        if (hi) {
          if ( hi->contains( p ) )
            return hi;
          if ( ++hi == hiBound )
          {
            hi = nullptr;
            if ( !lo )
              break;
          }
        }
      }

      return nullptr;
    }*/


    Chunk* findDeallocChunk(void* p) const noexcept {
      if ( chunks.empty() )
        return nullptr;

      assert(deallocChunk != nullptr);

      Chunk* lo = deallocChunk;
      Chunk* hi = deallocChunk + 1;
      const Chunk* loBound = &*chunks.cbegin();
      const Chunk* hiBound = &*chunks.cend();

      // Special case: deallocChunk_ is the last in the array
      if (hi == hiBound)
        hi = nullptr;

      for (;;)
      {
        if ( lo )
        {
          if ( lo->contains( p ) )
            return lo;
          if ( lo == loBound ) {
            lo = nullptr;
            if ( !hi )
              break;
          }
          else
            --lo;
        }
        if ( hi ) {
          if ( hi->contains( p ) )
            return hi;
          if ( ++hi == hiBound )
          {
            hi = nullptr;
            if ( !lo )
              break;
          }
        }
      }

      return nullptr;
    }
    size_t countEmptyChunks() const noexcept {
#if defined(VK_debug_memory)
      return std::ranges::count(chunks, [](const Chunk& chunk){ return chunk.empty(); });
#else
      return emptyChunk != nullptr;
#endif
    }
    void doDeallocate(void* p) noexcept {
      // Show that deallocChunk_ really owns the block at address p.
      assert( deallocChunk != nullptr);
      assert( deallocChunk->contains( p ) );
      // Either of the next two assertions may fail if somebody tries to
      // delete the same block twice.
      assert( emptyChunk != deallocChunk );
      assert( !deallocChunk->empty() );
      // prove either emptyChunk_ points nowhere, or points to a truly empty Chunk.
      assert( ( emptyChunk == nullptr ) || emptyChunk->empty() );

      // call into the chunk, will adjust the inner list but won't release memory
      deallocChunk->deallocate(p);

      if ( deallocChunk->empty() ) {
        assert( emptyChunk != deallocChunk );

        // deallocChunk_ is empty, but a Chunk is only released if there are 2
        // empty chunks.  Since emptyChunk_ may only point to a previously
        // cleared Chunk, if it points to something else besides deallocChunk_,
        // then FixedAllocator currently has 2 empty Chunks.
        if ( emptyChunk ) {
          // If last Chunk is empty, just change what deallocChunk_
          // points to, and release the last.  Otherwise, swap an empty
          // Chunk with the last, and then release it.
          auto* poppedChunk = this->popEmptyChunk();

          if ( ( allocChunk == poppedChunk ) || emptyChunk->full() )
            allocChunk = deallocChunk;
        }
        emptyChunk = deallocChunk;
      }

      // prove either emptyChunk_ points nowhere, or points to a truly empty Chunk.
      assert( ( emptyChunk == nullptr ) || emptyChunk->empty() );
    }

    Chunk* popEmptyChunk() noexcept {

      assert( !chunks.empty() );
      assert( emptyChunk && emptyChunk->empty() );

      Chunk* lastChunk = &chunks.back();

      if ( !lastChunk->empty() || lastChunk != emptyChunk )
        std::swap( *emptyChunk, *lastChunk );

      assert( lastChunk->empty() );

      lastChunk->destroy(pChunkResource);
      chunks.pop_back();

      return lastChunk;
    }

    inline constexpr static auto InvalidChunkIndex = static_cast<u32>(-1);

  public:
    FixedSizeAllocator() = default;
    explicit FixedSizeAllocator(std::pmr::memory_resource* pResource) noexcept
        : pChunkResource(pResource){
      chunks.reserve(4);
      chunks.emplace_back(pChunkResource);
    }

    ~FixedSizeAllocator() {
        assert( chunks.empty() && "Must release() FixedSizeAllocator before it is destroyed." );
    }

    /*FixedSizeAllocator(std::pmr::memory_resource* pChunkVectorResource, std::pmr::memory_resource* pChunkResource) noexcept
        : chunks(0, pChunkVectorResource),
          pChunkResource(pChunkResource){}*/

    u32    ownedChunkIndex(void* p) const noexcept {
      for (u32 i = 0; i < chunks.size(); ++i)
        if (chunks[i].contains(p))
          return i;
      return InvalidChunkIndex;
    }
    bool   tryDeallocate(void* p) noexcept {
      auto foundChunk = findDeallocChunk(p);
      if ( !foundChunk )
        return false;
      deallocChunk = foundChunk;
      doDeallocate(p);
      assert(countEmptyChunks() < 2);
      return true;
    }
    size_t trimEmptyChunk() {
      // prove either emptyChunk_ points nowhere, or points to a truly empty Chunk.
      assert( ( !emptyChunk ) || emptyChunk->empty() );
      if ( !emptyChunk )
        return 0;

      // If emptyChunk_ points to valid Chunk, then chunk list is not empty.
      assert( !chunks.empty() );
      // And there should be exactly 1 empty Chunk.
      assert( 1 == countEmptyChunks() );

      auto poppedChunk = this->popEmptyChunk();

      if ( chunks.empty() ) {
        allocChunk = nullptr;
        deallocChunk = nullptr;
      }
      else {
        if ( deallocChunk == poppedChunk ) {
          deallocChunk = &chunks.front();
          assert( !deallocChunk->empty() );
        }
        if ( allocChunk == poppedChunk ) {
          allocChunk = &chunks.back();
          assert( !allocChunk->empty() );
        }
      }

      emptyChunk = nullptr;
      assert( 0 == countEmptyChunks() );

      return ChunkMemorySize;
    }
    size_t trimChunkList() {
      if ( chunks.empty() ) {
        assert( !allocChunk );
        assert( !deallocChunk );
      }


      const size_t startCapacity = chunks.capacity();
      chunks.shrink_to_fit();

      return (startCapacity - chunks.capacity()) * sizeof(Chunk);
    }

    void  release() noexcept {

      for (Chunk& chunk : chunks )
        chunk.release(pChunkResource);

      chunks.clear();
    }
    void  reset() noexcept {
      release();
      chunks.emplace_back(pChunkResource);
    }

    void* allocate() noexcept {
      assert( !chunks.empty() );
      assert(!deallocChunk || ((&chunks.front() <= deallocChunk) && (deallocChunk <= &chunks.back())));
      assert(!allocChunk || ((&chunks.front() <= allocChunk) && (allocChunk <= &chunks.back())));
      assert( countEmptyChunks() < 2 );

      if ( !allocChunk || allocChunk->full()) {
        auto i         = chunks.begin();
        const auto end = chunks.end();
        for (;; ++i){
          if (i == end) {
            assert( !emptyChunk );
            chunks.emplace_back(pChunkResource);
            allocChunk   = &chunks.back();
            deallocChunk = &chunks.front();
            break;
          }
          if (!i->full()) {
            allocChunk = &*i;
            break;
          }
        }
      }
      assert( allocChunk );
      assert( !allocChunk->full() );

      if (allocChunk == emptyChunk)
        emptyChunk = nullptr;

      return allocChunk->allocate();
    }
    void* allocate(ChunkHint chunkHint) noexcept {
      assert(u32(chunkHint) < chunks.size());
      allocChunk = &chunks[u32(chunkHint)];

      if ( allocChunk->full() )
        return this->allocate();
      return allocChunk->allocate();
    }

    std::pair<void*, ChunkHint> allocateWithHint() noexcept {
      std::pair<void*, ChunkHint> ret;
      ret.first = this->allocate();
      ret.second = ChunkHint(allocChunk - &chunks.front());
      return ret;
    }
    std::pair<void*, ChunkHint> allocateWithHint(ChunkHint chunkHint) noexcept {
      std::pair<void*, ChunkHint> ret;
      ret.first = this->allocate(chunkHint);
      ret.second = ChunkHint(allocChunk - &chunks.front());
      return ret;
    }

    void  deallocate(void* p) noexcept {
      assert( countEmptyChunks() < 2 );

      auto foundChunk = findDeallocChunk( p );
      assert( foundChunk->contains( p ) );

      deallocChunk = foundChunk;
      doDeallocate( p );
      assert( countEmptyChunks() < 2 );
    }
    void  deallocate(void* p, ChunkHint chunk) noexcept {

      assert( countEmptyChunks() < 2 );

      assert( u32(chunk) < chunks.size() );

      deallocChunk = &chunks[u32(chunk)];
      deallocChunk = findDeallocChunk(p);

      assert( deallocChunk && deallocChunk->contains(p) );

      doDeallocate(p);

      assert( countEmptyChunks() < 2 );
    }


    Corruption checkCorruption(){
      const bool   isEmpty = chunks.empty();
      auto         begin   = chunks.begin();
      auto         end     = chunks.end();
      const size_t emptyChunkCount = countEmptyChunks();

      /*struct Variable{
        std::string name;
        std::string value;
      };

      struct Invariant{
        std::string statement;
        std::vector<Variable> variables;
      };

      struct AllocatorState{
        std::vector<Corruption> perChunk;
        std::vector<Invariant>  invariants;
      } state;

      state.perChunk.reserve(chunks.size());*/


      if ( isEmpty ) {
        if ( begin != end ) {
          assert( false );
          return Corruption::BadBookkeeping;
        }
        if ( emptyChunkCount ) {
          assert( false );
          return Corruption::BadBookkeeping;
        }
        if ( deallocChunk ) {
          assert( false );
          return Corruption::BadBookkeeping;
        }
        if ( allocChunk ) {
          assert( false );
          return Corruption::BadBookkeeping;
        }
        if ( emptyChunk ) {
          assert( false );
          return Corruption::BadBookkeeping;
        }
      }
      else {
        const Chunk * front = &chunks.front();
        const Chunk * back  = &chunks.back();
        if ( begin >= end ) {
          assert( false );
          return Corruption::BadBookkeeping;
        }
        if ( back < deallocChunk ) {
          assert( false );
          return Corruption::BadBookkeeping;
        }
        if ( back < allocChunk ) {
          assert( false );
          return Corruption::BadBookkeeping;
        }
        if ( front > deallocChunk ) {
          assert( false );
          return Corruption::BadBookkeeping;
        }
        if ( front > allocChunk ) {
          assert( false );
          return Corruption::BadBookkeeping;
        }

        switch ( emptyChunkCount ) {
          case 0:
            if ( emptyChunk ) {
              assert( false );
              return Corruption::BadBookkeeping;
            }
            break;
          case 1:
            if ( !emptyChunk ) {
              assert( false );
              return Corruption::BadBookkeeping;
            }
            if ( back < emptyChunk ) {
              assert( false );
              return Corruption::BadBookkeeping;
            }
            if ( front > emptyChunk ) {
              assert( false );
              return Corruption::BadBookkeeping;
            }
            if ( !emptyChunk->empty() ) {
              assert( false );
              return Corruption::BadBookkeeping;
            }
            break;
          default:
            assert(false);
            return Corruption::BadBookkeeping;
        }

        for (const Chunk& chunk : chunks ) {
          if (auto result = chunk.checkCorruption(); result != Corruption::None)
            return result;
        }
      }
      return Corruption::None;
    }
  };
  
  struct AddressInterval{
    void* min;
    void* max;
  };

  namespace meta{
    template <size_t N>
    struct CeilLog2 : std::integral_constant<size_t, CeilLog2<(N | (N - 1)) + 1>::value>{};
    template <size_t N> requires(!(N & (N - 1)))
    struct CeilLog2<N> : std::integral_constant<size_t, N>{};

    template <typename T>
    struct FixedChunkSize : public CeilLog2<sizeof(T)>{};
  }


  template <typename T>
  class ObjectAllocator{
  public:
    using value_type = T;
    using pointer = T*;


  };

  template <size_t BlockSize, u8 BlocksPerChunk = u8(256)>
  class FixedResource : public std::pmr::memory_resource{
  public:
    FixedResource() = default;
    explicit FixedResource(std::pmr::memory_resource* pUpstreamResource) noexcept
        : allocatorObj(pUpstreamResource){}

    void release() noexcept {
      allocatorObj.release();
    }
    void reset() noexcept {
      allocatorObj.reset();
    }

  private:

    void * do_allocate(size_t bytes, size_t align) override{
      assert( bytes <= BlockSize );
      assert( align <= BlockSize );

      return allocatorObj.allocate();
    }
    void do_deallocate(void * p, size_t bytes, size_t align) override {
      assert( bytes <= BlockSize );
      assert( align <= BlockSize );

      allocatorObj.deallocate(p);
    }
    [[nodiscard]] bool do_is_equal(const memory_resource & that) const noexcept override{
      return this == &that;
    }

    FixedSizeAllocator<BlockSize, BlocksPerChunk> allocatorObj;
  };


  template <typename O, typename T, typename U>
  concept weak_order = requires(O&& order, const T& t, const U& u){
    { std::forward<O>(order)(t, u) } -> std::convertible_to<std::weak_ordering>;
  };


  template <typename T, weak_order<const T&, const T&> Order = std::compare_three_way>
  class RBTree;

  class RBTreeBase{
  protected:

    class RBNodeBase{
      using Node = RBNodeBase;

    public:
      enum class Colour   : u8 { Red,  Black };
      enum class Position : u8 { Root, Left, Right };



    private:

      int             leftOffset   = 0;
      int             rightOffset  = 0;
      int             parentOffset = 0;
      u16             chunkHint;
      Colour          colour       = Colour::Red;
      Position        position;

      inline const Node* doGetLeftChild() const noexcept {
        return reinterpret_cast<const Node*>((reinterpret_cast<const std::byte*>(this)) + leftOffset);
      }
      inline const Node* doGetRightChild() const noexcept {
        return reinterpret_cast<const Node*>((reinterpret_cast<const std::byte*>(this)) + rightOffset);
      }
      inline const Node* doGetParent() const noexcept {
        return reinterpret_cast<const Node*>((reinterpret_cast<const std::byte*>(this)) + parentOffset);
      }

      inline int getOffsetFrom(const Node* other) const noexcept {
        const ptrdiff_t nodeOffset = reinterpret_cast<const std::byte*>(this) - reinterpret_cast<const std::byte*>(other);
        assert(nodeOffset <= std::numeric_limits<int>::max());
        assert(std::numeric_limits<int>::min() <= nodeOffset);
        return static_cast<int>(nodeOffset);
      }

      inline int leftOffsetFrom(const Node* other) const noexcept {
        return leftOffset + getOffsetFrom(other);
      }
      inline int rightOffsetFrom(const Node* other) const noexcept {
        return rightOffset + getOffsetFrom(other);
      }
      inline int parentOffsetFrom(const Node* other) const noexcept {
        return parentOffset + getOffsetFrom(other);
      }

    protected:

      explicit RBNodeBase(ChunkHint chunkHint) noexcept
          : chunkHint(u16(chunkHint)), colour(Colour::Black), position(Position::Root){}
      RBNodeBase(RBNodeBase* parent, ChunkHint chunkHint, Position pos) noexcept
          : chunkHint(u16(chunkHint)), position(pos){
        parent->setChild(this);
      }

      ~RBNodeBase() = default;

      RBNodeBase& operator=(RBNodeBase&&) noexcept {
        // Implemented as a NOOP so that move assignment of the derived Node type does not modify any of the node's metadata in any way
        return *this;
      }

      inline void setParent(Node* other) noexcept {
        parentOffset = other->getOffsetFrom(this);
      }
      inline void setRightChild(Node* other) noexcept {
        rightOffset = other->getOffsetFrom(this);
        other->setParent(this);
        other->position = Position::Right;
      }
      inline void setLeftChild(Node* other) noexcept {
        leftOffset = other->getOffsetFrom(this);
        other->setParent(this);
        other->position = Position::Left;
      }

      // Deduce right/left from other->position
      inline void setChild(Node* other) noexcept {
        assert(other->position != Position::Root);

        if (other->position == Position::Right){
          rightOffset = other->getOffsetFrom(this);
          other->setParent(this);
        } else {
          leftOffset = other->getOffsetFrom(this);
          other->setParent(this);
        }
      }
      inline void setChild(Node* other, Position pos) noexcept {
        assert(pos != Position::Root);
        if (pos == Position::Right)
          setRightChild(other);
        else
          setLeftChild(other);
      }


      inline void leftRotate(RBTreeBase* tree) noexcept {
        assert(isBranch());

        auto* y = getRightChild();
        switch (position) {
          case Position::Root:
            y->makeRoot(tree);
            break;
          case Position::Left:
            getParent()->setLeftChild(y);
            break;
          case Position::Right:
            getParent()->setRightChild(y);
            break;
          default:
            __assume(false);
            assert(false && "Node::position contained an invalid value");
        }
        setRightChild(y->getLeftChild());
        y->setLeftChild(this);
      }
      inline void rightRotate(RBTreeBase* tree) noexcept {
        assert(isBranch());

        auto* y = getLeftChild();

        switch (position) {
          case Position::Root:
            y->makeRoot(tree);
            break;
          case Position::Left:
            getParent()->setLeftChild(y);
            break;
          case Position::Right:
            getParent()->setRightChild(y);
            break;
          default:
            __assume(false);
            assert(false && "Node::position contained an invalid value");
        }
        setLeftChild(y->getRightChild());
        y->setRightChild(this);
      }
      inline void rotate(RBTreeBase* tree, Position pos) noexcept {
        if (pos == Position::Left)
          leftRotate(tree);
        else
          rightRotate(tree);
      }
      inline void reverseRotate(RBTreeBase* tree, Position pos) noexcept {
        if (pos == Position::Left)
          rightRotate(tree);
        else
          leftRotate(tree);
      }

      inline void  doInsert(RBTreeBase* tree) noexcept {

        Node* x = this;
        Node* y;
        Node* p;
        Node* pp;

        while (!x->isRoot() && (p = x->getParent())->isRed()) {
          pp = p->getParent();
          if (p->position == Position::Left) {
            y = pp->getRightChild();
            if (y->isRed()) {
              p->setBlack();
              y->setBlack();
              pp->setRed();
              x = pp;
            }
            else {
              if (x->position == Position::Right) {
                x = p;
                x->leftRotate(tree);
              }
              Node* _p = x->getParent();
              Node* _pp = _p->getParent();
              _p->setBlack();
              _pp->setRed();
              _pp->rightRotate(tree);
            }
          }
          else {
            y = pp->getLeftChild();
            if ( y->isRed() ) {
              p->setBlack();
              y->setBlack();
              pp->setRed();
              x = pp;
            }
            else {
              if (x->position == Position::Left) {
                x = p;
                x->rightRotate(tree);
              }
              Node* _p = x->getParent();
              Node* _pp = _p->getParent();
              _p->setBlack();
              _pp->setRed();
              _pp->leftRotate(tree);
            }
          }
        }
        if ( tree->pRoot->isRed() ) {
          ++tree->blackDepth;
          tree->pRoot->setBlack();
        }
      }
      /*inline Node* doDelete(RBTreeBase* tree, void(* pfnMoveAssign)(void* to, void* from) noexcept) noexcept {
        Node* removedNode;
        Node* subTree;
        Node* subTreeParent;
        Position subTreePos;
        
        if ( !(tree->isNull(getRightChild()) || tree->isNull(getLeftChild())) ) {
          removedNode = getSuccessor();
          pfnMoveAssign(this, removedNode);
        } else {
          if ( isRoot() ) {

          }
        }
        
        if ( tree->isNull(getRightChild()) ) {
          subTreePos = Position::Left;
        }
        else {
          if ( !tree->isNull(getLeftChild()) ) {
            
          }
        }

        subTreePos = rightOffset ? Position::Right : Position::Left;;

        if ( leftOffset && rightOffset ) {
          removedNode = getSuccessor();
          pfnMoveAssign(this, removedNode);
        } else {
          if ( isRoot() ) {
            getChild(subTreePos)->makeRoot(tree);
            return this;
          }
          removedNode = this;
        }

        subTreeParent = removedNode->getParent();
        subTree = removedNode->getChild(subTreePos);
        subTreeParent->setChild(subTree, subTreePos);

        *//*if (leftOffset && rightOffset) {
          removedNode = getRightChild()->getMinimum();
          subTree     = removedNode->getRightChild();
          subTreePos  = Position::Right;
          subTreeParent = removedNode->getParent();

          subTreeParent->setChild(subTree, removedNode->position);
        } else {
          removedNode = this;
          subTreePos = leftOffset ? Position::Left : Position::Right;
          subTreeParent = getParent();
          subTree = getChild(subTreePos);
          if (isRoot()) {
            tree->pRoot = subTree;
            if (subTree)
              subTree->position = Position::Root;
          } else {
            subTreeParent->setChild(subTree, position);
          }

        }*//*

        if (removedNode->isBlack())
          doPostDeleteRebalance(tree, subTreeParent, subTreePos);

        return removedNode;
      }*/
      inline static void doPostDeleteRebalance(RBTreeBase* tree, Node* node) noexcept {

        Node* parent;
        Node* sibling;

        if ( tree->pRoot != node && node->isBlack() ) {

          parent = node->getParent();

          if ( node->position == Position::Left ) {
            sibling = parent->getRightChild();

            if ( sibling->isRed() ) {
              sibling->setBlack();
              parent->setRed();
              parent->leftRotate(tree);
              parent = node->getParent();
              sibling = parent->getRightChild();
            }

            if ( sibling->getRightChild()->isBlack() ) {
              if ( sibling->getLeftChild()->isBlack() ) {
                sibling->setRed();
                return doPostDeleteRebalance(tree, parent);
              }
              sibling->getLeftChild()->setBlack();
              sibling->setRed();
              sibling->rightRotate(tree);
              sibling = parent->getRightChild();
            }

            sibling->colour = parent->colour;
            parent->setBlack();
            sibling->getRightChild()->setBlack();
            parent->leftRotate(tree);
          }
          else {

            sibling = parent->getLeftChild();

            if ( sibling->isRed() ) {
              sibling->setBlack();
              parent->setRed();
              parent->rightRotate(tree);
              parent = node->getParent();
              sibling = parent->getLeftChild();
            }

            if ( sibling->getLeftChild()->isBlack() ) {
              if ( sibling->getRightChild()->isBlack() ) {
                sibling->setRed();
                return doPostDeleteRebalance(tree, parent);
              }
              sibling->getRightChild()->setBlack();
              sibling->setRed();
              sibling->leftRotate(tree);
              sibling = parent->getLeftChild();
            }

            sibling->colour = parent->colour;
            parent->setBlack();
            sibling->getLeftChild()->setBlack();
            parent->rightRotate(tree);
          }

          tree->pRoot->setBlack();
        }
        else
          node->setBlack();
      }
      inline static void doPostDeleteRebalance(RBTreeBase* tree, Node* parent, Position pos) noexcept {

        __assume(pos != Position::Root);

        Node* subTree = parent->getChild(pos);
        Node* sibling = parent->getSibling(pos);
        Node* subRoot = parent;

        if ( subTree->isBlackOrNull() ) {

          if ( sibling->isRed() ) {
            sibling->setBlack();
            parent->setRed();
            subRoot = sibling;
            parent->rotate(tree, pos);
            sibling = parent->getSibling(pos);
            //parent =
          }


        }
        
        if ( !parent ) {
          if ( tree->pRoot )
            tree->pRoot->setBlack();
        } else {
          subTree = parent->getChild(pos);
          sibling = parent->getSibling(pos);
          
          if ( !subTree || subTree->isBlack() ) {

            if ( sibling->isRed() ) {
              sibling->setBlack();
              parent->setRed();
              parent->rotate(tree, pos);
              //parent = node->getParent();
              sibling = parent->getSibling(pos);
            }

            if ( sibling->getRightChild()->isBlack() ) {
              if ( sibling->getLeftChild()->isBlack() ) {
                sibling->setRed();
                return doPostDeleteRebalance(tree, subRoot, pos);
              }
              sibling->getChild(pos)->setBlack();
              sibling->setRed();
              sibling->reverseRotate(tree, pos);
              sibling = parent->getSibling(pos);
            }

            sibling->colour = parent->colour;
            parent->setBlack();
            sibling->getSibling(pos)->setBlack();
            parent->rotate(tree, pos);
          } 



        }

        if ( parent && (!(subTree = parent->getChild(subTreePos)) || subTree->isBlack()) ) {

          //parent = node->getParent();

          if ( node->position == Position::Left ) {
            sibling = parent->getRightChild();

            if ( sibling->isRed() ) {
              sibling->setBlack();
              parent->setRed();
              parent->leftRotate(tree);
              parent = node->getParent();
              sibling = parent->getRightChild();
            }

            if ( sibling->getRightChild()->isBlack() ) {
              if ( sibling->getLeftChild()->isBlack() ) {
                sibling->setRed();
                return doPostDeleteRebalance(tree, parent);
              }
              sibling->getLeftChild()->setBlack();
              sibling->setRed();
              sibling->rightRotate(tree);
              sibling = parent->getRightChild();
            }

            sibling->colour = parent->colour;
            parent->setBlack();
            sibling->getRightChild()->setBlack();
            parent->leftRotate(tree);
          }
          else {

            sibling = parent->getLeftChild();

            if ( sibling->isRed() ) {
              sibling->setBlack();
              parent->setRed();
              parent->rightRotate(tree);
              parent = node->getParent();
              sibling = parent->getLeftChild();
            }

            if ( sibling->getLeftChild()->isBlack() ) {
              if ( sibling->getRightChild()->isBlack() ) {
                sibling->setRed();
                return doPostDeleteRebalance(tree, parent);
              }
              sibling->getRightChild()->setBlack();
              sibling->setRed();
              sibling->leftRotate(tree);
              sibling = parent->getLeftChild();
            }

            sibling->colour = parent->colour;
            parent->setBlack();
            sibling->getLeftChild()->setBlack();
            parent->rightRotate(tree);
          }

          tree->pRoot->setBlack();
        }
        else
          node->setBlack();
      }

      inline Node* doDelete(RBTreeBase* tree, void(* pfnMoveAssign)(void* to, void* from) noexcept) noexcept {
        switch ( position ) {
          case Position::Root:
            tree->
          case Position::Right:
          case Position::Left:
          VK_no_default;
        }
      }
      inline Node* doLeftDelete(RBTreeBase* tree, void(* pfnMoveAssign)(void* to, void* from) noexcept) noexcept {

        Node* removedNode;

        if ( !tree->isNull(getRightChild())) {
          removedNode = getSuccessor(tree);
          pfnMoveAssign(this, removedNode);
        }
      }
      inline Node* doRightDelete(RBTreeBase* tree, void(* pfnMoveAssign)(void* to, void* from) noexcept) noexcept {
        if ( !tree->isNull(getLeftChild()) )
      }
      inline static void doPostDeleteRebalanceLeft(RBTreeBase* tree, Node* parent) noexcept {

        if ( !parent ) {
          if ( tree->pRoot ) {
            tree->pRoot->setBlack();
            return;
          }
        }


        Node* node    = parent->getLeftChild();
        Node* sibling = parent->getRightChild();
        Node* subRoot = parent;

        if ( node->isBlackOrNull() ) {

          if ( sibling->isRed() ) {
            sibling->setBlack();
            parent->setRed();
            subRoot = sibling;
            parent->leftRotate(tree);
            sibling = parent->getRightChild();
          }


        }

      }
      inline static void doPostDeleteRebalanceRight(RBTreeBase* tree, Node* parent) noexcept {}
    public:

      explicit RBNodeBase(std::nullptr_t, ChunkHint hint) noexcept
          : chunkHint(static_cast<u16>(hint)), colour(Colour::Black), position(Position::Root){}

      [[nodiscard]] inline bool  isRoot()   const noexcept {
        //assert(position == Position::Root);
        return parentOffset == 0;
      }
      [[nodiscard]] inline bool  isLeaf()   const noexcept {
        return leftOffset == 0 && rightOffset == 0;
      }
      [[nodiscard]] inline bool  isBranch() const noexcept {
        return leftOffset != 0 && rightOffset != 0;
      }

      
      /*[[nodiscard]] inline Node* getLeftChild() const noexcept {
        assert(leftOffset != 0);
        const auto* result = doGetLeftChild();
        assert(result != this);
        assert(result->position == Position::Left);
        return const_cast<Node*>(result);
      }
      [[nodiscard]] inline Node* getRightChild() const noexcept {
        assert(rightOffset != 0);
        const auto* result = doGetRightChild();
        assert(result != this);
        assert(result->position == Position::Right);
        return const_cast<Node*>(result);
      }
      [[nodiscard]] inline Node* getParent() const noexcept {
        assert(!isRoot());
        const auto* result = doGetParent();
        assert(result != this);
        assert((position == Position::Left && result->doGetLeftChild() == this) || (result->doGetRightChild() == this));
        return const_cast<Node*>(result);
      }*/


      [[nodiscard]] inline Node* getChild(Position pos) const noexcept {
        if (pos == Position::Right)
          return getRightChild();
        return getLeftChild();
      }
      [[nodiscard]] inline Node* getSibling(Position pos) const noexcept {
        if (pos == Position::Left)
          return getRightChild();
        return getLeftChild();
      }
      [[nodiscard]] inline Node* getLeftChild() const noexcept {
        /*if (leftOffset)
          return const_cast<Node*>(doGetLeftChild());
        return nullptr;*/
        return const_cast<Node*>(doGetLeftChild());
      }
      [[nodiscard]] inline Node* getRightChild() const noexcept {
        /*if (rightOffset)
          return const_cast<Node*>(doGetRightChild());
        return nullptr;*/
        return const_cast<Node*>(doGetRightChild());
      }
      [[nodiscard]] inline Node* getParent() const noexcept {
        /*if (parentOffset != 0)
          return const_cast<Node*>(doGetParent());
        return nullptr;*/
        return const_cast<Node*>(doGetParent());
      }

      [[nodiscard]] inline Node* getMaximum() const noexcept {

        if (this_is_null())
          return nullptr;

        const Node* x = this;
        while (x->rightOffset)
          x = x->getRightChild();
        return const_cast<Node*>(x);
      }
      [[nodiscard]] inline Node* getMinimum() const noexcept {

        if (this_is_null())
          return nullptr;

        const Node* x = this;

        while (x->leftOffset)
          x = x->getLeftChild();
        return const_cast<Node*>(x);
      }
      [[nodiscard]] inline Node* getSuccessor() const noexcept {

        if (this_is_null())
          return nullptr;

        if (rightOffset)
          return getRightChild()->getMinimum();
        const Node* x = this;
        const Node* y;
        do {
          y = x;
          x = x->getParent();
        } while(x && y->position == Position::Right);
        return const_cast<Node*>(x);
      }
      [[nodiscard]] inline Node* getPredecessor() const noexcept{

        if (this_is_null())
          return nullptr;

        if (leftOffset)
          return getLeftChild()->getMaximum();
        const Node* x = this;
        const Node* y;
        do {
          y = x;
          x = x->getParent();
        } while(x && y->position == Position::Left);
        return const_cast<Node*>(x);
      }

      [[nodiscard]] inline Node* getMaximum(RBTreeBase* tree) const noexcept {
        

        const Node* x = this;
        const Node* y = x->getRightChild();
        
        while ( !tree->isNull(y) ) {
          x = y;
          y = y->getRightChild();
        }
        return const_cast<Node*>(x);
      }
      [[nodiscard]] inline Node* getMinimum(RBTreeBase* tree) const noexcept {
        
        const Node* x = this;
        const Node* y = x->getLeftChild();

        while ( !tree->isNull(y) ) {
          x = y;
          y = y->getLeftChild();
        }
        return const_cast<Node*>(x);
      }
      [[nodiscard]] inline Node* getSuccessor(RBTreeBase* tree) const noexcept {
        
        if (const Node* rightChild = getRightChild(); !tree->isNull(rightChild) ) 
          return rightChild->getMinimum();
        
        const Node* x = this;
        const Node* y;
        do {
          y = x;
          x = x->getParent();
        } while( !tree->isNull(x) && y->position == Position::Right);
        return const_cast<Node*>(x);
      }
      [[nodiscard]] inline Node* getPredecessor(RBTreeBase* tree) const noexcept{

        if (const Node* leftChild = getLeftChild(); !tree->isNull(leftChild) )
          return leftChild->getMaximum();

        const Node* x = this;
        const Node* y;
        do {
          y = x;
          x = x->getParent();
        } while( !tree->isNull(x) && y->position == Position::Left);
        return const_cast<Node*>(x);
      }

      inline void  setRed() noexcept {
        colour = Colour::Red;
      }
      inline void  setBlack() noexcept {
        colour = Colour::Black;
      }

      inline void makeRoot(RBTreeBase* pTree) noexcept {
        pTree->pRoot = this;
        position = Position::Root;
        setParent(pTree->pNull);
      }

      [[nodiscard]] inline bool  isRed() const noexcept {
        return colour == Colour::Red;
      }
      [[nodiscard]] inline bool  isBlack() const noexcept {
        return colour == Colour::Black;
      }

      [[nodiscard]] inline ChunkHint getAllocHint() const noexcept {
        return ChunkHint(chunkHint);
      }

      [[nodiscard]] inline std::pair<u32, bool> checkTreeInvariants(const RBTreeBase* tree) const noexcept{

        if ( tree->isNull(this) )
          return { 0, true };

        auto [ leftDepth,  leftSuccess ]  = getLeftChild()->checkTreeInvariants(tree);
        auto [ rightDepth, rightSuccess ] = getRightChild()->checkTreeInvariants(tree);

        if ( !leftSuccess || !rightSuccess ) {
          assert( false );
          return { 0, false };
        }

        if ( leftDepth != rightDepth ) {
          assert( false );
          return { 0, false };
        }

        if (isRed()) {
          if ( getLeftChild()->isRed() ) {
            assert( false );
            return { 0, false };
          }
          if ( getRightChild()->isRed() ) {
            assert( false );
            return { 0, false };
          }
          return { leftDepth, true };
        }

        return { leftDepth + 1, true };
      }
    };


    RBNodeBase* pRoot;
    RBNodeBase* pNull;
    u32         blackDepth;
    u32         nodeCount;


    [[nodiscard]] inline bool isNull(const RBNodeBase* node) const noexcept {
      return node == pNull;
    }

  public:

    [[nodiscard]] bool checkInvariants() const noexcept {
      if ( !pRoot )
        return true;

      if ( pRoot->isRed() ) {
        assert( false );
        return false;
      }
      auto [ treeDepth, success ] = pRoot->checkTreeInvariants(this);

      if ( !success )
        return false;

      if ( treeDepth != blackDepth ) {
        assert( false );
        return false;
      }

      return true;
    }

  };

  template <typename T, weak_order<const T&, const T&> Order>
  class RBTree : public RBTreeBase{
    class RBNode : public RBNodeBase{

    public:

      T value_;


      explicit RBNode(ChunkHint hint) : RBNodeBase(hint), value_(){}

      template <typename ...Args> requires(std::constructible_from<T, Args...>)
      explicit RBNode(ChunkHint hint, Args&& ...args)
              noexcept(std::is_nothrow_constructible_v<T, Args...>)
          : RBNodeBase(hint),
            value_(std::forward<Args>(args)...){}
      template <typename ...Args> requires(std::constructible_from<T, Args...>)
      RBNode(RBNode* parentNode, ChunkHint hint, Position pos, Args&& ...args)
      noexcept(std::is_nothrow_constructible_v<T, Args...>)
          : RBNodeBase(parentNode, hint, pos),
            value_(std::forward<Args>(args)...){}

      RBNode(RBNode&& other) noexcept = default;
      RBNode& operator=(RBNode&& other) noexcept = default;


      template <typename F>
      RBNode* find(const RBTree<T>* tree, F&& eqFunc) noexcept {

        if (tree->isNull(this))
          return this;

        auto result = eqFunc(value_);
        if (result == std::weak_ordering::equivalent)
          return this;

        return (result == std::weak_ordering::less ?
                                                   leftChild() :
                                                   rightChild()
                )->find(tree, std::forward<F>(eqFunc));
      }

      template <typename F, typename ...Args>
      std::pair<RBNode*, bool> insert(RBTree<T>* tree, F&& func, Args&& ...args) noexcept {

        auto result = func(value_);

        if (result == std::weak_ordering::equivalent)
          return { this, false };

        auto pos = result == std::weak_ordering::less ? Position::Left : Position::Right;
        auto* childNode = child(pos);
        if ( tree->isNull(childNode) ) {
          childNode = tree->newNode(this, pos, std::forward<Args>(args)...);
          assert( childNode );
          childNode->doInsert(tree);
          return { childNode, true };
        }
        else
          return childNode->insert(tree, std::forward<F>(func), std::forward<Args>(args)...);
      }

      [[nodiscard]] RBNode* remove(RBTree<T>* tree) noexcept {
        return static_cast<RBNode*>(this->doDelete(tree, [](void* to, void* from) noexcept {
          (*static_cast<RBNode*>(to)) = std::move(*static_cast<RBNode*>(from));
        }));
      }


      RBNode* successor() const noexcept {
        return static_cast<RBNode*>(this->getSuccessor());
      }
      RBNode* predecessor() const noexcept {
        return static_cast<RBNode*>(this->getPredecessor());
      }
      RBNode* minimum() const noexcept {
        return static_cast<RBNode*>(this->getMinimum());
      }
      RBNode* maximum() const noexcept {
        return static_cast<RBNode*>(this->getMaximum());
      }

      RBNode* leftChild() const noexcept {
        return static_cast<RBNode*>(getLeftChild());
      }
      RBNode* rightChild() const noexcept {
        return static_cast<RBNode*>(getRightChild());
      }
      RBNode* child(Position pos) const noexcept {
        return static_cast<RBNode*>(getChild(pos));
      }

    };

    template <typename QualifiedType>
    class NodeIterator{


      inline void inc() noexcept {
        if ( *this )
          node = node->successor();
        else
          node = tree->root()->minimum();
      }
      inline void dec() noexcept {
        if ( *this )
          node = node->predecessor();
        else
          node = tree->root()->maximum();
      }

    public:

      using iterator_category = std::bidirectional_iterator_tag;
      using value_type = QualifiedType;
      using reference  = value_type&;
      using const_reference = const value_type&;
      using pointer    = value_type*;
      using const_pointer = const value_type*;

      using tree_type = std::conditional_t<std::is_const_v<value_type>, const RBTree, RBTree>;
      using node_type = std::conditional_t<std::is_const_v<value_type>, const RBNode, RBNode>;

      NodeIterator() = default;
      NodeIterator(tree_type* tree, node_type* node) noexcept
          : tree(tree), node(node){}

      template <typename OtherT> requires(std::assignable_from<QualifiedType*, OtherT*>)
      NodeIterator(const NodeIterator<OtherT>& other) noexcept
          : tree(other.tree), node(other.node){}


      reference operator*() const noexcept  {
        assert( *this );
        return node->value_;
      }
      pointer   operator->() const noexcept {
        assert( *this );
        return std::addressof(node->value_);
      }

      NodeIterator& operator++() noexcept {
        this->inc();
        return *this;
      }
      NodeIterator  operator++(int) noexcept {
        NodeIterator copy = *this;
        this->inc();
        return copy;
      }
      NodeIterator& operator--() noexcept {
        this->dec();
        return *this;
      }
      NodeIterator  operator--(int) noexcept {
        NodeIterator copy = *this;
        this->dec();
        return copy;
      }


      explicit operator bool() const noexcept {
        return !tree->isNull(node);
      }

      friend bool                  operator==(NodeIterator a, NodeIterator b) noexcept {
        return a.node == b.node;
      }
      friend std::partial_ordering operator<=>(NodeIterator a, NodeIterator b) noexcept {
        if (a.tree != b.tree)
          return std::partial_ordering::unordered;
        if ( !(a && b) ) {
          if ( a )
            return std::partial_ordering::less;
          if ( b )
            return std::partial_ordering::greater;
          return std::partial_ordering::equivalent;
        }
        return a.tree->getOrder()(a.node->value_, b.node->value_);
      }

      tree_type* tree = nullptr;
      node_type* node = nullptr;
    };




    template <typename ...Args> requires(std::constructible_from<T, Args...>)
    RBNode* newNode(Args&& ...args) noexcept(std::is_nothrow_constructible_v<T, Args...>) {
      auto&& [addr, allocHint] = nodeAllocator.allocateWithHint();
      return new(addr) RBNode(allocHint, std::forward<Args>(args)...);
      //return new(nodeAllocator.)
    }
    template <typename ...Args> requires(std::constructible_from<T, Args...>)
    RBNode* newNode(RBNode* parentNode, typename RBNode::Position pos, Args&& ...args) noexcept(std::is_nothrow_constructible_v<T, Args...>) {
      auto&& [addr, allocHint] = nodeAllocator.allocateWithHint();
      return new(addr) RBNode(parentNode, allocHint, pos, std::forward<Args>(args)...);
      //return new(nodeAllocator.)
    }

    void deleteNode(RBNode* pNode) noexcept {
      pNode->~RBNode();
      nodeAllocator.deallocate(pNode, pNode->getAllocHint());
    }

    void destroyTree() noexcept {
      destroySubTree(root());
      nodeAllocator.release();
    }
    void destroySubTree(RBNode* subRoot) noexcept {
      if (subRoot != pNull) {
        destroySubTree(subRoot->leftChild());
        destroySubTree(subRoot->rightChild());
        subRoot->~RBNode();
      }
    }

    void init() noexcept {
      ChunkHint nullHint;
      std::tie((void*&)pNull, nullHint) = nodeAllocator.allocateWithHint();
      ::new(pNull) RBNodeBase(nullptr, nullHint);
      pRoot = pNull;
    }

    /*template <typename F>
    void invokeForEachNode(F&& func) noexcept {

    }*/

    /*template <typename Ret, typename ...Args>
    Ret invokeOnRoot(Ret(RBNode::* pfnOp)(Args...), std::type_identity_t<Args>... args) noexcept {
      return std::forward<Ret>(())
    }
    template <typename Ret, typename ...Args>
    Ret invokeOnRoot(Ret(RBNode::* pfnOp)(Args...) const, std::type_identity_t<Args>... args) noexcept {

    }
    template <typename Ret, typename ...Args>
    Ret invokeOnRoot(Ret(RBNode::* pfnOp)(Args...) const, std::type_identity_t<Args>... args) const noexcept {

    }*/

    [[nodiscard]] RBNode* root() const noexcept {
      return static_cast<RBNode*>(pRoot);
    }
    [[nodiscard]] RBNode* null() const noexcept {
      return static_cast<RBNode*>(pNull);
    }


    template <typename F>
    inline bool doErase(F&& func) noexcept {

      RBNode* result = root()->find(this, std::forward<F>(func));

      if ( isNull(result) )
        return false;

      deleteNode(result->remove(this));
      return true;
    }


  public:

    using value_type = T;
    using allocator_type = FixedSizeAllocator<meta::FixedChunkSize<RBNode>::value>;
    using iterator = NodeIterator<T>;
    using const_iterator = NodeIterator<const T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;


    RBTree(Order order = {})
        : RBTreeBase(),
          nodeAllocator(std::pmr::get_default_resource()),
          order(order){
      init();
    }

    RBTree(const RBTree& other);

    explicit RBTree(std::pmr::memory_resource* pMemory, Order order = {}) noexcept
        : RBTreeBase(),
          nodeAllocator(pMemory),
          order(order){
      init();
    }


    ~RBTree() {
      destroyTree();
    }



    [[nodiscard]] size_t          size() const noexcept {
      return nodeCount;
    }


    [[nodiscard]] iterator        begin()       noexcept {
      return iterator{ this, root()->minimum() };
    }
    [[nodiscard]] const_iterator  begin() const noexcept {
      return const_iterator{ this, root()->minimum() };
    }
    [[nodiscard]] const_iterator cbegin() const noexcept {
      return this->begin();
    }


    [[nodiscard]] iterator        end()       noexcept {
      return iterator{ this, null() };
    }
    [[nodiscard]] const_iterator  end() const noexcept {
      return const_iterator{ this, null() };
    }
    [[nodiscard]] const_iterator cend() const noexcept {
      return this->end();
    }


    [[nodiscard]] reverse_iterator        rbegin()       noexcept {
      return reverse_iterator(this->end());
    }
    [[nodiscard]] const_reverse_iterator  rbegin() const noexcept {
      return const_reverse_iterator(this->end());
    }
    [[nodiscard]] const_reverse_iterator crbegin() const noexcept {
      return const_reverse_iterator(this->end());
    }

    [[nodiscard]] reverse_iterator        rend()       noexcept {
      return reverse_iterator(this->begin());
    }
    [[nodiscard]] const_reverse_iterator  rend() const noexcept {
      return const_reverse_iterator(this->begin());
    }
    [[nodiscard]] const_reverse_iterator crend() const noexcept {
      return const_reverse_iterator(this->cbegin());
    }


    const_iterator find(const T& value) const noexcept {
      return const_iterator{ this, root()->find(this, [&](const T& other) noexcept { return order(value, other); }) };
    }
    template <typename Key, std::strict_weak_order<const Key&, const T&> KeyOrder = Order>
    const_iterator find(const Key& key, KeyOrder&& keyOrder = {}) const noexcept {
      return const_iterator{ this, root()->find(this, [&](const T& other) noexcept { return keyOrder(key, other); }) };
    }

    std::pair<iterator, bool> insert(const T& value) noexcept {
      if ( isNull(root()) ) {
        newNode(value)->makeRoot(this);
        nodeCount = 1;
        blackDepth = 1;
        return { begin(), true };
      }
      std::pair<iterator, bool> result;
      RBNode* pResultNode;
      std::tie(pResultNode, result.second) = root()->insert(this, [&](const T& other) noexcept {
            return order(value, other);
          }, value);

      nodeCount += result.second;

      expensive_assert(checkInvariants());

      result.first = iterator(this, pResultNode);
      return result;
    }
    bool                      remove(const T& value) noexcept {
      bool result = this->doErase([&](const T& other){ return order(value, other); });
      this->nodeCount -= result;
      expensive_assert(checkInvariants());
      return result;
    }

    const Order& getOrder() const noexcept {
      return order;
    }


  private:
    allocator_type              nodeAllocator;
    [[no_unique_address]] Order order;
  };



  class AddressIntervalTree{
    class Node {
    public:
      enum class Colour   : unsigned short{ Red,  Black };
      enum class Position : unsigned short{ Root, Left, Right };

    private:
      AddressInterval interval;
      int             leftOffset;
      int             rightOffset;
      int             parentOffset;
      Colour          colour;
      Position        position;

      inline const Node* doGetLeftChild() const noexcept {
        return reinterpret_cast<const Node*>((reinterpret_cast<const std::byte*>(this)) + leftOffset);
      }
      inline const Node* doGetRightChild() const noexcept {
        return reinterpret_cast<const Node*>((reinterpret_cast<const std::byte*>(this)) + rightOffset);
      }
      inline const Node* doGetParent() const noexcept {
        return reinterpret_cast<const Node*>((reinterpret_cast<const std::byte*>(this)) + parentOffset);
      }

      inline int getOffsetFrom(const Node* other) const noexcept {
        const ptrdiff_t nodeOffset = reinterpret_cast<const std::byte*>(this) - reinterpret_cast<const std::byte*>(other);
        assert(nodeOffset <= std::numeric_limits<int>::max());
        assert(std::numeric_limits<int>::min() <= nodeOffset);
        return static_cast<int>(nodeOffset);
      }

      inline void setParent(Node* other) noexcept {
        parentOffset = other->getOffsetFrom(this);
      }
      inline void setRightChild(Node* other) noexcept {
        rightOffset = other->getOffsetFrom(this);
        other->setParent(this);
      }
      inline void setLeftChild(Node* other) noexcept {
        leftOffset = other->getOffsetFrom(this);
        other->setParent(this);
      }

      inline void makeRoot(AddressIntervalTree* pTree) noexcept {
        parentOffset = 0;
        position = Position::Root;
        pTree->setRoot(this);
      }

      inline int leftOffsetFrom(const Node* other) const noexcept {
        return leftOffset + getOffsetFrom(other);
      }
      inline int rightOffsetFrom(const Node* other) const noexcept {
        return rightOffset + getOffsetFrom(other);
      }
      inline int parentOffsetFrom(const Node* other) const noexcept {
        return parentOffset + getOffsetFrom(other);
      }

    public:

      inline void leftRotate(AddressIntervalTree* pTree) noexcept {
        assert(isBranch());
        assert(getRightChild()->isBranch());

        auto* y = getRightChild();
        setRightChild(y->getLeftChild());
        switch (position) {
          case Position::Root:
            y->makeRoot(pTree);
            break;
          case Position::Left:
            getParent()->setLeftChild(y);
            break;
          case Position::Right:
            getParent()->setRightChild(y);
            break;
          default:
            __assume(false);
            assert(false && "Node::position contained an invalid value");
        }
        y->setLeftChild(this);
      }
      inline void rightRotate(AddressIntervalTree* pTree) noexcept {
        assert(isBranch());
        assert(getLeftChild()->isBranch());

        auto* y = getLeftChild();
        setLeftChild(y->getRightChild());
        switch (position) {
          case Position::Root:
            y->makeRoot(pTree);
            break;
          case Position::Left:
            getParent()->setLeftChild(y);
            break;
          case Position::Right:
            getParent()->setRightChild(y);
            break;
          default:
            __assume(false);
            assert(false && "Node::position contained an invalid value");
        }
        y->setRightChild(this);
      }


      inline bool  isRoot()   const noexcept {
        assert(position == Position::Root);
        return parentOffset == 0;
      }
      inline bool  isLeaf()   const noexcept {
        return leftOffset == 0 && rightOffset == 0;
      }
      inline bool  isBranch() const noexcept {
        return leftOffset != 0 && rightOffset != 0;
      }

      inline Node* getLeftChild() noexcept {
        //assert(isBranch());
        const auto* result = doGetLeftChild();
        assert(result != this);
        assert(result->position == Position::Left);
        return const_cast<Node*>(result);
      }
      inline Node* getRightChild() noexcept {
        //assert(isBranch());
        const auto* result = doGetRightChild();
        assert(result != this);
        assert(result->position == Position::Right);
        return const_cast<Node*>(result);
      }
      inline Node* getParent() noexcept {
        assert(!isRoot());
        const auto* result = doGetParent();
        assert(result != this);
        assert((position == Position::Left && result->doGetLeftChild() == this) || (result->doGetRightChild() == this));
        return const_cast<Node*>(result);
      }

      inline void  setRed() noexcept {
        colour = Colour::Red;
      }
      inline void  setBlack() noexcept {
        colour = Colour::Black;
      }

      inline bool  isRed() const noexcept {
        return colour == Colour::Red;
      }
      inline bool  isBlack() const noexcept {
        return colour == Colour::Black;
      }
    };
    Node* pRoot;
    json_fixed_size_allocator nodeAllocator;
    json_u32_t blackDepth;

    inline void setRoot(Node* pNewRoot) noexcept {
      // Unsafe, should only be called by member functions of AddressIntervalTree::Node
      pRoot = pNewRoot;
    }

    static_assert(sizeof(Node) == 32);

    inline void doInsert(Node* node) noexcept {

    }


  public:

  };
}


#include <iostream>




int main() {
  using namespace json;

  using i64 = long long;

  using long_tree = RBTree<i64>;
  using tree_allocator = typename long_tree::allocator_type;

  tree_allocator allocator{};
  RBTree<i64> intTree{};


  const static auto printTree = [](const RBTree<i64>& tree){
    auto a = tree.begin();
    const auto b = tree.end();

    std::cout << "{ " << *a++;

    for (; a != b; ++a)
      std::cout << ", " << *a;

    std::cout << " }" << std::endl;
  };


  for (auto i : { 1, 6, -2, 30, -22, 0, 1, 3, 10 })
    intTree.insert(i);

  printTree(intTree);

  for (auto i : { 2, 1, -4, 0, 10 })
    intTree.remove(i);

  printTree(intTree);

  for (auto i : { 1, -20, -4, 2})
    intTree.insert(i);

  printTree(intTree);
}


/*
JSON_BEGIN_C_NAMESPACE









JSON_END_C_NAMESPACE
*/


